#include <stdio.h>

// для процессов:
#include <unistd.h>
#include <sys/types.h>

int main()
{
    // реальный идентификатор - RID - кто создал
    // эффективный - EUID - от чьего лица выполняется
    pid_t current_pid = getpid(); // возвращает идентификатор тккущего процесса

    pid_t parent_pid = getppid(); // идентификатор родительского процесса

    pid_t calling_group_pid = getpgid(0); // идентификатор группы вызывающего процесса

    pid_t real_user_id = getuid(); // реальный идентификатор владельца

    pid_t real_group_id = getgid(); // реальный идентификатор группы владельца

    pid_t effective_user_id = geteuid(); // эффективный идентификатор пользователя

    pid_t effective_group_id = getegid(); // эффективный идентификатор группы владельца


    printf("\nCurrent process ID: %d\n\n", current_pid);
    printf("Parent process ID: %d\n\n", parent_pid);
    printf("Calling group process ID: %d\n\n", calling_group_pid);
    printf("Real user ID: %d\n\n", real_user_id);
    printf("Real user's group ID: %d\n\n", real_group_id);
    printf("Effective user ID: %d\n\n", effective_user_id);
    printf("Effective user's group ID: %d\n\n", effective_group_id);

    return 0;
}