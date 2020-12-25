#ifndef STARTER_CODE_TASK_H
#define STARTER_CODE_TASK_H

#include <stdint.h>
#include "sha.h"
#include "bt_parse.h"
#include "queue.h"
#include "priority_queue.h"

#define WINDOW_SIZE 8
#define CHUNK_SIZE 512

typedef struct chunk_s {
    int id;
    uint8_t sha_1[SHA1_HASH_SIZE];
    int flag;
    int inuse;
    int num;
    char *data;
    queue *peers;
} chunk_t;

typedef struct down_conn_s {
    uint next_ack;
    chunk_t *chunk;
    int pos;
    struct timeval timer;
    bt_peer_t *sender;
} down_conn_t;

typedef struct up_conn_s {
    int last_ack;
    int last_sent;
    int available;
    int duplicate; //三个冗余ACK，就重传
    int rwnd; //流量控制窗口
    data_packet_t **pkts;
    struct timeval timer;
    bt_peer_t *receiver;
} up_conn_t;

typedef struct down_pool_s {
    down_conn_t **conns;
    int conn;
    int max_conn;
} down_pool_t;

typedef struct up_pool_s {
    up_conn_t **conns;//存有conn连接信息的指针数组
    int conn;//当前已经建立的连接数
    int max_conn;
} up_pool_t;

typedef struct task_s {
    int chunk_num;
    int need_num;
    int max_conn;
    queue *chunks;
    char output_file[BT_FILENAME_LEN];
    struct timeval timer;
} task_t;

//chunk函数
chunk_t *make_chunk(int, const uint8_t *);

void free_chunk(chunk_t *chunk);

//task函数
task_t *init_task(const char *, const char *, int);

int check_task(task_t *task);

void continue_task(task_t *task, down_pool_t *pool, int sock); // 继续task中的下载
task_t *finish_task(task_t *task);

//up_conn
void init_up_pool(up_pool_t *pool, int max_conn);

up_conn_t *get_up_conn(up_pool_t *, bt_peer_t *);

up_conn_t *add_to_up_pool(up_pool_t *pool, bt_peer_t *peer, data_packet_t **pkts);

void remove_from_up_pool(up_pool_t *pool, bt_peer_t *peer);

up_conn_t *create_up_conn(bt_peer_t *peer, data_packet_t **pkts);

//down_conn
void init_down_pool(down_pool_t *pool, int max_conn);

down_conn_t *get_down_conn(down_pool_t *, bt_peer_t *);

down_conn_t *add_to_down_pool(down_pool_t *pool, bt_peer_t *peer, chunk_t *chunk);

void remove_from_down_pool(down_pool_t *pool, bt_peer_t *peer);

down_conn_t *create_down_conn(bt_peer_t *peer, chunk_t *chunk);

//优先队列，寻找最少的块
char *find_chunk_data(task_t *task, uint8_t *sha_1);

void available_peer(task_t *task, uint8_t *sha_1, bt_peer_t *peer);

chunk_t *choose_chunk(task_t *task, queue *chunks, bt_peer_t *peer);

void print_data(char *data, int size);

int remove_stalled_chunks(down_pool_t *pool);

void remove_unack_peers(up_pool_t *pool, int sock);

#endif //STARTER_CODE_TASK_H
