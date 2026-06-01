#include "message.h"

void message_init(struct message *msg, message_type type, const char *text) {
    size_t len = MIN(strlen(text), TEXT_BUF_LEN);

    msg->type = (uint8_t)type;
    msg->len = htons((uint16_t)len);

    memcpy(msg->text, text, len);
}

size_t message_struct_len(const struct message *msg) {
    return sizeof(msg->type) + sizeof(msg->len) + message_text_len(msg);
}

size_t message_text_len(const struct message *msg) {
    return ntohs(msg->len);
}

void message_print(const struct message *msg) {
    for (size_t i = 0; i < message_text_len(msg); i++) {
        // putc is used for simplicity's sake
        putc(msg->text[i], stdout);
    }
}

void message_null_terminate(struct message *msg) {
    size_t len = message_text_len(msg);

    if (len < TEXT_BUF_LEN) {
        msg->text[len] = '\0';
    }
    else {
        msg->text[len - 1] = '\0';
    }
}


ssize_t send_raw_udp_message(int socket_fd, const struct sockaddr_in *dest_addr, uint16_t src_port, const char* text, message_type type) {
    struct message msg;
    message_init(&msg, type, text);

    struct udp_segment segment;
    udp_segment_build(&segment, src_port, ntohs(dest_addr->sin_port), (uint8_t*)&msg, message_struct_len(&msg));

    printf("Sending a message - '");
    message_print((struct message*)segment.app_data_buf);
    printf("'\n");

    return sendto(socket_fd, &segment, ntohs(segment.len), 0, (const struct sockaddr*)dest_addr, sizeof(struct sockaddr_in));
}

ssize_t recv_raw_inet_message(int socket_fd, struct sockaddr_in *src_addr, uint16_t listen_port, struct message *msg_output) {
    struct inet_datagram datagram;
    
    while (1) {
        ssize_t bytes_read = recv(socket_fd, &datagram, sizeof(datagram), 0);
        if (bytes_read <= 0) {
            return bytes_read;
        }
        
        
        if (datagram.protocol != IPPROTO_UDP) {
            continue;
        }

        if (ntohs(datagram.payload_udp.dst_port) != listen_port) {
            continue;
        }

        const struct message *msg = (struct message*)datagram.payload_udp.app_data_buf;
        if (ntohs(datagram.payload_udp.len) != (message_struct_len(msg) + 8)) {
            continue;
        }

        memcpy(msg_output, msg, sizeof(struct message));

        if (src_addr != NULL) {
            src_addr->sin_family = AF_INET;
            src_addr->sin_addr.s_addr = datagram.src_addr;
            src_addr->sin_port = datagram.payload_udp.src_port;
        }

        return bytes_read;
    }

    return -1;
}

