#include <arpa/inet.h>
#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"
#include "message.h"

int parse_args(int argc, char** argv, uint16_t *listen_port) {
    if (argc < 2) {
        printf("Not enough arguments!\n");
        printf("Usage: server [LISTEN_PORT]\n");
        return 1;
    }

    if (port_str_to_val(argv[1], listen_port) != 0) {
        fprintf(stderr, "Invalid listening port.\n");
        return 2;
    }

    return 0;
}

int create_raw_udp_socket() {
    int socket_fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    if (socket_fd == -1) {
        if (errno == EPERM) {
            fprintf(stderr, "Failed to create raw socket (root privilege is required).\n");
        }
        else {
            int saved_errno = errno;
            fprintf(stderr, "Failed to create raw socket (errno = %d).\n", saved_errno);
        }
    }

    return socket_fd;
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");


    uint16_t listen_port;
    int arg_status = parse_args(argc, argv, &listen_port);
    if (arg_status != 0) {
        return arg_status;
    }


    int socket_fd = create_raw_udp_socket();
    if (socket_fd == -1) {
        return 3;
    }

    // List is used because of the low-scale usage of the program
    // (maybe tens of simlutaneously connected users at most)
    // while also making it easier to test and implement the collection
    //
    // Search complexity: O(n)
    // Insert complexity: O(1)
    struct client_list list;
    client_list_init(&list);

    printf("Listening for the incoming messages...\n");

    // Infinite loop, send SIGINT to the program to exit
    while (1) {
        struct message msg;
        struct sockaddr_in client_sockaddr;

        if (recv_raw_inet_message(socket_fd, &client_sockaddr, listen_port, &msg) <= 0) {
            fprintf(stderr, "Failed to recieve a raw IPv4 packet.\n");
            printf("Quitting...\n");

            break;
        }

        char client_addr_str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_sockaddr.sin_addr, client_addr_str, INET_ADDRSTRLEN);

        in_addr_t client_addr = client_sockaddr.sin_addr.s_addr;
        uint16_t client_port = client_sockaddr.sin_port;
        const struct client_node *node;

        switch (msg.type) {
            case ECHO_REQUEST:
                printf("Received a message from %s:%d - '", client_addr_str, ntohs(client_port));
                message_print(&msg);
                printf("'\n");

                node = client_list_add_ping(&list, client_addr, client_port);
                if (node != NULL) {
                    char text_buf[TEXT_BUF_LEN];

                    message_null_terminate(&msg);
                    snprintf(text_buf, TEXT_BUF_LEN, "#%zu: %s", node->ping_count, msg.text);

                    if (send_raw_udp_message(socket_fd, &client_sockaddr, listen_port, text_buf, ECHO_REPLY) <= 0) {
                        fprintf(stderr, "Failed to send a ping message to that client.\n");
                    }
                }
                else {
                    fprintf(stderr, "Failed to register a new client.\n");
                }

                break;
            case CONN_TERM:
                printf("Received a termination request from %s:%d\n", client_addr_str, ntohs(client_port));

                node = client_list_reset_counter(&list, client_addr, client_port);
                if (node == NULL) {
                    fprintf(stderr, "The client is not registered.\n");
                }
                
                break;
        }

        printf("\n");
    }

    client_list_clear(&list);
    close(socket_fd);
    return 0;
}
