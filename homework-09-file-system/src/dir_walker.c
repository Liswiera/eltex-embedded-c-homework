#include "dir_walker.h"

static struct dir_walker* default_dir_walker() {
    struct dir_walker *walker = malloc(sizeof(struct dir_walker));
    if (walker == NULL) {
        return NULL;
    }

    walker->capacity = DIR_WALKER_START_CAPACITY;

    walker->path = malloc(sizeof(char) * walker->capacity);
    if (walker->path == NULL) {
        free(walker);
        return NULL;
    }

    walker->path[0] = '\0';

    return walker;
}

static int is_readable_dir(const char *path) {
    struct stat st;
    if (stat(path, &st) != 0) {
        return 0;
    }

    if (!S_ISDIR(st.st_mode)) {
        return 0;
    }

    return access(path, R_OK | X_OK) == 0;
}


struct dir_walker* dir_walker_from_cwd() {
    struct dir_walker *walker = default_dir_walker();
    if (walker == NULL) {
        return NULL;
    }

    char *ret = getcwd(walker->path, walker->capacity);
    if (ret == NULL) {
        dir_walker_free(walker);
        return NULL;
    }

    return walker;
}

struct dir_walker* dir_walker_from_path(const char *path) {
    if (strlen(path) + 1 > DIR_WALKER_START_CAPACITY) {
        return NULL;
    }
    
    struct dir_walker *walker = default_dir_walker();
    if (walker == NULL) {
        return NULL;
    }

    strcpy(walker->path, path);

    if (is_readable_dir(walker->path)) {
        return walker;
    }
    else {
        dir_walker_free(walker);
        return NULL;
    }
}

void dir_walker_free(struct dir_walker *walker) {
    free(walker->path);
    free(walker);
}

size_t dir_walker_go_into(struct dir_walker *walker, const char *dir) {
    // Check if *dir contains '/' and return immediately if it does
    const char* ch;
    for (ch = dir; *ch != '\0'; ch++) {
        if (*ch == '/') {
            return 0;
        }
    }
    
    size_t walker_len = dir_walker_get_path_len(walker);
    size_t dir_len = ch - dir;

    if (dir_len == 0) {
        return 0;
    }

    if (walker_len + 1 + dir_len + 1 > walker->capacity) {
        // Do not copy if the new path doesn't fit into the path buffer
        return 0;
    }

    walker->path[walker_len] = '/';
    strcpy(walker->path + walker_len + 1, dir);

    if (!is_readable_dir(walker->path)) {
        dir_walker_go_up(walker);
        return 0;
    }

    return 1 + dir_len;
}

size_t dir_walker_go_up(struct dir_walker *walker) {
    size_t end_pos = dir_walker_get_path_len(walker) - 1;

    for (size_t i = end_pos; i > 0; i--) {
        if (walker->path[i] == '/') {
            walker->path[i] = '\0';
            return end_pos - i;
        }
    }

    walker->path[1] = '\0';

    return end_pos;
}

size_t dir_walker_get_path_len(const struct dir_walker *walker) {
    return strlen(walker->path);
}
