#include "args_tools.h"

static void str_trim(const char* str, const char** start, const char** end) {
    const char *start_l, *end_l;

    for (start_l = str; *start_l != '\0'; start_l++) {
        if (!isspace(*start_l)) break;
    }

    for (end_l = start_l; *end_l != '\0'; end_l++);
    end_l--;

    for (; end_l >= start_l; end_l--) {
        if (!isspace(*end_l)) break;
    }
    end_l++;

    *start = start_l;
    *end = end_l;
}


char** args_create_empty(size_t arg_count, size_t arg_capacity) {
    char **args = malloc(sizeof(char*) * (arg_count + 1));;
    if (args == NULL) return NULL;

    for (size_t i = 0; i <= arg_count; i++) {
        args[i] = NULL;
    }

    int malloc_success = 1;
    for (size_t i = 0; i < arg_count; i++) {
        args[i] = malloc(sizeof(char) * arg_capacity);
        if (args[i] == NULL) {
            malloc_success = 0;
            break;
        }
    }

    if (!malloc_success) {
        for (size_t i = 0; i < arg_count; i++) {
            free(args[i]);
        }
        free(args);
        args = NULL;
    }

    return args;
}

char** args_parse_from_str(const char* str, size_t arg_capacity) {
    size_t arg_count;
    char** args = NULL;
    const char *start, *end;

    str_trim(str, &start, &end);

    if (start < end) {
        arg_count = 1;

        for (const char *ptr = start + 1; ptr < end; ptr++) {
            if (isspace(*ptr) && !isspace(*(ptr - 1))) {
                arg_count++;
            }
        }

        args = args_create_empty(arg_count, arg_capacity);

        size_t cur_arg_id = 0;
        char *arg_dest = args[cur_arg_id];

        for (const char *ptr = start; ptr < end; ptr++) {
            if (isspace(*ptr)) {
                if (!isspace(*(ptr - 1))) {
                    *arg_dest = '\0';
                    arg_dest = args[++cur_arg_id];
                }
            }
            else {
                *(arg_dest++) = *ptr;
            }
        }

        *arg_dest = '\0';
        args[arg_count] = NULL;
    }
    else {
        args = args_create_empty(0, arg_capacity);
    }

    return args;
}

void args_free(char **args) {
    if (args != NULL) {
        for (int i = 0; args[i] != NULL; i++) free(args[i]);
        free(args);
    }
}
