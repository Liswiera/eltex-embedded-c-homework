#include <locale.h>
#include "protocol.h"

#define MESSAGE "Hello!"
#define MESSAGE_LEN strlen(MESSAGE)
#define BUF_LEN (MSGSIZE + 1)

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int shm_id = shmget(SHM_KEY, MSG_DUPLEX_SIZE, 0);
    if (shm_id == -1) {
        fprintf(stderr, "Не удалось открыть объект разделяемой памяти.\n");
        return 1; 
    }

    int sem_id = semget(SEM_KEY, 4, 0);
    if (sem_id == -1) {
        fprintf(stderr, "Не удалось открыть семафор.\n");
        return 2;
    }

    printf("Объект разделяемой памяти был открыт.\n");

    struct message_duplex *duplex = shmat(shm_id, NULL, 0);
    if (duplex == (void*)-1) {
        fprintf(stderr, "Не удалось отобразить структуру коммуникации между клиентом и сервером в разделяемую память.\n");
        return 3;
    }

    // Передача сообщений между клиентом и сервером
    char buf[BUF_LEN];
    msg_duplex_server_send(duplex, MESSAGE, MESSAGE_LEN, sem_id, SERVER_WRITE_SEM_NUM, SERVER_READ_SEM_NUM);
    msg_duplex_client_recv(duplex, buf, BUF_LEN, sem_id, CLIENT_WRITE_SEM_NUM, CLIENT_READ_SEM_NUM);

    return 0;
}
