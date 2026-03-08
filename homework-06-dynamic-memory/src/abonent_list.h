#pragma once
#include <malloc.h>

#include "abonent.h"

struct abonent_list_node {
    struct abonent data;
    struct abonent_list_node *prev, *next;
};

struct abonent_list_node* abonent_list_node_build(const char *name, const char *second_name, const char *tel);
void abonent_list_node_free(struct abonent_list_node *node);

struct abonent_list {
    struct abonent_list_node *head, *tail;
    size_t size;
};

struct abonent_list* abonent_list_create_empty();
void abonent_list_free(struct abonent_list *list);
struct abonent_list_node* abonent_list_add(struct abonent_list *list, const char *name, const char *second_name, const char *tel);
struct abonent_list_node* abonent_list_get_node_at(struct abonent_list *list, size_t id);
struct abonent_list_node* abonent_list_extract_node_at(struct abonent_list *list, size_t id);
uint32_t abonent_list_remove_at(struct abonent_list *list, size_t id);
void abonent_list_search_by_name_and_print(const struct abonent_list *list, const char *name);
void abonent_list_print_all(const struct abonent_list *list);
