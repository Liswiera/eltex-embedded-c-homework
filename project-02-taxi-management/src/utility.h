#pragma once
#ifndef _PROJ_02_UTILITY_H_
#define _PROJ_02_UTILITY_H_

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <sys/epoll.h>
#include <sys/timerfd.h>

#define STR_LIMIT 511
#define SCN_STR_LIMIT "511"

void read_string(char *output_buf);
void set_task_timer(struct itimerspec *spec, time_t seconds);
int epoll_add_read_events(int epoll_fd, int fd_num, ...);

#endif
