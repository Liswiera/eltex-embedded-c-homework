#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include "child_proc_arr.h"

#define STR_LIMIT 255
#define SCN_STR_LIMIT "255"

static void read_string(char *output_buf) {
    printf("> ");
    fflush(stdout);

    int arg_read_count = scanf("%" SCN_STR_LIMIT "[^\n]", output_buf);
    if (arg_read_count == 0) {
        *output_buf = '\0';
    }
    getchar(); // Пропустить '\n' из буфера ввода
}


static int task_1() {
    pid_t child_pid = fork();
    int status;

    switch (child_pid) {
        case -1:
            printf("Не удалось создать дочерний процесс.\n");
            getch();
            return 1;
        case 0:
            printf("[PID=%d PPID=%d] Дочерний процесс\n", getpid(), getppid());
            exit(42);
        default:
            printf("[PID=%d PPID=%d] Родительский процесс\n", getpid(), getppid());
            waitpid(child_pid, &status, 0);
            printf("[PID=%d PPID=%d] Получен результат от дочернего процесса: %d\n", getpid(), getppid(), WEXITSTATUS(status));
    }

    return 0;
}

static int task_2() {
    // Общее количество процессов в дереве процессов, включая самый первый процесс
    const size_t proc_count = 6;

    struct child_proc_arr **processes = malloc(sizeof(struct child_proc_arr*) * proc_count);
    if (processes == NULL) {
        printf("Failed to allocate memory for **processes array\n");
        return 1;
    }

    // Создание иерархии процессов
    processes[0] = child_proc_arr_create(2, 1, 2); // Процесс #0 порождает #1 и #2
    processes[1] = child_proc_arr_create(2, 3, 4); // Процесс #1 порождает #3 и #4
    processes[2] = child_proc_arr_create(1, 5); // Процесс #2 порождает #5
    for (size_t i = 3; i < proc_count; i++) {
        processes[i] = child_proc_arr_create(0); // Оставшиеся дочерние процессы являются листовыми
    }

    // Error handling
    int malloc_success = 1;
    for (size_t i = 0; i < proc_count; i++) {
        if (processes[i] == NULL) {
            malloc_success = 0;
            break;
        }
    }

    if (malloc_success) {
        uint32_t cur_id = 0;

        PROC_BEGIN:
        // Начало работы процесса с ID=cur_id

        printf("[PID=%d PPID=%d] [#%" PRIu32 "] Процесс начал работу...\n",
            getpid(),
            getppid(),
            cur_id
        );
        size_t child_count = 0; // Количество успешно созданных дочерних процессов

        for (size_t i = 0; i < processes[cur_id]->count; i++) {
            uint32_t new_id = processes[cur_id]->id_arr[i]; // ID будущего дочернего процесса

            // Дополнительная проверка для избегания потенциальной fork-бомбы
            if (new_id <= cur_id) {
                fprintf(stderr, "WARNING: CUR_ID=%" PRIu32 ", NEW_ID=%" PRIu32 ", skipping...",
                    cur_id,
                    new_id
                );
                continue;
            }

            pid_t child_pid = fork();
            switch(child_pid) {
                case -1:
                    printf("[PID=%d PPID=%d] [#%" PRIu32 "] Не удалось создать дочерний процесс #%" PRIu32 "\n",
                        getpid(),
                        getppid(),
                        cur_id,
                        new_id
                    );
                    break;
                case 0:
                    // В дочернем процессе заменяем ID и переходим в начало
                    cur_id = new_id;
                    goto PROC_BEGIN;
                default:
                    // Дочерний процесс был успешно создан, необходимо его учесть в общее количество child_count
                    child_count++;
                    printf("[PID=%d PPID=%d] [#%" PRIu32 "] Был создан дочерний процесс #%" PRIu32 "\n",
                        getpid(),
                        getppid(),
                        cur_id,
                        new_id
                    );
            }
        }

        // Ожидание заврешения успешно созданных дочерних процессов
        for (size_t i = 0; i < child_count; i++) {
            int status;
            wait(&status);

            printf("[PID=%d PPID=%d] [#%" PRIu32 "] Получен результат: %d\n",
                getpid(),
                getppid(),
                cur_id,
                WEXITSTATUS(status)
            );
        }

        if (cur_id > 0) {
            int exit_status = (int)cur_id;
            printf("[PID=%d PPID=%d] [#%" PRIu32 "] Завершаю работу... (status = %d)\n",
                getpid(),
                getppid(),
                cur_id,
                exit_status
            );
            exit(exit_status);
        }
    }


    for (size_t i = 0; i < proc_count; i++) {
        child_proc_arr_free(processes[i]);
    }
    free(processes);

    return 0;
}

static int task_3() {
    while (1) {

    }

    return 0;
}

static void init_curses() {
    initscr();

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_RED);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_RED);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_RED);

    cbreak();
    noecho();
    curs_set(0);
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    init_curses();

    printw("Выберите номер задания:\n");
    printw("1. Родительский и дочерний процессы\n");
    printw("2. Иерархия процессов\n");
    printw("3. Командный интерпретатор\n");

    int result = 0;
    int done = FALSE;

    while (!done) {
        done = TRUE;

        int ch = getch();
        switch (ch) {
            case '1':
                endwin();
                result = task_1();
                break;
            case '2':
                endwin();
                result = task_2();
                break;
            case '3':
                endwin();
                result = task_3();
                break;
            default:
                done = FALSE;
        }
    }

    return result;
}