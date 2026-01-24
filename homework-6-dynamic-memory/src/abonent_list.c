#include "abonent_list.h"

struct abonent_list_node* abonent_list_node_build(const char *name, const char *second_name, const char *tel) {
    struct abonent_list_node *result = malloc(sizeof(struct abonent_list_node));
    if (result != NULL) {
        abonent_create_from(&result->data, name, second_name, tel);
        result->prev = result->next = NULL;
    }

    return result;
}

void abonent_list_node_free(struct abonent_list_node *node) {
    free(node);
}

struct abonent_list* abonent_list_create_empty() {
    struct abonent_list* result = malloc(sizeof(struct abonent_list));
    if (result != NULL) {
        result->head = result->tail = NULL;
        result->size = 0;
    }

    return result;
}

void abonent_list_free(struct abonent_list *list) {
    struct abonent_list_node *iter = list->head;
    while (iter != NULL) {
        struct abonent_list_node *next_node = iter->next;
        free(iter);

        iter = next_node;
    }

    free(list);
}

struct abonent_list_node* abonent_list_add(struct abonent_list *list, const char *name, const char *second_name, const char *tel) {
    struct abonent_list_node *new_node = abonent_list_node_build(name, second_name, tel);
    if (new_node == NULL) {
        return NULL;
    }

    if (list->size > 0) {
        list->tail->next = new_node;
        new_node->prev = list->tail;
    }
    else {
        list->head = new_node;
    }

    list->tail = new_node;
    list->size++;

    return new_node;
}

struct abonent_list_node* abonent_list_get_node_at(struct abonent_list *list, size_t id) {
    if (id < list->size) {
        struct abonent_list_node *node;
        size_t cur_id;

        if (id < list->size - id) {
            node = list->head;
            cur_id = 0;

            while (cur_id < id) {
                node = node->next;
                cur_id++;
            }
        }
        else {
            node = list->tail;
            cur_id = list->size - 1;

            while (cur_id > id) {
                node = node->prev;
                cur_id--;
            }
        }

        return node;
    }
    else {
        return NULL;
    }
}

struct abonent_list_node* abonent_list_extract_node_at(struct abonent_list *list, size_t id) {
    struct abonent_list_node *node = abonent_list_get_node_at(list, id);
    if (node != NULL) {
        if (list->head == node) {
            list->head = node->next;
        }
        else {
            node->prev->next = node->next;
        }

        if (list->tail == node) {
            list->tail = node->prev;
        }
        else {
            node->next->prev = node->prev;
        }

        list->size--;
    }
    
    return node;
}

uint32_t abonent_list_remove_at(struct abonent_list *list, size_t id) {
    struct abonent_list_node *node = abonent_list_extract_node_at(list, id);
    if (node != NULL) {
        abonent_list_node_free(node);
        return 1;
    }
    else {
        return 0;
    }
}

void abonent_list_search_by_name_and_print(const struct abonent_list *list, const char *name) {
    const struct abonent_list_node *iter = list->head;
    size_t id = 0;
    
    printf("Поиск абонентов с именем \"%s\":\n", name);
    while (iter != NULL) {
        if (strcmp(iter->data.name, name) == 0) {
            printf("[ID#%zu] ", id + 1);
            abonent_print(&iter->data);
        }

        iter = iter->next;
        id++;
    }
    printf("\n");
}

void abonent_list_print_all(const struct abonent_list *list) {
    const struct abonent_list_node *iter = list->head;
    size_t id = 0;
    
    printf("Список абонентов [size = %zu]\n", list->size);
    while (iter != NULL) {
        printf("[ID#%zu] ", id + 1);
        abonent_print(&iter->data);

        iter = iter->next;
        id++;
    }
    printf("\n");
}
