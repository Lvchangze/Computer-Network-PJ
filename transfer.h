#include "queue.h"
#include "handle_GET.h"
#include <netinet/in.h>

#ifndef STARTER_CODE_TRANS_H
#define STARTER_CODE_TRANS_H

queue *init_work();

queue *init_whohas_queue(const char *);

queue *init_ihave_queue(queue *);

data_packet_t **init_data_array(uint8_t *sha);

queue *init_chunk_file(const char *);

void init_has_chunks(const char *);

int check_I_have(uint8_t *);

queue *which_I_have(queue *);

queue *data_to_chunks_queue(void *);

queue *init_chunk_packet_queue(queue *chunks, data_packet_t *(*make_chunk_packet)(short, void *));

//发出队列中的所有packet给某个特定的peer
void send_pkts(int, struct sockaddr *, queue *);

//遍历询问网络中的所有节点
void flood_whohas(int, queue *);

//发送data数据包
void send_data_packets(up_conn_t *, int, struct sockaddr *);

#endif //STARTER_CODE_TRANS_H
