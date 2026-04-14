#include <curses.h>
#include <locale.h>
#include <pthread.h>

#include "history.h"
#include "protocol.h"
#include "ui.h"
#include "user.h"

#define BUF_LEN 256
#define HISTORY_LEN 1024

#define MAX_USER_COUNT 8


const char *user_name;
struct chat_ui *client_ui;
int connected_to_server;

void* thread_listener(void *arg) {
    struct message_cell* client_cell = (struct message_cell*)arg;

    struct history *hist = history_with_capacity(HISTORY_LEN);
    if (hist == NULL) {
        //fprintf(stderr, "Не удалось создать историю сообщений.\n");
        exit(1);
    }

    struct user users[MAX_USER_COUNT];
    size_t user_count = 0;

    struct message msg; // структура для получения/отправки сообщений
    int keep_listening = 1;

    while (keep_listening) {
        msg_cell_recv_message(client_cell, &msg);

        struct user *usr;
        int add_message_to_history = 0;

        switch (msg.type) {
            case disconnection_request:
                keep_listening = 0;
                break;
            case user_connected:
                usr = user_find_by_name(users, user_count, msg.user_name);
                if (usr != NULL) {
                    // Был найден уже существующий пользователь
                    usr->is_connected = 1;
                    ui_print_user_list(client_ui, users, user_count);
                }
                else {
                    if (user_count < MAX_USER_COUNT) {
                        usr = &users[user_count];
                        int user_init_status = user_init(usr, msg.user_name, 1, NULL);

                        if (user_init_status == 0) {
                            user_count++;
                            ui_print_user_list(client_ui, users, user_count);
                        }
                        else {
                            //printw("[ERROR] Не удалось инициализировать структуру пользователя с именем '%s'.\n", user_queue_name_buf);
                        }
                    }
                }

                add_message_to_history = 1;
                break;
            case user_disconnected:
                usr = user_find_by_name(users, user_count, msg.user_name);
                if (usr != NULL) {
                    usr->is_connected = 0;
                    ui_print_user_list(client_ui, users, user_count);
                }
                else {
                    //printw("[ERROR] Пользователь с именем '%s' не найден.\n", msg.user_name);
                }

                add_message_to_history = 1;
                break;
            case message_from_user:
            case message_from_server:
                add_message_to_history = 1;
                break;
            case session_ended:
                connected_to_server = 0;

                add_message_to_history = 1;
                keep_listening = 0;
                break;
            default:
                //printf("[WARN] Было получено сообщение с неверным типом из user_listener_queue.\n");
        }

        if (add_message_to_history) {
            int is_added = history_add_message(hist, &msg);
            if (is_added) {
                ui_print_chat_history(client_ui, hist);
            }
            else {
                //printf("[ERROR] У клента превышено количество сообщений в истории.\n");
            }
        }
    }

    return NULL;
}

static void init_curses() {
    initscr();

    curs_set(0);
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        printf("Usage: ./server [USER_NAME]\n");
        return 1;
    }
    user_name = argv[1];

    // Соединение с сервером
    int server_shm_fd = shm_open(SERVER_LISTENER_SHM, O_RDWR, 0600);
    if (server_shm_fd == -1) {
        fprintf(stderr, "Не удалось открыть объект разделяемой памяти '%s'.\n", SERVER_LISTENER_SHM);
        return 2;
    }

    struct message_cell *server_cell = mmap(0, MSG_CELL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, server_shm_fd, 0);
    if (server_cell == MAP_FAILED) {
        fprintf(stderr, "Не удалось получить указатель на разделяемую память сервера.\n");

        close(server_shm_fd);
        shm_unlink(SERVER_LISTENER_SHM);
        return 2;
    }
    connected_to_server = 1;

    // Создание клиентского объекта разделяемой памяти, из которой клиент будет получать сообщения от сервера
    char user_shm_name_buf[BUF_LEN];
    get_user_shm_from_name(user_shm_name_buf, user_name);

    int client_shm_fd = shm_open(user_shm_name_buf, O_CREAT | O_RDWR, 0600);
    if (client_shm_fd == -1) {
        fprintf(stderr, "Не удалось открыть объект разделяемой памяти '%s'.\n", user_shm_name_buf);

        close(server_shm_fd);
        shm_unlink(SERVER_LISTENER_SHM);
        return 3;
    }
    if (ftruncate(client_shm_fd, MSG_CELL_SIZE) == -1) {
        fprintf(stderr, "Не удалось изменить размер разделяемой памяти '%s'\n", user_shm_name_buf);

        close(client_shm_fd);
        shm_unlink(user_shm_name_buf);
        return 3;
    }

    struct message_cell *client_cell = mmap(0, MSG_CELL_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, client_shm_fd, 0);
    if (client_cell == MAP_FAILED) {
        fprintf(stderr, "Не удалось получить указатель на разделяемую память клиента.\n");
        return 3;
    }
    msg_cell_init(client_cell);

    // Создание визуальной части приложения
    init_curses();
    client_ui = chat_ui_create();
    if (client_ui == NULL) {
        msg_cell_destroy(client_cell);
        munmap(client_cell, MSG_CELL_SIZE);
        close(client_shm_fd);
        shm_unlink(user_shm_name_buf);

        munmap(server_cell, MSG_CELL_SIZE);
        close(server_shm_fd);

        endwin();

        fprintf(stderr, "Не удалось создать subwindow для чата.\n");
        return 4;
    }
    touchwin(stdscr);

    // Говорим серверу, что мы присоединяемся к чату
    struct message msg;
    message_set(&msg, connection_request, user_name, "I have joined the chat!");
    msg_cell_send_message(server_cell, &msg);

    // Начинаем прослушивание сообщений от сервера
    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, thread_listener, client_cell);
    
    char input_buf[BUF_LEN];
    while (1) {
        ui_print_text_prompt(client_ui);

        wmove(client_ui->prompt_wnd, 1, 0);
        wgetnstr(client_ui->prompt_wnd, input_buf, BUF_LEN - 1);

        if (strlen(input_buf) == 0) {
            continue;
        }

        if (strcmp(input_buf, "exit") == 0) {
            break;
        }
        else {
            message_set(&msg, message_from_user, user_name, input_buf);
            msg_cell_send_message(server_cell, &msg);
        }
    }

    if (connected_to_server) {
        // Посылаем сообщение потоку listener_thread о завершении сеанса
        message_set(&msg, disconnection_request, user_name, "Left the chat.");
        msg_cell_send_message(client_cell, &msg);

        // Уведомляем сервер об отключении
        msg_cell_send_message(server_cell, &msg);
    }

    pthread_join(listener_thread, NULL);
    
    // Cleanup
    msg_cell_destroy(client_cell);
    munmap(client_cell, MSG_CELL_SIZE);
    close(client_shm_fd);
    shm_unlink(user_shm_name_buf);

    munmap(server_cell, MSG_CELL_SIZE);
    close(server_shm_fd);

    chat_ui_free(client_ui);
    endwin();
    return 0;
}
