#include "user.h"

int user_init(struct user *new_user, const char *name, int is_connected, struct message_cell *cell) {
    new_user->name = malloc(sizeof(char) * (strlen(name) + 1));
    if (new_user->name == NULL) {
        return 1;
    }

    strcpy(new_user->name, name);
    new_user->is_connected = is_connected;
    new_user->cell = cell;
    new_user->messages_sent = 0;

    return 0;
}

struct user* user_build(const char *name, int is_connected, struct message_cell *cell) {
    struct user *new_user = malloc(sizeof(struct user));
    if (new_user == NULL) {
        return NULL;
    }

    int status = user_init(new_user, name, is_connected, cell);
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

int user_connect(struct user *usr, struct message_cell *cell) {
    if (!usr->is_connected) {
        usr->is_connected = 1;
        usr->cell = cell;

        return 0;
    }
    else {
        return 1;
    }
}

int user_disconnect(struct user *usr) {
    if (usr->is_connected) {
        usr->is_connected = 0;
        usr->messages_sent = 0;

        if (usr->cell != NULL) {
            msg_cell_send_message(usr->cell, NULL); // Notify the user that the cell can be destroyed safely
            munmap(usr->cell, MSG_CELL_SIZE);
            usr->cell = NULL;
            // Client is responsible for both destroying the message cell and unlinking the shared memory object
        }

        return 0;
    }
    else {
        return 1;
    }
}

void user_destroy(struct user* usr) {
    user_disconnect(usr);
    free(usr->name);
}

void user_free(struct user* usr) {
    if (usr != NULL) {
        user_destroy(usr);
        free(usr);
    }
}
