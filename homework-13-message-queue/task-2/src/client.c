#include <locale.h>
#include "message_types.h"

#define BUF_LEN 256

static void print_usage() {
    printf("Usage: ./server [USER_NAME]\n");
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "ru_RU.UTF-8");

    if (argc < 2) {
        print_usage();
        return 1;
    }
    const char *user_name = argv[1];

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

    mqd_t user_queue = mq_open(user_queue_name_buf, O_CREAT | O_RDONLY, 0600, &attributes);
    if (user_queue == (mqd_t)-1) {
        fprintf(stderr, "Не удалось создать очередь '%s' на чтение.\n", user_queue_name_buf);
        return 3; 
    }


    struct message msg;
    message_set(&msg, connection_request, user_name, "I have joined the chat!");
    mq_send(user_queue, &msg, MQ_MSGSIZE, 0); // Return value is ignored

    

    return 0;
}
