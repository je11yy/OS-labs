#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

enum ERRORS
{
    success = 0,
    incorrect_input = -1,
    incorrect_console_input = -2,
    file_error = -3,
    no_memory = -4
};

void print_error(int error);

int input_binary(char *file_string);

int read_binary_file(char *file_string);

int file_fseek(char *file_string, unsigned char new_buffer[], int length);

void print_states_file(FILE* file);

////////////////////////////////////////////

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        print_error(incorrect_input);
        return 0;
    }

    // запись последовательности
    int result = input_binary(argv[1]);
    if (result == file_error)
    {
        print_error(file_error);
        return 0;
    }

    //вывод файла в консоль
    result = read_binary_file(argv[1]);
    if (result == file_error)
    {
        print_error(file_error);
        return 0;
    }

    //смена указателя, запись в буффер 4 байтов и вывод
    int length = 4;
    unsigned char buffer[length];
    result = file_fseek(argv[1], buffer, length);
    if (result == file_error)
    {
        print_error(file_error);
        return 0;
    }

    printf("Buffer from 3d to 7th elements:\n");
    for (int i = 0; i < length; ++i)
    {
        printf("%u ", buffer[i]);
    }
    printf("\n");

    printf("\nProgram has finished!");
    return 0;
}

////////////////////////////////////////////

int file_fseek(char *file_string, unsigned char new_buffer[], int length)
{
    // seek_set - начало
    // seek_cur - текущая
    // seek_end - конец
    FILE* file = fopen(file_string, "rb");
    if (!file) return file_error;

    // offset - 3 - в байтах
    int result = fseek(file, 3, SEEK_SET);
    if (result != 0) return file_error;

    fread(new_buffer, sizeof(unsigned char), length * sizeof(unsigned char), file);

    fclose(file);
    return success;
}

int read_binary_file(char *file_string)
{
    FILE* file = fopen(file_string, "rb");
    if (!file) return file_error;

    unsigned char buffer;
    while (fread(&buffer, sizeof(unsigned char), 1, file) == 1)
    {
        // вывод из файла
        printf("element: %u\n\n", buffer);

        // состояние полей
        printf("FILE STATES:\n");
        print_states_file(file);
    }
    fclose(file);
    return success;
}

void print_states_file(FILE* file)
{
    char *temp = file -> _base; // buffer
    size_t length = strlen(temp);
    printf("base: ");
    for (int i = 0; i < length; ++i)
    {
            printf("%u ", temp[i]);
    }
    printf("\n");
    printf("bufsiz: %d\n", file -> _bufsiz); //buffer size
    printf("charbuf: %d\n", file -> _charbuf); // ??
    printf("cnt: %d\n", file -> _cnt); // number of available characters in buffer
    printf("file: %d\n", file -> _file); // file descriptor
    printf("flag: %d\n", file -> _flag); // the state of the stream
    printf("ptr: %u\n", *(file -> _ptr)); // указатель на следующий элемень
    printf("tmpfname: %s\n\n", file -> _tmpfname); // имя временного файла?
}

int input_binary(char *file_string)
{
    FILE* file = fopen(file_string, "wb");
    if (!file) return file_error;

    int size = 11, count;
    unsigned char data[11] = { 3, 1, 4, 1, 5, 9, 2, 6, 5, 3, 5 };

    count = fwrite(data, sizeof(unsigned char), size, file);
    // возвращает количество записанных функцией элементов. 
    // если меньше изначального колва, то была ошибка

    fclose(file);
    if (count < size) return file_error;
    return success;
}

void print_error(int error)
{
    switch (error)
    {
        case file_error:
            printf("\nFile Error!\n");
            break;
        case incorrect_input:
            printf("\nIncorrect input!\n");
            break;
        case incorrect_console_input:
            printf("\nIncorrect console input!\n");
            break;
        case no_memory:
            printf("\nComputer doesn't have enough memory for this calculation\n");
            break;
    }
    return;
}