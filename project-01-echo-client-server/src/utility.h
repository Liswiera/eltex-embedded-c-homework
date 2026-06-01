#pragma once
#ifndef _PROJ_01_UTILITY_H_
#define _PROJ_01_UTILITY_H_

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <unistd.h>

#define MIN(x, y) ((x) < (y) ? (x) : (y))
#define MAX(x, y) ((x) > (y) ? (x) : (y))

int is_number(const char *str);
uint16_t calc_checksum16(const void *data, size_t byte_count);
int port_str_to_val(const char *port_str, uint16_t *port_out);
void string_trim_end(char *str);

#endif
