#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define YEAR 1900
#define MONTH 1
#define SEC 60
#define DAY 24
#define CURRENT_YEAR 2023

struct User
{
    char login[7]; // логин
    int pin_code; // пароль
    int requests; // сколько было запросов в одном сеансе, считается, только если стоит флаг sunctions
    int max_requests; // сколько может сделать максимум запросов. -1, если нет ограничений
    int sanctions; // есть ли санкции
    int is_active; // находится ли в сессии в данный момент 
};

enum ERRORS
{
    success = 0,
    incorrect_input = -1,
    incorrect_console_input = -2,
    file_error = -3,
    no_memory = -4,
    no_sanctions = -5
};

enum COMMANDS
{
    time_cmm = 1,
    date = 2,
    howmuch = 3,
    logout = 4,
    sanctions = 5,
    // кастомные
    check_users = 6,
    login_cmm = 7,
    registration_cmm = 8,
    exit_command = 9
};

void login(struct User *users, int length, int *id);

int get_string(char **string);

int check_valid_login(char *login, int length);

int check_valid_pin(char *pin, int length);

void menu();

struct User registration(struct User *users, int quantity);

struct User create_new_user(char *login, int pin_code);

void users_in_system(struct User *users, int count);

void start_page();

int append_user(struct User user, struct User **users, int *quantity);

int check_valid_command(char *command);

void print_error(int error);

void get_time();

void get_date();

int how_much_time(char *command);

int log_out(struct User user);

int make_sanctions(char *command, struct User *users, int quantity);

void exiting_program();

int is_digit(char *number);

int check_valid_time(char *command, char **day, char **month, char **year, char **flag);

int get_user_date(char **day, char **month, char **year, char **flag, char *command);

int check_sanctions(struct User user);

int get_sanctions(char *command, char **login, char **max_requests);

///////////////////////////////////
///////////////////////////////////

int main()
{
    struct User *users = NULL;
    int current_user_id = 0;
    int quantity = 0;
    int result;
    struct User user;

    start_page();
    user = registration(users, quantity);
    result = append_user(user, &users, &quantity);

    if (result == no_memory)
    {
        free(users);
        print_error(result);
        return no_memory;
    }

    menu();
    char *command = NULL;
    result = get_string(&command);
    if (result == no_memory)
    {
        free(users);
        users = NULL;
        free(command);
        command = NULL;
        print_error(result);
        return no_memory;
    }
    int current_command = check_valid_command(command);
    while (current_command == incorrect_input)
    {
        printf("Incorrect input! Try again: ");
        free(command);
        command = NULL;
        result = get_string(&command);
        if (result == no_memory)
        {
            free(users);
            users = NULL;
            free(command);
            command = NULL;
            print_error(result);
            return no_memory;
        }
        current_command = check_valid_command(command);
    }
    while (current_command != exit_command)
    {
        if (check_sanctions(users[current_user_id]) != no_sanctions)
        {
            if (users[current_user_id].requests < users[current_user_id].max_requests)
            {
                users[current_user_id].requests += 1;
            }
            else
            {
                printf("Your maximum for requests was reached! You've been logged out.\n");
                current_command = logout;
            }
        }
        switch (current_command)
        {
            case time_cmm:
                get_time();
                break;
            case date:
                get_date();
                break;
            case howmuch:
                result = how_much_time(command);
                if (result == no_memory)
                {
                    free(users);
                    users = NULL;
                    print_error(result);
                    return no_memory;
                }
                break;
            case logout:
                result = log_out(user);
                if (result == no_memory)
                {
                    free(users);
                    users = NULL;
                    print_error(result);
                    return no_memory;
                }
                if (result == login_cmm)
                {
                    login(users, quantity, &current_user_id);
                }
                else
                {
                    user = registration(users, quantity);
                    result = append_user(user, &users, &quantity);
                    current_user_id = quantity - 1;
                    if (result == no_memory)
                    {
                        free(users);
                        users = NULL;
                        print_error(result);
                        return no_memory;
                    }
                }
                break;
            case sanctions:
                result = make_sanctions(command, users, quantity);
                if (result == no_memory)
                {
                    free(users);
                    users = NULL;
                    print_error(result);
                    return no_memory;
                }
                break;
            case check_users:
                users_in_system(users, quantity);
                break;
        }
        printf("\n\nContinue? Input any keywords: ");
        free(command);
        command = NULL;
        result = get_string(&command);

        menu();
        free(command);
        command = NULL;
        result = get_string(&command);
        if (result == no_memory)
        {
            free(users);
            print_error(result);
            return no_memory;
        }
        current_command = check_valid_command(command);
        while (current_command == incorrect_input)
        {
            printf("Incorrect input! Try again: ");
            free(command);
            command = NULL;
            result = get_string(&command);
            if (result == no_memory)
            {
                free(users);
                print_error(result);
                return no_memory;
            }
            current_command = check_valid_command(command);
        }
    }
    free(command);
    free(users);
    exiting_program();
    return success;
}

///////////////////////////////////
///////////////////////////////////

int check_sanctions(struct User user)
{
    if (user.sanctions == 0) return no_sanctions;

    return user.max_requests;
}

int is_digit(char *number)
{
    int length = strlen(number);
    for (int i = 0; i < length; ++i)
    {
        if (!(number[i] >= '0' && number[i] <= '9')) return incorrect_input;
    }
    return success;
}

int get_sanctions(char *command, char **login, char **max_requests)
{
    int length = strlen(command);
    int i = 0;
    int j = 0;
    int count = 0;
    *login = (char*)malloc(length * sizeof(char));

    if (*login == NULL) return no_memory;
    *max_requests = (char*)malloc(length * sizeof(char));

    if (*max_requests == NULL)
    {
        free(*login);
        *login = NULL;
        return no_memory;
    }

    char *temp = (char*)malloc(length * sizeof(char));
    if (temp == NULL)
    {
        free(*login);
        *login = NULL;
        free(*max_requests);
        *max_requests = NULL;
        return no_memory;
    }
    
    while (i < length)
    {
        if (command[i] == ' ')
        {
            temp[j] = '\0';
            if (count == 1)
            {
                if (strlen(temp) > 6)
                {
                    free(*login);
                    *login = NULL;

                    free(*max_requests);
                    *max_requests = NULL;

                    free(temp);
                    temp = NULL;
                    return incorrect_input;
                }
                strcpy(*login, temp);
            }
            count++;
            i++;
            free(temp);
            temp = NULL;
            temp = (char*)malloc(length * sizeof(char));
            if (temp == NULL)
            {
                free(*login);
                *login = NULL;

                free(*max_requests);
                *max_requests = NULL;
                return no_memory;
            }
            j = 0;
        }
        else
        {
            temp[j] = command[i];
            j++;
            i++;
        }
    }
    temp[j] = '\0';

    if (strlen(temp) > 2)
    {
        free(*login);
        *login = NULL;
                
        free(*max_requests);
        *max_requests = NULL;

        free(temp);
        temp = NULL;
        return incorrect_input;
    }
    strcpy(*max_requests, temp);
    int result = check_valid_login(*login, strlen(*login));
    if (result == incorrect_input)
    {
        free(*login);
        *login = NULL;
                
        free(*max_requests);
        *max_requests = NULL;
        
        free(temp);
        temp = NULL;
        return incorrect_input;
    }
    result = is_digit(*max_requests);
    if (result == incorrect_input)
    {
        free(*login);
        *login = NULL;
                
        free(*max_requests);
        *max_requests = NULL;
        
        free(temp);
        temp = NULL;
        return incorrect_input;
    }
    int requests = atoi(*max_requests);
    if (!(requests > 0 && requests <= 10))
    {
        free(*login);
        *login = NULL;
                
        free(*max_requests);
        *max_requests = NULL;
        
        free(temp);
        temp = NULL;
        return incorrect_input;
    }
    free(temp);
    temp = NULL;
    return success;
}

int make_sanctions(char *command, struct User *users, int quantity)
{
    char *login = NULL;
    char *max_requests = NULL;
    int result = get_sanctions(command, &login, &max_requests);
    int res_for_scan;
    int res = 0;
    while (result != success)
    {
        if (result == no_memory)
        {
            free(command);
            command = NULL;
            return no_memory;
        }
        printf("Incorrect input! Username's length must be lower than 6 letters, requests must be lower than 10 and more than 0.\n");
        printf("Or there isnt user with this username in the system\n");
        printf("Try again: ");
        res_for_scan = get_string(&command);
        if (res_for_scan == no_memory)
        {
            free(command);
            command = NULL;
            return no_memory;
        }
        result = get_sanctions(command, &login, &max_requests);
        if (result == success)
        {
            res = 0;
            for (int i = 0; i < quantity; ++i)
            {
                if (!strcmp(users[i].login, login))
                {
                    res = 1;
                }
            }
            if (!res) result = incorrect_input;
        }
    }

    printf("\nAre you sure? Input the secret word: ");
    free(command);
    command = NULL;
    result = get_string(&command);
    if (strcmp(command, "12345"))
    {
        free(command);
        command = NULL;
        free(login);
        login = NULL;
        free(max_requests);
        max_requests = NULL;
        printf("\n\nSANCTIONS WERE CANCELED\n\n");
        return no_sanctions;
    }

    int requests = atoi(max_requests);
    for (int i = 0; i < quantity; ++i)
    {
        if (!strcmp(users[i].login, login))
        {
            users[i].max_requests = requests;
            users[i].sanctions = 1;
        }
    }

    free(login);
    login = NULL;
    free(max_requests);
    max_requests = NULL;
    return success;
}

int log_out(struct User user)
{
    user.is_active = 0;
    printf("___________________________________________________________________________________________________\n");
    printf("\t\t\t\tYou've been loged out\n");
    printf("___________________________________________________________________________________________________\n");
    printf("\t\t\tChoose if you want to log in or to register\n\n");
    printf("\t\tInput 'Register' for registration or 'Login' to log in\n\n");
    printf("___________________________________________________________________________________________________\n");
    printf("Input section: ");
    char *result;
    int res = get_string(&result);
    if (res == no_memory) return no_memory;
    int res_1 = strcmp(result, "Login");
    int res_2 = strcmp(result, "Register");
    while (res_1 && res_2)
    {
        printf("\nIncorrect input. Try again: ");
        res = get_string(&result);
        if (res == no_memory) return no_memory;
        res_1 = strcmp(result, "Login");
        res_2 = strcmp(result, "Register");
    }
    if (!res_1) return login_cmm;
    return registration_cmm;
}

void get_time()
{
    time_t ttime;
    ttime = time(NULL);
    char *time_string = ctime(&ttime);
    char *temp = strtok(time_string, " ");
    for (int i = 0; i < 3; ++i)
    {
        temp = strtok(NULL, " ");
    }

    printf("\n\t\t\t\tCurrent time: %s\n", temp);
    return;
}

void get_date()
{
    time_t mytime = time(NULL);
    struct tm *now = localtime(&mytime);
    printf("\n\t\t\t\tCurrent date: %d:%d:%d\n", now -> tm_mday, now -> tm_mon + MONTH, now -> tm_year + YEAR);
    return;
}

int get_user_date(char **day, char **month, char **year, char **flag, char *command)
{
    *day = (char*)malloc(3 * sizeof(char));
    *month = (char*)malloc(3 * sizeof(char));
    *year = (char*)malloc(5 * sizeof(char));
    *flag = (char*)malloc(3 * sizeof(char));
    int length = strlen(command);
    int i = 0;
    int j = 0;
    int count = 0;
    char *temp = (char*)malloc(length *sizeof(char));
    if (temp == NULL) return no_memory;
    while (i < length)
    {
        if (command[i] == ' ')
        {
            temp[j] = '\0';
            if (count == 1)
            {
                if (strlen(temp) != 2) return incorrect_input;
                strcpy(*day, temp);
            }
            else if (count == 2)
            {
                if (strlen(temp) != 2) return incorrect_input;
                strcpy(*month, temp);
            }
            else if (count == 3)
            {
                if (strlen(temp) != 4) return incorrect_input;
                strcpy(*year, temp);
            }
            count++;
            free(temp);
            temp = NULL;
            temp = (char*)malloc(length *sizeof(char));
            if (temp == NULL) return no_memory;
            j = 0;
            i++;
            
        }
        else
        {
            temp[j] = command[i];
            i++;
            j++;
        }
    }
    temp[j] = '\0';
    if (strlen(temp) != 2) return incorrect_input;
    strcpy(*flag, temp);
    free(temp);
    temp = NULL;
    return success;
}

int check_valid_time(char *command, char **day, char **month, char **year, char **flag)
{
    if (strlen(command) != 21) return incorrect_input;

    int result = get_user_date(day, month, year, flag, command);
    
    if (result == no_memory) return no_memory;
    if (result == incorrect_input) return incorrect_input;
    
    if (is_digit(*day) != success)
    {
        free(*day);
        free(*month);
        free(*year);
        free(*flag);
        *day = NULL;
        *month = NULL;
        *year = NULL;
        *flag = NULL;
        return incorrect_input;
    }

    if (is_digit(*month) != success)
    {
        free(*day);
        free(*month);
        free(*year);
        free(*flag);
        *day = NULL;
        *month = NULL;
        *year = NULL;
        *flag = NULL;
        return incorrect_input;
    }

    if (is_digit(*year) != success)
    {
        
        free(*day);
        free(*month);
        free(*year);
        free(*flag);
        *day = NULL;
        *month = NULL;
        *year = NULL;
        *flag = NULL;
        return incorrect_input;
    }

    if ((*flag)[0] != '-' && !((*flag)[1] == 's' || (*flag)[1] == 'm' || (*flag)[1] == 'h'))
    {
        free(*day);
        free(*month);
        free(*year);
        free(*flag);
        *day = NULL;
        *month = NULL;
        *year = NULL;
        *flag = NULL;
        return incorrect_input;
    }
    return success;
}

int how_much_time(char *command)
{
    char* day = NULL;
    char *month = NULL;
    char *year = NULL;
    char *flag = NULL;
    int year_int;
    int month_int;
    int day_int;
    int result = check_valid_time(command, &day, &month, &year, &flag);
    while (result != success)
    {
        printf("\nIncorrect input! Try again with format xx (>1) xx (>1) xxxx (>=1900) -flag: ");
        result = get_string(&command);
        printf("\n");
        if (result == no_memory)
        {
            free(day);
            day = NULL;
            free(month);
            month = NULL;
            free(year);
            year = NULL;
            free(flag);
            flag = NULL;
            free(command);
            command = NULL;
            return no_memory;
        }
        result = check_valid_time(command, &day, &month, &year, &flag);
        if (result == success)
        {
            year_int = atoi(year);
            month_int = atoi(month);
            day_int = atoi(day);
            if (!(year_int >= 1900 && year_int < CURRENT_YEAR) || !(month_int >= 1 && month_int <= 12) || !(day_int >= 1 && day_int <= 31))
            {
                result = incorrect_input;
            }
        }
    }

    time_t current_time = time(NULL);
    struct tm user_time;

    user_time.tm_year = year_int - 1900;
    user_time.tm_mon = month_int - 1;
    user_time.tm_mday = day_int;
    user_time.tm_hour = 0;
    user_time.tm_min = 0;
    user_time.tm_sec = 0;

    struct tm *now = localtime(&current_time);
    time_t time_passed = mktime(now) - mktime(&user_time);

    long long int seconds = time_passed;
    long long int minutes = (time_passed / SEC);
    long long int hours = (time_passed / (SEC * SEC));

    if (flag[1] == 's')
    {
        printf("\nPassed time in seconds: %lli", seconds);
        return success;
    }
    if (flag[1] == 'm')
    {
        printf("\nPassed time in seconds: %lli", minutes);
        return success;
    }
    printf("\nPassed time in seconds: %lli", hours);
    return success;
}

void exiting_program()
{
    printf("___________________________________________________________________________________________________\n");
    printf("\t\t\tThank you for using the program!\n\n");
    printf("\t\t\t Session has been finished.\n");
    printf("___________________________________________________________________________________________________\n\n");
    return;
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

// если правильный ввод, возвращает номер команды, иначе incorrect_input
int check_valid_command(char *command)
{
    int length = strlen(command);
    if (length < 4) return incorrect_input;
    char *temp = malloc(length * sizeof(char));
    int i = 0;
    while (command[i] != ' ' && command[i] != '\0')
    {
        temp[i] = command[i];
        i++;
    }
    temp[i] = '\0';

    if (!strcmp(temp, "Time")) return time_cmm;
    if (!strcmp(temp, "Date")) return date;
    if (!strcmp(temp, "Howmuch")) return howmuch;
    if (!strcmp(temp, "Logout")) return logout;
    if (!strcmp(temp, "Sanctions")) return sanctions;
    if (!strcmp(temp, "Users")) return check_users;
    if (!strcmp(temp, "Exit")) return exit_command;

    
    return incorrect_input;
}

int append_user(struct User user, struct User **users, int *quantity)
{
    *quantity += 1;
    struct User *temp = (struct User*)realloc(*users, (*quantity) * sizeof(struct User));
    if (temp == NULL)
    {
        free(*users);
        return no_memory;
    }
    *users = temp;
    (*users)[*quantity - 1] = user;
    return success;
}

void start_page()
{
    printf("___________________________________________________________________________________________________\n");
    printf("\t\t\t\t\tSTART PAGE\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("\t\t\t\tWelcome to the start page!\n\n");
    printf("\t\t\t    At first, let's register in system.\n\n");
    return;
}

void users_in_system(struct User *users, int count)
{
    printf("___________________________________________________________________________________________________\n");
    printf("\t\t\t\t    CURRENT USERS LIST\n");
    printf("___________________________________________________________________________________________________\n\n");
    if (users == NULL)
    {
        printf("\t\t\t      Currently no users in system!\n");
        printf("___________________________________________________________________________________________________\n\n");
        return;
    }

    for (int i = 0; i < count; ++i)
    {
        printf("\t\t%d. %55s\n\n", i + 1, users[i].login);
    }
    printf("___________________________________________________________________________________________________\n\n");
    return;
}

void menu()
{
    printf("___________________________________________________________________________________________________\n");
    printf("\t\t\t\t\tMENU\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("\t\t\t\tWelcome to the menu!\n\n");
    printf("\t\t\tHere you can see available commands\n\n");
    printf("\tEnter name of command you want to run in section below the list of command\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("\t\t\t\t  LIST OF COMMANDS\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("1. Time     --     request for the current time in the standart format hours:minutes:seconds\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("2. Date     --     request for the current date in the standart format day:month:year\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("3. Howmuch <time> flag -- request for the elapsed time since a specificied data in parameter <data>,\n\n");
    printf("the flag parameter determines the type of presentation of the result (-s in second,\n\n");
    printf("-m in minutes, -h in hours, -y in years)\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("4. Logout   --     get back to login page\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("5. Sanctions username <number>     --     allows you to create a sanction on a user,\n");
    printf("<number> - maximum of requests that user will be avalaible to make during one session, must be < 10.\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("6. Users    --     check what users are already in system\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("7. Exit     --     finish the program\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("Enter the name of command: ");
    return;
}

struct User create_new_user(char *login, int pin_code)
{
    struct User user;
    strcpy(user.login, login);
    user.pin_code = pin_code;
    user.requests = 0;
    user.max_requests = -1;
    user.sanctions = 0;
    user.is_active = 1;
    return user;
}

struct User registration(struct User *users, int quantity)
{
    char *login;
    char *pin_code;
    int result, length;

    // интерфейс
    printf("___________________________________________________________________________________________________\n");
    printf("\t\t\t\t    REGISTRATION PAGE\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("\t\t\t  Please, be careful! Folow the format:\n\n");
    printf("\t\t\t\t   Login: your_login\n");
    printf("\n\t\t       ! Login must have LESS than SIX characters !\n\n");
    printf("\t\t\t\tPIN-code: your_pin_code\n");
    printf("\n\t\t    ! PIN-code must be a number within 0 and 100000 !\n\n");
    printf("___________________________________________________________________________________________________\n");

    printf("\nEnter your login: ");

    length = get_string(&login);
    result = check_valid_login(login, length);
    if (result != incorrect_input)
    {
        for (int i = 0; i < quantity; ++i)
            {
                if (!strcmp(users[i].login, login))
                {
                    result = incorrect_input;
                    break;
                }
            }
    }
    while (result == incorrect_input)
    {
        printf("\n\nLogin must have LESS than SIX characters OR user with this login is already existing!\nTry again: ");
        length = get_string(&login);
        result = check_valid_login(login, length);
        if (result != incorrect_input)
        {
            for (int i = 0; i < quantity; ++i)
            {
                if (!strcmp(users[i].login, login))
                {
                    result = incorrect_input;
                    break;
                }
            }
        }
    }

    printf("___________________________________________________________________________________________________\n");
    printf("\nEnter your PIN-code: ");
    length = get_string(&pin_code);
    result = check_valid_pin(pin_code, length);
    while (result == incorrect_input)
    {
        printf("\n\nPIN-code must be a number within 0 and 100000. Try again: ");
        length = get_string(&pin_code);
        result = check_valid_pin(pin_code, length);
    }
    printf("___________________________________________________________________________________________________\n");

    /////////////////////////////////////////////////////////

    return create_new_user(login, atoi(pin_code));
}

void login(struct User *users, int count, int *id)
{
    char *login;
    char *pin_code;
    int result, length;

    // интерфейс
    printf("___________________________________________________________________________________________________\n");
    printf("\t\t\t\t\t  LOGIN PAGE\n");
    printf("___________________________________________________________________________________________________\n\n");
    printf("\t\t\t    Please, be careful! Folow the format:\n\n");
    printf("\t\t\t\t     Login: your_login\n");
    printf("\n\t\t\t! Login must have LESS than SIX characters !\n\n");
    printf("\t\t\t\t  PIN-code: your_pin_code\n");
    printf("\n\t\t     ! PIN-code must be a number within 0 and 100000 !\n\n");
    printf("___________________________________________________________________________________________________\n");

    printf("\nEnter your login: ");

    length = get_string(&login);
    result = check_valid_login(login, length);
    struct User user;
    if (result != incorrect_input)
    {
        result = 0;
        for (int i = 0; i < count; ++i)
        {
            if (!strcmp(users[i].login, login))
            {
                result = 1;
                *id = i;
                user = users[i];
            }
        }
        if (result != 1) result = incorrect_input;
    }
    while (result == incorrect_input)
    {
        printf("\n\nIncorrect input or user is not registered! Try again: ");
        length = get_string(&login);
        result = check_valid_login(login, length);

        // есть ли такой юзер в системе
        if (result != incorrect_input)
        {
            result = 0;
            for (int i = 0; i < count; ++i)
            {
                if (!strcmp(users[i].login, login))
                {
                    result = 1;
                    *id = i;
                    user = users[i];
                }
            }
            if (result != 1) result = incorrect_input;
        }
    }

    /////////////////////////////////////////////////////////

    printf("___________________________________________________________________________________________________\n");
    printf("\nEnter your PIN-code: ");
    length = get_string(&pin_code);
    result = check_valid_pin(pin_code, length);
    if (result != incorrect_input)
    {
        if (result != user.pin_code) result = incorrect_input;
    }
    while (result == incorrect_input)
    {
        printf("\n\nPIN-code must be a number within 0 and 100000.\nOr not right pin_code. Try again: ");
        length = get_string(&pin_code);
        result = check_valid_pin(pin_code, length);

        // проверка, подходит ли пароль под юзера
        if (result != incorrect_input)
        {
            if (result != user.pin_code) result = incorrect_input;
        }
    }
    printf("___________________________________________________________________________________________________\n");

    /////////////////////////////////////////////////////////
    // тут должна быть проверка, подходит ли пароль под юзера
    /////////////////////////////////////////////////////////
}

int check_valid_login(char *login, int length)
{
    if (length > 7) return incorrect_input;

    for (int i = 0; i < length; ++i)
    {
        if (!(isdigit(login[i]) || isalpha(login[i]))) return incorrect_input;
    }
    
    return success;
}

// возвращает число в интовом представлении, если все ок
int check_valid_pin(char *pin, int length)
{
    if (length > 7) return incorrect_input;

    if (pin[0] == '-') return incorrect_input;

    for (int i = 0; i < length; ++i)
    {
        if (!isdigit(pin[i])) return incorrect_input;
    }

    int number = atoi(pin);
    if (number > 100000) return incorrect_input;

    return number;
}

int get_string(char **new_string)
{
    int length = 2;
    char *string = malloc(length * sizeof(char));
    if (string == NULL) return no_memory;
    char temp;
    char *temp_string;
    temp = getc(stdin);
    int count = 0;

    while(temp != '\n')
    {
        if (length <= count)
        {
            length *= 2;
            temp_string = (char*)realloc(string, length * sizeof(char));
            if (temp_string == NULL)
            {
                free(string);
                return no_memory;
            }
            string = temp_string;
        }
        string[count] = temp;
        temp = fgetc(stdin);
        count++;
    }
    if (length <= count)
    {
        length += 1;
        temp_string = realloc(string, length * sizeof(char));
        if (temp_string == NULL)
        {
            free(string);
            return no_memory;
        }
        string = temp_string;
    }
    string[count] = '\0';

    *new_string = string;
    return count;
}