/*
 * peer.c
 * 
 * Author: Yi Lu <19212010040@fudan.edu.cn>,
 *
 * Modified from CMU 15-441,
 * Original Authors: Ed Bardsley <ebardsle+441@andrew.cmu.edu>,
 *                   Dave Andersen
 * 
 * Class: Networks (Spring 2015)
 * Name: Lv Changze
 * Id: 18302010026
 */

#include "peer.h"

int main(int argc, char **argv) {

    bt_init(&config, argc, argv);

    DPRINTF(DEBUG_INIT, "peer.c main beginning\n");

#ifdef TESTING
    config.identity = 1; // your student number here
    strcpy(config.chunk_file, "chunkfile");
    strcpy(config.has_chunk_file, "haschunks");
#endif

    bt_parse_command_line(&config);
    init_up_pool(&up_pool, config.max_conn);
    init_down_pool(&down_pool, config.max_conn);

#ifdef DEBUG
    if (debug & DEBUG_INIT) {
        bt_dump_config(&config);
    }
#endif
    peer_run(&config);
    free(up_pool.conns);
    free(down_pool.conns);
    return 0;
}

void process_inbound_udp(int sock) {
    struct sockaddr_in from;
    socklen_t fromlen;
    char buf[BUFLEN];
    data_packet_t *pkt;
    fromlen = sizeof(from);
    if (spiffy_recvfrom(sock, buf, BUFLEN, 0, (struct sockaddr *) &from, &fromlen) != -1) {
        // printf("PROCESS_INBOUND_UDP SKELETON -- replace!\n""Incoming message from %s:%d\n%s\n\n", inet_ntoa(from.sin_addr), ntohs(from.sin_port), buf);
        pkt = (data_packet_t *) buf;
        net_to_host(pkt);
        int packet_type = packet_parser(pkt);
        bt_peer_t *peer = get_peer(&config, from);
        switch (packet_type) {
            case WHOHAS: {
                handle_whohas(pkt, peer);
                break;
            }
            case IHAVE: {
                handle_ihave(pkt, peer);
                break;
            }
            case GET: {
                handle_get(pkt, peer);
                break;
            }
            case DATA: {
                handle_data(pkt, peer);
                break;
            }
            case ACK: {
                handle_ack(pkt, peer);
                break;
            }
            case DENIED: {
                break;
            }
            default: {
                break;
            }
        }
    }
}

void process_get(char *chunkfile, char *outputfile) {
    printf("PROCESS GET SKELETON CODE CALLED.  Fill me in!  (%s, %s)\n", chunkfile, outputfile);
    set_peer_file(config.output_file, outputfile);//set the output file of the bt config
    queue *who_has_queue = init_whohas_queue(chunkfile);//create the who_has packet queue
    task = init_task(outputfile, chunkfile, config.max_conn);//init task
    flood_whohas(sock, who_has_queue);//ask all in the network who has the chunks
    free_queue(who_has_queue, 0);//free the queue we create as it is useless
    timer_start(&(task->timer));//start timer
}

void handle_user_input(char *line, void *cbdata) {
    char chunkf[128], outf[128];

    bzero(chunkf, sizeof(chunkf));
    bzero(outf, sizeof(outf));

    if (sscanf(line, "GET %120s %120s", chunkf, outf)) {//接受用户手动输入参数
        if (strlen(outf) > 0) {
            process_get(chunkf, outf); //处理get请求
        }
    }
}

//
void peer_run(bt_config_t *config) {
    struct sockaddr_in myaddr;
    fd_set readfds;
    struct user_iobuf *userbuf;

    if ((userbuf = create_userbuf()) == NULL) {
        perror("peer_run could not allocate userbuf");
        exit(-1);
    }

    if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP)) == -1) {
        perror("peer_run could not create socket");
        exit(-1);
    }

    bzero(&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(config->myport);

    if (bind(sock, (struct sockaddr *) &myaddr, sizeof(myaddr)) == -1) {
        perror("peer_run could not bind socket");
        exit(-1);
    }

    spiffy_init(config->identity, (struct sockaddr *) &myaddr, sizeof(myaddr));

    while (1) {
        int nfds;
        struct timeval timer;
        timer.tv_sec = 2;
        timer.tv_usec = 0;
        FD_SET(STDIN_FILENO, &readfds);
        FD_SET(sock, &readfds);

        nfds = select(sock + 1, &readfds, NULL, NULL, &timer); //sock是最大fd描述符，需要设置成最大描述符+1

        if (nfds > 0) {
            if (FD_ISSET(sock, &readfds)) {//检查文件符是否可供读写
                process_inbound_udp(sock);
            }
            if (FD_ISSET(STDIN_FILENO, &readfds)) {
                process_user_input(STDIN_FILENO, userbuf, handle_user_input, "Currently unused");
            }
        }
        if (task != NULL) { //说明task 但task完成时不进入该分支
            int f = remove_stalled_chunks(&down_pool);
            if (f) {
                continue_task(task, &down_pool, sock);
            }
        }
        remove_unack_peers(&up_pool, sock);
    }
}

void handle_whohas(data_packet_t *pkt, bt_peer_t *peer) {
    queue *chunks = data_to_chunks_queue(pkt->data);
    queue *ihave = which_I_have(chunks);
    if (ihave != NULL) {
        queue *pkts = init_ihave_queue(ihave);
        send_pkts(sock, (struct sockaddr *) &peer->addr, pkts);
        free_queue(pkts, 0);
        free_queue(ihave, 0);
    }
    free_queue(chunks, 0);
}

void handle_ihave(data_packet_t *pkt, bt_peer_t *peer) {
    if (get_down_conn(&down_pool, peer) != NULL) { //already use the conn
        return;
    } else {
        queue *chunks = data_to_chunks_queue(pkt->data);
        for (node *n = chunks->head; n != NULL; n = n->next) {
            uint8_t *sha_1 = n->data;
            available_peer(task, sha_1, peer);
        }
        if (down_pool.conn == down_pool.max_conn) { //不存在可用连接
            fprintf(stderr, "Full pool!");
        } else {
            chunk_t *chunk = choose_chunk(task, chunks, peer);//查找需要下载的块
            down_conn_t *down_conn = add_to_down_pool(&down_pool, peer, chunk);
            timer_start(&down_conn->timer); //启动定时器
            chunk->inuse = 1;
            data_packet_t *get = create_get_packet(HEADERLEN + SHA1_HASH_SIZE, (char *) chunk->sha_1);
            send_packet(sock, get, (struct sockaddr *) &peer->addr);
            free_packet(get);
        }
        free_queue(chunks, 1);
    }
}

void handle_get(data_packet_t *pkt, bt_peer_t *peer) {
    up_conn_t *up_conn = get_up_conn(&up_pool, peer);
    if (up_conn != NULL && up_conn->last_sent == 512) {
        remove_from_up_pool(&up_pool, peer);
        up_conn = NULL;
    }
    if (up_conn == NULL) {
        if (up_pool.conn >= up_pool.max_conn) {//已满，拒绝请求
            data_packet_t *denied_pkt = create_denied_packet();
            send_packet(sock, denied_pkt, (struct sockaddr *) &peer->addr);
            free_packet(denied_pkt);
        } else {
            data_packet_t **pkts = init_data_array((uint8_t *) pkt->data);
            up_conn = add_to_up_pool(&up_pool, peer, pkts);
            timer_start(&up_conn->timer); //启动定时器
            if (up_conn != NULL) {
                send_data_packets(up_conn, sock, (struct sockaddr *) &peer->addr);
            }
        }
    } else {//已经加入队列,无法修改
        fprintf(stderr, "already in pool!\n");
    }
}

void handle_data(data_packet_t *pkt, bt_peer_t *peer) {
    down_conn_t *down_conn = get_down_conn(&down_pool, peer);
    uint seq = pkt->header.seq_num;
    int data_len = pkt->header.packet_len - HEADERLEN;
    data_packet_t *ack_packet = NULL;
    if (down_conn != NULL) {
        timer_start(&down_conn->timer);
        if (seq == down_conn->next_ack) {
            memcpy(down_conn->chunk->data + down_conn->pos, pkt->data, (size_t) data_len); //copy the data
            down_conn->pos += data_len;
            down_conn->next_ack += 1;
            ack_packet = create_ack_packet(seq, 0);
        } else {
            ack_packet = create_ack_packet(down_conn->next_ack - 1, 0); //发送冗余ack
        }
        if (ack_packet != NULL) {
            send_packet(sock, ack_packet, (struct sockaddr *) &peer->addr);
            timer_start(&down_conn->timer); //重启定时器
            free_packet(ack_packet);
        }
        if (down_conn->pos == BT_CHUNK_SIZE) {
            down_conn->chunk->flag = 1;
            task->chunk_num++;
            remove_from_down_pool(&down_pool, peer);
            if (task->chunk_num == task->need_num && check_task(task)) {
                task = finish_task(task);
            } else {
                continue_task(task, &down_pool, sock);
            }
        }
    }
}

void handle_ack(data_packet_t *pkt, bt_peer_t *peer) {
    up_conn_t *up_conn = get_up_conn(&up_pool, peer);
    if (up_conn == NULL)
        return;
    timer_start(&up_conn->timer);
    if (pkt->header.ack_num == CHUNK_SIZE) {
        remove_from_up_pool(&up_pool, peer);
    } else if (pkt->header.ack_num >= up_conn->last_ack + 1) { //有效ack
        up_conn->last_ack = pkt->header.ack_num; //更新ack
        up_conn->available = up_conn->last_ack + up_conn->rwnd;
        up_conn->duplicate = 1; // 更新duplicate
        send_data_packets(up_conn, sock, (struct sockaddr *) &peer->addr);
    } else if (pkt->header.ack_num == up_conn->last_ack) {//冗余ACK
        up_conn->duplicate++;
        if (up_conn->duplicate >= 3) {
            up_conn->last_sent = pkt->header.ack_num;
            send_data_packets(up_conn, sock, (struct sockaddr *) &peer->addr);
            up_conn->duplicate = 1;
        }
    }
}
