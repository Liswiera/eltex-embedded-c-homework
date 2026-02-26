#include <locale.h>
#include <ncurses.h>
#include <stdlib.h>

#include "dir_view.h"

#define VIEW_COUNT 2

static void interactive_mode(struct dir_view **views, int view_count) {
    for (int i = 0; i < view_count; i++) {
        dir_view_print_entries(views[i]);
        keypad(views[i]->wnd, TRUE);
    }

    int cur_view_id = 0;
    
    while (TRUE) {
        struct dir_view *cur_view = views[cur_view_id];

        int ch = wgetch(cur_view->wnd);

        switch (ch) {
            case KEY_UP:
            case 'k':
                dir_view_move_one_up(cur_view);
                break;
            case KEY_DOWN:
            case 'j':
                dir_view_move_one_down(cur_view);
                break;
            case 'K':
                dir_view_move_selected_entry(cur_view, -5);
                break;
            case 'J':
                dir_view_move_selected_entry(cur_view, 5);
                break;
            case 'g':
                dir_view_move_top(cur_view);
                break;
            case 'G':
                dir_view_move_bottom(cur_view);
                break;
            case ' ':
            case '\n':
                dir_view_open_selected_dir(cur_view);
                break;
            case '\t':
                cur_view_id++;
                cur_view_id %= view_count;
                break;
            case 'q':
                return;
        }
    }
}

static struct dir_view** create_dir_view_subwins(WINDOW *wnd, int n) {
    struct dir_view **arr = malloc(sizeof(struct dir_view*) * n);
    if (arr == NULL) return NULL;

    int wnd_beg_y, wnd_beg_x;
    int wnd_max_y, wnd_max_x;
    getbegyx(wnd, wnd_beg_y, wnd_beg_x);
    getmaxyx(wnd, wnd_max_y, wnd_max_x);

    int wnd_lines = wnd_max_y - wnd_beg_y;
    int wnd_cols = wnd_max_x - wnd_beg_x;

    int malloc_status = 1;
    for (int i = 0; i < n; i++) {
        arr[i] = NULL;

        int xbeg = (i * wnd_cols) / n;
        int xend = ((i + 1) * wnd_cols) / n;

        WINDOW *new_win = derwin(wnd, wnd_lines, xend - xbeg, 0, xbeg);
        if (new_win == NULL) {
            malloc_status = 0;
            break;
        }

        arr[i] = dir_view_create(new_win);
        if (arr[i] == NULL) {
            delwin(new_win);

            malloc_status = 0;
            break;
        }
    }

    if (malloc_status == 1) {
        touchwin(wnd);
        return arr;
    }
    else {
        for (int i = 0; i < n; i++) {
            if (arr[i] == NULL) break;

            dir_view_free(arr[i]);
        }
        free(arr);

        return NULL;
    }
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

int task_1() {
    return 0;
}

int task_2() {
    struct dir_view **views = create_dir_view_subwins(stdscr, VIEW_COUNT);
    if (views == NULL) {
        endwin();
        printf("Failed to initialize the directory viewer.\n");
        return 1;
    }

    interactive_mode(views, VIEW_COUNT);

    for (int i = 0; i < VIEW_COUNT; i++) {
        dir_view_free(views[i]);
    }
    free(views);

    return 0;
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    init_curses();

    printw("Выберите номер задания:\n");
    printw("1. Создание файла, запись и чтение\n");
    printw("2. Файловый менеджер\n");

    int result = 0;

    do {
        int ch = getch();
        switch (ch) {
            case '1':
                result = task_1();
                break;
            case '2':
                result = task_2();
                break;
            default:
                continue;
        }
    } while(FALSE);

    endwin();
    return result;
}
