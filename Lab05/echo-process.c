#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "sockop.h"

#define BUFSIZE 1024


int handleRequest(int ,int);


int main(int argc, char *argv[])
{

	int server_socket;
	int new_client_socket;
	int num;
	int n = 1;
	char bufw[BUFSIZE], bufr[BUFSIZE];

	pid_t pid;//process ID


	struct sockaddr_in addr_cln;
	socklen_t sLen = sizeof(addr_cln);

	server_socket = passivesock(argv[1],"tcp",10);    //port is argv[1]

	while(true){

	/*Wait for connection*/
	new_client_socket=accept(server_socket,(struct sockaddr *)&addr_cln,&sLen);

	if ((new_client_socket)==-1){

	printf("Failure in accept().\n");
	exit(EXIT_FAILURE);

	}//if (new_client_socket=accept(server_socket,NULL,NULL))


	pid=fork();//create a new process

	switch(pid){

		case -1:
		printf("Cannot create child process.");
		close(new_client_socket);
		break;

		case  0: //child process
		close(server_socket);
		handleRequest(new_client_socket,n);
		exit(EXIT_SUCCESS);

		default: //parent process
		close(new_client_socket);
		break;

	}//switch

	n++;


	}//while(true)
	close(new_client_socket);
	return(0);

}//int main

        int handleRequest(int new_client_socket,int num){
	
	
	printf("Accept Client %d\n",num);

	while(1){

	char rcv[BUFSIZE];
	memset(rcv,0,BUFSIZE);
	

	if ((read(new_client_socket,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	
	printf("Received %s from Client %d\n",rcv,num);

	if(rcv[0] =='\0') break;

	if ((write(new_client_socket,rcv,strlen(rcv))) == -1){
	   errexit("Error: write()!\n");
	}
	}

	}//int handleRequest






