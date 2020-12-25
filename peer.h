#ifndef STARTER_CODE_PEER_H
#define STARTER_CODE_PEER_H

#include <sys/types.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"
#include "spiffy.h"
#include "bt_parse.h"
#include "input_buffer.h"
#include "packet.h"
#include "transfer.h"
#include "timer.h"
#include "handle_GET.h"
#include "chunk.h"

bt_config_t config;
int sock;
task_t *task;
queue *has_chunks;
up_pool_t up_pool;
down_pool_t down_pool;

void handle_whohas(data_packet_t *pkt, bt_peer_t *peer);

void handle_ihave(data_packet_t *pkt, bt_peer_t *peer);

void handle_get(data_packet_t *pkt, bt_peer_t *peer);

void handle_data(data_packet_t *pkt, bt_peer_t *peer);

void handle_ack(data_packet_t *pkt, bt_peer_t *peer);

void peer_run(bt_config_t *config);

#endif //STARTER_CODE_PEER_H
