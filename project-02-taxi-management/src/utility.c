#include "utility.h"

void read_string(char *output_buf) {
    printf("> ");
    fflush(stdout);

    int arg_read_count = scanf("%" SCN_STR_LIMIT "[^\n]", output_buf);
    if (arg_read_count == 0) {
        *output_buf = '\0';
    }
    getchar(); // Drop '\n' from standard input
}

void set_task_timer(struct itimerspec *spec, time_t seconds) {
    spec->it_value.tv_sec = seconds;
    spec->it_value.tv_nsec = 0;
    spec->it_interval.tv_sec = 0;
    spec->it_interval.tv_nsec = 0;
}

int epoll_add_read_events(int epoll_fd, int fd_num, ...) {
    va_list fd_args;
    va_start(fd_args, fd_num);

    int epoll_status = 0;
    for (int i = 0; i < fd_num; i++) {
        struct epoll_event event;
        event.data.fd = va_arg(fd_args, int);
        event.events = EPOLLIN;

        epoll_status |= epoll_ctl(epoll_fd, EPOLL_CTL_ADD, event.data.fd, &event) == -1;
    }
    va_end(fd_args);

    return epoll_status;
}
