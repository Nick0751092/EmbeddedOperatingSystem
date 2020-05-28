#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int  SIGALRM_count = 0;


int  i;

void timer_handler(int signum){



SIGALRM_count++;
	

}//void timer_handler(void)



int main(int argc, char **argv)
{



	for(i=0;i<5;i++){
	struct sigaction sa;
	struct itimerval timer;

	/* Install timer_handler as the signal handler for SIGVTALRM */
	memset(&sa,0,sizeof(sa));
	sa.sa_handler = &timer_handler;
	sigaction(SIGALRM,&sa,NULL);

	/* Configure the timer to expire after 10 sec */
	timer.it_value.tv_sec = 10;
	timer.it_value.tv_sec = 0;

	/* Reset the timer back to 10 sec after expired */
	timer.it_interval.tv_sec = 10;
	timer.it_interval.tv_sec = 0;


	
	/* Start a virtual timer */
	if(setitimer(ITIMER_REAL,&timer,NULL)<0){
	printf("Setting timer error.\n");
	//exit(1);
	}//if(setitimer(ITIMER_VIRTUAL,timer,NULL)<0)


	

	printf("SIGALRM_count = %d\n",SIGALRM_count);

	}

	return(0);

}//int main

        






