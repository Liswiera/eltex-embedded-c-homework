#include <arpa/inet.h>
#include <errno.h>
#include <locale.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "message.h"


int socket_fd = -1;
uint16_t src_port; // Little-endian
struct sockaddr_in server_sockaddr;
    

void session_terminated_handler(int signo, siginfo_t *info, void *context) {
    printf("\nReceived signal SIGINT.\n");

    if (socket_fd != -1) {
        send_raw_udp_message(socket_fd, &server_sockaddr, src_port, "", CONN_TERM);

        printf("Session terminated.\n");
        exit(0);
    }
}


int parse_args(int argc, char** argv, struct in_addr *server_in_addr, uint16_t *server_port, uint16_t *src_port) {
    if (argc < 4) {
        printf("Not enough arguments!\n");
        printf("Usage: client [SERVER_IPV4_ADDR] [SERVER_PORT] [SRC_PORT]\n");
        return 1;
    }

    if (inet_pton(AF_INET, argv[1], server_in_addr) != 1) {
        fprintf(stderr, "Invalid server IPv4 address.\n");
        return 2;
    }

    if (port_str_to_val(argv[2], server_port) != 0) {
        fprintf(stderr, "Invalid server port.\n");
        return 3;
    }

    if (port_str_to_val(argv[3], src_port) != 0) {
        fprintf(stderr, "Invalid source port.\n");
        return 4;
    }

    return 0;
}

int sigint_set_disposition() {
    struct sigaction act = { 0 };
    act.sa_sigaction = &session_terminated_handler;

    int status = sigaction(SIGINT, &act, NULL);
    if (status == -1) {
        fprintf(stderr, "Failed to set the disposition of SIGINT.\n");
    }

    return status;
}

void build_server_sockaddr(struct sockaddr_in *server_sockaddr, const struct in_addr *server_in_addr, uint16_t server_port) {
    memset(server_sockaddr, 0, sizeof(struct sockaddr_in));

    server_sockaddr->sin_family = AF_INET;
    memcpy(&server_sockaddr->sin_addr, server_in_addr, sizeof(struct in_addr));
    server_sockaddr->sin_port = htons(server_port);
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


    struct in_addr server_in_addr;
    uint16_t server_port;
    
    int arg_status = parse_args(argc, argv, &server_in_addr, &server_port, &src_port);
    if (arg_status != 0) {
        return arg_status;
    }

    build_server_sockaddr(&server_sockaddr, &server_in_addr, server_port);

    if (sigint_set_disposition() == -1) {
        return 5;
    }

    socket_fd = create_raw_udp_socket();
    if (socket_fd == -1) {
        return 6;
    }

    // Infinite loop, send SIGINT to the program to exit
    while (1) {
        char text_buf[TEXT_BUF_LEN];

        printf("Enter message: ");
        fgets(text_buf, TEXT_BUF_LEN, stdin);

        string_trim_end(text_buf);

        if (send_raw_udp_message(socket_fd, &server_sockaddr, src_port, text_buf, ECHO_REQUEST) <= 0) {
            fprintf(stderr, "Failed to send a raw UDP packet.\n");
            printf("Quitting...\n");

            break;
        }

        struct message msg;
        if (recv_raw_inet_message(socket_fd, NULL, src_port, &msg) <= 0) {
            fprintf(stderr, "Failed to recieve a raw IPv4 packet.\n");
            printf("Quitting...\n");

            break;
        }

        printf("Received a message from the server: '");
        message_print(&msg);
        printf("'\n");
    }

    close(socket_fd);
    return 0;
}
