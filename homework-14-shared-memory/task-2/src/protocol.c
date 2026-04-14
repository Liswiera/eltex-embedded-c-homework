#include "protocol.h"

void get_user_shm_from_name(char* restrict queue_name_buf, const char* restrict user_name) {
    sprintf(queue_name_buf, "%s-%s", USER_SHM_PREFIX, user_name);
}
