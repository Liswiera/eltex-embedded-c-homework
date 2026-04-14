#include <locale.h>
#include <curses.h>
#include <pthread.h>

#include "history.h"
#include "protocol.h"
#include "ui.h"
#include "user.h"

#define BUF_LEN 256
#define HISTORY_LEN 1024
#define MAX_USER_COUNT 8


struct chat_ui *server_ui;

static void broadcast_msg(const struct user *users, size_t user_count, const struct message *msg) {
    for (size_t id = 0; id < user_count; id++) {
        const struct user *usr = &users[id];
        if (usr->is_connected) {
            msg_cell_send_message(usr->cell, msg);
        }
    }
}

void* thread_listener(void *arg) {
    struct message_cell *server_cell = (struct message_cell*)arg;

    struct history *hist = history_with_capacity(HISTORY_LEN);
    if (hist == NULL) {
        //printw("Не удалось создать историю сообщений.\n");
        exit(1);
    }

    struct user users[MAX_USER_COUNT];
    size_t user_count = 0;

    struct message msg; // структура для получения/отправки сообщений
    int keep_listening = 1;
    
    while (keep_listening) {
        msg_cell_recv_message(server_cell, &msg);

        struct user *usr;
        int add_message_to_history = 0;

        switch (msg.type) {
            case connection_request:
                char user_shm_name_buf[BUF_LEN];
                get_user_shm_from_name(user_shm_name_buf, msg.user_name);

                int client_shm_fd = shm_open(user_shm_name_buf, O_RDWR, 0600);
                if (client_shm_fd == -1) {
                    //fprintf(stderr, "Не удалось открыть объект разделяемой памяти '%s'.\n", user_shm_name_buf);
                    break;
                }

                struct message_cell *client_cell = mmap(0, MSG_CELL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, client_shm_fd, 0);
                if (client_cell == MAP_FAILED) {
                    //fprintf(stderr, "Не удалось получить указатель на разделяемую память клиента.\n");
                    close(client_shm_fd);
                    break;
                }
                msg_cell_init(client_cell);

                int new_user_connected = 0;

                usr = user_find_by_name(users, user_count, msg.user_name);
                if (usr != NULL) {
                    // Был найден уже существующий пользователь, пробуем подсоединить его обратно в чат
                    
                    if (!usr->is_connected) {
                        user_connect(usr, client_cell);
                        new_user_connected = 1;
                    }
                    else {
                        //printw("[ERROR] Пользователь с именем '%s' уже подключён.\n", msg.user_name);
                    }
                }
                else {
                    // Выделяем место под нового пользователя

                    if (user_count < MAX_USER_COUNT) {
                        usr = &users[user_count];
                        int user_init_status = user_init(usr, msg.user_name, 1, client_cell);

                        if (user_init_status == 0) {
                            user_count++;
                            new_user_connected = 1;
                        }
                        else {
                            //printw("[ERROR] Не удалось инициализировать структуру пользователя с именем '%s'.\n", user_queue_name_buf);
                        }
                    }
                    else {
                        //printw("[WARN] Количество пользователей достигнуло максимума.\n");
                    }
                }


                if (new_user_connected) {
                    // Уведомить всех пользователей о подключении нового
                    msg.type = user_connected;
                    add_message_to_history = 1;

                    ui_print_user_list(server_ui, users, user_count);
                }
                else {
                    // Уведомить пользователя о невозможности его обслуживания
                    message_set(&msg, session_ended, "Server", "Internal Error.");
                    msg_cell_send_message(client_cell, &msg);

                    munmap(client_cell, MSG_CELL_SIZE);
                }

                close(client_shm_fd);
                break;
            case disconnection_request:
                usr = user_find_by_name(users, user_count, msg.user_name);
                if (usr != NULL) {
                    if (usr->is_connected) {
                        user_disconnect(usr);

                        // Уведомить всех пользователей об отсоединении
                        msg.type = user_disconnected;
                        add_message_to_history = 1;

                        ui_print_user_list(server_ui, users, user_count);
                    }
                    else {
                        //printw("[ERROR] Пользователь с именем '%s' уже отсоединён.\n", msg.user_name);
                    }
                }
                else {
                    //printw("[ERROR] Пользователь с именем '%s' не найден.\n", msg.user_name);
                }

                break;
            case message_from_user:
                // Переслать сообщение всем пользователям
                add_message_to_history = 1;
                break;
            case session_ended:
                keep_listening = 0;
                break;
            default:
                //printw("[WARN] Было получено сообщение с неверным типом из server_listener_queue.\n");
        }

        if (add_message_to_history) {
            int is_added = history_add_message(hist, &msg);
            if (is_added) {
                ui_print_chat_history(server_ui, hist);
            }
            else {
                //printw("[ERROR] На сервере превышено количество сообщений в истории.\n");
            }

            //printw("%s: %s\n", msg.user_name, msg.text);
        }

        // Отправляем оставшиеся сообщения пользователям, которые они ещё не получили
        for (size_t usr_id = 0; usr_id < user_count; usr_id++) {
            struct user *usr = &users[usr_id];

            if (!usr->is_connected) {
                continue;
            }

            for (size_t msg_id = usr->messages_sent; msg_id < hist->msg_count; msg_id++) {
                const struct message *msg_to_send = history_get_message(hist, msg_id);
                msg_cell_send_message(usr->cell, msg_to_send);
            }
            usr->messages_sent = hist->msg_count;
        }
    }

    // Посылаем каждому подключенному пользователю сообщение о завершении работы сервера
    // Это сообщение не является частью истории сообщений, поэтому оно будет отправлено
    // вне зависимости от переполненности истории
    message_set(&msg, session_ended, "Server", "Server is shutting down.");
    broadcast_msg(users, user_count, &msg);

    // Высвобождение ресурсов у массива пользователей
    for (size_t id = 0; id < user_count; id++) {
        user_destroy(&users[id]);
    }

    return NULL;
}

static void init_curses() {
    initscr();

    cbreak();
    noecho();
    curs_set(0);
}

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");
    
    int server_shm_fd = shm_open(SERVER_LISTENER_SHM, O_CREAT | O_RDWR, 0600);
    if (server_shm_fd == -1) {
        fprintf(stderr, "Не удалось открыть объект разделяемой памяти '%s'.\n", SERVER_LISTENER_SHM);
        return 1;
    }
    if (ftruncate(server_shm_fd, MSG_CELL_SIZE) == -1) {
        fprintf(stderr, "Не удалось изменить размер разделяемой памяти '%s'\n", SERVER_LISTENER_SHM);

        close(server_shm_fd);
        shm_unlink(SERVER_LISTENER_SHM);
        return 2;
    }

    struct message_cell *server_cell = mmap(NULL, MSG_CELL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, server_shm_fd, 0);
    if (server_cell == MAP_FAILED) {
        fprintf(stderr, "Не удалось получить указатель на разделяемую память.\n");

        close(server_shm_fd);
        shm_unlink(SERVER_LISTENER_SHM);
        return 3;
    }
    msg_cell_init(server_cell);

    // Создание визуальной части приложения
    init_curses();
    server_ui = chat_ui_create();
    if (server_ui == NULL) {
        msg_cell_destroy(server_cell);
        munmap(server_cell, MSG_CELL_SIZE);
        close(server_shm_fd);
        shm_unlink(SERVER_LISTENER_SHM);

        endwin();

        fprintf(stderr, "Не удалось создать subwindow для чата.\n");
        return 2;
    }
    touchwin(stdscr);

    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, thread_listener, server_cell);

    // Главный поток ожидает ввода 'q', после которого работа сервера завершается
    ui_print_exit_prompt(server_ui);
    
    int ch;
    do {
        ch = wgetch(server_ui->prompt_wnd);
    } while (ch != 'q');

    // Посылаем сообщение потоку listener_thread о завершении сеанса
    struct message msg;
    message_set(&msg, session_ended, "Server main thread", "Message for the listener thread.");
    msg_cell_send_message(server_cell, &msg);

    pthread_join(listener_thread, NULL);

    // Cleanup
    msg_cell_destroy(server_cell);
    munmap(server_cell, MSG_CELL_SIZE);
    close(server_shm_fd);
    shm_unlink(SERVER_LISTENER_SHM);

    chat_ui_free(server_ui);
    endwin();
    return 0;
}
