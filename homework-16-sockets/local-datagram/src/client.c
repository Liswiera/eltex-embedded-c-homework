#include <locale.h>
#include "common.h"

#define MESSAGE "Hello!"

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
    strncpy(my_addr.sun_path, CLIENT_SOCKET_PATH, sizeof(my_addr.sun_path) - 1);
    
    int bind_status = bind(socket_fd, (struct sockaddr*)&my_addr, sizeof(my_addr));
    if (bind_status == -1) {
        fprintf(stderr, "Не удалось привязать имя к сокету.\n");

        close(socket_fd);
        return 2;
    }

    struct sockaddr_un server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sun_family = AF_LOCAL;
    strncpy(server_addr.sun_path, SERVER_SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

    send_message_to(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr), MESSAGE);
    receive_message_from(socket_fd, NULL, NULL);

    // Cleanup
    close(socket_fd);
    unlink(CLIENT_SOCKET_PATH);
    return 0;
}
