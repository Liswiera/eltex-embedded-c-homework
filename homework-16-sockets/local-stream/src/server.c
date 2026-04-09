#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/homework-16-local-stream"
#define LISTEN_BACKLOG 4
#define MESSAGE "Hi!"
#define BUF_LEN 32

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int listener_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    if (listener_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 1;
    }
    
    struct sockaddr_un my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sun_family = AF_LOCAL;
    strncpy(my_addr.sun_path, SOCKET_PATH, sizeof(my_addr.sun_path) - 1);
    
    int bind_status = bind(listener_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (bind_status == -1) {
        fprintf(stderr, "Не удалось привязать имя к сокету.\n");

        close(listener_fd);
        return 2;
    }

    int listen_status = listen(listener_fd, LISTEN_BACKLOG);
    if (listen_status == -1) {
        fprintf(stderr, "Не удалось сделать сокет прослушиваемым.\n");

        close(listener_fd);
        unlink(SOCKET_PATH);
        return 3;
    }

    printf("Ожидаю соединение от клиента...\n");
    
    int client_fd = accept(listener_fd, NULL, NULL);
    if (client_fd == -1) {
        fprintf(stderr, "Не удалось соединиться с клиентом.\n");

        close(listener_fd);
        unlink(SOCKET_PATH);
        return 4;
    }

    // Принимаем сообщение от клиента
    char buf[BUF_LEN];
    int bytes_read = recv(client_fd, buf, BUF_LEN - 1, 0);
    if (bytes_read != -1) {
        buf[bytes_read] = '\0';
        printf("Было получено сообщение от клиента: '%s'.\n", buf);
    }
    else {
        fprintf(stderr, "Не удалось принять сообщение от клиента.\n");
    }

    // Отправляем сообщение клиенту
    int bytes_sent = send(client_fd, MESSAGE, strlen(MESSAGE), 0);
    if (bytes_sent != -1) {
        printf("Клиенту было отправлено сообщение '%s'.\n", MESSAGE);
    }
    else {
        fprintf(stderr, "Не удалось отправить сообщение клиенту.\n");
    }

    // Cleanup
    close(client_fd);
    close(listener_fd);
    unlink(SOCKET_PATH);
    return 0;
}
