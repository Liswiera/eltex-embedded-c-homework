#include <locale.h>
#include "protocol.h"

#define MESSAGE "Hello!"
#define MESSAGE_LEN strlen(MESSAGE)
#define BUF_LEN (MQ_MSGSIZE + 1)


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
