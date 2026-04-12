#include <locale.h>
#include "protocol.h"

#define MESSAGE "Hello!"
#define MESSAGE_LEN strlen(MESSAGE)
#define BUF_LEN (SHM_MSGSIZE + 1)


int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int fd = shm_open(SHM_PATH, O_RDWR, 0600);
    if (fd == -1) {
        fprintf(stderr, "Не удалось открыть объект разделяемой памяти.\n");
        return 1; 
    }

    printf("Объект разделяемой памяти был открыт.\n");

    struct message_cell *cells = mmap(NULL, MSG_CELL_SIZE * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cells == MAP_FAILED) {
        fprintf(stderr, "Не удалось отобразить структуру сообщения в разделяемую память.\n");

        close(fd);
        return 3;
    }

    // Передача сообщений между клиентом и сервером
    char buf[BUF_LEN];
    send_string(&cells[SERVER_ID], MESSAGE, MESSAGE_LEN);
    receive_string(&cells[CLIENT_ID], buf, BUF_LEN);

    // Cleanup
    munmap(cells, MSG_CELL_SIZE * 2);
    close(fd);
    return 0;
}
