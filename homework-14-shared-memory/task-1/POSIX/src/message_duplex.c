#include "message_duplex.h"

void msg_duplex_init(struct message_duplex *duplex) {
    msg_cell_init(&duplex->server);
    msg_cell_init(&duplex->client);
}

void msg_duplex_destroy(struct message_duplex *duplex) {
    msg_cell_destroy(&duplex->server);
    msg_cell_destroy(&duplex->client);
}
