#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

enum ERRORS
{
    success = 0,
    incorrect_input = -1,
    file_open_error = -2,
    no_memory = -3,
    fail = -4,
    create_process_fail = -5
};

pthread_cond_t woman_condition;
pthread_cond_t man_condition;

int quantity = 0; // количество людей в ванной
char current_gender = 0;
pthread_mutex_t mutex;
int N = 5;

void bathroom_simulation(int man, int woman);
void * man_thread(void * argument);
void * woman_thread(void * argument);
void woman_wants_to_enter();
void man_wants_to_enter();
void woman_leaves();
void man_leaves();

int main()
{
    printf("Enter number of men and women who want to go to the bathroom.\n\n");
    int man, woman;
    printf("Women: ");
    scanf("%d", &woman);
    printf("\nMen: ");
    scanf("%d", &man);
    printf("\n");

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&woman_condition, NULL);
    pthread_cond_init(&man_condition, NULL);

    bathroom_simulation(man, woman);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&woman_condition);
    pthread_cond_destroy(&man_condition);

    return success;
}

// количество женщин и мужчин
void bathroom_simulation(int man, int woman)
{
    pthread_t women[woman];
    pthread_t men[man];
    int want_to_pipi = 1;

    for (int i = 0, j = 0; i < man, j < woman; ++i, ++j)
    {
        if (i >= 0) pthread_create(&men[i], NULL, man_thread, &want_to_pipi);
        if (j >= 0) pthread_create(&women[i], NULL, woman_thread, &want_to_pipi);
    }
    for (int i = 0, j = 0; i < man, j < woman; ++i, ++j)
    {
        if (i >= 0) pthread_join(men[i], NULL);
        if (j >= 0) pthread_join(women[i], NULL);
    }
}

void * man_thread(void * argument)
{
    int man_quantity = *((int *) argument);
    for (int i = 0; i < man_quantity; ++i)
    {
        man_wants_to_enter();
        man_leaves();
    }
    return NULL;
}

void * woman_thread(void * argument)
{
    int woman_quantity = *((int *) argument);
    for (int i = 0; i < woman_quantity; ++i)
    {
        woman_wants_to_enter();
        woman_leaves();
    }
    return NULL;
}

void woman_wants_to_enter()
{
    pthread_mutex_lock(&mutex); // к ресурсам имеет доступ только один поток, который владеет мьютексом. Он единственный, кто может его разблокировать
    while (current_gender == 'M' || quantity == N)
    {
        pthread_cond_wait(&woman_condition, &mutex); // блокирует нить, пока не выполнится условие
    }
    printf("Woman enters\n");
    current_gender = 'F';
    printf("Sign on the door: %c\n", current_gender);
    quantity++;
    printf("Current quantity of people in the bathroom: %d\n\n", quantity);
    pthread_mutex_unlock(&mutex);
}

void man_wants_to_enter()
{
    pthread_mutex_lock(&mutex); 
    while (current_gender == 'F' || quantity == N)
    {
        pthread_cond_wait(&man_condition, &mutex); // блокирует нить, пока не выполнится условие
    }
    printf("Man enters\n");
    current_gender = 'M';
    printf("Sign on the door: %c\n", current_gender);
    quantity++;
    printf("Current quantity of people in the bathroom: %d\n\n", quantity);
    pthread_mutex_unlock(&mutex);
}

void woman_leaves()
{
    pthread_mutex_lock(&mutex);
    quantity--;
    if (quantity == 0)
    {
        current_gender = 0;
        pthread_cond_broadcast(&man_condition); // разблокировка заблокированных нитей
    }
    printf("Woman leaves\n");
    printf("Current quantity of people in the bathroom: %d\n\n", quantity);
    pthread_mutex_unlock(&mutex);
}

void man_leaves()
{
    pthread_mutex_lock(&mutex);
    quantity--;
    if (quantity == 0)
    {
        current_gender = 0;
        pthread_cond_broadcast(&woman_condition); // разблокировка заблокированных нитей
    }
    printf("Man leaves\n");
    printf("Current quantity of people in the bathroom: %d\n\n", quantity);
    pthread_mutex_unlock(&mutex);
}