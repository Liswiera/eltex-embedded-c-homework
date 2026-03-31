#include <locale.h>
#include "protocol.h"

#define MESSAGE "Hi!"
#define MESSAGE_LEN strlen(MESSAGE)
#define BUF_LEN (MQ_MSGSIZE + 1)


int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    struct mq_attr attributes;
    attributes.mq_maxmsg = 8;
    attributes.mq_msgsize = MQ_MSGSIZE;

    mqd_t mq = mq_open(MQUEUE_PATH, O_CREAT | O_RDWR, 0600, &attributes);
    if (mq == (mqd_t)-1) {
        fprintf(stderr, "Не удалось создать очередь сообщений.\n");
        return 1; 
    }

    printf("Очередь сообщений была создана.\n");

    char buf[BUF_LEN];
    ssize_t bytes_read = receive_string(mq, buf, BUF_LEN, NULL);
    if (bytes_read == -1) {
        mq_close(mq);
        mq_unlink(MQUEUE_PATH);
        return 3;
    }

    int status = send_string(mq, MESSAGE, MESSAGE_LEN, 0);
    if (status == -1) {
        mq_close(mq);
        mq_unlink(MQUEUE_PATH);
        return 2;
    }


    mq_close(mq);
    mq_unlink(MQUEUE_PATH);

    printf("Очередь удалена.\n");

    return 0;
}
