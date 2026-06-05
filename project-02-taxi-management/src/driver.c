#include "driver.h"

int driver_init_fork(struct driver *drv, size_t id) {
    int downstream_pipefd[2];
    if (pipe(downstream_pipefd) == -1) {
        return -1;
    }

    int upstream_pipefd[2];
    if (pipe(upstream_pipefd) == -1) {
        close(downstream_pipefd[0]);
        close(downstream_pipefd[1]);
        return -1;
    }

    // The new driver is considered to be available right from the start
    drv->status = S_DRIVER_AVAILABLE;
    drv->id = id;

    drv->pid = fork();
    switch (drv->pid) {
        case -1:
            // Failed to fork, cleaning up the structure
            close(downstream_pipefd[0]);
            close(downstream_pipefd[1]);
            close(upstream_pipefd[0]);
            close(upstream_pipefd[1]);
            return -1;
        case 0:
            // Child process
            drv->read_fd = downstream_pipefd[0];
            drv->write_fd = upstream_pipefd[1];

            close(upstream_pipefd[0]); // Do not allow reading messages from the manager's side
            close(downstream_pipefd[1]); // Do not allow sending messages from the driver into itself
            break;
        default:
            // Parent process
            drv->read_fd = upstream_pipefd[0];
            drv->write_fd = downstream_pipefd[1];

            close(downstream_pipefd[0]); // Do not allow reading messages from the driver's side
            close(upstream_pipefd[1]); // Do not allow sending messages from the manager into itself
            break;
    }

    return 0;
}

int driver_terminate(struct driver *drv) {
    return ((kill(drv->pid, SIGTERM) == 0)
        && (waitpid(drv->pid, NULL, 0) != -1))
        ? 0 : -1;
}

static void get_status_handler(struct driver *self, int timer_fd) {
    struct message msg;

    if (self->status == S_DRIVER_AVAILABLE) {
        msg.type = M_DRIVER_AVAILABLE;
    }
    else {
        struct itimerspec spec;
        if (timerfd_gettime(timer_fd, &spec) == -1) {
            fprintf(stderr, "Driver #%zu (PID=%d): failed to get timer spec.\n", self->id, self->pid);
            return;
        }

        msg.type = M_DRIVER_BUSY;
        msg.time = (uint32_t)spec.it_value.tv_sec;
    }

    if (write(self->write_fd, &msg, sizeof(msg)) != sizeof(msg)) {
        fprintf(stderr, "Driver #%zu (PID=%d): failed to write into pipe.\n", self->id, self->pid);
        return;
    }
}

static void send_task_handler(struct driver *self, int timer_fd, uint32_t task_timer) {
    struct message msg;

    if (self->status == S_DRIVER_AVAILABLE) {
        if (task_timer > 0) {
            // Driver accepts the task (the driver is available and the task timer is correct)
            struct itimerspec spec;
            set_task_timer(&spec, (time_t)task_timer);

            if (timerfd_settime(timer_fd, 0, &spec, NULL) == -1) {
                fprintf(stderr, "Driver #%zu (PID=%d): failed to set the timer.\n", self->id, self->pid);
                exit(EXIT_FAILURE);
            }
            self->status = S_DRIVER_BUSY;

            msg.type = M_TASK_ACCEPTED;
        }
        else {
            // Driver can't accept a task that takes 0 seconds to complete
            msg.type = M_TASK_REFUSED_BAD_ARG;
        }
    }
    else {
        // Driver is currently doing another task, refuse the new one
        msg.type = M_DRIVER_BUSY;
    }

    if (write(self->write_fd, &msg, sizeof(msg)) != sizeof(msg)) {
        fprintf(stderr, "Driver #%zu (PID=%d): failed to write into pipe.\n", self->id, self->pid);
        return;
    }
}

noreturn void driver_entry(struct driver *self) {
    int timer_fd = timerfd_create(CLOCK_REALTIME, 0);
    if (timer_fd == -1) {
        fprintf(stderr, "Driver #%zu (PID=%d): failed to create a timer.\n", self->id, self->pid);
        exit(EXIT_FAILURE);
    }

    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        fprintf(stderr, "Driver #%zu (PID=%d): failed to create an epoll object.\n", self->id, self->pid);
        exit(EXIT_FAILURE);
    }

    if (epoll_add_read_events(epoll_fd, 2, timer_fd, self->read_fd) != 0) {
        fprintf(stderr, "Driver #%zu (PID=%d): failed to register epoll.\n", self->id, self->pid);
        exit(EXIT_FAILURE);
    }

    self->status = S_DRIVER_AVAILABLE;
    while (1) {
        // Driver listens for a new message from the manager
        // and for the timer expiration event using epoll
        struct epoll_event event;
        int ready_count = epoll_wait(epoll_fd, &event, 1, -1);

        if (ready_count == -1) {
            fprintf(stderr, "Driver #%zu (PID=%d): epoll_wait error.\n", self->id, self->pid);
            exit(EXIT_FAILURE);  
        }

        if (ready_count == 0) {
            continue;
        }

        // Driver handles the new event
        int ready_fd = event.data.fd;
        if (ready_fd == timer_fd) {
            // Timer has expired

            self->status = S_DRIVER_AVAILABLE;
        }
        else if (ready_fd == self->read_fd) {
            // Got a message from the manager

            struct message msg;
            if (read(self->read_fd, &msg, sizeof(msg)) != sizeof(msg)) {
                fprintf(stderr, "Driver #%zu (PID=%d): failed to read from pipe.\n", self->id, self->pid);
                continue;
            }

            switch (msg.type) {
                case M_GET_STATUS:
                    get_status_handler(self, timer_fd);
                    break;
                case M_SEND_TASK:
                    send_task_handler(self, timer_fd, msg.time);
                    break;
                default:
                    fprintf(stderr, "Driver #%zu (PID=%d): received unknown message from the manager.\n", self->id, self->pid);
                    break;
            }
        }
    }

    exit(EXIT_SUCCESS);
}
