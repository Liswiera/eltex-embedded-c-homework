#include <locale.h>
#include "protocol.h"

#define MESSAGE "Hello!"
#define MESSAGE_LEN strlen(MESSAGE)
#define BUF_LEN (MSGSIZE + 1)

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int fd = shm_open(SHM_PATH, O_RDWR, 0600);
    if (fd == -1) {
        fprintf(stderr, "Не удалось открыть объект разделяемой памяти.\n");
        return 1; 
    }

    printf("Объект разделяемой памяти был открыт.\n");

    struct message_duplex *duplex = mmap(NULL, sizeof(struct message_duplex), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (duplex == MAP_FAILED) {
        fprintf(stderr, "Не удалось отобразить структуру коммуникации между клиентом и сервером в разделяемую память.\n");

        close(fd);
        return 2;
    }

    // Передача сообщений между клиентом и сервером
    char buf[BUF_LEN];
    msg_cell_send_string(&duplex->server, MESSAGE, MESSAGE_LEN);
    msg_cell_recv_string(&duplex->client, buf, BUF_LEN);

    // Cleanup
    munmap(duplex, sizeof(struct message_duplex));
    close(fd);
    return 0;
}
