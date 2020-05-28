#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <errno.h>
#include <string.h>

#define SEM_MODE 0666
#define SEM_KEY 1122334455


int P(int semid)
{
	struct sembuf sop;
	sop.sem_num = 0;
	sop.sem_op  = -1;/* wait */
	sop.sem_flg = SEM_UNDO;
		
	if (semop (semid,&sop,1) < 0){
	fprintf(stderr,"P(): semop failed: %s\n",strerror(errno));
	return -1;
	}
	else{
	return 0;
	}

}


int V(int semid)
{
	struct sembuf sop;
	sop.sem_num = 0;
	sop.sem_op  = 1;/* signal */
	sop.sem_flg = SEM_UNDO;

	if (semop (semid,&sop,1) < 0){
	fprintf(stderr,"V(): semop failed: %s\n",strerror(errno));
	return -1;
	}
	else{
	return 0;
	}

}


int main (int args, char ** argv){
	if(args != 3){
		printf("Usage: ./add_shm <shm_key> <amount>\n");
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


	/* semaphore variables*/
	int s;
	long int key ;
	
	/* set semaphore */
	s = semget(SEM_KEY,1,SEM_MODE);

	//printf("semkey:%x\n",SEM_KEY);	//sem key
	//printf("semid:%d\n",s);	//sem id


	/* initialize semaphore*/
	if(semctl(s,0,SETVAL,1) < 0){
	fprintf(stderr,"Unable to initialize semaphore: %s\n",strerror(errno));
	exit(0);
	}






	/* variables */
	int j;
	int i,tmp=0;
	int amount;
	amount = atoi(argv[2]);
	printf("subtract amount = %d\n",amount);
	

	

	

	for(j=0;j<amount;j++){
	
	P(s);//lock
	
	for(i=0;i<4;i++){
		tmp = ((tmp<<8)&0xFFFFFF00) | ((*(shm+i))&0xFF);
	}
	
	
	printf("subtract: %d\n", tmp);
	tmp--;
	
	for(i=0;i<4;i++){
		*(shm+i) = ((tmp>>8*(3-i))&0xFF);
	}
	for(i=0;i<4;i++){
		tmp = ((tmp<<8)&0xFFFFFF00) | ((*(shm+i))&0xFF);
	}
	
	
	
	V(s);//unlock
	
	
	}//for(i=0;i<atoi(argv[2]);i++)

	
	
	return 0;
}
