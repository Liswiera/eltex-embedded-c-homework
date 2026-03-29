#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <unistd.h>

#define FTOK_PATH "./server"
#define FTOK_PROJ_ID 83

#define MQ_MSGSIZE 512

#define MESSAGE "Hello!"
#define MESSAGE_LEN strlen(MESSAGE)

#define BUF_LEN (MQ_MSGSIZE + 1)


int send_string(int queue_id, const void *msgp, size_t str_len, int msgflg) {
    int status = msgsnd(queue_id, msgp, str_len, msgflg);

    if (status == -1) {
        fprintf(stderr, "Не удалось передать строку через очередь сообщений.\n");
    } else {
        printf("Строка была успешно записана в очередь.\n");
    }

    return status;
}

ssize_t receive_string(int queue_id, void *msgp, size_t buf_len, long msgtyp, int msgflg) {
    char *mtext = (char*)(msgp + sizeof(long));

    ssize_t bytes_read = msgrcv(queue_id, msgp, buf_len - 1, msgtyp, msgflg);

    if (bytes_read == -1) {
        fprintf(stderr, "Не удалось прочитать строку из очереди.\n");
    }
    else {
        mtext[bytes_read] = '\0';
        printf("Была получена строка '%s'.\n", mtext);
    }
    
    return bytes_read;
}

struct message {
    long mtype;
    char mtext[BUF_LEN];
};

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

    // Клиент сначала получает сообщение (тип=1) от сервера через очередь
    struct message msg;
    ssize_t bytes_read = receive_string(queue_id, &msg, BUF_LEN, 1, MSG_NOERROR);
    if (bytes_read == -1) {
        return 3;
    }

    // Затем клиент отправляет сообщение (тип=2) серверу через очередь
    msg.mtype = 2;
    strncpy(msg.mtext, MESSAGE, MESSAGE_LEN);
    msg.mtext[MESSAGE_LEN] = '\0';

    int status = send_string(queue_id, &msg, MESSAGE_LEN, 0);
    if (status == -1) {
        return 2;
    }


    return 0;
}
