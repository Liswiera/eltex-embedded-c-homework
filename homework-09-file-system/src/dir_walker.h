#pragma once
#include <inttypes.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define DIR_WALKER_START_CAPACITY 4096

struct dir_walker {
    char *path;
    size_t capacity;
};

struct dir_walker* dir_walker_from_cwd();
struct dir_walker* dir_walker_from_path(const char *path);
void dir_walker_free(struct dir_walker *walker);
size_t dir_walker_go_into(struct dir_walker *walker, const char *dir);
size_t dir_walker_go_up(struct dir_walker *walker);
size_t dir_walker_get_path_len(const struct dir_walker *walker);
