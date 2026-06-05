#include <stdio.h>
#include "driver_vec.h"
#include "utility.h"

#define MAX_DRIVER_COUNT 3

static void print_command_list() {
    printf("Full list of available commands:\n");
    printf("    create_driver - create a new driver;\n");
    printf("    send_task <pid> <task_timer> - assign a task to the driver with PID=<pid> for <task_timer> seconds;\n");
    printf("    get_status <pid> - get the current status of the driver with PID=<pid>;\n");
    printf("    get_drivers - get each driver's current status;\n");
    printf("    help - print this page;\n");
    printf("    exit - quit the program.\n");
    printf("\n");
}

static void create_driver(struct driver_vec *vec) {
    if (vec->len < vec->capacity) {
        struct driver drv;
        size_t drv_id = vec->len + 1;

        if (driver_init_fork(&drv, drv_id) == -1) {
            fprintf(stderr, "Failed to create a new driver.\n");
            return;
        }
        
        if (drv.pid == 0) {
            // Child goes to a non-returning function
            driver_entry(&drv);
        }
        else {
            // Parent adds the child to the vector
            driver_vec_push_back(vec, &drv); // Return value is ignored
            printf("New driver #%zu (PID=%d) has been created.\n", drv.id, drv.pid);
        }
    }
    else {
        printf("ERROR: Can't create any more drivers (max capacity is %zu).\n", vec->capacity);
    }
}

static void send_task(struct driver_vec *vec, pid_t pid, uint32_t task_timer) {
    struct driver *drv = driver_vec_find_with_pid(vec, pid);
    if (drv == NULL) {
        printf("ERROR: No driver with PID=%d.\n", pid);
        return;
    }

    struct message msg;
    msg.type = M_SEND_TASK;
    msg.time = task_timer;

    if (write(drv->write_fd, &msg, sizeof(msg)) < sizeof(msg)) {
        fprintf(stderr, "Failed to send a message to the driver.\n");
        return;
    }

    if (read(drv->read_fd, &msg, sizeof(msg)) < sizeof(msg)) {
        fprintf(stderr, "Failed to read a message from the driver.\n");
        return; 
    }

    printf("Driver #%zu [PID=%d] ", drv->id, drv->pid);
    switch (msg.type) {
        case M_TASK_ACCEPTED:
            printf("has accepted the task (%" PRIu32 "s).\n", task_timer);
            break;
        case M_TASK_REFUSED_BAD_ARG:
            printf("has refused the task (bad task timer).\n");
            break;
        case M_DRIVER_BUSY:
            printf("has refused the task (driver is busy).\n");
            break;
        default:
            printf("sent unknown message type.\n");
            break;
    }
}

static void get_status(struct driver_vec *vec, pid_t pid) {
    struct driver *drv = driver_vec_find_with_pid(vec, pid);
    if (drv == NULL) {
        printf("ERROR: No driver with PID=%d.\n", pid);
        return;
    }
    
    struct message msg;
    msg.type = M_GET_STATUS;

    if (write(drv->write_fd, &msg, sizeof(msg)) < sizeof(msg)) {
        fprintf(stderr, "Failed to send a message to the driver.\n");
        return;
    }

    if (read(drv->read_fd, &msg, sizeof(msg)) < sizeof(msg)) {
        fprintf(stderr, "Failed to read a message from the driver.\n");
        return; 
    }

    printf("Driver #%zu [PID=%d] ", drv->id, drv->pid);
    switch (msg.type) {
        case M_DRIVER_AVAILABLE:
            printf("is available.\n");
            break;
        case M_DRIVER_BUSY:
            printf("is busy (%" PRIu32 "s left).\n", msg.time);
            break;
        default:
            printf("has unknown status.\n");
            break;
    }
}

static void get_drivers(struct driver_vec *vec) {
    for (size_t i = 0; i < vec->len; i++) {
        get_status(vec, vec->drivers[i].pid);
    }
}

static void cli_loop(struct driver_vec *vec) {
    char line_buf[STR_LIMIT + 1];
    int pid;
    uint32_t task_timer;

    print_command_list();
    while (1) {
        read_string(line_buf);

        if (strcmp(line_buf, "create_driver") == 0) {
            create_driver(vec);
        }
        else if (sscanf(line_buf, "send_task %d %" SCNu32, &pid, &task_timer) == 2) {
            send_task(vec, pid, task_timer);
        }
        else if (sscanf(line_buf, "get_status %d", &pid) == 1) {
            get_status(vec, pid);
        }
        else if (strcmp(line_buf, "get_drivers") == 0) {
            get_drivers(vec);
        }
        else if (strcmp(line_buf, "help") == 0) {
            print_command_list();
        }
        else if (strcmp(line_buf, "exit") == 0) {
            break;
        }
        else {
            printf("ERROR: Unknown command\n");
        }
    }
}

int main() {
    struct driver_vec vec;
    if (driver_vec_init(&vec, MAX_DRIVER_COUNT) != 0) {
        fprintf(stderr, "Failed to initialize a driver vector.\n");
        return 1;
    }

    cli_loop(&vec);

    driver_vec_destroy(&vec);
    return 0;
}
