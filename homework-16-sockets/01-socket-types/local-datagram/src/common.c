#include "common.h"

int send_message_to(int fd, const struct sockaddr *dest_addr, socklen_t addrlen, const char *message) {
    int bytes_sent = sendto(fd, message, strlen(message), 0, dest_addr, addrlen);
    if (bytes_sent != -1) {
        printf("Было отправлено сообщение '%s'.\n", message);
    }
    else {
        fprintf(stderr, "Не удалось отправить сообщение '%s'.\n", message);
    }

    return bytes_sent;
}

int receive_message_from(int fd, struct sockaddr* restrict src_addr, socklen_t* restrict addrlen) {
    char buf[BUF_LEN];

    int bytes_read = recvfrom(fd, buf, BUF_LEN - 1, 0, src_addr, addrlen);
    if (bytes_read != -1) {
        buf[bytes_read] = '\0';
        printf("Было получено сообщение: '%s'.\n", buf);
    }
    else {
        fprintf(stderr, "Не удалось принять сообщение.\n");
    }

    return bytes_read;
}
