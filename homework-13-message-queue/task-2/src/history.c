#include "history.h"

struct history* history_with_capacity(size_t capacity) {
    struct history *hist = malloc(sizeof(struct history));
    if (hist == NULL) {
        return NULL;
    }

    hist->msgs = malloc(sizeof(struct message) * capacity);
    if (hist->msgs == NULL) {
        history_destroy(hist);
        return NULL;
    }

    hist->capacity = capacity;
    hist->msg_count = 0;
    
    return hist;
}

int history_add_message(struct history *hist, const struct message *msg) {
    if (hist->msg_count < hist->capacity) {
        size_t id = hist->msg_count;
        memcpy(&hist->msgs[id], msg, sizeof(struct message));

        hist->msg_count++;
        return 1;
    }

    return 0;
}

struct message* history_get_message(struct history *hist, size_t id) {
    return (id < hist->msg_count) ? &hist->msgs[id] : NULL;
}

void history_destroy(struct history *hist) {
    if (hist != NULL) {
        free(hist->msgs);
        free(hist);
    }
}
