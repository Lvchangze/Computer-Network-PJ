#ifndef STARTER_CODE_BT_PARSE_H
#define STARTER_CODE_BT_PARSE_H

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "queue.h"

#define BT_FILENAME_LEN 255
#define BT_MAX_PEERS 1024

typedef struct bt_peer_s {
    short id;
    struct sockaddr_in addr;
    struct bt_peer_s *next;
} bt_peer_t;

struct bt_config_s {
    char chunk_file[BT_FILENAME_LEN];
    char has_chunk_file[BT_FILENAME_LEN];
    char output_file[BT_FILENAME_LEN];
    char peer_list_file[BT_FILENAME_LEN];
    int max_conn;
    short identity;
    unsigned short myport;

    int argc;
    char **argv;

    bt_peer_t *peers;
};
typedef struct bt_config_s bt_config_t;


void bt_init(bt_config_t *c, int argc, char **argv);

void bt_parse_command_line(bt_config_t *c);

void bt_parse_peer_list(bt_config_t *c);

void bt_dump_config(bt_config_t *c);

bt_peer_t *bt_peer_info(const bt_config_t *c, int peer_id);

//my methods
bt_peer_t *get_peer(const bt_config_t *bt, struct sockaddr_in addr);

int check_peers(queue *peers, bt_peer_t *peer);

void add_peer(bt_peer_t *, bt_peer_t *);

void set_peer_file(char *peer_file, const char *file_name);

#endif //STARTER_CODE_BT_PARSE_H
