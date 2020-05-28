#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include "sockop.h"

#define BUFSIZE 1024

int  SIGALRM_count = 0;
char rcv[BUFSIZE];
int  new_client_socket;


struct sigaction sa;
struct itimerval timer;


void timer_handler(int signum){



SIGALRM_count++;
//printf("SIGALRM_count = %d\n",SIGALRM_count);	

}//void timer_handler(void)








int handleRequest(int new_client_socket,int num){
	
	
	
	int    ptr;
	int    i,a;
	
	
	
	
	












	printf("Accept Client %d\n",num);

	while(1){

	
	//memset(rcv,0,BUFSIZE);


	
	/* Install timer_handler as the signal handler for SIGALRM */
	memset(&sa,0,sizeof(sa));
	// sa to be function timer_handler
	sa.sa_handler = &timer_handler;  

	

	/* Configure the timer to expire after 10 sec */
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_sec = 10;

	/* Reset the timer back to 10 sec after expired */
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_sec = 10;


	
	/* Start a real timer */
	if(setitimer(ITIMER_REAL,&timer,NULL)<0){
	printf("Setting timer error.\n");
	exit(1);
	}//if(setitimer(ITIMER_REAL,timer,NULL)<0)


	

	/* Alarm  to do sa */
	sigaction(SIGALRM,&sa,NULL);






	


	a = read(new_client_socket,rcv,BUFSIZE);

	

	

	
	if ( a == -1 ){

	if (SIGALRM_count > 0){

	printf("Timeout:Client%d\n",num);
	close(new_client_socket);
	break;

	}
	
	
	}

	
	
	printf("Received %s from Client %d\n",rcv,num);
	

	






	if(rcv[0] =='\0') break;

	if ((write(new_client_socket,rcv,strlen(rcv))) == -1){
	   errexit("Error: write()!\n");
	}



	}//while(1)

}//int handleRequest


int main(int argc, char *argv[])
{

	int server_socket;
	pid_t pid;//process ID
	int num;
	int n = 1;
	char bufw[BUFSIZE], bufr[BUFSIZE];

	


	struct sockaddr_in addr_cln;
	socklen_t sLen = sizeof(addr_cln);

	server_socket = passivesock(argv[1],"tcp",10);    //port is argv[1]


	


	while(true){


	






	/*Wait for connection*/
	new_client_socket=accept(server_socket,(struct sockaddr *)&addr_cln,&sLen);

	if ((new_client_socket)==-1){

	/*printf("Failure in accept().\n");
	exit(EXIT_FAILURE);*/

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

        






