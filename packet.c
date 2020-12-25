#include <malloc.h>
#include <netinet/in.h>
#include "packet.h"
#include "spiffy.h"

void init_packet(data_packet_t *pkt, char type, short len, u_int seq, u_int ack, char *data) {
    pkt->header.magicnum = 15441;
    pkt->header.version = 1;
    pkt->header.packet_type = type;
    pkt->header.header_len = HEADERLEN;
    pkt->header.packet_len = len;
    pkt->header.seq_num = seq;
    pkt->header.ack_num = ack;
    if (pkt->data != NULL) {
        memcpy(pkt->data, data, len - HEADERLEN);
    }
    return;
}

data_packet_t *create_packet(char type, short len, u_int seq, u_int ack, char *data) {
    data_packet_t *pkt = malloc(sizeof(data_packet_t));
    init_packet(pkt, type, len, seq, ack, data);
    return pkt;
}

void free_packet(data_packet_t *pkt) {
    free(pkt);
}

data_packet_t *create_whohas_packet(short len, void *data) {
    data_packet_t *pkt = create_packet(WHOHAS, len, 0, 0, data);
    return pkt;
}

data_packet_t *create_ihave_packet(short len, void *data) {
    return create_packet(IHAVE, len, 0, 0, data);
}

data_packet_t *create_get_packet(short len, char *data) {
    return create_packet(GET, len, 0, 0, data);
}

data_packet_t *create_data_packet(short len, uint ack_num, uint seq_num, char *data) {
    return create_packet(DATA, len, seq_num, ack_num, data);
}

data_packet_t *create_ack_packet(uint ack_num, uint seq_num) {
    return create_packet(ACK, HEADERLEN, seq_num, ack_num, NULL);
}

data_packet_t *create_denied_packet() {
    return create_packet(DENIED, HEADERLEN, 0, 0, NULL);
}

int packet_parser(void *packet) {
    data_packet_t *pkt = (data_packet_t *) packet;
    header_t header = pkt->header;
    if (header.magicnum != MAGICNUM) {
        return -1;
    }
    if (header.version != VERSION)
        return -1;
    if (header.packet_type < WHOHAS || header.packet_type > DENIED)
        return -1;
    return header.packet_type;
}

//字节顺序转换
void host_to_net(data_packet_t *pkt) {
    pkt->header.magicnum = htons(pkt->header.magicnum);
    pkt->header.header_len = htons(pkt->header.header_len);
    pkt->header.packet_len = htons(pkt->header.packet_len);
    pkt->header.seq_num = htonl(pkt->header.seq_num);
    pkt->header.ack_num = htonl(pkt->header.ack_num);
}

void net_to_host(data_packet_t *pkt) {
    pkt->header.magicnum = ntohs(pkt->header.magicnum);
    pkt->header.header_len = ntohs(pkt->header.header_len);
    pkt->header.packet_len = ntohs(pkt->header.packet_len);
    pkt->header.seq_num = ntohl(pkt->header.seq_num);
    pkt->header.ack_num = ntohl(pkt->header.ack_num);
}

void send_packet(int sock, data_packet_t *pkt, struct sockaddr *to) {
    size_t len = (size_t) pkt->header.packet_len;
    host_to_net(pkt);
    spiffy_sendto(sock, pkt, len, 0, to, sizeof(*to));
    net_to_host(pkt);
}