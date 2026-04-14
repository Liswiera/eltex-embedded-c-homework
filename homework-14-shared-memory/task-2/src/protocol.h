#pragma once
#include "message_cell.h"

#define SERVER_LISTENER_SHM "/homework-14-2-server-listener-shm"
#define USER_SHM_PREFIX "/homework-14-2-user-shm"
#define USER_SHM_BUF_LEN 256

void get_user_shm_from_name(char* restrict queue_name_buf, const char* restrict user_name);
