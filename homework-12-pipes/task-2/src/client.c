#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PIPE_PATH "pipe"
#define BUF_LEN 1024

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    printf("Открываю канал на чтение...\n");

    int fd = open(PIPE_PATH, O_RDONLY);
    if (fd == -1) {
        fprintf(stderr, "Не удалось открыть pipe на чтение.\n");
        return 2;
    }

    printf("Канал открыт на чтение.\n");

    char buf[BUF_LEN];
    ssize_t bytes_read = read(fd, buf, BUF_LEN - 1);
    if (bytes_read == -1) {
        fprintf(stderr, "Не удалось прочитать строку из канала.\n");
        close(fd);
        return 3;
    }
    buf[bytes_read] = '\0';

    printf("Была получена строка '%s'.\n", buf);

    close(fd);
    unlink(PIPE_PATH);
    printf("Канал удалён.\n");
    return 0;
}
