#include "message.h"

void send_message(char text[MAX_LENGTH], int priority, int queue_id)
{
    Message msg;
    strcpy(msg.text, text);
    msg.type = priority;
    msgsnd(queue_id, &msg, sizeof(msg), 0);
    printf("Client sent a message...\n");
}

int main(int argc, char * argv[])
{
    if (argc < 2)
    {
        perror("Incorrect input.\n");
        exit(1);
    }

    int server_queue, client_queue;
    key_t server_key = ftok("server_key", 'r');
    key_t client_key = ftok("client_key", 'r');
    server_queue = msgget(server_key, 0); // подключение к СОЗДАННОЙ серверной очереди
    if (server_queue < 0)
    {
        perror("Server queue doesn't exist.\n");
        exit(1);
    }
    client_queue = msgget(client_key, 0); // подключение к СОЗДАННОЙ клиентской очереди
    if (client_queue < 0)
    {
        perror("Client queue doesn't exist.\n");
        exit(1);
    }

    FILE * file;
    char * line = NULL;
    size_t len = 0;
    int res;
    char text[MAX_LENGTH];
    int priority;

    int message_sent = 0;
    for (int i = 1; i < argc; ++i)
    {
        file = fopen(argv[i], "r");
        if (!file)
        {
            perror("Can't open file.\n");
            exit(1);
        }
        while ((res = getline(&line, &len, file)) != EOF)
        {
            sscanf(line, "prior=%d", &priority);
            int j = 0;
            int k = 0;
            while (line[k] != '=') k++;
            k++;
            while (line[k] != '=') k++;
            k += 2;
            while (j < MAX_LENGTH && line[k] != '\"')
            {
                text[j] = line[k];
                j++;
                k++;
            }
            text[j] = 0;
            send_message(text, priority, server_queue);
            message_sent++;
        }
        fclose(file);
    }
    sleep(1); // дать время серверу обработать все сообщения
    Message msg;
    while (message_sent)
    {
        client_queue = msgget(client_key, 0); // проверка, существует ли очереди
        if (client_queue < 0) break;
        /*
        If msgtyp is 0, then the first message in the queue is read.
        */
        if (msgrcv(client_queue, &msg, sizeof(msg), -MAX_PRIORITY, IPC_NOWAIT) < 0) continue;
        printf("Server answer: %s\nPriority: %ld\n", msg.text, msg.type);
        if (msg.type == 1)
        {
            printf("Server got message with highest priority. Ending session...\n");
            break;
        }
        message_sent--;
    }

    printf("Client process is finished.\n");

    return 0;
}