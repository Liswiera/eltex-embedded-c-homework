#include <locale.h>
#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MQUEUE_PATH "/homework-13-1-queue"
#define MQ_MSGSIZE 512

#define MESSAGE "Hello!"
#define MESSAGE_LEN strlen(MESSAGE)

#define BUF_LEN (MQ_MSGSIZE + 1)


int send_string(mqd_t mq, const char *str_ptr, size_t str_len, unsigned int prio) {
    int status = mq_send(mq, str_ptr, str_len, prio);

    if (status == -1) {
        fprintf(stderr, "Не удалось передать строку через очередь сообщений.\n");
    } else {
        printf("Строка '%s' была успешно записана в очередь.\n", MESSAGE);
    }

    return status;
}

ssize_t receive_string(mqd_t mq, char *buf_ptr, size_t buf_len, unsigned int *prio) {
    ssize_t bytes_read = mq_receive(mq, buf_ptr, buf_len - 1, prio);

    if (bytes_read == -1) {
        fprintf(stderr, "Не удалось прочитать строку из очереди.\n");
    }
    else {
        buf_ptr[bytes_read] = '\0';
        printf("Была получена строка '%s'.\n", buf_ptr);
    }
    
    return bytes_read;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");


    mqd_t mq = mq_open(MQUEUE_PATH, O_RDWR);
    if (mq == (mqd_t)-1) {
        fprintf(stderr, "Не удалось открыть очередь сообщений.\n");
        return 1;
    }

    printf("Очередь сообщений была открыта.\n");

    int status = send_string(mq, MESSAGE, MESSAGE_LEN, 0);
    if (status == -1) {
        mq_close(mq);
        return 2;
    }

    char buf[BUF_LEN];
    ssize_t bytes_read = receive_string(mq, buf, BUF_LEN, NULL);
    if (bytes_read == -1) {
        mq_close(mq);
        return 3;
    }


    mq_close(mq);
    return 0;
}
