#ifndef MESSAGE_H
#define MESSAGE_H

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_LENGTH 100
#define MAX_PRIORITY 5

typedef struct
{
    long type;
    char text[MAX_LENGTH];
} Message, *Message_ptr;


#endif