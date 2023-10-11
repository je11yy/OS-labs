#include <stdio.h>
#include <stdlib.h>

enum ERRORS
{
    success = 0,
    incorrect_input = -1,
    file_error = -2,
    no_memory = -3,
    file_input_error = -4
};

void print_error(int error);

int copy_file(FILE *input, FILE *output);

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        print_error(incorrect_input);
        return incorrect_input;
    }
    FILE *input = fopen(argv[1], "rb");
    if (!input)
    {
        print_error(file_error);
        return file_error;
    }
    FILE *output = fopen(argv[2], "wb");
    if (!output)
    {
        print_error(file_error);
        return file_error;
    }
    int result = copy_file(input, output);
    if (result == file_input_error)
    {
        print_error(result);
        return result;
    }
    printf("\nThe program has finished. Check the file.");
    return success;
}

int copy_file(FILE *input, FILE *output)
{
    char symbol;
    while(!feof(input))
    {
        if (ferror(input)) return file_input_error;
        symbol = fgetc(input);
        if (symbol == EOF) break;
        if (ferror(output)) return file_input_error;
        fputc(symbol, output);
    }
    return success;
}

void print_error(int error)
{
    switch (error)
    {
        case file_error:
            printf("\nCannot open file!\n");
            break;
        case incorrect_input:
            printf("\nIncorrect input!\n");
            break;
        case no_memory:
            printf("\nComputer doesn't have enough memory for this calculation\n");
            break;
        case file_input_error:
            printf("\nError with reading or writing in file\n");
            break;
    }
    return;
}