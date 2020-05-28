#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>

#include "sockop.h"

#define BUFSIZE 1024

struct ps{
int st;
int num;
};//global struct ps

void *child (void *arg){
struct ps *p = (struct ps *)arg;  //arg void type to struct type ps *p including st,num
int st = p->st;
int num = p->num;
char rcv[BUFSIZE];


	printf("Accept Client %d\n",num);

	while(1){
	
	memset(rcv,0,BUFSIZE);
	if ((read(st,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	
	printf("Received %s from Client %d\n",rcv,num);

	if(rcv[0] =='\0') break;

	if ((write(st,rcv,strlen(rcv))) == -1){
	   errexit("Error: write()!\n");
	sleep(1);
	}
	}//while(1)


}//void* child

int main(int argc, char *argv[])
{

	int server_socket;
	int new_client_socket;
	int num;
	int n = 1;
	int i;
	int rc;
	char bufw[BUFSIZE], bufr[BUFSIZE];

	pthread_t tid[10]; //thread ID

	struct sockaddr_in addr_cln;
	socklen_t sLen = sizeof(addr_cln);

	server_socket = passivesock(argv[1],"tcp",10);    //port is argv[1]

	while(true){

	/*Wait for connection*/
	new_client_socket = accept(server_socket,(struct sockaddr *)&addr_cln,&sLen);

	if ((new_client_socket)==-1){

	printf("Failure in accept().\n");
	exit(EXIT_FAILURE);

	}//if (new_client_socket=-1)

	struct ps ps1;
	ps1.st = new_client_socket;
	ps1.num = n;

	

	
	
	rc = pthread_create(&tid[n] , NULL , child , &ps1);  //&ps1 is struct ps1 (st,num) to (void *arg)

	if (rc < 0){
	printf("ERROR return code from pthread_create() is %d\n",rc);
	exit(1);
	}
	


	
	n++;
	


	}//while(true)

	close(new_client_socket);
	return(0);

}//int main



