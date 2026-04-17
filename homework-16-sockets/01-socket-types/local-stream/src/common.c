#include "common.h"

int send_message(int fd, const char *message) {
    int bytes_sent = send(fd, message, strlen(message), 0);
    if (bytes_sent != -1) {
        printf("Было отправлено сообщение '%s'.\n", message);
    }
    else {
        fprintf(stderr, "Не удалось отправить сообщение '%s'.\n", message);
    }

    return bytes_sent;
}

int receive_message(int fd) {
    char buf[BUF_LEN];

    int bytes_read = recv(fd, buf, BUF_LEN - 1, 0);
    if (bytes_read != -1) {
        buf[bytes_read] = '\0';
        printf("Было получено сообщение: '%s'.\n", buf);
    }
    else {
        fprintf(stderr, "Не удалось принять сообщение.\n");
    }

    return bytes_read;
}
