#include "message_types.h"

void message_set(struct message *msg, enum msg_type type, const char *user_name, const char* text) {
    msg->type = type;
    strcpy(msg->user_name, user_name);
    strcpy(msg->text, text);
}
