#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

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

int get_decimal_number(char* number, int base);

int check_valid_flag(const char *flag);

int check_valid_mask(char *mask_string);

unsigned char xor_8(FILE *file);

void xor_32(FILE *file, unsigned char result[]);

int find_mask(FILE* file, int mask_int);

int main(int argc, char *argv[])
{
    FILE *file = fopen(argv[1], "wb");
    int length = 10;
    unsigned char numbers[] = {1, 7, 4, 3, 9, 0, 1, 0, 5, 2};
    for (int i = 0; i < length; ++i)
    {
        fwrite(&(numbers[i]), sizeof(unsigned char), 1, file);
    }
    fclose(file);

    FILE* input;
    int flag;
    unsigned char result;
    unsigned char array_result[] = {0, 0, 0, 0};
    int int_result;

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
            printf("Result for XOR-operation with all bytes in file: %u\n", result);
        }
        else
        {
            xor_32(input, array_result);
            printf("Result for XOR-operation for 4-bytes groups in file: ");
            for (int i = 0; i < 4; ++i)
            {
                printf("%u ", array_result[i]);
            }
            printf("\n");
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
        if (flag != mask)
        {
            print_error(incorrect_input);
            fclose(input);
            return incorrect_input;
        }
        int_result = check_valid_mask(argv[3]);
        if (int_result == incorrect_input)
        {
            print_error(incorrect_input);
            fclose(input);
            return incorrect_input;
        }
        int_result = find_mask(input, int_result);
        printf("Number of 4-bytes numbers that match the mask: %d", int_result);
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

unsigned char xor_8(FILE *file)
{
    unsigned char byte;
    unsigned char result = 0;
    while (fread(&byte, sizeof(unsigned char), 1, file) == 1)
    {
        result ^= byte;
    }
    return result;
}

void xor_32(FILE *file, unsigned char result[])
{
    unsigned char bytes[4] = {0, 0, 0, 0};
    unsigned char byte;
    int index = 0;
    int check;
    int value;
    while (!feof(file))
    {
        check = fread(&byte, sizeof(unsigned char), 1, file);
        if (check != 1) break;

        bytes[index] = byte;
        if (index == 3)
        {
            for (int i = 0; i < 4; ++i)
            {
                result[i] ^= bytes[i];
            }
            index = 0;
        }
        else index++;
    }
    for (int i = index; i < 4; ++i)
    {
        bytes[i] = 0;
    }
    for (int i = 0; i < 4; ++i)
    {
        result[i] ^= bytes[i];
    }

    return;
}

int find_mask(FILE* file, int mask_int)
{
    unsigned char bytes[4];
    int count = 0;
    int value;
    while (fread(&bytes, 4 * sizeof(unsigned char), 1, file) == 1)
    {
        for (int i = 0; i < 4; ++i)
        {
            printf("%u ", bytes[i]);
        }
        printf("\n");
        value = pow(256, 0) * bytes[0] + pow(256, 1) * bytes[1] + pow(256, 2) * bytes[2] + pow(256, 3) * bytes[3];
        printf("%d %d\n", value, mask_int);
        if (memcmp(bytes, &mask_int, 4) == 0) count++;
    }
    return count;
}

int get_decimal_number(char* number, int base)
{
    int result = 0;
    char *ptr = number - 1;
    int flag = 0;
    if (*(++ptr) == '-') flag = 1;
    else --ptr;

    while (*(++ptr) != '\0')
    {
        result = result * base + (isdigit(*ptr) ? *ptr - '0' : tolower(*ptr) - 'a' + 10);
    }
    if (flag == 1) result *= -1;

    return result;
}

int check_valid_mask(char *mask_string)
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
    int result = get_decimal_number(mask_string, 16);
    int size = (log2(result - 1) + 1) / 8;
    if (size > 4) return incorrect_input;

    return result;
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
