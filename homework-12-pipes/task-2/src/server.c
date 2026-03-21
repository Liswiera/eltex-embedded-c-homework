#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PIPE_PATH "pipe"
#define MESSAGE "Hi!"
#define MESSAGE_LEN 3

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int status = mkfifo(PIPE_PATH, 0700);
    if (status == 0) {
        printf("Канал создан.\n");
    }
    else {
        if (errno == EEXIST) {
            printf("[WARN] Канал уже существует\n");
        }
        else {
            fprintf(stderr, "Не удалось создать канал.\n");
            return 1;
        }
    }

    printf("Открываю канал на запись...\n");

    int fd = open(PIPE_PATH, O_WRONLY);
    if (fd == -1) {
        fprintf(stderr, "Не удалось открыть pipe на запись.\n");
        return 2;
    }

    printf("Канал открыт на запись.\n");

    ssize_t bytes_written = write(fd, MESSAGE, MESSAGE_LEN);
    if (bytes_written < MESSAGE_LEN) {
        fprintf(stderr, "Не удалось передать строку через канал.\n");
        close(fd);
        return 3;
    }

    printf("Строка '%s' была успешно записана в канал.\n", MESSAGE);

    close(fd);
    return 0;
}
