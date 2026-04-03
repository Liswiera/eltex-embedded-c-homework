#include <locale.h>
#include <pthread.h>
#include "history.h"
#include "message_types.h"
#include "queue_pair.h"

#define BUF_LEN 256
#define HISTORY_LEN 1024


const char *u_name; // TODO: TEMPORARY GLOBAL VARIABLE

void* thread_listener(void *arg) {
    mqd_t user_listener_queue = *(mqd_t*)arg;

    struct history *hist = history_with_capacity(HISTORY_LEN);
    if (hist == NULL) {
        fprintf(stderr, "Не удалось создать историю сообщений.\n");
        exit(1);
    }

    struct message msg; // структура для получения/отправки сообщений
    int keep_listening = 1;
    size_t messages_shown = 0;

    while (keep_listening) {
        ssize_t bytes_read = mq_receive(user_listener_queue, (char*)&msg, MQ_MSGSIZE, NULL);
        if (bytes_read == -1) {
            fprintf(stderr, "[ERROR] Не удалось прочитать сообщение из user_listener_queue.\n");
            continue;
        }

        int add_message_to_history = 0;
        switch (msg.type) {
            case disconnection_request:
                keep_listening = 0;
                break;
            case user_connected:
            case user_disconnected:
            case message_from_user:
            case message_from_server:
                add_message_to_history = 1;
                break;
            case session_ended:
                add_message_to_history = 1;
                keep_listening = 0;
                break;
            default:
                printf("[WARN] Было получено сообщение с неверным типом из user_listener_queue.\n");
        }

        if (add_message_to_history) {
            int is_added = history_add_message(hist, &msg);
            if (!is_added) {
                printf("[ERROR] У клента превышено количество сообщений в истории.\n");
            }
        }

        // Выводим новые сообщения на экран
        for (size_t msg_id = messages_shown; msg_id < hist->msg_count; msg_id++) {
            const struct message *msg_to_show = history_get_message(hist, msg_id);
            printf("%s: %s\n", msg_to_show->user_name, msg_to_show->text);
        }
        messages_shown = hist->msg_count;
    }

    return NULL;
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        printf("Usage: ./server [USER_NAME]\n");
        return 1;
    }
    const char *user_name = argv[1];
    u_name = user_name; // TODO: PLACEHOLDER

    mqd_t server_queue = mq_open(SERVER_LISTENER_QUEUE, O_WRONLY);
    if (server_queue == (mqd_t)-1) {
        fprintf(stderr, "Не удалось открыть очередь '%s' на запись.\n", SERVER_LISTENER_QUEUE);
        return 2; 
    }

    // Создание пользовательской очереди, из которой пользователь будет получать сообщения от сервера
    char user_queue_name_buf[BUF_LEN];
    get_user_queue_from_name(user_queue_name_buf, user_name);

    struct mq_attr attributes;
    set_queue_creat_attributes(&attributes);

    struct queue_pair user_listener_queue;
    int status = create_queue_pair(user_queue_name_buf, &user_listener_queue);
    if (status != 0) {
        return 1;
    }

    struct message msg;
    message_set(&msg, connection_request, user_name, "I have joined the chat!");
    mq_send(server_queue, (char*)&msg, MQ_MSGSIZE, 0); // Return value is ignored

    pthread_t listener_thread;
    pthread_create(&listener_thread, NULL, thread_listener, &user_listener_queue.read_end);


    // TODO: use ncurses, wait until 'q' is pressed
    // Must also be able to send messages to other users
    sleep(1);
    message_set(&msg, message_from_user, user_name, "Hello everyone!!!!");
    mq_send(server_queue, (char*)&msg, MQ_MSGSIZE, 0);
    sleep(3);
    message_set(&msg, message_from_user, user_name, "How are you doing?");
    mq_send(server_queue, (char*)&msg, MQ_MSGSIZE, 0);
    sleep(3);
    message_set(&msg, message_from_user, user_name, "Alright, bye!");
    mq_send(server_queue, (char*)&msg, MQ_MSGSIZE, 0);
    sleep(1);


    // Посылаем сообщение потоку listener_thread о завершении сеанса
    message_set(&msg, disconnection_request, user_name, "Left the chat.");
    mq_send(user_listener_queue.write_end, (char*)&msg, MQ_MSGSIZE, 0);
    pthread_join(listener_thread, NULL);

    // Уведомляем сервер об отключении
    // TODO: отправляем только если пользователь сам завершил работу
    mq_send(server_queue, (char*)&msg, MQ_MSGSIZE, 0);

    // Cleanup
    mq_close(server_queue);
    destroy_queue_pair(&user_listener_queue);
    return 0;
}
