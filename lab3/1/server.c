#include "message.h"

enum ERRORS
{
    success = 0,
    fail = -1,
    file_open_error = -2,
    incorrect_input = -3
};

int server_queue, client_queue;

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
    int i = 0;
    while (i < length)
    {
        prev = cur;
        cur = tmp[i];
        if (prev == cur && (cur == ' ' || cur == '\t' || cur == '\n'))
        {
            for (int j = i; j < length; ++j)
            {
                tmp[j] = tmp[j + 1];
            }
        }
        else i++;
    }
    strcpy(msg.text, tmp);
    msgsnd(queue_id, &msg, sizeof(msg), IPC_NOWAIT);
}

void delete_queues()
{
    int res = msgctl(server_queue, IPC_RMID, 0);
    if (res != -1) printf("Server queue was cleared.\n");
    else printf("Server queue doesn't exist! Failed to delete queue.\n");
    res = msgctl(client_queue, IPC_RMID, 0);
    if (res != -1) printf("Client queue was cleared.\n");
    else printf("Client queue doesn't exist! Failed to delete queue.\n");
}

int main()
{
    // создание клиентской и серверной очередей
    generate_queues(&server_queue, &client_queue);

    atexit(delete_queues);

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
        process_message(msg, client_queue);
        if (msg.type == 1)
        {
            printf("Server got the message with highest priority. Ending session...\n");
            sleep(2);
            break;
        }
    }

    printf("Server process is finished.\n");
    exit(EXIT_SUCCESS);
}