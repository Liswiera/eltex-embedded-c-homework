#include "protocol.h"

int send_string(int queue_id, const void *msgp, size_t str_len, int msgflg) {
    int status = msgsnd(queue_id, msgp, str_len, msgflg);

    if (status == -1) {
        fprintf(stderr, "Не удалось передать строку через очередь сообщений.\n");
    } else {
        const char* str_ptr = ((const struct message*)msgp)->mtext;
        printf("Строка '%s' была успешно записана в очередь.\n", str_ptr);
    }

    return status;
}

ssize_t receive_string(int queue_id, void *msgp, size_t buf_len, long msgtyp, int msgflg) {
    char *mtext = (char*)(msgp + sizeof(long));

    ssize_t bytes_read = msgrcv(queue_id, msgp, buf_len - 1, msgtyp, msgflg);

    if (bytes_read == -1) {
        fprintf(stderr, "Не удалось прочитать строку из очереди.\n");
    }
    else {
        mtext[bytes_read] = '\0';
        printf("Была получена строка '%s'.\n", mtext);
    }
    
    return bytes_read;
}
