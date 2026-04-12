#pragma once
#include "message_cell.h"

struct message_duplex {
    struct message_cell server;
    struct message_cell client;
};

void msg_duplex_init(struct message_duplex *duplex);
void msg_duplex_destroy(struct message_duplex *duplex);
