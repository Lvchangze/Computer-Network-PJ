#ifndef STARTER_CODE_PACKET_H
#define STARTER_CODE_PACKET_H

#include <assert.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>

#define MAGICNUM 15441
#define VERSION 1
#define BUFLEN 1500
#define PACKETLEN 1500 //packet大小
#define HEADERLEN 16 //头部16字节
#define SHA1_HASH_SIZE 20 //hash16字节
#define MAX_CHANK_NUM 74
#define DATALEN PACKETLEN-HEADERLEN //1500-16
#define DATA_PACKET_DATA_LEN 1400
#define SEND_PACKET_DATA_LEN 1024
#define WHOHAS        0
#define IHAVE        1
#define GET            2
#define DATA        3
#define ACK        4
#define DENIED        5

typedef struct header_s {
    short magicnum;
    char version;
    char packet_type;
    short header_len;
    short packet_len;
    u_int seq_num;
    u_int ack_num;
} header_t;

typedef struct data_packet {
    header_t header;
    char data[DATALEN];
} data_packet_t;

void init_packet(data_packet_t *, char, short, u_int, u_int, char *);

data_packet_t *create_packet(char, short, u_int, u_int, char *);

void free_packet(data_packet_t *);

//different packet
data_packet_t *create_whohas_packet(short, void *);

data_packet_t *create_ihave_packet(short, void *);

data_packet_t *create_get_packet(short, char *);

data_packet_t *create_data_packet(short, uint, uint, char *);

data_packet_t *create_ack_packet(uint, uint);

data_packet_t *create_denied_packet();

//字节顺序转换
void host_to_net(data_packet_t *);

void net_to_host(data_packet_t *);

void send_packet(int, data_packet_t *, struct sockaddr *);

//check
int packet_parser(void *);

#endif // STARTER_CODE_PACKET_H
