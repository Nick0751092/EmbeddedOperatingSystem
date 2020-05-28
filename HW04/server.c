#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <unistd.h>



//socket
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/ipc.h>

#define errexit(format,arg...) exit(printf(format,##arg))

#define  BUFSIZE     10000




#define  CLIENTS_NUM 30
#define CONCERTS_NUM 3
#define PROBLEM_2_CLIENTS_NUMBER 6
#define SEAT_CLASS_NUMBER 3
#define CLIENTS_NUM 30
#define BUFFER_LENGTH 1024
#define RECV_MODE 0   // 0 = recv all , 1 = recv per line
#define FILE_PATH "result/"
#define P2_FILE_PATH "p2_result/"

//time
#include <time.h>
#define  CCT (+8)

//semaphore
#define SEM_MODE 0666
#define SEM_KEY 1122334455




/* Global variable ps */
struct ps{
int st;
int num;
};

/* Global variables process */
pid_t childpid;
pid_t wpid;

/* Global time variables */
time_t t1,t2;




/* console & concert variables */
typedef struct{

char place[10];
char placearea[5][10];   //'A' 'B' 'C' 'D' 'E' must be a string , so placearea is a string array
int  placeareanum[5];

}setConsole;

typedef struct{
char conname[10];
char conplace[10];
char conarea[5][10];    //'A' 'B' 'C' 'D' 'E' must be a string , so conarea is a string array
int  conareanum[5];
	
}setConcert;



/* places' & concerts' variables */
int  placenum,connum;
setConsole txt1[2];
setConcert txt2[5];
setConcert txt3[5];

/* semaphore variables*/
int s;
long int key ;

/* shared memory variables */
int shmid, pid;
setConcert *shm;
int size;



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
	
	int  i,j,u,l,r=0;
	int  mode,bookmode;		
	char rcv[BUFSIZE];
	char snd[BUFSIZE];
	
	char name[20][40],area[20][10];
	int  booknum[20];
	const char blank[] = " ";
	char  *tmp,*tmp1;
	
	char str1[20][50],str2[20][10];
	char str3[20][100];//output to snd

	/* string comparison */
	char cmd[2][40];
	char sbuf1[40],sbuf2[40],sbuf3[40],sbuf4[40],sbuf5[10],sbuf6[10];
	int  ptr1,ptr2,ptr3,ptr4;


	/* string comparison for booking tickets */
	char sbuf7[40],sbuf8[40],sbuf9[40],sbuf10[40];
	int  ptr5,ptr6;
	int  ptr7,ptr8;
	int  ptr9,ptr10;


	/* show all */
	char show[50];

	/* show <con> */ 
	char showcon[10];


	printf("Accept Client %d\n",num);


	

	
	while(1){

	/* semaphore lock */
	semaphore_P(s);
	
	/* zeroing */
	int k=0;

	memset(rcv,0,BUFSIZE);
	memset(snd,0,BUFSIZE);


	memset(name,0,sizeof(name));
	memset(area,0,sizeof(area));
	memset(booknum,0,sizeof(booknum));

	








	




	/* read message from the client and print */
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	/* end*/
	if(rcv[0] =='\0') break;

	
	
	/* txt2 = shm */
	for(i=0;i<connum;i++){

	
	strcpy(txt2[i].conname,shm[i].conname);
	strcpy(txt2[i].conplace,shm[i].conplace);


	for(j=0;j<5;j++){

	strcpy(txt2[i].conarea[j],shm[i].conarea[j]);
	txt2[i].conareanum[j]=shm[i].conareanum[j];

	}//for(j=0;j<5;j++)

	}//for(i=0;i<connum;i++)


	
	
	/* Start time */
	t1 = time(NULL);
	sscanf(rcv,"%s %s",cmd[0],cmd[1]);

	tmp = strtok(rcv,blank);

	while( tmp != NULL ){
	sscanf(tmp,"%[^/]/%[^/]/%d",name[k],area[k],&(booknum[k]));
	tmp = strtok(NULL,blank);
	k++;
	}
	
	


	if(area[0][0] == '\0'){

	mode = 0;	// mode = 0 (exit , showall , show <concert_name> , input format not valid)
	bookmode = 0;

	sprintf(sbuf1,"%s",cmd[0]);
	sprintf(sbuf2,"showall");
	sprintf(sbuf3,"exit");
	sprintf(sbuf3,"exit");
	sprintf(sbuf4,"show");


	ptr1 = strcmp(sbuf1,sbuf2);
	ptr2 = strcmp(sbuf1,sbuf3);
	ptr3 = strcmp(sbuf1,sbuf4);
	
	if(ptr1 == 0){
	//showall

	for(i=0;i<connum;i++){
	sprintf(show,"%s %s %d %s %d %s %d %s %d %s %d\n",txt2[i].conname,txt2[i].conarea[0],txt2[i].conareanum[0],txt2[i].conarea[1],txt2[i].conareanum[1],txt2[i].conarea[2],txt2[i].conareanum[2],txt2[i].conarea[3],txt2[i].conareanum[3],txt2[i].conarea[4],txt2[i].conareanum[4]);

	strcat(snd,show);

	}//for(i=0;i<connum;i++)
	
	}//if(ptr1 == 0)



	else if(ptr2 == 0){
	//exit
	close(connfd);	
	exit(1);
	}//else if(ptr2 == 0)



	else if(ptr3 == 0){
	//show <concert name>

	for(i=0;i<connum;i++){
	sprintf(sbuf5,"%s",cmd[1]);
	sprintf(sbuf6,"%s",txt2[i].conname);
	ptr4 = strcmp(sbuf5,sbuf6);
	if(ptr4 == 0){
	
	sprintf(snd,"%s %s %d %s %d %s %d %s %d %s %d\n",txt2[i].conname,txt2[i].conarea[0],txt2[i].conareanum[0],txt2[i].conarea[1],txt2[i].conareanum[1],txt2[i].conarea[2],txt2[i].conareanum[2],txt2[i].conarea[3],txt2[i].conareanum[3],txt2[i].conarea[4],txt2[i].conareanum[4]);

	break;

	}//if(ptr4 == 0)

	
	sprintf(snd,"%s not found\n",cmd[1]);

	}//for(i=0;i<connum;i++)







	}//else if(ptr3 == 0)


	else{
	sprintf(snd,"input format not valid\n");	//input format not valid	
	}

	

	}//if(area[0][0] == '\0')






	else{
	mode = 1;	//mode = 1 (booking tickets)
	}



char namenotfound[40];
char namefound[40];
char areanotfound[10];
int  ticketremain[20];
int  ticketcount;
int  c[20];	
/* booking tickets */
	
if(mode == 1){

for(u=0;u<k;u++){

	for (i=0;i<connum;i++){

	sprintf(sbuf7,"%s",name[u]);
	sprintf(sbuf8,"%s",txt2[i].conname);
	ptr5 = strcmp(sbuf7,sbuf8);      

	if (ptr5 == 0){

		for (j=0;j<5;j++){

		sprintf(sbuf9,"%s",area[u]);
		sprintf(sbuf10,"%s",txt2[i].conarea[j]);
		ptr6 = strcmp(sbuf9,sbuf10);

		if ( ptr6 == 0 ){

		ticketremain[u] = txt2[i].conareanum[j] - booknum[u];
		
		bookmode = 1;
			
		break;

		}//if ( ptr6 == 0 )

		}//for (j=0;j<5;j++)

	break;



	}//if (ptr5 == 0)



	}//for (i=0;i<connum;i++)









	/* <concert_name> not found */
	if (ptr5 != 0){
	bookmode = 0;
	strcpy(namenotfound,name[u]);
	break;
	}


	/* <concert_name> does not have <ticket_class> ticket */
	if (ptr6 != 0){
	bookmode = -1;
	strcpy(namefound,name[u]);
	strcpy(areanotfound,area[u]);
	break;
	}


	/* sorry,remaining ticket number not enough */
	if ( ticketremain[u] < 0 ){
	bookmode = -2;
	break;

	}//if ( ticketremain[u] < 0 )
	









}//for(u=0;u<k;u++)

	

switch(bookmode){
	
	
	
	case 0:
	sprintf(snd,"%s not found\n",namenotfound);
	break;


	case -1:
	sprintf(snd,"%s does not have %s ticket\n",namefound,areanotfound);
	break;

	case -2:
	sprintf(snd,"sorry, remaining ticket number not enough\n");
	break;



	case 1:

	int a[20];
	int b[20];
	

	for(u=0;u<k;u++){

		for(i=0;i<connum;i++){
		ptr9 = strcmp(name[u],txt3[i].conname);  
		if(ptr9 == 0){

			for(j=0;j<5;j++){
			ptr10 = strcmp(area[u],txt3[i].conarea[j]);

			if(ptr10 == 0){

			
			c[u] = txt3[i].conareanum[j];
		

			}//if(ptr9 == 0)

			}//for(j=0;j<5;j++)



		}//if(ptr10 == 0)






		}//for(i=0;i<connum;i++)

	}//for(u=0;u<k;u++)







	
	

	
	
	











	/* counting remaining ticket number */
	for(u=0;u<k;u++){

		for(i=0;i<connum;i++){
		ptr7 = strcmp(name[u],txt2[i].conname);  
		if(ptr7 == 0){

			for(j=0;j<5;j++){
			ptr8 = strcmp(area[u],txt2[i].conarea[j]);

			if(ptr8 == 0){

			a[u] =  txt2[i].conareanum[j];
			txt2[i].conareanum[j] = txt2[i].conareanum[j] - booknum[u];
			b[u] =  txt2[i].conareanum[j];

			
			break;



			}//if(ptr8 == 0)

			}//for(j=0;j<5;j++)


		}//if(ptr7 == 0)


		}//for(i=0;i<connum;i++)


	}//for(u=0;u<k;u++)

	

	/* shm = txt2 */
	for(i=0;i<connum;i++){

	
	strcpy(shm[i].conname,txt2[i].conname);
	strcpy(shm[i].conplace,txt2[i].conplace);


	for(j=0;j<5;j++){

	strcpy(shm[i].conarea[j],txt2[i].conarea[j]);
	shm[i].conareanum[j]=txt2[i].conareanum[j];

	}//for(j=0;j<5;j++)

	}//for(i=0;i<connum;i++)




/*a[i]=%d b[i]=%d c[i]=%d*/

	for(i=0;i<k;i++){
	for(l=((c[i])-(a[i]));l<((c[i])-(b[i]));l++){	
	t2 = time(NULL);
	sprintf(str3[i],"%s %s %s%d %ld \n",name[i],area[i],area[i],l,t1-t2);
	strcat(snd,str3[i]);
	}
	}

	

	for(u=0;u<k;u++){

	if(b[u] < 0 ){
	sprintf(snd,"sorry, remaining ticket number not enough\n");
	break;
	}
	}
	

	
	

	break;




}//switch(bookmode)




	




}//if(mode ==1)







	/* write message to client */
	if ((write(connfd,snd,strlen(snd))) == -1){
	   errexit("Error: write()!\n");
	}
	
	
	/* semaphore unlock */
	semaphore_V(s);


	}//while(1)

	


	




}//int handleRequest






int main(int argc,char *argv[]){




	/* variables */
	int  b;
	int  k,u;
	int  j=0;
	int  n=0,q[20];		// maximum of booking times:20 //c counts the total number of tickets user booked
	char p[20][10];		//p is a string array to store conname
	char r[20];

	/* time variables */
	time_t timep;		//getting time
	struct tm *ptm;	
	char   timej[20][50];	//timej is a string array

	int t1hour,t1min,t1sec;		//counting time
	time_t t2;
	struct tm *ptm2;
	int countmin;
	int countsec;
	int counthour;


	/* client number */
	int clientnum = 1;


	
	/* socket variables */
	int  sockfd, connfd;
	struct sockaddr_in addr_cln;
	socklen_t sLen = sizeof(addr_cln);
	int status;



	/* create shared memory */


	size = sizeof(setConcert) *5;
	
	if((shmid = shmget(8882, size, IPC_CREAT | 0666)) < 0){
		printf("shm get fail\n");
		exit(1);
	}



	/* attach shared memory to data space and the shared memory is called shm*/ 

	//store struct array to the shared memory segments!

	if((shm = (setConcert*)shmat(shmid,0, 0)) == (setConcert*) -1 ){
		printf("shm attach fail\n");
		exit(1);
	}


	






	/* create semaphore */
	s = semget(SEM_KEY,1,SEM_MODE | IPC_CREAT);

	/* initialize semaphore value to 1 (binary semaphore) */
	if(semctl(s,0,SETVAL,1) < 0){
	fprintf(stderr,"Unable to initialize semaphore: %s\n",strerror(errno));
	exit(0);
	}








	/* ./<server> <console.txt> <concert.txt> <port_num> */
	if(argc!=4){
	printf("./<server> <console.txt> <concert.txt> <port_num>\n");
	exit(-1);
	}
	
	/* create socket and bind the socket to port */
	sockfd = passivesock(argv[3], "tcp" , 10);




	/* console.txt , concert.txt */

	int  i;
	
	int  a;
	char buf1[10],buf2[10];










	FILE *fp1,*fp2;
	


	if ( (fp1=fopen(argv[1],"r")) == 0 ) {
	printf("Open %s error!\n",argv[1]);
	exit(0);
	}

	
	if ( (fp2=fopen(argv[2],"a+")) == 0 ) {
	printf("Open %s error!\n",argv[2]);
	exit(0);
	}




	/* format argv[1] */

	fscanf(fp1,"%d\n",&placenum);
	

	


	for(i=0;i<placenum;i++){

	fscanf(fp1,"%s %s %d %s %d %s %d %s %d %s %d",txt1[i].place,txt1[i].placearea[0],&txt1[i].placeareanum[0],txt1[i].placearea[1],&txt1[i].placeareanum[1],txt1[i].placearea[2],&txt1[i].placeareanum[2],txt1[i].placearea[3],&txt1[i].placeareanum[3],txt1[i].placearea[4],&txt1[i].placeareanum[4]);
	
	}

	/* format argv[2] */

	fscanf(fp2,"%d\n",&connum);
	

	

	for(i=0;i<connum;i++){
	fscanf(fp2,"%s %s %s %d %s %d %s %d %s %d %s %d",txt2[i].conname,txt2[i].conplace,txt2[i].conarea[0],&txt2[i].conareanum[0],txt2[i].conarea[1],&txt2[i].conareanum[1],txt2[i].conarea[2],&txt2[i].conareanum[2],txt2[i].conarea[3],&txt2[i].conareanum[3],txt2[i].conarea[4],&txt2[i].conareanum[4]);

	
	
	}//for(i=0;i<connum;i++)



/* txt3 = txt2 */
for(i=0;i<connum;i++){

strcpy(txt3[i].conname,txt2[i].conname);

strcpy(txt3[i].conarea[0],txt2[i].conarea[0]);
strcpy(txt3[i].conarea[1],txt2[i].conarea[1]);
strcpy(txt3[i].conarea[2],txt2[i].conarea[2]);
strcpy(txt3[i].conarea[3],txt2[i].conarea[3]);
strcpy(txt3[i].conarea[4],txt2[i].conarea[4]);


txt3[i].conareanum[0]=txt2[0].conareanum[0];
txt3[i].conareanum[1]=txt2[i].conareanum[1];
txt3[i].conareanum[2]=txt2[i].conareanum[2];
txt3[i].conareanum[3]=txt2[i].conareanum[3];
txt3[i].conareanum[4]=txt2[i].conareanum[4];
}


	/* shm = txt2 */
	for(i=0;i<connum;i++){

	
	strcpy(shm[i].conname,txt2[i].conname);
	strcpy(shm[i].conplace,txt2[i].conplace);


	for(j=0;j<5;j++){

	strcpy(shm[i].conarea[j],txt2[i].conarea[j]);
	shm[i].conareanum[j]=txt2[i].conareanum[j];

	}



	}



	int ptr;
	for (k=0;k<connum;k++){

	for (i=0;i<placenum;i++){

	sprintf(buf1,"%s",txt1[i].place);
	sprintf(buf2,"%s",txt2[k].conplace);
	ptr = strcmp(buf2,buf1);      //ptr==0 if buf1 , buf2 is the same string
	if (ptr==0){
	for(u=0;u<5;u++){

	if(txt2[k].conareanum[u] > txt1[i].placeareanum[u]){
	printf("%s setting failure.\n",txt2[k].conname);
	exit(-1);
	}//if(txt2[k].conareanum[u] > txt1[i].placeareanum[u])
	}//for(u=0;u<5;u++)



	}//if (ptr==0)




	}//for (i=0;i<placenum;i++)
	
	
	}//for (k=0;k<connum;k++)

	printf("%d\n",placenum);
	for(i=0;i<placenum;i++){
	printf("%s %s %d %s %d %s %d %s %d %s %d\n",txt1[i].place,txt1[i].placearea[0],txt1[i].placeareanum[0],txt1[i].placearea[1],txt1[i].placeareanum[1],txt1[i].placearea[2],txt1[i].placeareanum[2],txt1[i].placearea[3],txt1[i].placeareanum[3],txt1[i].placearea[4],txt1[i].placeareanum[4]);
}


	printf("%d\n",connum);
	for(i=0;i<connum;i++){
	printf("%s %s %s %d %s %d %s %d %s %d %s %d\n",txt2[i].conname,txt2[i].conplace,txt2[i].conarea[0],txt2[i].conareanum[0],txt2[i].conarea[1],txt2[i].conareanum[1],txt2[i].conarea[2],txt2[i].conareanum[2],txt2[i].conarea[3],txt2[i].conareanum[3],txt2[i].conarea[4],txt2[i].conareanum[4]);
}

	
	
	/*printf("%d\n",connum);
	for(i=0;i<connum;i++){
	printf("%s %s %s %d %s %d %s %d %s %d %s %d\n",shm[i].conname,shm[i].conplace,shm[i].conarea[0],shm[i].conareanum[0],shm[i].conarea[1],shm[i].conareanum[1],shm[i].conarea[2],shm[i].conareanum[2],shm[i].conarea[3],shm[i].conareanum[3],shm[i].conarea[4],shm[i].conareanum[4]);
}
*/



	while(true){

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
	close(connfd);







	
fclose(fp1);
fclose(fp2);
return 0;
		

}
