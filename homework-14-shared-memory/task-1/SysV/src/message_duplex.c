#include "message_duplex.h"

static void send_string(char *msg_buf, const char *str_ptr, size_t str_len, int sem_id, int sem_write_num, int sem_read_num) {
    struct sembuf sop;
    sop.sem_flg = 0;

    sop.sem_num = sem_write_num;
    sop.sem_op = -1;
    semop(sem_id, &sop, 1);

    if (str_ptr != NULL) {
        strncpy(msg_buf, str_ptr, MSGSIZE);
        msg_buf[MSGSIZE - 1] = '\0';
        printf("Строка '%s' была записана в разделяемую память.\n", str_ptr);
    }

    sop.sem_num = sem_read_num;
    sop.sem_op = 1;
    semop(sem_id, &sop, 1);
}

void msg_duplex_server_send(struct message_duplex *duplex, const char *str_ptr, size_t str_len, int sem_id, int sem_write_num, int sem_read_num) {
    send_string(duplex->server, str_ptr, str_len, sem_id, sem_write_num, sem_read_num);
}

void msg_duplex_client_send(struct message_duplex *duplex, const char *str_ptr, size_t str_len, int sem_id, int sem_write_num, int sem_read_num) {
    send_string(duplex->client, str_ptr, str_len, sem_id, sem_write_num, sem_read_num);
}

static void recv_string(const char* msg_buf, char *output_buf_ptr, size_t buf_len, int sem_id, int sem_write_num, int sem_read_num) {
    struct sembuf sop;
    sop.sem_flg = 0;

    sop.sem_num = sem_read_num;
    sop.sem_op = -1;
    semop(sem_id, &sop, 1);
    
    if (output_buf_ptr != NULL) {
        strncpy(output_buf_ptr, msg_buf, buf_len);
        output_buf_ptr[buf_len - 1] = '\0';
        printf("Была получена строка '%s'.\n", output_buf_ptr);
    }

    sop.sem_num = sem_write_num;
    sop.sem_op = 1;
    semop(sem_id, &sop, 1);
}

void msg_duplex_server_recv(struct message_duplex *duplex, char *buf_ptr, size_t buf_len, int sem_id, int sem_write_num, int sem_read_num) {
    recv_string(duplex->server, buf_ptr, buf_len, sem_id, sem_write_num, sem_read_num);
}

void msg_duplex_client_recv(struct message_duplex *duplex, char *buf_ptr, size_t buf_len, int sem_id, int sem_write_num, int sem_read_num) {
    recv_string(duplex->client, buf_ptr, buf_len, sem_id, sem_write_num, sem_read_num);
}
