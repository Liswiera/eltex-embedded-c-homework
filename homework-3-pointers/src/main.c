#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include "lib.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("USAGE: main [1-4]\n");
        return 1;
    }

    int task_id = atoi(argv[1]);

    switch (task_id) {
        default:
            printf("Неправильный номер задания (число должно быть от 1 до 4)\n");
            return 2;
    }


    return 0;
}