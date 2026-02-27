#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


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
    clear();

    return 0;
}

static int task_3() {
    clear();

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