#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

enum ERRORS
{
    success = 0,
    incorrect_input = -1,
    file_error = -2,
    no_memory = -3,
    file_input_error = -4
};

enum FLAGS
{
    xor8 = 1,
    xor32 = 2,
    mask = 3
};

void print_error(int error);

int check_valid_flag(const char *flag);

int get_decimal_number(char* number, int base);

int make_decimal(int array[]);

int check_valid_mask(const char *mask_string);

long int xor_8(FILE *file);

int xor_32(FILE *file);

int find_mask(FILE* file, char *mask_string);

int main(int argc, unsigned char *argv[])
{
    FILE* input;
    int flag;
    int mask_int;
    long int result;
    if (argc == 3)
    {
        input = fopen(argv[1], "rb");
        if (!input)
        {
            print_error(file_error);
            return file_error;
        }

        flag = check_valid_flag(argv[2]);
        if (flag == mask || flag == incorrect_input)
        {
            fclose(input);
            print_error(incorrect_input);
            return incorrect_input;
        }
        else if (flag == xor8)
        {
            result = xor_8(input);
            if (result == file_input_error)
            {
                print_error(result);
                fclose(input);
                return result;
            }
            printf("Result for XOR-operation with all bytes in file: %li\n", result);
        }
        else
        {
            result = xor_32(input);
            if (result == file_input_error)
            {
                print_error(result);
                fclose(input);
                return result;
            }
            printf("Result for XOR-operation for 4-bytes groups in file: %d\n", result);
        }
    }
    else if (argc == 4)
    {
        input = fopen(argv[1], "rb");
        if (!input)
        {
            print_error(file_error);
            return file_error;
        }

        flag = check_valid_flag(argv[2]);
        mask_int = check_valid_mask(argv[3]);
        if (flag != mask)
        {
            print_error(incorrect_input);
            fclose(input);
            return incorrect_input;
        }
        if (mask_int == incorrect_input)
        {
            print_error(incorrect_input);
            fclose(input);
            return incorrect_input;
        }
        result = find_mask(input, argv[3]);
        if (result == file_input_error)
        {
            print_error(result);
            fclose(input);
            return result;
        }
        printf("Number of 4-bytes numbers that match the mask: %li", result);
    }
    else
    {
        print_error(incorrect_input);
        return incorrect_input;
    }
    fclose(input);
    printf("\nThe program has finished correctly.");
    return success;
}

long int xor_8(FILE *file)
{
    unsigned char byte;
    long int result = 0;
    while (fread(&byte, sizeof(unsigned char), 1, file) == 1)
    {
        if (ferror(file)) return file_input_error;
        result ^= byte;
    }
    return result;
}

int xor_32(FILE *file)
{
    int bytes[4] = {0, 0, 0, 0};
    int byte;
    long int result = 0;
    int index = 0;
    int value;
    while (!feof(file))
    {
        byte = fgetc(file);
        if (byte == EOF) break;
        bytes[index] = byte;
        if (ferror(file)) return file_input_error;
        if (index == 3)
        {
            value = bytes[0] ^ bytes[1] ^ bytes[2] ^ bytes[3];
            result ^= value;
            index = 0;
        }
        else index++;
    }
    for (int i = index; i < 4; ++i)
    {
        bytes[i] = 0;
    }
    value = bytes[0] ^ bytes[1] ^ bytes[2] ^ bytes[3];
    result ^= value;

    return result;
}

int get_decimal_number(char* number, int base)
{
    int result = 0;
    char *ptr = number - 1;

    while (*(++ptr))
    {
        result = result * base + (isdigit(*ptr) ? *ptr - '0' : tolower(*ptr) - 'a' + 10);
    }

    return result;
}

int make_decimal(int array[])
{
    int result = 0;
    int step = 1000;
    for (int i = 0; i < 4; ++i)
    {
        result += step * array[i];
        step /= 10;
    }
    return result;
}

int find_mask(FILE* file, char *mask_string)
{
    int bytes[4] = {0, 0, 0, 0};
    int byte;
    int index = 0;
    int count = 0;
    int mask_int = get_decimal_number(mask_string, 16);
    int value;
    while ((byte = fgetc(file)) != EOF)
    {
        bytes[index] = byte;
        if (index == 3)
        {
            value = make_decimal(bytes);
            if (value == mask_int) count++;
            index = 0;
        }
        else index++;
    }
    return count;
}

int check_valid_mask(const char *mask_string)
{
    int length = strlen(mask_string);
    char symbol;
    for (int i = 0; i < length; ++i)
    {
        symbol = mask_string[i];
        if (!(isdigit(symbol) || (symbol >= 'A' && symbol <= 'F') || (symbol >= 'a' && symbol <= 'f')))
        {
            return incorrect_input;
        }
    }

    return success;
}

int check_valid_flag(const char *flag)
{
    int length = strlen(flag);
    if (length < 4) return incorrect_input;

    char *xor_8 = "xor8";
    char *xor_32 = "xor32";
    char *mask_string = "mask";

    if (length == 4)
    {
        if (!strcmp(flag, xor_8)) return xor8;
        else if (!strcmp(flag, mask_string)) return mask;
        else return incorrect_input;
    }
    
    if (length == 5)
    {
        if (!strcmp(flag, xor_32)) return xor32;
        else return incorrect_input;
    }

    return incorrect_input;
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
