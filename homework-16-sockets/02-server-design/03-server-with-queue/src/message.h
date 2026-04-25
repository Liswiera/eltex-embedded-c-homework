#pragma once
#include "common.h"

#define CONTENTS_BUF_LEN (MQ_MSGSIZE - sizeof(struct sockaddr_in))

struct message {
    struct sockaddr_in client;
    char contents[CONTENTS_BUF_LEN];
};
