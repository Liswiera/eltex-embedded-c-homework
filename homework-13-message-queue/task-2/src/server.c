#include <locale.h>
#include <pthread.h>

#include "message_types.h"
#include "queue_pair.h"
#include "history.h"
#include "user.h"

#define BUF_LEN 256
#define HISTORY_LEN 1024
#define MAX_USER_COUNT 8


static void broadcast_msg(const struct user *users, size_t user_count, const struct message *msg) {
    for (size_t id = 0; id < user_count; id++) {
        const struct user *usr = &users[id];
        if (usr->is_connected) {
            mq_send(users[id].queue_id, (char*)msg, MQ_MSGSIZE, 0);
        }
    }
}

void* thread_listener(void *arg) {
    mqd_t server_listener_queue = *(mqd_t*)arg;

    struct history *hist = history_with_capacity(HISTORY_LEN);
    if (hist == NULL) {
        fprintf(stderr, "Не удалось создать историю сообщений.\n");
        exit(1);
    }

    struct user users[MAX_USER_COUNT];
    size_t user_count = 0;

    struct message msg; // структура для получения/отправки сообщений
    int keep_listening = 1;
    
    printf("[INFO] Жду сообщения от пользователей...\n");
    while (keep_listening) {
        ssize_t bytes_read = mq_receive(server_listener_queue, (char*)&msg, MQ_MSGSIZE, NULL);
        if (bytes_read == -1) {
            fprintf(stderr, "[ERROR] Не удалось прочитать сообщение из server_listener_queue.\n");
            continue;
        }

        struct user *usr;
        int add_message_to_history = 0;

        switch (msg.type) {
            case connection_request:
                char user_queue_name_buf[BUF_LEN];
                get_user_queue_from_name(user_queue_name_buf, msg.user_name);

                mqd_t user_queue = mq_open(user_queue_name_buf, O_WRONLY);
                if (user_queue == (mqd_t)-1) {
                    fprintf(stderr, "[ERROR] Не удалось открыть очередь '%s' на запись.\n", user_queue_name_buf);
                    break;
                }

                int new_user_connected = 0;

                usr = user_find_by_name(users, user_count, msg.user_name);
                if (usr != NULL) {
                    // Был найден уже существующий пользователь, пробуем подсоединить его обратно в чат
                    
                    if (!usr->is_connected) {
                        user_connect(usr, user_queue);
                        new_user_connected = 1;
                    }
                    else {
                        fprintf(stderr, "[ERROR] Пользователь с именем '%s' уже подключён.\n", msg.user_name);
                    }
                }
                else {
                    // Выделяем место под нового пользователя

                    if (user_count < MAX_USER_COUNT) {
                        usr = &users[user_count];
                        int user_init_status = user_init(usr, msg.user_name, 1, user_queue);

                        if (user_init_status == 0) {
                            user_count++;
                            new_user_connected = 1;
                        }
                        else {
                            fprintf(stderr, "[ERROR] Не удалось инициализировать структуру пользователя с именем '%s'.\n", user_queue_name_buf);
                        }
                    }
                    else {
                        printf("[WARN] Количество пользователей достигнуло максимума.\n");
                    }
                }


                if (new_user_connected) {
                    // Уведомить всех пользователей о подключении нового
                    msg.type = user_connected;
                    add_message_to_history = 1;
                }
                else {
                    // Уведомить пользователя о невозможности его обслуживания
                    message_set(&msg, session_ended, "Server", "Internal Error.");
                    mq_send(user_queue, (char*)&msg, MQ_MSGSIZE, 0); // Return value is ignored

                    mq_close(user_queue);
                    // Queue must be unlinked by the user
                }

                break;
            case disconnection_request:
                usr = user_find_by_name(users, user_count, msg.user_name);
                if (usr != NULL) {
                    if (usr->is_connected) {
                        user_disconnect(usr);

                        // Уведомить всех пользователей об отсоединении
                        msg.type = user_disconnected;
                        add_message_to_history = 1;
                    }
                    else {
                        fprintf(stderr, "[ERROR] Пользователь с именем '%s' уже отсоединён.\n", msg.user_name);
                    }
                }
                else {
                    fprintf(stderr, "[ERROR] Пользователь с именем '%s' не найден.\n", msg.user_name);
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
                printf("[WARN] Было получено сообщение с неверным типом из server_listener_queue.\n");
        }

        if (add_message_to_history) {
            int is_added = history_add_message(hist, &msg);
            if (!is_added) {
                printf("[ERROR] На сервере превышено количество сообщений в истории.\n");
            }

            printf("%s: %s\n", msg.user_name, msg.text);
        }

        // Отправляем оставшиеся сообщения пользователям, которые они ещё не получили
        for (size_t usr_id = 0; usr_id < user_count; usr_id++) {
            struct user *usr = &users[usr_id];

            if (!usr->is_connected) {
                continue;
            }

            for (size_t msg_id = usr->messages_sent; msg_id < hist->msg_count; msg_id++) {
                const struct message *msg_to_send = history_get_message(hist, msg_id);
                mq_send(usr->queue_id, (char*)msg_to_send, MQ_MSGSIZE, 0); // Return value is ignored
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

int main() {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    // server_listener_queue:
    // ожидает входящие сообщения от пользователей и главного потока сервера
    struct queue_pair server_listener_queue;
    int status = create_queue_pair(SERVER_LISTENER_QUEUE, &server_listener_queue);
    if (status != 0) {
        return 1;
    }

    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, thread_listener, &server_listener_queue.read_end);


    // TODO: use ncurses, wait until 'q' is pressed
    sleep(15);


    // Посылаем сообщение потоку listener_thread о завершении сеанса
    struct message msg;
    message_set(&msg, session_ended, "Server main thread", "Message for the listener thread.");
    mq_send(server_listener_queue.write_end, (char*)&msg, MQ_MSGSIZE, 0);

    pthread_join(listener_thread, NULL);

    // Cleanup
    destroy_queue_pair(&server_listener_queue);
    return 0;
}
