#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <unistd.h>


#define SHMSZ 27

int main(int argc,char *argv[]){

	/* shared memory variables*/
	int shmid;
	int retval;
	key_t key;

	/* pointer */
	char *shm , *s;
	

	/* variables to save */
	char i;

	/* Name share memory segment 5678 */
	key = 5678;

	/* Create the segment */
	if ((shmid = shmget(key, SHMSZ , IPC_CREAT | 0666)) < 0){
	   perror("shmat");
	   exit(1);
	}
	

	/* Attach the segment to data space */
	if((shm = shmat(shmid, NULL, 0)) == (char*) -1 ){
	   printf("shm attach fail\n");
	   exit(1);
	}

	printf("Now we create and attach the shared memory.\n");


	/* Execute */
	for(i='a';i<='z';i++){
	*s++ = i ;
	}

	//*s = '\0';

	

	while(*shm != '*'){

	sleep(1);

	}



	/* Detach the share memory segment */
	shmdt(shm);





	/* Destroy the share memory segment */
	printf("Destroy the share memory.\n");
	retval = shmctl(shmid,IPC_RMID,NULL);
	if (retval < 0){

	fprintf(stderr,"Remove share memory failed.\n");
	exit(1);
	}

	return 0;








}//int main
