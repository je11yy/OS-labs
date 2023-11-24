#include "message.h"

enum ERRORS
{
    success = 0,
    fail = -1,
    file_open_error = -2,
    incorrect_input = -3
};

int is_diviser(char symbol)
{
    char * div = "!.,?";
    int len = strlen(div);
    for (int i = 0; i < len; ++i) if (symbol == div[i]) return 1;
    return 0;
}

void generate_queues(int * server_id, int * client_id)
{
    key_t server_key = ftok("server_key", 'r');
    key_t client_key = ftok("client_key", 'r');

    int server_queue = msgget(server_key, 0666 | IPC_CREAT);
    int client_queue = msgget(client_key, 0666 | IPC_CREAT);
    *server_id = server_queue;
    *client_id = client_queue;
}

void process_message(Message msg, int queue_id)
{
    int length = strlen(msg.text);
    char tmp[MAX_LENGTH];
    strcpy(tmp, msg.text);
    char prev, cur;
    for (int i = 0; i < length; ++i)
    {
        prev = cur;
        cur = tmp[i];
        if (prev == cur && (cur == ' ' || cur == '\t' || cur == '\n' || is_diviser(cur)))
        {
            for (int j = i; j < length; ++j)
            {
                tmp[j] = tmp[j + 1];
            }
        }
    }
    strcpy(msg.text, tmp);
    msgsnd(queue_id, &msg, sizeof(msg), IPC_NOWAIT);
}

int main()
{
    int server_queue, client_queue;
    // создание клиентской и серверной очередей
    generate_queues(&server_queue, &client_queue);

    printf("Server online...\n");

    while (1)
    {
        Message msg;
        // получаем в очередь сервера, отправляем на очередь клиента
        /*
            If msgtyp is less than 0, then the first message in the queue with 
            the lowest type less than or equal to the absolute value of msg‐typ will be read.
        */
        if (msgrcv(server_queue, &msg, sizeof(msg), -MAX_PRIORITY, IPC_NOWAIT) < 0) continue;
        printf("Server got the message...\n");
        if (msg.type == 1)
        {
            printf("Server got the message with highest priority. Ending session...\n");
            // отправляем так же сообщение с типом 0, чтобы завершить сессию клиента
            break;
        }
        process_message(msg, client_queue);
    }

    msgctl(server_queue, IPC_RMID, 0);
    printf("Server queue was cleared.\n");
    msgctl(client_queue, IPC_RMID, 0);
    printf("Client queue was cleared.\n");

    printf("Server process is finished.\n");
    return success;
}