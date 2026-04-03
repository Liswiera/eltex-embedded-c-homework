#pragma once
#include "protocol.h"

enum msg_type {
    connection_request,
    disconnection_request,
    user_connected,
    user_disconnected,
    message_from_user,
    message_from_server,
    session_ended,
};

struct message {
    enum msg_type type;
    char user_name[USER_NAME_BUF_LEN];
    char text[TEXT_BUF_LEN];
};

void message_set(struct message *msg, enum msg_type type, const char *user_name, const char* text);
