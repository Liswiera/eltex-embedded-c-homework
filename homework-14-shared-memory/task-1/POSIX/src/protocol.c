#include "protocol.h"

void message_cell_init(struct message_cell *cell) {
    sem_init(&cell->read_sem, 1, 0); // Return value is ignored
    sem_init(&cell->write_sem, 1, 1); // Return value is ignored
}

void send_string(struct message_cell *cell, const char *str_ptr, size_t str_len) {
    sem_wait(&cell->write_sem);

    strncpy(cell->msg, str_ptr, SHM_MSGSIZE);
    cell->msg[SHM_MSGSIZE - 1] = '\0';
    printf("Строка '%s' была записана в разделяемую память.\n", str_ptr);

    sem_post(&cell->read_sem);
}

void receive_string(struct message_cell *cell, char *buf_ptr, size_t buf_len) {
    sem_wait(&cell->read_sem);

    strncpy(buf_ptr, cell->msg, buf_len);
    buf_ptr[buf_len - 1] = '\0';
    printf("Была получена строка '%s'.\n", buf_ptr);

    sem_post(&cell->write_sem);
}
