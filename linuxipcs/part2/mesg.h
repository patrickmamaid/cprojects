/***********************************************
File: mesg.h

Author: Patrick Mamaid

Purpose: header file includes important
		 prototypes and global definitions
		 essential for server.c and client.c

***********************************************/


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/sem.h>
#include <pthread.h>


#define STDOUT_FILENO 1
#define MAXMESSAGEDATA (4096-16) /* don't want sizeof(Mesg) >4096 */
#define MESGHDRSIZE         (sizeof(mymesg) - MAXMESSAGEDATA)
#define MQ_KEY1 1234L
#define MQ_KEY2 2345L

                             /* length of mesg_len and mesg_type */
typedef struct
{
	long mesg_len;     /* #bytes in mesg_data */
        long mesg_type; /* message type */
        char mesg_data [MAXMESSAGEDATA];
} mymesg;

typedef struct
{
	int fd;
} FILE_INFO, *PFI;


ssize_t mesg_send(int, mymesg *);
ssize_t mesg_recv(int, mymesg *);
ssize_t Mesg_recv(int, mymesg *);

void client(int,int);
void server(int,int);

int getSem(int);
int getQueue(int);
void P(int);
void V(int);

