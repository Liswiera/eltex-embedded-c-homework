#pragma once
#include "message_cell.h"

struct user {
    char *name;
    int is_connected;
    
    struct message_cell *cell;
    size_t messages_sent;
};

int user_init(struct user *new_user, const char *name, int is_connected, struct message_cell *cell);
struct user* user_build(const char *name, int is_connected, struct message_cell *cell);
struct user* user_find_by_name(struct user *users, size_t user_count, const char *name);
int user_connect(struct user *usr, struct message_cell *cell);
int user_disconnect(struct user *usr);
void user_destroy(struct user* usr);
void user_free(struct user* usr);
