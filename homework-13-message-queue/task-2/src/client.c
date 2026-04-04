#include <curses.h>
#include <locale.h>
#include <pthread.h>

#include "history.h"
#include "message_types.h"
#include "queue_pair.h"
#include "ui.h"
#include "user.h"

#define BUF_LEN 256
#define HISTORY_LEN 1024

#define MAX_USER_COUNT 8


const char *user_name;
struct chat_ui *client_ui;
int connected_to_server;

void* thread_listener(void *arg) {
    mqd_t user_listener_queue = *(mqd_t*)arg;

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
        ssize_t bytes_read = mq_receive(user_listener_queue, (char*)&msg, MQ_MSGSIZE, NULL);
        if (bytes_read == -1) {
            //fprintf(stderr, "[ERROR] Не удалось прочитать сообщение из user_listener_queue.\n");
            continue;
        }

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
                        int user_init_status = user_init(usr, msg.user_name, 1, (mqd_t)-1);

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

    cbreak();
    noecho();
    curs_set(0);
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        printf("Usage: ./server [USER_NAME]\n");
        return 1;
    }
    user_name = argv[1];

    mqd_t server_queue = mq_open(SERVER_LISTENER_QUEUE, O_WRONLY);
    if (server_queue == (mqd_t)-1) {
        fprintf(stderr, "Не удалось открыть очередь '%s' на запись.\n", SERVER_LISTENER_QUEUE);
        return 2; 
    }
    connected_to_server = 1;

    // Создание пользовательской очереди, из которой пользователь будет получать сообщения от сервера
    char user_queue_name_buf[BUF_LEN];
    get_user_queue_from_name(user_queue_name_buf, user_name);

    // user_listener_queue:
    // ожидает входящие сообщения от сервера и главного потока клиента
    struct queue_pair user_listener_queue;
    int status = create_queue_pair(user_queue_name_buf, &user_listener_queue);
    if (status != 0) {
        mq_close(server_queue);
        return 1;
    }

    // Создание визуальной части приложения
    init_curses();
    client_ui = chat_ui_create();
    if (client_ui == NULL) {
        destroy_queue_pair(&user_listener_queue);
        mq_close(server_queue);
        endwin();

        fprintf(stderr, "Не удалось создать subwindow для чата.\n");
        return 2;
    }
    touchwin(stdscr);

    // Говорим серверу, что мы присоединяемся к чату
    struct message msg;
    message_set(&msg, connection_request, user_name, "I have joined the chat!");
    mq_send(server_queue, (char*)&msg, MQ_MSGSIZE, 0); // Return value is ignored

    // Начинаем прослушивание сообщений от сервера
    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, thread_listener, &user_listener_queue.read_end);

    
    int ch;
    do {
        ch = wgetch(client_ui->prompt_wnd);

        message_set(&msg, message_from_user, user_name, "Message sent.");
        mq_send(server_queue, (char*)&msg, MQ_MSGSIZE, 0);
    } while (ch != 'q');

    if (connected_to_server) {
        // Посылаем сообщение потоку listener_thread о завершении сеанса
        message_set(&msg, disconnection_request, user_name, "Left the chat.");
        mq_send(user_listener_queue.write_end, (char*)&msg, MQ_MSGSIZE, 0);

        // Уведомляем сервер об отключении
        mq_send(server_queue, (char*)&msg, MQ_MSGSIZE, 0);
    }

    pthread_join(listener_thread, NULL);

    // Cleanup
    mq_close(server_queue);
    destroy_queue_pair(&user_listener_queue);
    chat_ui_free(client_ui);
    endwin();
    return 0;
}
