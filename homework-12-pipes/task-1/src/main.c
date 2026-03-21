#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define MESSAGE "Hi!"
#define MESSAGE_LEN 3

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    int fd[2];

    int status = pipe(fd);
    if (status != 0) {
        fprintf(stderr, "Не удалось создать канал.\n");
        return 1;
    }

    pid_t child_id = fork();

    switch (child_id) {
        case -1:
            fprintf(stderr, "Не удалось создать дочерний процесс.\n");
            return 2;
        case 0:
            // Дочерний процесс
            close(fd[1]);
            
            char buf[MESSAGE_LEN + 1];
            ssize_t bytes_read = read(fd[0], buf, MESSAGE_LEN);
            if (bytes_read < MESSAGE_LEN) {
                fprintf(stderr, "Не удалось получить строку от родительского процесса.\n");
                exit(EXIT_FAILURE);
            }
            buf[MESSAGE_LEN] = '\0';

            printf("Получено сообщение: %s\n", buf);
            
            exit(EXIT_SUCCESS);
        default:
            // Родительский процесс
            close(fd[0]);

            ssize_t bytes_written = write(fd[1], MESSAGE, MESSAGE_LEN);
            if (bytes_written < MESSAGE_LEN) {
                fprintf(stderr, "Не удалось передать строку дочернему процессу.\n");
            }

            waitpid(child_id, NULL, 0);
    }

    return 0;
}
