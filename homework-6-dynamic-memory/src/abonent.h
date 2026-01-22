#pragma once

#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#define CHAR_ARR_SIZE 10

struct abonent {
    char name[CHAR_ARR_SIZE];
    char second_name[CHAR_ARR_SIZE];
    char tel[CHAR_ARR_SIZE];
};

void abonent_create_from(struct abonent *restrict dest, const char *name, const char *second_name, const char *tel);
void abonent_print(const struct abonent *obj);
