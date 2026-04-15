#include <locale.h>
#include "protocol.h"

#define MESSAGE "Hi!"
#define MESSAGE_LEN strlen(MESSAGE)
#define BUF_LEN (MSGSIZE + 1)

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int shm_id = shmget(SHM_KEY, MSG_DUPLEX_SIZE, IPC_CREAT | 0600);
    if (shm_id == -1) {
        fprintf(stderr, "Не удалось открыть объект разделяемой памяти.\n");
        return 1;
    }

    // [server_write, server_read, client_write, client_read]
    int sem_id = semget(SEM_KEY, 4, IPC_CREAT | 0600);
    if (sem_id == -1) {
        fprintf(stderr, "Не удалось создать семафор.\n");

        shmctl(shm_id, IPC_RMID, NULL);
        return 2;
    }

    struct message_duplex *duplex = shmat(shm_id, NULL, 0);
    if (duplex == (void*)-1) {
        fprintf(stderr, "Не удалось отобразить структуру коммуникации между клиентом и сервером в разделяемую память.\n");

        semctl(sem_id, 0, IPC_RMID);
        shmctl(shm_id, IPC_RMID, NULL);
        return 3;
    }

    // Инициализация семафора
    unsigned short sem_init_arr[4];
    sem_init_arr[SERVER_WRITE_SEM_NUM] = sem_init_arr[CLIENT_WRITE_SEM_NUM] = 1;
    sem_init_arr[SERVER_READ_SEM_NUM] = sem_init_arr[CLIENT_READ_SEM_NUM] = 0;

    union semun arg;
    arg.array = sem_init_arr;
    semctl(sem_id, 0, SETALL, arg);

    printf("Ожидаю сообщение от клиента...\n");

    // Передача сообщений между клиентом и сервером
    char buf[BUF_LEN];
    msg_duplex_server_recv(duplex, buf, BUF_LEN, sem_id, SERVER_WRITE_SEM_NUM, SERVER_READ_SEM_NUM);
    msg_duplex_client_send(duplex, MESSAGE, MESSAGE_LEN, sem_id, CLIENT_WRITE_SEM_NUM, CLIENT_READ_SEM_NUM);

    // Отправляем пустое сообщение для синхронизации с клиентом
    msg_duplex_client_send(duplex, NULL, 0, sem_id, CLIENT_WRITE_SEM_NUM, CLIENT_READ_SEM_NUM);

    // Cleanup
    semctl(sem_id, 0, IPC_RMID);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}
