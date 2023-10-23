#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <stdlib.h>

enum PHILOSOPHERS
{
    philos = 5,
    thinking = 1,
    eating = 2
};

sem_t forks[philos];
void * philosophers_problem(void * arg);

int main()
{
    pthread_t philosophers[philos];
    int philosophers_id[philos] = {0, 1, 2, 3, 4};

    for (int i = 0; i < philos; ++i)
    {
        sem_init(&forks[i], 0, 1); // инициализирует безымянный семафор, 0 -> доступен в нитях процесса
    }
    for (int i = 0; i < philos; ++i)
    {
        pthread_create(&philosophers[i], NULL, philosophers_problem, &philosophers_id[i]); // создание потока для каждого философа
    }
    for (int i = 0; i < philos; ++i)
    {
        pthread_join(philosophers[i], NULL); // позволяет нити дождаться завершения другой нити
    }

    for (int i = 0; i < philos; ++i)
    {
        sem_destroy(&forks[i]); // уничтожение семафоров
    }

    return 0;
}

void * philosophers_problem(void * arg)
{
    int id = *(int*)arg; // индекс философов
    int left_fork = id, right_fork = (id + 1) % philos;

    while (1)
    {
        printf("Philosopher %d is thinking\n", id);
        sleep(thinking);
        printf("Philosopher %d is hungry\n", id);

        sem_wait(&forks[left_fork]); // уменьшает семафор. если значение семафора равно нулю, вызов блокируется до тех пор, пока не станет возможным выполнить уменьшение
        printf("Philosopher %d takes left fork\n", id);

        sem_wait(&forks[right_fork]);
        printf("Philosopher %d takes right fork\n", id);

        printf("Philosopher %d is eating\n", id);
        sleep(eating);

        sem_post(&forks[left_fork]); // повышает (?) семафор
        sem_post(&forks[right_fork]);
    }
}