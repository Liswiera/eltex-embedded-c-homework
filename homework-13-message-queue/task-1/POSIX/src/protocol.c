#include "protocol.h"

int send_string(mqd_t mq, const char *str_ptr, size_t str_len, unsigned int prio) {
    int status = mq_send(mq, str_ptr, str_len, prio);

    if (status == -1) {
        fprintf(stderr, "Не удалось передать строку через очередь сообщений.\n");
    } else {
        printf("Строка '%s' была успешно записана в очередь.\n", str_ptr);
    }

    return status;
}

ssize_t receive_string(mqd_t mq, char *buf_ptr, size_t buf_len, unsigned int *prio) {
    ssize_t bytes_read = mq_receive(mq, buf_ptr, buf_len - 1, prio);

    if (bytes_read == -1) {
        fprintf(stderr, "Не удалось прочитать строку из очереди.\n");
    }
    else {
        buf_ptr[bytes_read] = '\0';
        printf("Была получена строка '%s'.\n", buf_ptr);
    }
    
    return bytes_read;
}
