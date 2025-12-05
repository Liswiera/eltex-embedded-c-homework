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


struct abonent_vec {
    struct abonent *abonents;
    size_t capacity;
    size_t len;
};

struct abonent_vec abonent_vec_create(struct abonent *abonents, size_t capacity);
uint32_t abonent_vec_push(struct abonent_vec *restrict vec, const char *name, const char *second_name, const char *tel);
uint32_t abonent_vec_remove_at(struct abonent_vec *vec, size_t id);
void abonent_vec_search_by_name_and_print(const struct abonent_vec *vec, const char *name);
void abonent_vec_print_all(const struct abonent_vec *vec);
