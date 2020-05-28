#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <time.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"
#define  CCT (+8)
#define  BUFSIZE 10000

//socket
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#define errexit(format,arg...) exit(printf(format,##arg))

//semaphore,shared memory segments
#include <sys/shm.h>
#include <sys/sem.h>
#define SEM_MODE 0666
#define SEM_KEY 1122334455

/* semaphore variables*/
int s;
long int key ;

/* shared memory segments' variables */
int shmid, pid;
int size,size1;

/* control elements */
int fd , ret , retval ;				//file
int led_count ;

unsigned short data[10]={LED_D9_INDEX ,LED_D10_INDEX, LED_D11_INDEX, LED_D12_INDEX, LED_D13_INDEX, LED_D14_INDEX, LED_D15_INDEX, LED_D16_INDEX, LED_ALL_OFF, LED_ALL_ON};	//LED
_7seg_info_t seg[5];				// 7 seg 
unsigned short keypad;				//key
lcd_write_info_t display;			//LCD

/* Global variables */
char user[8][20];
char pass[30];


//char class_set[8][20];
typedef struct{
int income;
int outcome;
int total;
int target;
char set[20];
    
int in_num;      //counter
int in_list[100];
char in_timer[100][10];
int in_name[100];

    
int out_num;     //counter
int out_list[100];
char out_timer[100][10];
int out_name[100];
int out_class[100];


}setgoal;


typedef struct{

setgoal goal[3];
int  led;
}household;


typedef struct{

char username[10];
setgoal goal[3];
int homegoal[3];

}userdata;

household home;
household *shm;

userdata  name[4];
userdata *shm1;





time_t timep;
time_t timep1;		//getting time
struct tm *ptm;
struct tm *ptm1;	
char   timej1[20][50];
char   timej2[20][50];

int passivesock(const char *service, const char *transport, int qlen)
{
	struct servent *pse;	/* pointer to service information entry */
	struct sockaddr_in sin;	/* an Internet endpoint address */
	int s, type;		/* socket descriptor and socket type */

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_addr.s_addr = INADDR_ANY;

	/* Map service name to port number */
	if ((pse = getservbyname(service, transport)))
		sin.sin_port = htons(ntohs((unsigned short)pse->s_port));
	else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
		errexit("Can't find \"%s\" service entry\n", service);

	/* Use protocol to choose a socket type */
	if (strcmp(transport, "udp") == 0)
		type = SOCK_DGRAM;
	else
		type = SOCK_STREAM;

	/* Allocate a socket */
	s = socket(PF_INET, type, 0);
	if (s < 0)
		errexit("Can't create socket: %s\n", strerror(errno));

	/* Bind the socket */
	if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
		errexit("Can't bind to port %s: %s\n",
				service, strerror(errno));

	/* Set the maximum number of waiting connection */
	if (type == SOCK_STREAM && listen(s, qlen) < 0)
		errexit("Can't listen on port %s: %s\n",
				service, strerror(errno));

	return s;
}

int semaphore_P(int semid)
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


int semaphore_V(int semid)
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

int handleRequest(int connfd,int num){
	


	/* variables */
	char rcv[BUFSIZE];
	char snd[BUFSIZE];
	char strmode[BUFSIZE];
	char strlcd[BUFSIZE];
	char cmd[2][50];
    
   	int offset;
   	char strtmp[BUFSIZE];

	int  i,j,l,c,k=0,r=0;

	int  use1,use2,use3,use4;
	int  usenum;
	int  ptr;

	int  mode,depmode,goalmode,withmode,classmode,dismode,targmode,setmode;
    int recordmode;

	int mode_min = 1,mode_max = 5;
	int depmode_min = 1,depmode_max = 2;
	int goalmode_min = 0,goalmode_max = 2;
	int withmode_min = 1,withmode_max = 2;
	int classmode_min = 0,classmode_max = 7;
	int dismode_min = 1,dismode_max = 2;
    int targmode_min = 1, targmode_max = 2;
	int setmode_min = 1,setmode_max = 2;
	
	int  amount;
	int  goalincount[100];
	int  goaloutcount[100];
	char in[100],out[100];

    char class_set[8][30] = {"Food","Clothing","Housing","Transportation","Education","Entertainment" ,"Bill","Others"};





	printf("Accept Client %d\n",num);


	
	
	do{

	/* check user */
	sprintf(snd,"==============================\nUser:");
	write(connfd,snd,strlen(snd));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	sscanf(rcv,"%s ",user[num]);


	use1 = strcmp(user[num],"Nick");
	use2 = strcmp(user[num],"Mary");
	use3 = strcmp(user[num],"Wolf");
	use4 = strcmp(user[num],"Elsa");

	
	/* check password */
	sprintf(snd,"Password:");
	write(connfd,snd,strlen(snd));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	sscanf(rcv,"%s ",pass);


	if(use1 == 0){
	ptr = strcmp(pass,"12");
	usenum = 0;
	}

	else if(use2 == 0){
	ptr = strcmp(pass,"34");
	usenum = 1;
	}

	else if(use3 == 0){
	ptr = strcmp(pass,"56");
	usenum = 2;
	}

	else if(use4 == 0){
	ptr = strcmp(pass,"78");
	usenum = 3;
	}



	if(use1 != 0 && use2 != 0 && use3 != 0 && use4 != 0){

	write(connfd,"Invalid User!\n",strlen("Invalid User!\n"));

	}

	if(ptr != 0){
	write(connfd,"Invalid Password!\n",strlen("Invalid Password!\n"));
	}



	}while((use1 != 0 && use2 != 0 && use3 != 0 && use4 != 0) || ptr != 0);


	semaphore_P(s);

	home.led = shm->led;	
	home.led ++;
	ioctl(fd,LED_IOCTL_BIT_SET,&data[home.led-1]);
	shm->led = home.led;

	semaphore_V(s);



    
	offset = 0;      
	memset(strmode,0,sizeof(strmode));        
	offset += sprintf(strmode + offset ,"==============================\n");
    offset += sprintf(strmode + offset ,"Select mode: 1.Deposit 2.Withdraw 3.Display 4.Setting 5.Exit \n");
    offset += sprintf(strmode + offset ,"==============================\n");
   // write(connfd,strmode,strlen(strmode));
    
	//write(connfd,"==============================\n",strlen("==============================\n"));

	/* print user on putty */
	printf("User: %s\n",user[num]);









	
	

	while(1){



	/* mode */
	do{
	write(connfd,strmode,strlen(strmode));
	write(connfd,"mode:",strlen("mode:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	mode = atoi(rcv);
	}while(mode < mode_min || mode > mode_max);


	switch(mode){

	case 1: // 存款
	write(connfd,"==============================\n",strlen("==============================\n"));
	write(connfd,"Select deposit mode: 1.Private 2.Household\n",strlen("Select deposit mode: 1.Private 2.Household\n"));
	write(connfd,"==============================\n",strlen("==============================\n"));



	do{

	write(connfd,"Deposit type:",strlen("Deposit type:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	depmode = atoi(rcv);

	}while(depmode < depmode_min || depmode > depmode_max);

		switch(depmode){
	
		case 1: //Private
		

		do{

		write(connfd,"Private Goal:",strlen("Private Goal:"));
		if ((read(connfd,rcv,BUFSIZE)) == -1){
		   errexit("Error: read()!\n");
		}
		goalmode = atoi(rcv);   //goal 0 1 2
		}while(goalmode < goalmode_min  || goalmode > goalmode_max);
		    
		// key goal money
		write(connfd,"Amount:",strlen("Amount:"));
		if ((read(connfd,rcv,BUFSIZE)) == -1){
		errexit("Error: read()!\n");
		}
		amount = atoi(rcv);

		semaphore_P(s);//lock
    
		//name = shm1
                
                for(i=0;i<4;i++){
                    //strcpy(name[i].username , shm1[i].username);
                    for(j=0;j<3;j++){
                        name[i].goal[j].income = shm1[i].goal[j].income;
                        name[i].goal[j].outcome= shm1[i].goal[j].outcome;
                        name[i].goal[j].total = shm1[i].goal[j].total;
                        name[i].goal[j].in_num = shm1[i].goal[j].in_num;
                        name[i].goal[j].out_num = shm1[i].goal[j].out_num;
                        for(l=0;l<100;l++){
                            name[i].goal[j].in_list[l] = shm1[i].goal[j].in_list[l];
                            name[i].goal[j].out_list[l] = shm1[i].goal[j].out_list[l];
                            strcpy(name[i].goal[j].in_timer[l], shm1[i].goal[j].in_timer[l]);
                            strcpy(name[i].goal[j].out_timer[l], shm1[i].goal[j].out_timer[l]);
                            name[i].goal[j].out_class[l] = shm1[i].goal[j].out_class[l];
                        }
                    }
                }
                
	
            
		c = name[usenum].goal[goalmode].in_num;
		name[usenum].goal[goalmode].income = name[usenum].goal[goalmode].income + amount;
		name[usenum].goal[goalmode].total = name[usenum].goal[goalmode].income - name[usenum].goal[goalmode].outcome;
		        
		name[usenum].goal[goalmode].in_list[c] = amount;
		name[usenum].goal[goalmode].in_num ++ ;


		time(&timep);
		ptm = gmtime (&timep);
		
		sprintf(name[usenum].goal[goalmode].in_timer[c],"%02d:%02d:%02d",(ptm->tm_hour+CCT)%24,ptm->tm_min,ptm->tm_sec);
	        
		        
		// shm1 = name
		        
		for(i=0;i<4;i++){
                //strcpy(shm1[i].username , name[i].username);
                    for(j=0;j<3;j++){
                        shm1[i].goal[j].income = name[i].goal[j].income;
                        shm1[i].goal[j].outcome= name[i].goal[j].outcome;
                        shm1[i].goal[j].total = name[i].goal[j].total;
                        shm1[i].goal[j].in_num = name[i].goal[j].in_num;
                        shm1[i].goal[j].out_num = name[i].goal[j].out_num;
                        for(l=0;l<100;l++){
                            shm1[i].goal[j].in_list[l] = name[i].goal[j].in_list[l];
                            shm1[i].goal[j].out_list[l] = name[i].goal[j].out_list[l];
                            strcpy(shm1[i].goal[j].in_timer[l],name[i].goal[j].in_timer[l]);
                            strcpy( shm1[i].goal[j].out_timer[l] ,name[i].goal[j].out_timer[l]);
                            shm1[i].goal[j].out_class[l] = name[i].goal[j].out_class[l];
                        }
                    }
                }//
                
                


		semaphore_V(s);//unlock

		write(connfd,"==============================\n",strlen("==============================\n"));        

		offset = 0;      
		memset(strtmp,0,sizeof(strtmp));
		        
		offset += sprintf(strtmp + offset ,"Private:\n");
		offset += sprintf(strtmp + offset ,"Goal%d:\n",goalmode);
		offset += sprintf(strtmp + offset ,"%d %s\n",name[usenum].goal[goalmode].in_list[c],name[usenum].goal[goalmode].in_timer[c]);
		offset += sprintf(strtmp + offset ,"Deposite ok!\n");
		offset += sprintf(strtmp + offset ,"Goal%d total now: %d\n",goalmode,name[usenum].goal[goalmode].total);

		        
		 
		sprintf(snd,"%s",strtmp);
		write(connfd,snd,strlen(snd));
		write(connfd,"==============================\n",strlen("==============================\n"));
		break;        
                
                
                

		
		

		case 2: //home

		do{

		write(connfd,"Home Goal:",strlen("Home Goal:"));
		if ((read(connfd,rcv,BUFSIZE)) == -1){
		   errexit("Error: read()!\n");
		}
		goalmode = atoi(rcv);
		}while(goalmode < 0  || goalmode > 2);
		//goal 1 2 3

		    // key goal money
		    write(connfd,"Amount:",strlen("Amount:"));
		    if ((read(connfd,rcv,BUFSIZE)) == -1){
		     errexit("Error: read()!\n");
		    }
		    amount = atoi(rcv);

		    semaphore_P(s);//lock
		        
		        //home = shm
                
           
                
                    for(j=0;j<3;j++){
                        home.goal[j].income = shm->goal[j].income;
                        home.goal[j].outcome= shm->goal[j].outcome;
                        home.goal[j].total = shm->goal[j].total;
                        home.goal[j].in_num = shm->goal[j].in_num;
                        home.goal[j].out_num = shm->goal[j].out_num;
                        for(l=0;l<100;l++){
                            home.goal[j].in_list[l] = shm->goal[j].in_list[l];
                            home.goal[j].out_list[l] = shm->goal[j].out_list[l];
                            strcpy(home.goal[j].in_timer[l], shm->goal[j].in_timer[l]);
                            strcpy(home.goal[j].out_timer[l], shm->goal[j].out_timer[l]);
                            home.goal[j].in_name[l] = shm->goal[j].in_name[l];
                            home.goal[j].out_name[l] = shm->goal[j].out_name[l];
                            home.goal[j].out_class[l] = shm->goal[j].out_class[l];
                        }
                    }
                
	
		    c = home.goal[goalmode].in_num;
		    home.goal[goalmode].income = home.goal[goalmode].income + amount;
		    home.goal[goalmode].total = home.goal[goalmode].income - home.goal[goalmode].outcome;
		        
		    home.goal[goalmode].in_list[c] = amount;
            home.goal[goalmode].in_name[c] = usenum;
		    home.goal[goalmode].in_num ++ ;

            
            time(&timep);
            ptm = gmtime (&timep);
			sprintf(home.goal[goalmode].in_timer[c],"%02d:%02d:%02d",(ptm->tm_hour+CCT)%24,ptm->tm_min,ptm->tm_sec);
				
	
                
                    for(j=0;j<3;j++){
                        shm->goal[j].income = home.goal[j].income;
                        shm->goal[j].outcome= home.goal[j].outcome;
                        shm->goal[j].total = home.goal[j].total;
                        shm->goal[j].in_num = home.goal[j].in_num;
                        shm->goal[j].out_num = home.goal[j].out_num;
                        for(l=0;l<100;l++){
                            shm->goal[j].in_list[l] = home.goal[j].in_list[l];
                            shm->goal[j].out_list[l] = home.goal[j].out_list[l];
                            strcpy(shm->goal[j].in_timer[l],home.goal[j].in_timer[l]);
                            strcpy( shm->goal[j].out_timer[l] ,home.goal[j].out_timer[l]);
                            shm->goal[j].in_name[l] = home.goal[j].in_name[l];
                            shm->goal[j].out_name[l] = home.goal[j].out_name[l];
                            shm->goal[j].out_class[l] = home.goal[j].out_class[l];
                        }
                    }
  
 
	  


		int home_total=0;
		for(i=0;i<3;i++){
		    home_total += shm->goal[i].total;
		}
		        
			char segnum1[10]= "0x";
			char segnum2[5];
		
			sprintf(segnum2,"%d",home_total);
	 		
			seg[4].Mode = _7SEG_MODE_HEX_VALUE;
			strcat(segnum1,segnum2);
			seg[4].Value = strtol(segnum1,NULL,16);
			ioctl(fd,_7SEG_IOCTL_SET,&seg[4]);

      
		semaphore_V(s);//unlock
   

		write(connfd,"==============================\n",strlen("==============================\n"));
		offset = 0;      
		memset(strtmp,0,sizeof(strtmp));
		offset += sprintf(strtmp + offset ,"Household:\n");
		offset += sprintf(strtmp + offset ,"goal%d:\n",goalmode);
		offset += sprintf(strtmp + offset ,"$%d %s\n",home.goal[goalmode].in_list[c],home.goal[goalmode].in_timer[c]);
		offset += sprintf(strtmp + offset ,"deposite ok!\n");
		offset += sprintf(strtmp + offset ,"Goal%d total now: %d\n",goalmode,home.goal[goalmode].total);
		
		
		        
		sprintf(snd,"%s",strtmp);
		write(connfd,snd,strlen(snd));
		write(connfd,"==============================\n",strlen("==============================\n"));

		break;

		}//switch(depmode)

	break; //存款END
    
	    case 2: // 提款
	    write(connfd,"==============================\n",strlen("==============================\n"));
		write(connfd,"Select Withdraw mode: 1.Private 2.Household\n",strlen("Select Withdraw mode: 1.Private 2.Household\n"));
		write(connfd,"==============================\n",strlen("==============================\n"));

		do{

		write(connfd,"Withdraw type:",strlen("Withdraw type:"));
		if ((read(connfd,rcv,BUFSIZE)) == -1){
		   errexit("Error: read()!\n");
		}
		withmode = atoi(rcv);
		}while(withmode < withmode_min || withmode > withmode_max);

		switch(withmode){   
		case 1: //Private withdraw

		    // Account
		    do{

		    write(connfd,"Select an account:\n",strlen("Select an account:\n"));
		    write(connfd,"Private Goal:",strlen("Private Goal:"));
		    if ((read(connfd,rcv,BUFSIZE)) == -1){
		       errexit("Error: read()!\n");
		    }
		    goalmode = atoi(rcv);   //goal 0 1 2
		    }while(goalmode < goalmode_min  || goalmode > goalmode_max);
		    
		    
		        // Class
		        offset = 0;      
		        memset(strtmp,0,sizeof(strtmp));
			offset += sprintf(strtmp + offset ,"==============================\n");        
		        offset += sprintf(strtmp + offset ,"Select class: 0.Food 1.Clothing 2.Housing 3.Transportation 4.Education 5.Entertainment 6.Bill 7.Others \n");
			offset += sprintf(strtmp + offset ,"==============================\n");  
		        write(connfd,strtmp,strlen(strtmp));
		    
		        do{
		        write(connfd,"Class:",strlen("Class:"));
		        if ((read(connfd,rcv,BUFSIZE)) == -1){
		           errexit("Error: read()!\n");
		        }
		        classmode = atoi(rcv);   //goal 0 1 2
		        }while(classmode < classmode_min  || classmode > classmode_max);

		            // key withdraw money
		            write(connfd,"Amount:",strlen("Amount:"));
		            if ((read(connfd,rcv,BUFSIZE)) == -1){
		            errexit("Error: read()!\n");
		            }
		            amount = atoi(rcv);

	    
		    
		 semaphore_P(s);//lock
	    
			//name = shm1
		        
		        for(i=0;i<4;i++){
		           // strcpy(name[i].username , shm1[i].username);
		            for(j=0;j<3;j++){
		                name[i].goal[j].income = shm1[i].goal[j].income;
		                name[i].goal[j].outcome= shm1[i].goal[j].outcome;
		                name[i].goal[j].total = shm1[i].goal[j].total;
		                name[i].goal[j].in_num = shm1[i].goal[j].in_num;
		                name[i].goal[j].out_num = shm1[i].goal[j].out_num;
		                for(l=0;l<100;l++){
		                    name[i].goal[j].in_list[l] = shm1[i].goal[j].in_list[l];
		                    name[i].goal[j].out_list[l] = shm1[i].goal[j].out_list[l];
		                    strcpy(name[i].goal[j].in_timer[l], shm1[i].goal[j].in_timer[l]);
		                    strcpy(name[i].goal[j].out_timer[l], shm1[i].goal[j].out_timer[l]);
		                    name[i].goal[j].out_class[l] = shm1[i].goal[j].out_class[l];
		                }
		            }
		        }   //name = shm1
		    
		    
		//compute    
		    
		c = name[usenum].goal[goalmode].out_num;
		//Check
		if(amount > name[usenum].goal[goalmode].total){
		//跳出
		write(connfd,"You don't have so much money!\n",strlen("You don't have so much money!\n"));
		semaphore_V(s);//unlock
		break;
		}
		    
		else{
		
			name[usenum].goal[goalmode].outcome = name[usenum].goal[goalmode].outcome + amount;
			name[usenum].goal[goalmode].total = name[usenum].goal[goalmode].income - name[usenum].goal[goalmode].outcome;
				
			name[usenum].goal[goalmode].out_list[c] = amount;
			name[usenum].goal[goalmode].out_class[c] = classmode;
			name[usenum].goal[goalmode].out_num ++ ;
		    
		time(&timep);
			ptm = gmtime (&timep);
			sprintf(name[usenum].goal[goalmode].out_timer[c],"%02d:%02d:%02d",(ptm->tm_hour+CCT)%24,ptm->tm_min,ptm->tm_sec);
		    
		}
		        
			// shm1 = name
				
			for(i=0;i<4;i++){
		        //strcpy(shm1[i].username , name[i].username);
		            for(j=0;j<3;j++){
		                shm1[i].goal[j].income = name[i].goal[j].income;
		                shm1[i].goal[j].outcome= name[i].goal[j].outcome;
		                shm1[i].goal[j].total = name[i].goal[j].total;
		                shm1[i].goal[j].in_num = name[i].goal[j].in_num;
		                shm1[i].goal[j].out_num = name[i].goal[j].out_num;
		                for(l=0;l<100;l++){
		                    shm1[i].goal[j].in_list[l] = name[i].goal[j].in_list[l];
		                    shm1[i].goal[j].out_list[l] = name[i].goal[j].out_list[l];
		                    strcpy(shm1[i].goal[j].in_timer[l],name[i].goal[j].in_timer[l]);
		                    strcpy( shm1[i].goal[j].out_timer[l] ,name[i].goal[j].out_timer[l]);
		                    shm1[i].goal[j].out_class[l] = name[i].goal[j].out_class[l];
		                }
		            }
		        }//shm1 = name
		    
			semaphore_V(s);//unlock


		    
		write(connfd,"==============================\n",strlen("==============================\n"));        
		
		offset = 0;      
		memset(strtmp,0,sizeof(strtmp));
		        
		offset += sprintf(strtmp + offset ,"Private:\n");
		offset += sprintf(strtmp + offset ,"Goal%d:\n",goalmode);
		offset += sprintf(strtmp + offset ,"Class %d:%s\n",classmode,class_set[classmode]);
		offset += sprintf(strtmp + offset ,"$%d %s\n",name[usenum].goal[goalmode].out_list[c],name[usenum].goal[goalmode].out_timer[c]);
		offset += sprintf(strtmp + offset ,"Withdraw ok!\n");
		offset += sprintf(strtmp + offset ,"Goal%d total now: %d\n",goalmode,name[usenum].goal[goalmode].total);

		        
		 
		sprintf(snd,"%s",strtmp);
		write(connfd,snd,strlen(snd));
		write(connfd,"==============================\n",strlen("==============================\n"));
            	break;
        
		case 2: //home withdraw
		    
		// Account
		    do{

		    write(connfd,"Select an account:\n",strlen("Select an account:\n"));
		    write(connfd,"Home Goal:",strlen("Home Goal:"));
		    if ((read(connfd,rcv,BUFSIZE)) == -1){
		       errexit("Error: read()!\n");
		    }
		    goalmode = atoi(rcv);   //goal 0 1 2
		    }while(goalmode < goalmode_min  || goalmode > goalmode_max);
            
            
                // Class
                // Select class: 0.Food 1.Clothing 2.Housing 3.Transportation 4.Education 5.Entertainment 6.Bill 7.Others 
                offset = 0;      
                memset(strtmp,0,sizeof(strtmp));  
                offset += sprintf(strtmp + offset ,"Select class: ");
		offset += sprintf(strtmp + offset ,"\n==============================\n"); 
                for(i=0;i<8;i++){
                offset += sprintf(strtmp + offset ,"%d.%s ",i,class_set[i]);
                }
		offset += sprintf(strtmp + offset ,"\n==============================\n"); 
                write(connfd,strtmp,strlen(strtmp));
            
            
                do{
                write(connfd,"Class:",strlen("Class:"));
                if ((read(connfd,rcv,BUFSIZE)) == -1){
                   errexit("Error: read()!\n");
                }
                classmode = atoi(rcv);   //goal 0 1 2
                }while(classmode < classmode_min  || classmode > classmode_max);

                    // key withdraw money
                    write(connfd,"Amount:",strlen("Amount:"));
                    if ((read(connfd,rcv,BUFSIZE)) == -1){
                    errexit("Error: read()!\n");
                    }
                    amount = atoi(rcv);

            
          	  semaphore_P(s);//lock
		        
		        //home = shm
                    for(j=0;j<3;j++){
                        home.goal[j].income = shm->goal[j].income;
                        home.goal[j].outcome= shm->goal[j].outcome;
                        home.goal[j].total = shm->goal[j].total;
                        home.goal[j].in_num = shm->goal[j].in_num;
                        home.goal[j].out_num = shm->goal[j].out_num;
                        for(l=0;l<100;l++){
                            home.goal[j].in_list[l] = shm->goal[j].in_list[l];
                            home.goal[j].out_list[l] = shm->goal[j].out_list[l];
                            strcpy(home.goal[j].in_timer[l], shm->goal[j].in_timer[l]);
                            strcpy(home.goal[j].out_timer[l], shm->goal[j].out_timer[l]);
                            home.goal[j].in_name[l] = shm->goal[j].in_name[l];
                            home.goal[j].out_name[l] = shm->goal[j].out_name[l];
                            home.goal[j].out_class[l] = shm->goal[j].out_class[l];
                        }
                    }
                
	
            //compute 
		    c = home.goal[goalmode].out_num;
            //Check
            if(amount > home.goal[goalmode].total){
            //跳出
		write(connfd,"You don't have so much money!\n",strlen("You don't have so much money!\n"));
		semaphore_V(s);//unlock
            break;
            }

            else{
            
            
		    home.goal[goalmode].outcome = home.goal[goalmode].outcome + amount;
		    home.goal[goalmode].total = home.goal[goalmode].income - home.goal[goalmode].outcome;
		        
		    home.goal[goalmode].out_list[c] = amount;
            home.goal[goalmode].out_name[c] = usenum;
            home.goal[goalmode].out_class[c] = classmode;
		    home.goal[goalmode].out_num ++ ;

            
            time(&timep);
            ptm = gmtime (&timep);
			sprintf(home.goal[goalmode].out_timer[c],"%02d:%02d:%02d",(ptm->tm_hour+CCT)%24,ptm->tm_min,ptm->tm_sec);
				
	
                
                
            }
            
                    for(j=0;j<3;j++){
                        shm->goal[j].income = home.goal[j].income;
                        shm->goal[j].outcome= home.goal[j].outcome;
                        shm->goal[j].total = home.goal[j].total;
                        shm->goal[j].in_num = home.goal[j].in_num;
                        shm->goal[j].out_num = home.goal[j].out_num;
                        for(l=0;l<100;l++){
                            shm->goal[j].in_list[l] = home.goal[j].in_list[l];
                            shm->goal[j].out_list[l] = home.goal[j].out_list[l];
                            strcpy(shm->goal[j].in_timer[l],home.goal[j].in_timer[l]);
                            strcpy( shm->goal[j].out_timer[l] ,home.goal[j].out_timer[l]);
                            shm->goal[j].in_name[l] = home.goal[j].in_name[l];
                            shm->goal[j].out_name[l] = home.goal[j].out_name[l];
                            shm->goal[j].out_class[l] = home.goal[j].out_class[l];
                        }
                    }

		int home_total=0;
		for(i=0;i<3;i++){
		    home_total += shm->goal[i].total;
		}
		        
			char segnum1[10]= "0x";
			char segnum2[5];
		
			sprintf(segnum2,"%d",home_total);
	 		
			seg[4].Mode = _7SEG_MODE_HEX_VALUE;
			strcat(segnum1,segnum2);
			seg[4].Value = strtol(segnum1,NULL,16);
			ioctl(fd,_7SEG_IOCTL_SET,&seg[4]);

      
		semaphore_V(s);//unlock
            
            

        write(connfd,"==============================\n",strlen("==============================\n"));        
        
		offset = 0;      
		memset(strtmp,0,sizeof(strtmp));
		        
		offset += sprintf(strtmp + offset ,"Household:\n");
		offset += sprintf(strtmp + offset ,"Goal%d:\n",goalmode);
        offset += sprintf(strtmp + offset ,"Class %d:%s\n",classmode,class_set[classmode]);
		offset += sprintf(strtmp + offset ,"$%d %s\n",home.goal[goalmode].out_list[c],home.goal[goalmode].out_timer[c]);
		offset += sprintf(strtmp + offset ,"Withdraw ok!\n");
		offset += sprintf(strtmp + offset ,"Goal%d total now: %d\n",goalmode,home.goal[goalmode].total);


		sprintf(snd,"%s",strtmp);
		write(connfd,snd,strlen(snd));
		write(connfd,"==============================\n",strlen("==============================\n"));
        }//switch(withmode)
    
    break; //提款END
            
            
    case 3: // 顯示
    write(connfd,"==============================\n",strlen("==============================\n"));
	write(connfd,"Select Display mode: 1.Record 2.Achieve 3.Contribute\n",strlen("Select Display mode: 1.Record 2.Achieve 3.Contribute\n"));
	write(connfd,"==============================\n",strlen("==============================\n"));

	do{

	write(connfd,"Display type:",strlen("Display type:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	errexit("Error: read()!\n");
	}
	dismode = atoi(rcv);
	}while(dismode < 1 || dismode > 3);


	switch(dismode){

	case 1://record
	write(connfd,"==============================\n",strlen("==============================\n"));
    write(connfd,"Select Record mode: 1.Private 2.Home\n",strlen("Select Record mode: 1.Private 2.Home\n"));
	write(connfd,"==============================\n",strlen("==============================\n"));
            
    do{
	write(connfd,"Record mode:",strlen("Record mode:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	errexit("Error: read()!\n");
	}
	recordmode = atoi(rcv);
	}while(recordmode < 1 || recordmode > 2);
    
        switch(recordmode) {
        case 1: //private record
        
        do{
        write(connfd,"Goal:",strlen("Goal:"));
        if ((read(connfd,rcv,BUFSIZE)) == -1){
        errexit("Error: read()!\n");
        }
        goalmode = atoi(rcv);
        }while(goalmode < 0 || goalmode > 2);        
        
        //shm
        semaphore_P(s);//lock
	    
			//name = shm1
		        
		        for(i=0;i<4;i++){
		            //strcpy(name[i].username , shm1[i].username);
		            for(j=0;j<3;j++){
		                name[i].goal[j].income = shm1[i].goal[j].income;
		                name[i].goal[j].outcome= shm1[i].goal[j].outcome;
		                name[i].goal[j].total = shm1[i].goal[j].total;
		                name[i].goal[j].in_num = shm1[i].goal[j].in_num;
		                name[i].goal[j].out_num = shm1[i].goal[j].out_num;
		                for(l=0;l<100;l++){
		                    name[i].goal[j].in_list[l] = shm1[i].goal[j].in_list[l];
		                    name[i].goal[j].out_list[l] = shm1[i].goal[j].out_list[l];
		                    strcpy(name[i].goal[j].in_timer[l], shm1[i].goal[j].in_timer[l]);
		                    strcpy(name[i].goal[j].out_timer[l], shm1[i].goal[j].out_timer[l]);
		                    name[i].goal[j].out_class[l] = shm1[i].goal[j].out_class[l];
		                }
		            }
		        }   //name = shm1
                
        semaphore_V(s);//unlock
        //shm end
                
        // string list
        offset = 0;      
        memset(strtmp,0,sizeof(strtmp));
        offset += sprintf(strtmp + offset ,"==============================\n");
        offset += sprintf(strtmp + offset ,"Private Goal %d Record\n",goalmode);
        offset += sprintf(strtmp + offset ,"Income:\n");
        for(j=0;j<name[usenum].goal[goalmode].in_num;j++){
            offset += sprintf(strtmp + offset ,"%d.",j+1);
            offset += sprintf(strtmp + offset ,"%d ",name[usenum].goal[goalmode].in_list[j]);
            offset += sprintf(strtmp + offset ,"[%s]\n",name[usenum].goal[goalmode].in_timer[j]);
        }
        offset += sprintf(strtmp + offset ,"\n");
        write(connfd,strtmp,strlen(strtmp));  // income
        
        offset = 0;      
        memset(strtmp,0,sizeof(strtmp));
        offset += sprintf(strtmp + offset ,"Outcome:\n");
        for(j=0;j<name[usenum].goal[goalmode].out_num;j++){
            offset += sprintf(strtmp + offset ,"%d.",j+1);
            offset += sprintf(strtmp + offset ,"%d ",name[usenum].goal[goalmode].out_list[j]);
            offset += sprintf(strtmp + offset ,"<%s> ",class_set[name[usenum].goal[goalmode].out_class[j]]);
            offset += sprintf(strtmp + offset ,"[%s]\n",name[usenum].goal[goalmode].out_timer[j]);
        }
        offset += sprintf(strtmp + offset ,"\n");
        write(connfd,strtmp,strlen(strtmp));  // outcome
                
        break;//private record
                
        case 2: //home record
        do{
        write(connfd,"Goal:",strlen("Goal:"));
        if ((read(connfd,rcv,BUFSIZE)) == -1){
        errexit("Error: read()!\n");
        }
        goalmode = atoi(rcv);
        }while(goalmode < 0 || goalmode > 2); 
                
        //shm 
        semaphore_P(s);//lock
		        
		        //home = shm
                    for(j=0;j<3;j++){
                        home.goal[j].income = shm->goal[j].income;
                        home.goal[j].outcome= shm->goal[j].outcome;
                        home.goal[j].total = shm->goal[j].total;
                        home.goal[j].in_num = shm->goal[j].in_num;
                        home.goal[j].out_num = shm->goal[j].out_num;
                        for(l=0;l<100;l++){
                            home.goal[j].in_list[l] = shm->goal[j].in_list[l];
                            home.goal[j].out_list[l] = shm->goal[j].out_list[l];
                            strcpy(home.goal[j].in_timer[l], shm->goal[j].in_timer[l]);
                            strcpy(home.goal[j].out_timer[l], shm->goal[j].out_timer[l]);
                            home.goal[j].in_name[l] = shm->goal[j].in_name[l];
                            home.goal[j].out_name[l] = shm->goal[j].out_name[l];
                            home.goal[j].out_class[l] = shm->goal[j].out_class[l];
                        }
                    }
        semaphore_V(s);//unlock
        //shm end  
                
                
        // string list
        offset = 0;      
        memset(strtmp,0,sizeof(strtmp));
        offset += sprintf(strtmp + offset ,"==============================\n");
        offset += sprintf(strtmp + offset ,"Household Goal %d Record\n",goalmode);
        offset += sprintf(strtmp + offset ,"Income:\n");
        for(j=0;j<home.goal[goalmode].in_num;j++){
            offset += sprintf(strtmp + offset ,"%d.",j+1);
	    offset += sprintf(strtmp + offset ,"[%s] ",name[home.goal[goalmode].in_name[j]].username);
            offset += sprintf(strtmp + offset ,"%d ",home.goal[goalmode].in_list[j]);
            offset += sprintf(strtmp + offset ,"[%s]\n",home.goal[goalmode].in_timer[j]);
        }
        offset += sprintf(strtmp + offset ,"\n");
        write(connfd,strtmp,strlen(strtmp));  // income
        
        offset = 0;      
        memset(strtmp,0,sizeof(strtmp));
        offset += sprintf(strtmp + offset ,"Outcome:\n");
        for(j=0;j<home.goal[goalmode].out_num;j++){
            offset += sprintf(strtmp + offset ,"%d.",j+1);
	    offset += sprintf(strtmp + offset ,"[%s] ",name[home.goal[goalmode].out_name[j]].username);
            offset += sprintf(strtmp + offset ,"%d ",home.goal[goalmode].out_list[j]);
            offset += sprintf(strtmp + offset ,"<%s> ",class_set[home.goal[goalmode].out_class[j]]);
            offset += sprintf(strtmp + offset ,"[%s]\n",home.goal[goalmode].out_timer[j]);
        }
        offset += sprintf(strtmp + offset ,"\n");
        write(connfd,strtmp,strlen(strtmp));  // outcome
                

        break;//home record

}//switch(recordmode)


	break;//record

	case 2://achieve

    	/* Clear LCD */
	ioctl(fd,LCD_IOCTL_CLEAR,NULL);
    //shm
        semaphore_P(s);//lock
	    
			//name = shm1
		      for(j=0;j<3;j++){
		          name[usenum].goal[j].total = shm1[usenum].goal[j].total;
                  name[usenum].goal[j].target = shm1[usenum].goal[j].target;
                  home.goal[j].total = shm->goal[j].total;
                  home.goal[j].target = shm->goal[j].target;

		            
		        }   //name = shm1
                
        semaphore_V(s);//unlock
        //shm end        
         

            
            
    // home
    float a1,a2,a3;
    offset = 0;      
    memset(strtmp,0,sizeof(strtmp));
    offset += sprintf(strtmp + offset ,"==============================\n");
    offset += sprintf(strtmp + offset ,"Goal Target Achieving rate\n");
    offset += sprintf(strtmp + offset ,"Home:\n");
    for(i=0;i<3;i++){
        a1 = (home.goal[i].total)*100;
        a2 = home.goal[i].target;
        a3 = a1/a2;
        offset += sprintf(strtmp + offset ," Goal %d: %d / %d (%.2f%)\n",i,home.goal[i].total,home.goal[i].target,a3);
    }
    
    write(connfd,strtmp,strlen(strtmp));


	/* print on PXA LCD */
	offset = 0;      
   	memset(strlcd,0,sizeof(strlcd));
    	offset += sprintf(strlcd + offset ,"Home:\n");
	offset += sprintf(strlcd + offset ,"===============\n");
    	for(i=0;i<3;i++){
        a1 = (home.goal[i].total)*100;
        a2 = home.goal[i].target;
        a3 = a1/a2;
        offset += sprintf(strlcd + offset ,"Goal %d:\n%d/%d (%.2f%)\n\n",i,home.goal[i].total,home.goal[i].target,a3);
    	}
	display.Count = sprintf((char*)display.Msg,"%s",strlcd);
	ioctl(fd,LCD_IOCTL_WRITE,&display);

        
    // private
            
    offset = 0;      
    memset(strtmp,0,sizeof(strtmp));
    offset += sprintf(strtmp + offset ,"==============================\n");
    offset += sprintf(strtmp + offset ,"Goal Target Achieving rate\n");
    offset += sprintf(strtmp + offset ,"Private:\n");
    for(i=0;i<3;i++){
        a1 = (name[usenum].goal[i].total)*100;
        a2 = name[usenum].goal[i].target;
        a3 = a1/a2;
        offset += sprintf(strtmp + offset ," Goal %d: %d / %d (%.2f%)\n",i,name[usenum].goal[i].total,name[usenum].goal[i].target,a3);
    }
    offset += sprintf(strtmp + offset ,"==============================\n");
    write(connfd,strtmp,strlen(strtmp));
             
                
                
                

	break;//achieve
            
            
    case 3://contribute
            
           semaphore_P(s);//lock
		        
		        //home = shm
                    for(j=0;j<3;j++){
                        home.goal[j].income = shm->goal[j].income;
                        home.goal[j].outcome= shm->goal[j].outcome;
                        home.goal[j].total = shm->goal[j].total;
                        home.goal[j].in_num = shm->goal[j].in_num;
                        home.goal[j].out_num = shm->goal[j].out_num;
                        for(i=0;i<4;i++){
                        name[i].homegoal[j] = shm1[i].homegoal[j];
                        }
                        for(l=0;l<100;l++){
                            home.goal[j].in_list[l] = shm->goal[j].in_list[l];
                            home.goal[j].out_list[l] = shm->goal[j].out_list[l];

                            home.goal[j].in_name[l] = shm->goal[j].in_name[l];
                            home.goal[j].out_name[l] = shm->goal[j].out_name[l];

                        }
                    }
            

	for(i=0;i<4;i++){
	for(j=0;j<3;j++){
	name[i].homegoal[j] = 0;
	}
	}







            for(j=0;j<3;j++){
            for(i=0;i<home.goal[j].in_num;i++){                
                name[home.goal[j].in_name[i]].homegoal[j] += home.goal[j].in_list[i];
                }
            for(i=0;i<home.goal[j].out_num;i++){                
                name[home.goal[j].out_name[i]].homegoal[j] -= home.goal[j].out_list[i];
                }

            }







            
            
           		        //home = shm
                    for(j=0;j<3;j++){
                        shm->goal[j].income = home.goal[j].income;
                        shm->goal[j].outcome = home.goal[j].outcome;
                        shm->goal[j].total = home.goal[j].total;
                        shm->goal[j].in_num = home.goal[j].in_num;
                        shm->goal[j].out_num = home.goal[j].out_num;
                        for(i=0;i<4;i++){
                        shm1[i].homegoal[j] = name[i].homegoal[j];
                        }
                        for(l=0;l<100;l++){
                            shm->goal[j].in_list[l] = home.goal[j].in_list[l];
                            shm->goal[j].out_list[l] = home.goal[j].out_list[l];

                            shm->goal[j].in_name[l] = home.goal[j].in_name[l];
                            shm->goal[j].out_name[l] = home.goal[j].out_name[l];

                        }
                    } 
            
           
           semaphore_V(s);//unlock
  
	write(connfd,"==============================\n",strlen("==============================\n"));   

/*for(j=0;j<3;j++){
for(i=0;i<4;i++){
printf("%d ",name[i].homegoal[j]);
}
printf("%d\n",home.goal[j].total);
}*/


     
    offset = 0;      
    memset(strtmp,0,sizeof(strtmp));
    float tmp1,tmp2,tmp;
 
    
		        
    for(i=0;i<3;i++){
    offset += sprintf(strtmp + offset ,"Home goal %d:\n",i);
        for(j=0;j<4;j++){
	tmp = (name[j].homegoal[i])*100;
	tmp1= home.goal[i].total;
	tmp2= tmp/tmp1;
        offset += sprintf(strtmp + offset ,"%s :%.2f%\n",name[j].username,tmp2);
		
        }
    
    } 

    write(connfd,strtmp,strlen(strtmp));
    write(connfd,"==============================\n",strlen("==============================\n"));
	break;


	}//switch(dismode)


    break; //顯示END
            
    
    case 4: // 設定
            
	write(connfd,"==============================\n",strlen("==============================\n"));
	write(connfd,"Select Setting mode: 1.Private 2.Household\n",strlen("Select Setting mode: 1.Private 2.Household\n"));
	write(connfd,"==============================\n",strlen("==============================\n"));

	do{

	write(connfd,"Setting type:",strlen("Setting type:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	errexit("Error: read()!\n");
	}
	setmode = atoi(rcv);
	}while(setmode < setmode_min || setmode > setmode_max);


	switch(setmode){
	
	case 1://private
	do{
	write(connfd,"Select an account:\n",strlen("Select an account:\n"));
	write(connfd,"Private Goal:",strlen("Private Goal:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	errexit("Error: read()!\n");
	}
	goalmode = atoi(rcv);   //goal 0 1 2
	}while(goalmode < goalmode_min  || goalmode > goalmode_max);


	 // key setting money
	do{
         write(connfd,"Amount:",strlen("Amount:"));
         if ((read(connfd,rcv,BUFSIZE)) == -1){
         errexit("Error: read()!\n");
         }
         amount = atoi(rcv);
	}while(amount < 0);



	semaphore_P(s);//lock
	/* name = shm1 */
	for(i=0;i<4;i++){
	for(j=0;j<3;j++){
	name[i].goal[j].target = shm1[i].goal[j].target;
	}
	}
	

	
	name[usenum].goal[goalmode].target = amount;



	/* shm1 = name */
	for(i=0;i<4;i++){
	for(j=0;j<3;j++){
	shm1[i].goal[j].target = name[i].goal[j].target;
	}
	}

	semaphore_V(s);//unlock


	 write(connfd,"==============================\n",strlen("==============================\n"));        
        
	offset = 0;      
	memset(strtmp,0,sizeof(strtmp));

	offset += sprintf(strtmp + offset ,"Setting ok!\n");	        
	offset += sprintf(strtmp + offset ,"Private:\n");
	offset += sprintf(strtmp + offset ,"Goal%d:\ntarget now: %d\n",goalmode,name[usenum].goal[goalmode].target);


	sprintf(snd,"%s",strtmp);
	write(connfd,snd,strlen(snd));
	write(connfd,"==============================\n",strlen("==============================\n"));



	break;

	case 2://home
	if(usenum != 0){
	write(connfd,"Permission Denied!\n",strlen("Permission Denied!\n"));
	write(connfd,"==============================\n",strlen("==============================\n"));
	break;
	}



	do{
	write(connfd,"Select an account:\n",strlen("Select an account:\n"));
	write(connfd,"Home Goal:",strlen("Home Goal:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	errexit("Error: read()!\n");
	}
	goalmode = atoi(rcv);   //goal 0 1 2
	}while(goalmode < goalmode_min  || goalmode > goalmode_max);


	 // key setting money
	do{
         write(connfd,"Amount:",strlen("Amount:"));
         if ((read(connfd,rcv,BUFSIZE)) == -1){
         errexit("Error: read()!\n");
         }
         amount = atoi(rcv);
	}while(amount < 0);



	semaphore_P(s);//lock
	/* name = shm1 */
	
	for(j=0;j<3;j++){
	home.goal[j].target = shm->goal[j].target;
	}
	
	

	
	home.goal[goalmode].target = amount;



	/* shm1 = name */

	for(j=0;j<3;j++){
	shm->goal[j].target = home.goal[j].target;
	}


	semaphore_V(s);//unlock

	write(connfd,"==============================\n",strlen("==============================\n"));        
        
	offset = 0;      
	memset(strtmp,0,sizeof(strtmp));

	offset += sprintf(strtmp + offset ,"Setting ok!\n");	        
	offset += sprintf(strtmp + offset ,"Home:\n");
	offset += sprintf(strtmp + offset ,"Goal%d:\ntarget now: %d\n",goalmode,home.goal[goalmode].target);


	sprintf(snd,"%s",strtmp);
	write(connfd,snd,strlen(snd));
	write(connfd,"==============================\n",strlen("==============================\n"));












	break;
	}//switch(setmode)





    break; //設定END
            
    case 5: // 登出
	write(connfd,"==============================\n",strlen("==============================\n"));
	offset = 0;      
	memset(strtmp,0,sizeof(strtmp));

	


	semaphore_P(s);

	home.led = shm->led;
	ioctl(fd,LED_IOCTL_BIT_CLEAR,&data[home.led-1]);
	home.led --;
	shm->led = home.led;

	semaphore_V(s);





	
	offset += sprintf(strtmp + offset ,"%s log out!\n",user[num]);
	write(connfd,strtmp,strlen(strtmp));

    	write(connfd,"==============================\n",strlen("==============================\n"));
	exit(1);       
    break; //登出END



	}//switch(mode)

	

	}//while(1)



}//int handleRequest(int connfd,int num)











int main(int argc,char *argv[]){


	/* variables */
	int clientnum=1,childpid;
    	int i,j;




	/* Control elements' variables */
	seg[0].Which=_7SEG_D5_INDEX;
	seg[1].Which=_7SEG_D6_INDEX;
	seg[2].Which=_7SEG_D7_INDEX;
	seg[3].Which=_7SEG_D8_INDEX;
	seg[4].Which=_7SEG_ALL;

	
	/* socket variables */
	int  sockfd, connfd;
	struct sockaddr_in addr_cln;
	socklen_t sLen = sizeof(addr_cln);
	int status;


	/*  ./<server> <port_num>  */
	if(argc!=2){
	printf("./<server> <port_num>\n");
	exit(-1);
	}


	/* create socket and bind the socket to port */
	sockfd = passivesock(argv[1], "tcp" , 10);



	/* create semaphore */
	s = semget(SEM_KEY,1,SEM_MODE | IPC_CREAT);

	/* initialize semaphore value to 1 (binary semaphore) */
	if(semctl(s,0,SETVAL,1) < 0){
	fprintf(stderr,"Unable to initialize semaphore: %s\n",strerror(errno));
	exit(0);
	}

    
    
    
    	/* create shared memory */
	size1 = sizeof(userdata) *4;
	if((shmid = shmget(8888, size1, IPC_CREAT | 0666)) < 0){
		printf("shm get fail\n");
		exit(1);
	}



	/* attach shared memory to data space and the shared memory is called shm*/ 
	if((shm1 = (userdata*)shmat(shmid,0, 0)) == (userdata*) -1 ){
		printf("shm attach fail\n");
		exit(1);
	}

    
    

	/* create shared memory */
	size = sizeof(household);
	
	if((shmid = shmget(8887, size, IPC_CREAT | 0666)) < 0){
		printf("shm get fail\n");
		exit(1);
	}



	/* attach shared memory to data space and the shared memory is called shm*/ 
	if((shm = (household*)shmat(shmid,0, 0)) == (household*) -1 ){
		printf("shm attach fail\n");
		exit(1);
	}





	
	
	memset(shm,0,size);
  	memset(shm1,0,size1);


	/* decide name[i].username */
	strcpy(name[0].username,"Nick");
	strcpy(name[1].username,"Mary");
	strcpy(name[2].username,"Wolf");
	strcpy(name[3].username,"Elsa");




	/*zeroing*/
	led_count = 0;
	for(j=0;j<4;j++){
	for(i=0;i<3;i++){
	name[j].goal[i].in_num = 0;
	name[j].goal[i].out_num = 0;
	}    
	}

	for(i=0;i<3;i++){
	home.goal[i].total = 0;
	home.goal[i].in_num = 0;
	home.goal[i].out_num = 0;
	}


   	for(i=0;i<3;i++){
        for(j=0;j<4;j++){
        name[j].homegoal[i] = 0;
        }
   	}

	home.led = 0;
	shm->led = 0;
	/* initialize */

	
	for(i=0;i<3;i++){
	home.goal[i].target = 1000;
	shm->goal[i].target = 1000;
	for(j=0;j<4;j++){
	name[j].goal[i].target = 1000;
	shm1[j].goal[i].target = 1000;
	}
	}






	/* Open device /dev/lcd */


	if((fd=open("/dev/lcd",O_RDWR)) < 0) 
	{
	  printf("Open /dev/lcd failed.\n");
	  exit(-1);
	}








	
	/* Clear */
	ioctl(fd,KEY_IOCTL_CLEAR,key);


	ioctl(fd,LCD_IOCTL_CLEAR,NULL);

	ioctl(fd, _7SEG_IOCTL_OFF,NULL);
	ioctl(fd, _7SEG_IOCTL_ON,NULL);
	seg[4].Mode=_7SEG_MODE_PATTERN;
	seg[4].Value= 0x00000000;
	ioctl(fd,_7SEG_IOCTL_SET, &seg[4]);

	ioctl(fd,LED_IOCTL_SET, &data[8]);



	
	while(1){

	/*Wait for connection*/
	connfd=accept(sockfd,(struct sockaddr *)&addr_cln,&sLen);

	if ((connfd)==-1){

	printf("Failure in accept().\n");
	exit(EXIT_FAILURE);

	}//if (connfd=accept(server_socket,NULL,NULL))


	childpid=fork();//create a new process



	switch(childpid){

		case -1:
		printf("Cannot create child process.");
		close(connfd);
		break;

		case  0: //child process
		close(sockfd);
		
		handleRequest(connfd,clientnum);
		exit(EXIT_SUCCESS);

		default: //parent process
		close(connfd);
		break;

	}//switch

	clientnum++;


	}//while(true)



return 0;
close(fd);


}//int main
