#include "message_cell.h"

void msg_cell_init(struct message_cell *cell) {
    sem_init(&cell->read_sem, 1, 0); // Return value is ignored
    sem_init(&cell->write_sem, 1, 1); // Return value is ignored
}

void msg_cell_destroy(struct message_cell *cell) {
    sem_destroy(&cell->read_sem);
    sem_destroy(&cell->write_sem);
}

void msg_cell_send_string(struct message_cell *cell, const char *str_ptr, size_t str_len) {
    sem_wait(&cell->write_sem);

    strncpy(cell->msg, str_ptr, MSGSIZE);
    cell->msg[MSGSIZE - 1] = '\0';
    printf("Строка '%s' была записана в разделяемую память.\n", str_ptr);

    sem_post(&cell->read_sem);
}

void msg_cell_recv_string(struct message_cell *cell, char *buf_ptr, size_t buf_len) {
    sem_wait(&cell->read_sem);

    strncpy(buf_ptr, cell->msg, buf_len);
    buf_ptr[buf_len - 1] = '\0';
    printf("Была получена строка '%s'.\n", buf_ptr);

    sem_post(&cell->write_sem);
}
