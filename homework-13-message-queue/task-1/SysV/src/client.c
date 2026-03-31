#include <locale.h>
#include <stdlib.h>
#include <string.h>
#include "protocol.h"

#define MESSAGE "Hello!"
#define MESSAGE_LEN strlen(MESSAGE)


int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");


    key_t key = ftok(FTOK_PATH, FTOK_PROJ_ID);
    if (key == -1) {
        fprintf(stderr, "Не удалось получить ключ для System V IPC (файл '%s').\n", FTOK_PATH);
        return 5;
    }

    int queue_id = msgget(key, 0);
    if (queue_id == -1) {
        fprintf(stderr, "Не удалось открыть очередь сообщений.\n");
        return 1;
    }

    printf("Очередь сообщений была открыта.\n");

    struct message msg;

    // Клиент сначала получает сообщение от сервера через очередь
    ssize_t bytes_read = receive_string(queue_id, &msg, BUF_LEN, MSG_TYPE_SERVER, MSG_NOERROR);
    if (bytes_read == -1) {
        return 3;
    }

    // Затем клиент отправляет сообщение серверу через очередь
    msg.mtype = MSG_TYPE_CLIENT;
    strncpy(msg.mtext, MESSAGE, MESSAGE_LEN);
    msg.mtext[MESSAGE_LEN] = '\0';

    int status = send_string(queue_id, &msg, MESSAGE_LEN, 0);
    if (status == -1) {
        return 2;
    }


    return 0;
}
