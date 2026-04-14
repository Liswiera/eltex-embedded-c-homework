#include "message_cell.h"

void message_set(struct message *msg, enum msg_type type, const char *user_name, const char* text) {
    msg->type = type;
    strcpy(msg->user_name, user_name);
    strcpy(msg->text, text);
}


void msg_cell_init(struct message_cell *cell) {
    sem_init(&cell->read_sem, 1, 0); // Return value is ignored
    sem_init(&cell->write_sem, 1, 1); // Return value is ignored
}

void msg_cell_destroy(struct message_cell *cell) {
    sem_destroy(&cell->read_sem);
    sem_destroy(&cell->write_sem);
}

void msg_cell_send_message(struct message_cell *cell, const struct message *msg) {
    sem_wait(&cell->write_sem);
    if (msg != NULL) {
        memcpy(&cell->msg, msg, sizeof(struct message));
    }
    sem_post(&cell->read_sem);
}

void msg_cell_recv_message(struct message_cell *cell, struct message *msg) {
    sem_wait(&cell->read_sem);
    if (msg != NULL) {
        memcpy(msg, &cell->msg, sizeof(struct message));
    }
    sem_post(&cell->write_sem);
}
