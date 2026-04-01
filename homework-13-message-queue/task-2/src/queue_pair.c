#include "queue_pair.h"

int create_queue_pair(const char *q_name, struct queue_pair *q_pair) {
    struct mq_attr attributes;
    set_queue_creat_attributes(&attributes);

    mqd_t mq = mq_open(q_name, O_CREAT | O_RDONLY, 0600, &attributes);
    if (mq == (mqd_t)-1) {
        fprintf(stderr, "Не удалось создать очередь '%s' на чтение.\n", q_name);
        return 1; 
    }
    mqd_t read_end = mq;

    mq = mq_open(q_name, O_WRONLY);
    if (mq == (mqd_t)-1) {
        fprintf(stderr, "Не удалось открыть очередь '%s' на запись.\n", q_name);

        mq_close(read_end);
        mq_unlink(read_end);
        return 2; 
    }
    mqd_t write_end = mq;

    q_pair->read_end = read_end;
    q_pair->write_end = write_end;
    return 0;
}

int close_queue_pair(struct queue_pair *q_pair) {
    mqd_t mq;

    mq = q_pair->write_end;
    mq_close(mq);
    
    mq = q_pair->read_end;
    mq_close(mq);
    mq_unlink(mq);
}
