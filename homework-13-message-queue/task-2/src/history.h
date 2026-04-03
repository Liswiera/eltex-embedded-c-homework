#pragma once
#include "message_types.h"

struct history {
    struct message *msgs;
    size_t capacity;
    size_t msg_count;
};

struct history* history_with_capacity(size_t capacity);
int history_add_message(struct history *hist, const struct message *msg);
struct message* history_get_message(struct history *hist, size_t id);
void history_destroy(struct history *hist);
