#include "protocol.h"

void get_user_queue_from_name(char* restrict queue_name_buf, const char* restrict user_name) {
    sprintf(queue_name_buf, "%s-%s", USER_QUEUE_PREFIX, user_name);
}

void set_queue_creat_attributes(struct mq_attr *attributes) {
    attributes->mq_maxmsg = MQ_MAXMSG;
    attributes->mq_msgsize = MQ_MSGSIZE;
}
