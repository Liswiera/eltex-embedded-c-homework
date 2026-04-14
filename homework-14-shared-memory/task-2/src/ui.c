#include "ui.h"

static int get_screen_height() {
    return getmaxy(stdscr);
}
static int get_screen_width() {
    return getmaxx(stdscr);
}
static int get_chat_height() {
    return get_screen_height() - PROMPT_LINE_HEIGHT - 1;
}
static int get_chat_width() {
    return get_screen_width() - USER_LIST_WIDTH - 1;
}
static int get_user_list_height() {
    return get_screen_height() - PROMPT_LINE_HEIGHT - 1;
}
static int get_user_list_width() {
    return USER_LIST_WIDTH;
}

static struct chat_ui *chat_ui_create_empty() {
    struct chat_ui *ui = malloc(sizeof(struct chat_ui));
    if (ui == NULL) {
        return NULL;
    }

    ui->chat_history_wnd = NULL;
    ui->user_list_wnd = NULL;
    ui->prompt_wnd = NULL;

    return ui;
}

struct chat_ui *chat_ui_create() {
    struct chat_ui *ui = chat_ui_create_empty();
    if (ui == NULL) {
        return NULL;
    }

    int malloc_success = 1;

    if (malloc_success) {
        ui->chat_history_wnd = derwin(stdscr, get_chat_height(), get_chat_width(), 0, 0);
        malloc_success = (ui->chat_history_wnd != NULL);
    }
    if (malloc_success) {
        ui->user_list_wnd = derwin(stdscr, get_user_list_height(), get_user_list_width(), 0, get_chat_width() + 1);
        malloc_success = (ui->user_list_wnd != NULL);
    }
    if (malloc_success) {
        ui->prompt_wnd = derwin(stdscr, PROMPT_LINE_HEIGHT, get_screen_width(), get_chat_height() + 1, 0);
        malloc_success = (ui->prompt_wnd != NULL);
    }

    if (!malloc_success) {
        chat_ui_free(ui);
        ui = NULL;
    }

    return ui;
}

void chat_ui_free(struct chat_ui *ui) {
    if (ui != NULL) {
        delwin(ui->chat_history_wnd);
        delwin(ui->user_list_wnd);
        delwin(ui->prompt_wnd);
        free(ui);
    }
}

char line_buf[LINE_BUF_CAPACITY];

void ui_print_chat_history(struct chat_ui *ui, struct history *hist) {
    wclear(ui->chat_history_wnd);

    int height = get_chat_height();
    size_t msg_start_id = (hist->msg_count <= height) ? 0 : (hist->msg_count - height);

    int line_y_coord = 0;
    for (size_t msg_id = msg_start_id; msg_id < hist->msg_count; msg_id++) {
        const struct message *msg = history_get_message(hist, msg_id);

        switch (msg->type) {
            case user_connected:
                snprintf(line_buf, LINE_BUF_CAPACITY - 1, "%s has joined the chat!", msg->user_name);
                break;
            case user_disconnected:
                snprintf(line_buf, LINE_BUF_CAPACITY - 1, "%s has left the chat.", msg->user_name);
                break;
            case message_from_user:
                snprintf(line_buf, LINE_BUF_CAPACITY - 1, "%s: %s", msg->user_name, msg->text);
                break;
            case message_from_server:
                snprintf(line_buf, LINE_BUF_CAPACITY - 1, "[SERVER] %s", msg->text);
                break;
            case session_ended:
                snprintf(line_buf, LINE_BUF_CAPACITY - 1, "Server has shut down.");
                break;
            default:
                line_buf[0] = '\0';
        }
        
        mvwaddnstr(ui->chat_history_wnd, line_y_coord, 0, line_buf, get_chat_width());
        wprintw(ui->chat_history_wnd, "\n");

        line_y_coord++;
    }

    wrefresh(ui->chat_history_wnd);
}

void ui_print_user_list(struct chat_ui *ui, const struct user *users, size_t user_count) {
    wclear(ui->user_list_wnd);
    
    int height = get_user_list_height();
    
    int line_y_coord = 0;
    for (size_t usr_id = 0; usr_id < user_count; usr_id++) {
        if (line_y_coord >= height) {
            break;
        }

        const struct user *usr = &users[usr_id];
        if (!usr->is_connected) {
            // Пропускаем ранее отсоединившихся от чата пользователей
            continue;
        }
        
        mvwaddnstr(ui->user_list_wnd, line_y_coord, 0, usr->name, get_user_list_width());
        wprintw(ui->user_list_wnd, "\n");

        line_y_coord++;
    }

    wrefresh(ui->user_list_wnd);
}

void ui_print_exit_prompt(struct chat_ui *ui) {
    wclear(ui->prompt_wnd);

    mvwaddnstr(ui->prompt_wnd, 0, 0, "Press 'q' to exit", get_screen_width());

    wrefresh(ui->prompt_wnd);
}

void ui_print_text_prompt(struct chat_ui *ui) {
    wclear(ui->prompt_wnd);

    mvwaddnstr(ui->prompt_wnd, 0, 0, "Submit your message or type 'exit' to quit", get_screen_width());

    wrefresh(ui->prompt_wnd);
}
