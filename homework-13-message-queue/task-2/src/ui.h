#pragma once
#include <curses.h>
#include "history.h"
#include "user.h"

#define USER_LIST_WIDTH 10
#define PROMPT_LINE_HEIGHT 1
#define LINE_BUF_CAPACITY 512

struct chat_ui {
    WINDOW *chat_history_wnd;
    WINDOW *user_list_wnd;
    WINDOW *prompt_wnd;
};

struct chat_ui *chat_ui_create();
void chat_ui_free(struct chat_ui *ui);
void ui_print_chat_history(struct chat_ui *ui, struct history *hist);
void ui_print_user_list(struct chat_ui *ui, const struct user *users, size_t user_count);
void ui_print_exit_prompt(struct chat_ui *ui);
