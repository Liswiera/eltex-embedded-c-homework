#include "list.h"

void client_list_init(struct client_list *list) {
    list->len = 0;
    list->head = NULL;
    list->tail = NULL;
}

void client_list_free(struct client_list *list) {
    if (list != NULL) {
        client_list_clear(list);
        free(list);
    }
}

struct client_node* client_list_push_back(struct client_list *list, in_addr_t addr, uint16_t port) {
    struct client_node *new_node = malloc(sizeof(struct client_node));
    if (new_node == NULL) {
        return NULL;
    }

    client_node_init(new_node, addr, port);

    if (list->len > 0) {
        struct client_node *tail_old = list->tail;
        list->tail = new_node;

        tail_old->next = new_node;
        new_node->prev = tail_old;
    }
    else {
        list->head = new_node;
        list->tail = new_node;
    }

    list->len++;
    return new_node;
}

struct client_node* client_list_find(struct client_list *list, in_addr_t addr, uint16_t port) {
    for (struct client_node *node = list->head; node != NULL; node = node->next) {
        if ((node->addr == addr) && (node->port == port)) {
            return node;
        }
    }

    return NULL;
}

struct client_node* client_list_reset_counter(struct client_list *list, in_addr_t addr, uint16_t port) {
    struct client_node *node = client_list_find(list, addr, port);
    if (node != NULL) {
        node->ping_count = 0;
    }

    return node;
}

struct client_node* client_list_add_ping(struct client_list *list, in_addr_t addr, uint16_t port) {
    struct client_node *node = client_list_find(list, addr, port);
    if (node == NULL) {
        node = client_list_push_back(list, addr, port);
    }

    if (node != NULL) {
        node->ping_count++;
    }

    return node;
}

int client_list_remove(struct client_list *list, in_addr_t addr, uint16_t port) {
    struct client_node *node = client_list_find(list, addr, port);

    if (node != NULL) {
        if (node == list->head) {
            list->head = node->next;
        }
        else {
            node->prev->next = node->next;
        }

        if (node == list->tail) {
            list->tail = node->prev;
        }
        else {
            node->next->prev = node->prev;
        }

        client_node_free(node);
        list->len--;
        return 1;
    }
    else {
        return 0;
    }
}

void client_list_clear(struct client_list *list) {
    struct client_node *node = list->head;
    while (node != NULL) {
        struct client_node *next_node = node->next;

        client_node_free(node);
        node = next_node;
    }

    client_list_init(list);
}


void client_node_init(struct client_node *node, in_addr_t addr, uint16_t port) {
    node->addr = addr;
    node->port = port;
    node->ping_count = 0;
    node->next = NULL;
    node->prev = NULL;
}

void client_node_free(struct client_node *node) {
    free(node);
}
