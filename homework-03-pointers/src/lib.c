#include "lib.h"


uint32_t uint32_set_byte_2(uint32_t value, uint8_t new_byte) {
    uint8_t *byte_ptr = (uint8_t*)&value;
    *(byte_ptr + 2) = new_byte;

    return value;
}

void task_2(void) {
    float x = 5.0;
    printf("x = %f, ", x);
    float y = 6.0;
    printf("y = %f\n", y);

    // float *xp = &x; // TODO: отредактируйте эту строку, и только правую часть уравнения
    float *xp = &y; // Новая строка
    float *yp = &y;
    printf("Результат: %f\n", *xp + *yp);
}

void print_array(uint32_t *arr, size_t length) {
    uint32_t *end = arr + length;
    for (uint32_t *ptr = arr; ptr < end; ptr++) {
        printf("%" PRIu32 " ", *ptr);
    }
    printf("\n");
}


static size_t str_length(const char* str) {
    const char* end = str;
    while (*end != '\0') {
        end++;
    }

    return end - str;
}

const char* find_substr_naive(const char *text, const char *pattern) {
    size_t pattern_len = str_length(pattern);

    const char *text_start = text;
    const char *text_end = text + str_length(text);
    for (; text_start + pattern_len <= text_end; text_start++) {
        int found = 1;

        const char* text_pos = text_start;
        const char* pat_pos = pattern;
        for (; *pat_pos != '\0'; text_pos++, pat_pos++) {
            if (*text_pos != *pat_pos) {
                found = 0;
                break;
            }
        }

        if (found) {
            return text_start;
        }
    }

    return NULL;
}
