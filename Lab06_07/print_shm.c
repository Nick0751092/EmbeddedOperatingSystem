#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>

int main (int args, char ** argv){
	if(args != 2){
		printf("Usage: ./add_shm <shm_key>\n");
		exit(1);
	}
	int shmid, pid;
	char* shm;

	if((shmid = shmget(atoi(argv[1]), 4, IPC_CREAT | 0666)) < 0){
		printf("shm get fail\n");
		exit(1);
	}
	if((shm = shmat(shmid, NULL, 0)) == (char*) -1 ){
		printf("shm attach fail\n");
		exit(1);
	}
	int i, tmp = 0;
	for(i=0;i<4;i++){
		tmp = ((tmp<<8)&0xFFFFFF00) | ((*(shm+i))&0xFF);
	}
	printf("%d\n", tmp);
	return 0;

}
