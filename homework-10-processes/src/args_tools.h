#pragma once
#include <ctype.h>
#include <inttypes.h>
#include <stdlib.h>

char** args_create_empty(size_t arg_count, size_t arg_capacity);
char** args_parse_from_str(const char* str, size_t arg_capacity);
void args_free(char **args);
