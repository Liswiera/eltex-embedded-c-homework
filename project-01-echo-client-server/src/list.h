#pragma once
#ifndef _PROJ_01_LIST_H_
#define _PROJ_01_LIST_H_

#include <inttypes.h>
#include <malloc.h>
#include <netinet/in.h>
#include <unistd.h>

struct client_list {
    size_t len;
    struct client_node *head;
    struct client_node *tail;
};

struct client_node {
    in_addr_t addr; // Big-endian
    uint16_t port;  // Big-endian
    size_t ping_count;

    struct client_node *next;
    struct client_node *prev;
};

void client_list_init(struct client_list *list);
void client_list_free(struct client_list *list);
struct client_node* client_list_push_back(struct client_list *list, in_addr_t addr, uint16_t port);
struct client_node* client_list_find(struct client_list *list, in_addr_t addr, uint16_t port);
struct client_node* client_list_reset_counter(struct client_list *list, in_addr_t addr, uint16_t port);
struct client_node* client_list_add_ping(struct client_list *list, in_addr_t addr, uint16_t port);
int client_list_remove(struct client_list *list, in_addr_t addr, uint16_t port);
void client_list_clear(struct client_list *list);

void client_node_init(struct client_node *node, in_addr_t addr, uint16_t port);
void client_node_free(struct client_node *node);

#endif
