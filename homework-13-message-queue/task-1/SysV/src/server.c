#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

#define MESSAGE "Hi!"
#define MESSAGE_LEN strlen(MESSAGE)


int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    
    key_t key = ftok(FTOK_PATH, FTOK_PROJ_ID);
    if (key == -1) {
        fprintf(stderr, "Не удалось получить ключ для System V IPC (файл '%s').\n", FTOK_PATH);
        return 5;
    }

    int queue_id = msgget(key, IPC_CREAT | S_IRUSR | S_IWUSR);
    if (queue_id == -1) {
        fprintf(stderr, "Не удалось создать очередь сообщений.\n");
        return 1; 
    }

    printf("Очередь сообщений была создана.\n");

    struct message msg;
    
    // Сервер сначала отправляет сообщение клиенту через очередь
    msg.mtype = MSG_TYPE_SERVER;
    strncpy(msg.mtext, MESSAGE, MESSAGE_LEN);
    msg.mtext[MESSAGE_LEN] = '\0';

    int status = send_string(queue_id, &msg, MESSAGE_LEN, 0);
    if (status == -1) {
        msgctl(queue_id, IPC_RMID, NULL);
        return 2;
    }

    // Затем сервер получает сообщение от клиента через очередь
    ssize_t bytes_read = receive_string(queue_id, &msg, BUF_LEN, MSG_TYPE_CLIENT, MSG_NOERROR);
    if (bytes_read == -1) {
        msgctl(queue_id, IPC_RMID, NULL);
        return 3;
    }


    msgctl(queue_id, IPC_RMID, NULL);
    printf("Очередь удалена.\n");

    return 0;
}
