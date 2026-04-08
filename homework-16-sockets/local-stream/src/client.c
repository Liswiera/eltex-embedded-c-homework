#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/homework-16-local-stream"
#define MESSAGE "Hello!"
#define BUF_LEN 32

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int server_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (server_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 1;
    }

    struct sockaddr_un my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sun_family = AF_LOCAL;
    strncpy(my_addr.sun_path, SOCKET_PATH, sizeof(my_addr.sun_path) - 1);

    int connect_status = connect(server_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (connect_status == -1) {
        fprintf(stderr, "Не удалось подключиться к серверу.\n");

        close(server_fd);
        return 2;
    }

    // Отправляем сообщение серверу
    int bytes_sent = send(server_fd, MESSAGE, strlen(MESSAGE), 0);
    if (bytes_sent != -1) {
        printf("Серверу было отправлено сообщение '%s'.\n", MESSAGE);
    }
    else {
        fprintf(stderr, "Не удалось отправить сообщение серверу.\n");
    }

    // Принимаем сообщение от сервера
    char buf[BUF_LEN];
    int bytes_read = recv(server_fd, buf, BUF_LEN - 1, 0);
    if (bytes_read != -1) {
        buf[bytes_read] = '\0';
        printf("Было получено сообщение от сервера: '%s'.\n", buf);
    }
    else {
        fprintf(stderr, "Не удалось принять сообщение от сервера.\n");
    }

    // Cleanup
    close(server_fd);
    return 0;
}
