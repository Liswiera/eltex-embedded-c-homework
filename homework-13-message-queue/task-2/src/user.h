#pragma once
#include <inttypes.h>
#include <malloc.h>
#include <mqueue.h>
#include <string.h>
#include <unistd.h>

struct user {
    char *name;
    int is_connected;
    mqd_t queue_id;
    size_t messages_sent;
};

int user_init(struct user *new_user, const char *name, int is_connected, mqd_t queue_id);
struct user* user_build(const char *name, int is_connected, mqd_t queue_id);
struct user* user_find_by_name(struct user *users, size_t user_count, const char *name);
void user_connect(struct user *usr, mqd_t queue_id);
void user_disconnect(struct user *usr);
void user_destroy(struct user* usr);
void user_free(struct user* usr);
