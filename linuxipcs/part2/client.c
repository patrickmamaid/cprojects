/***********************************************
File: client.c

Author: Patrick Mamaid

Purpose: opens an ipc and sends the server 
a file name and attempts to open it on the server end
when it does open it 
the client will display the contents of
the file onto stdout.


***********************************************/

#include "mesg.h"

int main(void){
	
	int msqid;
	
	//server must create the queue
	msqid = msgget(MQ_KEY1,0);
	// same queue for both directions
	client(msqid, msqid);
	exit(0);
}


/***********************************************
Method: void client(int readfd, int writefd)

Purpose: reads a pathname from stdin
		 sends it to the server
		 when server finds the file it will
		 print it out on stdout

Params: 
readfd, writefd is the message queue id to read from
and write to

Return: nothing
***********************************************/
void client(int readfd, int writefd){

	size_t len;
	ssize_t n;
	char *ptr;
	mymesg mesg;


	//start buffer with msqid and a blank
	snprintf(mesg.mesg_data, MAXMESSAGEDATA, "%ld ", (long)getpid());
	len = strlen(mesg.mesg_data);
	ptr = mesg.mesg_data + len;
	// read pathname
	fgets(ptr,MAXMESSAGEDATA - len, stdin);
	len = strlen(mesg.mesg_data);
	if (mesg.mesg_data[len-1]=='\n')
		len--;		// delete newline from fgets()
	mesg.mesg_len = len;
	mesg.mesg_type = 1;
	// write pid and pathname to IPC channel
	mesg_send(writefd, &mesg);
	//read from IPC, write to stand output
	mesg.mesg_type = getpid();
	while ((n = mesg_recv(readfd, &mesg)) > 0)
		write(STDOUT_FILENO, mesg.mesg_data,n);
}

/***********************************************
Method: ssize_t mesg_recv(int id, mymesg *mptr)

Purpose: recieves a message from msgrcv() when called
		 and stores the message to [out] mptr

Params: id, is the id of the message queue to check
		mymesg *mptr [out] pointer to a mymesg struct
		saves the # of bytes of data to mptr->mesg_len
		
Return: -1 on error, 0 at EOF or else > 0
***********************************************/
ssize_t mesg_recv(int id, mymesg *mptr){

	ssize_t n;

	n = msgrcv(id, &(mptr->mesg_type), MAXMESSAGEDATA, mptr->mesg_type,0);
	mptr->mesg_len = n;		// return #bytes of data
	return (n);		// -1 on error, 0 at EOF, else > 0
}

/***********************************************
Method: mesg_send(int id, mymesg *mptr)

Purpose: sends a message to msgsnd() when called
		 and returns the result

Params: id, is the id of the message queue to check
		mymesg *mptr [in] pointer to a mymesg struct
		sends out message type and the message length
		
Return: see error messages of the msgsnd() function
***********************************************/
ssize_t mesg_send(int id, mymesg *mptr){
        return(msgsnd(id, &(mptr->mesg_type),mptr->mesg_len,0));
}
