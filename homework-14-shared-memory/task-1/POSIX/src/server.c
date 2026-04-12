#include <locale.h>
#include "protocol.h"

#define MESSAGE "Hi!"
#define MESSAGE_LEN strlen(MESSAGE)
#define BUF_LEN (SHM_MSGSIZE + 1)

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int fd = shm_open(SHM_PATH, O_CREAT | O_RDWR, 0600);
    if (fd == -1) {
        fprintf(stderr, "Не удалось создать объект разделяемой памяти.\n");
        return 1; 
    }

    printf("Объект разделяемой памяти был создан.\n");

    int trunc_status = ftruncate(fd, MSG_CELL_SIZE * 2);
    if (trunc_status == -1) {
        fprintf(stderr, "Не удалось установить размер разделяемой памяти.\n");

        close(fd);
        shm_unlink(SHM_PATH);
        return 2;
    }

    struct message_cell *cells = mmap(NULL, MSG_CELL_SIZE * 2, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (cells == MAP_FAILED) {
        fprintf(stderr, "Не удалось отобразить структуру сообщения в разделяемую память.\n");

        close(fd);
        shm_unlink(SHM_PATH);
        return 3;
    }

    message_cell_init(&cells[SERVER_ID]);
    message_cell_init(&cells[CLIENT_ID]);

    printf("Ожидаю сообщение от клиента...\n");

    // Передача сообщений между клиентом и сервером
    char buf[BUF_LEN];
    receive_string(&cells[SERVER_ID], buf, BUF_LEN);
    send_string(&cells[CLIENT_ID], MESSAGE, MESSAGE_LEN);

    // Cleanup
    munmap(cells, MSG_CELL_SIZE * 2);
    close(fd);
    shm_unlink(SHM_PATH);
    return 0;
}
