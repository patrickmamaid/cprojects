/***********************************************
File: server.c

Author: Patrick Mamaid

Purpose: opens an ipc sets up the server waits for
		 a file name 
		 FROM MULTIPLE CLIENTS
		 and attempts to open it.
		 when it does open it 
		 the server will send the contents to 
		 the client using message queues.


***********************************************/
#include "mesg.h"


/***********************************************
Method: int main()

Purpose: gets a msgquid and sends it to the 
		 server function to bind with it.

Return: program exits when finished 
***********************************************/
int main (){
	int msqid;
	msqid = msgget(MQ_KEY1, 0644|IPC_CREAT);
	server(msqid, msqid);
	exit(0);
}

/***********************************************
Method: void server(int readfd, int writefd)

Purpose: reads a pathname from the client
		 attempts to open the file
		 when server finds the file it will
		 print it out on client's stdout
		 the program forks to allow multiple 
		 clients

Params: 
readfd, writefd is the message queue id to read from
and write to

Return: nothing
***********************************************/
void server(int readfd, int writefd){

	FILE *fp;
	int fd;
	char *ptr;
	pid_t pid;
	ssize_t n;
	mymesg mesg;	
	for( ; ; ){
		//read pathname from the queue
		mesg.mesg_type = 1;
		if ((n = mesg_recv(readfd, &mesg))==0){
			//err_msg("pathname missing");
			printf("pathname missing\n");
			continue;
		}	
		
		mesg.mesg_data[n] = '\0';	// null terminate pathname	
		
		if ((ptr = strchr(mesg.mesg_data, ' ')) == NULL){
			//err_msg("bogus request: %s", mesg.mesg_data);
			printf("bogus request: %s\n", mesg.mesg_data);
			continue;
		}

		*ptr++ = 0;	//null terminate msqid, ptr = pathname	
		pid = atol(mesg.mesg_data);
		mesg.mesg_type = pid;	//for message back to client


		if(fork()==0){
			if ((fp=fopen(ptr, "r")) == NULL){
					// error: must tell client
				snprintf(mesg.mesg_data + n, sizeof(mesg.mesg_data) - n,
					": cant' open, %s\n", strerror(errno));
				mesg.mesg_len = strlen(ptr);
				memmove(mesg.mesg_data, ptr, mesg.mesg_len);
				mesg_send(writefd, &mesg);
			} 
			else{
				//fopen succeded: copy file to client's queue
				while(fgets(mesg.mesg_data,MAXMESSAGEDATA,fp)!=NULL){
					mesg.mesg_len = strlen(mesg.mesg_data);
					mesg_send(writefd, &mesg);
				}

				fclose(fp);
			}
		}
		// send a 0-length message to signify the end
		mesg.mesg_len = 0;
		mesg_send(writefd, &mesg);
		//exit(0);
	}



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
