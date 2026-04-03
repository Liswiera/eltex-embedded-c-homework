#include "user.h"

int user_init(struct user *new_user, const char *name, int is_connected, mqd_t queue_id) {
    new_user->name = malloc(sizeof(char) * (strlen(name) + 1));
    if (new_user->name == NULL) {
        return 1;
    }

    strcpy(new_user->name, name);
    new_user->is_connected = is_connected;
    new_user->queue_id = queue_id;
    new_user->messages_sent = 0;

    return 0;
}

struct user* user_build(const char *name, int is_connected, mqd_t queue_id) {
    struct user *new_user = malloc(sizeof(struct user));
    if (new_user == NULL) {
        return NULL;
    }

    int status = user_init(new_user, name, is_connected, queue_id);
    if (status != 0) {
        user_destroy(new_user);
        return NULL;
    }

    return new_user;
}

struct user* user_find_by_name(struct user *users, size_t user_count, const char *name) {
    for (size_t i = 0; i < user_count; i++) {
        struct user *usr = &users[i];
        if (strcmp(usr->name, name) == 0) {
            return usr;
        }
    }

    return NULL;
}

void user_connect(struct user *usr, mqd_t queue_id) {
    usr->is_connected = 1;
    usr->queue_id = queue_id;
}

void user_disconnect(struct user *usr) {
    usr->is_connected = 0;
    usr->messages_sent = 0;
}

void user_destroy(struct user* usr) {
    mq_close(usr->queue_id);

    free(usr->name);
}

void user_free(struct user* usr) {
    if (usr != NULL) {
        user_destroy(usr);
        free(usr);
    }
}
