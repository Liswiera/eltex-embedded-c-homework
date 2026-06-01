#include "utility.h"

int is_number(const char *str) {
    for (; *str != '\0'; str++) {
        if (!isdigit(*str)) {
            return 0;
        }
    }

    return 1;
}

uint16_t calc_checksum16(const void *data, size_t byte_count) {
    const uint16_t *data_u16 = data;
    uint32_t checksum = 0;

    while (byte_count >= 2) {
        checksum += *data_u16;

        data_u16++;
        byte_count -= 2;
    }

    if (byte_count > 0) {
        checksum += *(uint8_t*)data_u16;
    }

    while ((checksum >> 16) != 0) {
        checksum = (checksum & 0xFFFF) + (checksum >> 16);
    }

    return checksum;
}

int port_str_to_val(const char *port_str, uint16_t *port_out) {
    if (!is_number(port_str)) {
        return 1;
    }

    errno = 0;
    long port = strtol(port_str, NULL, 10);
    if ((port < 0) || (port > __UINT16_MAX__) || (errno == ERANGE)) {
        return 2;
    }

    *port_out = (uint16_t)port;
    return 0;
}

void string_trim_end(char *str) {
    char *ch_ptr;
    for (ch_ptr = str; *ch_ptr != '\0'; ch_ptr++);
    
    for (ch_ptr--; ch_ptr >= str; ch_ptr--) {
        if (isspace(*ch_ptr)) {
            *ch_ptr = '\0';
        }
        else {
            break;
        }
    }
}
