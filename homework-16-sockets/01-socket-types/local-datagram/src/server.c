#include <locale.h>
#include "common.h"

#define LISTEN_BACKLOG 4
#define MESSAGE "Hi!"

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int socket_fd = socket(AF_LOCAL, SOCK_DGRAM, 0);
    if (socket_fd == -1) {
        fprintf(stderr, "Не удалось создать сокет.\n");
        return 1;
    }
    
    struct sockaddr_un my_addr;
    memset(&my_addr, 0, sizeof(my_addr));
    my_addr.sun_family = AF_LOCAL;
    strncpy(my_addr.sun_path, SERVER_SOCKET_PATH, sizeof(my_addr.sun_path) - 1);
    
    int bind_status = bind(socket_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (bind_status == -1) {
        fprintf(stderr, "Не удалось привязать имя к сокету.\n");

        close(socket_fd);
        return 2;
    }

    printf("Ожидаю сообщение от клиента...\n");

    struct sockaddr_un client_addr;
    socklen_t client_addr_len = sizeof(client_addr);

    receive_message_from(socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);
    send_message_to(socket_fd, (struct sockaddr*)&client_addr, client_addr_len, MESSAGE);

    // Cleanup
    close(socket_fd);
    unlink(SERVER_SOCKET_PATH);
    return 0;
}
