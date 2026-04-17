#include <locale.h>
#include "common.h"

#define MESSAGE "Hello!"

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

    send_message(server_fd, MESSAGE);
    receive_message(server_fd);

    // Cleanup
    close(server_fd);
    return 0;
}
