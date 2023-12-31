#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>

int main()
{
    printf("Part 2:\n\n");

    fork(); // создание родительского процесса
    printf("Hi\n");
    fork(); // создание точной копии родительского процесса - дочернего процесса
    printf("Hi\n");

    ////////////////////////////////
    //////      объяснение
    ////////////////////////////////
    //             p0            - изначальный процесс строчка 9
    //           /    \
    //          p1     hi        - p1 - копия p0, начинается со строчки 9
    //         /      /  \
    //        hi     p3   hi     - p2 - копия p1, начинается со строчки 11
    //      /   \     \
    //     hi    p2    hi        - p3 - копия p0, начинается со строчки 11
    //             \
    //              hi
    ////////////////////////////////
    // fork() - создает точную копию родительского процесса, который начинается с той же точки кода
}