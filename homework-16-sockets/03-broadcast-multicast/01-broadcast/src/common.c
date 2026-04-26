#include "common.h"

int is_number(const char *str) {
    for (; *str != '\0'; str++) {
        if (!isdigit(*str)) {
            return 0;
        }
    }

    return 1;
}

int port_str_to_val(const char *port_str, uint16_t *port_out) {
    if (!is_number(port_str)) {
        return 1;
    }

    errno = 0;
    long port = strtol(port_str, NULL, 10);
    if ((port < 0) || (port > __UINT16_MAX__) || (errno == ERANGE)) {
        return 2;
    }

    *port_out = (uint16_t)port;
    return 0;
}

ssize_t send_message(int fd, const char *message) {
    ssize_t bytes_sent = send(fd, message, strlen(message), 0);
    if (bytes_sent != -1) {
        printf("Было отправлено сообщение '%s' (TCP).\n", message);
    }
    else {
        fprintf(stderr, "Не удалось отправить сообщение '%s'.\n", message);
    }

    return bytes_sent;
}

ssize_t receive_message(int fd) {
    char buf[BUF_LEN];

    ssize_t bytes_read = recv(fd, buf, BUF_LEN - 1, 0);
    if (bytes_read != -1) {
        buf[bytes_read] = '\0';
        printf("Было получено сообщение: '%s' (TCP).\n", buf);
    }
    else {
        fprintf(stderr, "Не удалось принять сообщение.\n");
    }

    return bytes_read;
}

ssize_t send_message_to(int fd, const char *message, const struct sockaddr_in *addr, socklen_t addr_len) {
    ssize_t bytes_sent = sendto(fd, message, strlen(message), 0, (struct sockaddr*)addr, addr_len);
    if (bytes_sent != -1) {
        printf("Было отправлено сообщение '%s' (UDP).\n", message);
    }
    else {
        fprintf(stderr, "Не удалось отправить сообщение '%s'.\n", message);
    }

    return bytes_sent;
}

ssize_t receive_message_from(int fd, struct sockaddr_in *addr, socklen_t *addr_len) {
    char buf[BUF_LEN];

    ssize_t bytes_read = recvfrom(fd, buf, BUF_LEN - 1, 0, (struct sockaddr*)addr, addr_len);
    if (bytes_read != -1) {
        buf[bytes_read] = '\0';
        printf("Было получено сообщение: '%s' (UDP).\n", buf);
    }
    else {
        fprintf(stderr, "Не удалось принять сообщение.\n");
    }

    return bytes_read;
}
