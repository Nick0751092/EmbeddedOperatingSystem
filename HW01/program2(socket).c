#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"


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
//time
#include <time.h>
#define  CCT (+8)


/* socket variables */
int  sockfd, connfd;
struct sockaddr_in addr_cln;
socklen_t sLen = sizeof(addr_cln);
char rcv[BUFSIZE];
char snd[BUFSIZE];
char std[BUFSIZE];
char sad[BUFSIZE];
char sbd[20][BUFSIZE];
char scd[BUFSIZE];
char sdd[BUFSIZE];
char sed[20][BUFSIZE];
char sfd[BUFSIZE];

/* Global variables process */
pid_t childpid;
pid_t wpid;

/* Global time variables */
time_t t1,t2;


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


int main(int argc,char *argv[]){

	/* Open device /dev/lcd */
	int fd , ret , retval ;

	if((fd=open("/dev/lcd",O_RDWR)) < 0) 
	{
	  printf("Open /dev/lcd failed.\n");
	  exit(-1);
	}

	/* variables */
	int  b,l=0;
	int  k,u;
	int  j=0;
	int  n=0,q[20];		// maximum of booking times:20
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




	/* Control elements */

	unsigned short data[10]={LED_D9_INDEX ,LED_D10_INDEX, LED_D11_INDEX, LED_D12_INDEX, LED_D13_INDEX, LED_D14_INDEX, LED_D15_INDEX, LED_D16_INDEX, LED_ALL_OFF, LED_ALL_ON};   //LED

	_7seg_info_t seg[7];            // 7 seg 
	seg[0].Which=_7SEG_D5_INDEX;
	seg[1].Which=_7SEG_D6_INDEX;
	seg[2].Which=_7SEG_D7_INDEX;
	seg[3].Which=_7SEG_D8_INDEX;
	seg[4].Which=_7SEG_ALL;
	seg[5].Which=_7SEG_D5_INDEX | _7SEG_D6_INDEX;
	seg[6].Which=_7SEG_D7_INDEX | _7SEG_D8_INDEX;


	unsigned short key;        //key
	lcd_write_info_t display;  //LCD
	
	/* Clear */
	ioctl(fd,KEY_IOCTL_CLEAR,key);


	ioctl(fd,LCD_IOCTL_CLEAR,NULL);

	ioctl(fd, _7SEG_IOCTL_OFF,NULL);
	ioctl(fd, _7SEG_IOCTL_ON,NULL);
	seg[4].Mode=_7SEG_MODE_PATTERN;
	seg[4].Value= 0x00000000;
	ioctl(fd,_7SEG_IOCTL_SET, &seg[4]);

	ioctl(fd,LED_IOCTL_SET, &data[8]);




	/* console.txt , concert.txt */

	int  i;
	int  placenum,connum;
	int  a;
	char buf1[10],buf2[10];

	typedef struct{

	char place[10];
	char placearea[5][2];   //'A' 'B' 'C' 'D' 'E' must be a string , so placearea is a string array
	int  placeareanum[5];

	}setConsole;

	typedef struct{
	char conname[10];
	char conplace[10];
	char conarea[5][2];    //'A' 'B' 'C' 'D' 'E' must be a string , so conarea is a string array
	int  conareanum[5];

	}setConcert;




	/* ./<server> <console.txt> <concert.txt> <port_num> */
	if(argc!=4){
	printf("./<server> <console.txt> <concert.txt> <port_num>\n");
	exit(-1);
	}
	
	/* create socket and bind the socket to port */
	sockfd = passivesock(argv[3], "tcp" , 10);




	/*Wait for connection*/
	connfd=accept(sockfd,(struct sockaddr *)&addr_cln,&sLen);

	if ((connfd)==-1){

	printf("Failure in accept().\n");
	exit(EXIT_FAILURE);

	}//if (connfd=accept(server_socket,NULL,NULL))


	printf("Accept Client\n");



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
	printf("%d\n",placenum);

	setConsole txt1[placenum];


	for(i=0;i<placenum;i++){

	fscanf(fp1,"%s %s %d %s %d %s %d %s %d %s %d",txt1[i].place,txt1[i].placearea[0],&txt1[i].placeareanum[0],txt1[i].placearea[1],&txt1[i].placeareanum[1],txt1[i].placearea[2],&txt1[i].placeareanum[2],txt1[i].placearea[3],&txt1[i].placeareanum[3],txt1[i].placearea[4],&txt1[i].placeareanum[4]);
	
	printf("%s %s %d %s %d %s %d %s %d %s %d\n",txt1[i].place,txt1[i].placearea[0],txt1[i].placeareanum[0],txt1[i].placearea[1],txt1[i].placeareanum[1],txt1[i].placearea[2],txt1[i].placeareanum[2],txt1[i].placearea[3],txt1[i].placeareanum[3],txt1[i].placearea[4],txt1[i].placeareanum[4]);
	}

	/* format argv[2] */

	fscanf(fp2,"%d\n",&connum);
	printf("%d\n",connum);

	setConcert txt2[connum];

	for(i=0;i<connum;i++){
	fscanf(fp2,"%s %s %s %d %s %d %s %d %s %d %s %d",txt2[i].conname,txt2[i].conplace,txt2[i].conarea[0],&txt2[i].conareanum[0],txt2[i].conarea[1],&txt2[i].conareanum[1],txt2[i].conarea[2],&txt2[i].conareanum[2],txt2[i].conarea[3],&txt2[i].conareanum[3],txt2[i].conarea[4],&txt2[i].conareanum[4]);

	
	printf("%s %s %s %d %s %d %s %d %s %d %s %d\n",txt2[i].conname,txt2[i].conplace,txt2[i].conarea[0],txt2[i].conareanum[0],txt2[i].conarea[1],txt2[i].conareanum[1],txt2[i].conarea[2],txt2[i].conareanum[2],txt2[i].conarea[3],txt2[i].conareanum[3],txt2[i].conarea[4],txt2[i].conareanum[4]);
	}//for(i=0;i<3;i++)


	int ptr;
	for (k=0;k<connum;k++){

	for (i=0;i<placenum;i++){

	sprintf(buf1,"%s",txt1[i].place);
	sprintf(buf2,"%s",txt2[k].conplace);
	ptr = strcmp(buf2,buf1);      //ptr==0 if buf1 , buf2 is the same string
	if (ptr==0){
	for(u=0;u<5;u++){

	if(txt2[k].conareanum[u] > txt1[i].placeareanum[u]){
	printf("Error concert area number!\n");
	printf("%s %s %d should be lower than %d!\n",txt2[k].conname,txt2[k].conarea[u],txt2[k].conareanum[u],txt1[i].placeareanum[u]);
	break;

	}//if(txt2[k].conareanum[u] > txt1[i].placeareanum[u])
	}//for(u=0;u<5;u++)



	}//if (ptr==0)




	}//for (i=0;i<placenum;i++)
	
	
	}//for (k=0;k<connum;k++)











	
	/* Input Mode */
	char mode;
	int  name;
	char area;
	int  number;
	int  name1;



	while(1){

	




	/* Select mode */
	write(connfd,"Select mode:",strlen("Select mode:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	sscanf(rcv," %c",&mode);


	if(mode == '#'){
	write(connfd,"Output Mode !\n",strlen("Output Mode !\n"));
	break;
	}
	
	write(connfd,"Continue !\n",strlen("Continue !\n"));
	
	





	/* Show concert numbers */
	for(k=0;k<connum;k++){

	sprintf(snd,"%d: %s\n",k+1,txt2[k].conname);
	write(connfd,snd,strlen(snd));

	}//for(k=0;k<connum;k++)

	write(connfd,"\n===============\n",strlen("\n===============\n"));
	

	


	/*Enter the concert*/
	
	
	do{
	
	write(connfd,"Enter the concert:",strlen("Enter the concert:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	sscanf(rcv,"%d ",&name);
	a = name;
	}while( a > connum );

	
	
	
	
	ioctl(fd,LCD_IOCTL_CLEAR, NULL);
	
	sprintf(p[j],"%s",txt2[a-1].conname);

	/* display on LCD */
	display.Count = sprintf((char *) display.Msg, "%s:\nA lefts %d\nB lefts %d\nC lefts %d\nD lefts %d\nE lefts %d\n",txt2[a-1].conname,txt2[a-1].conareanum[0],txt2[a-1].conareanum[1],txt2[a-1].conareanum[2],txt2[a-1].conareanum[3],txt2[a-1].conareanum[4]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);

	display.Count = sprintf((char *) display.Msg, "===============\n");
	ioctl(fd, LCD_IOCTL_WRITE, &display);

	/* display on PUTTY */
	sprintf(sad,"%s:\nA lefts %d\nB lefts %d\nC lefts %d\nD lefts %d\nE lefts %d\n===============\n",txt2[a-1].conname,txt2[a-1].conareanum[0],txt2[a-1].conareanum[1],txt2[a-1].conareanum[2],txt2[a-1].conareanum[3],txt2[a-1].conareanum[4]);
	write(connfd,sad,strlen(sad));

	
	


	char book[20][100];


	/* Tickets client booked */

	// display on PUTTY
	for(k=0;k<j;k++){

	sprintf(book[k],"%s/%c/%d  ",p[k],r[k],q[k]);
	strcat(sbd[j],book[k]);

	}//for(k=0;k<j;k++)

	strcat(sbd[j],"\n===============\n");
	write(connfd,sbd[j],strlen(sbd[j]));
	







	// display on LCD
	for(k=0;k<j;k++){
	display.Count = sprintf((char *) display.Msg, "%s/%c/%d  ",p[k],r[k],q[k]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);
	}//for(k=0;k<j;k++)


	display.Count = sprintf((char *) display.Msg, "\n===============\n");
	ioctl(fd, LCD_IOCTL_WRITE, &display);


	/* Booking tickets */


	/* display on PUTTY */
	
	
	while(1){
	write(connfd,"ticket:",strlen("ticket:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	sscanf(rcv,"%c ",&area);
	if( area == 'A' || area == 'B' || area == 'C' || area == 'D' || area == 'E'){
	break;
	}
	}
	
	
	r[j] = area;



	/* display on LCD */
	display.Count = sprintf((char *) display.Msg, "ticket:");
	ioctl(fd, LCD_IOCTL_WRITE, &display);

	
	
	
	display.Count = sprintf((char *) display.Msg, "%c\n",r[j]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);



	/* display on PUTTY */
	
	while(1){
	write(connfd,"number:",strlen("number:"));
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	sscanf(rcv," %d",&number);
	if( number == 1 || number == 2 || number == 3 || number == 4 || number == 5 || number == 6 || number == 7 || number == 8 || number == 9 ){
	break;
	}
	}
	


	q[j]= number;
	 n  = n + q[j];



	/* display on LCD */
	display.Count = sprintf((char *) display.Msg, "number:");
	ioctl(fd, LCD_IOCTL_WRITE, &display);


	
	display.Count = sprintf((char *) display.Msg, "%d\n",q[j]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);


	write(connfd,"If output mode : Press #\n",strlen("If output mode : Press #\n"));


	/* Get time */
	time(&timep);
	ptm = gmtime (&timep);
	
	/*display.Count = sprintf((char *) display.Msg,"%02d:%02d:%02d\n",(ptm->tm_hour+CCT)%24,ptm->tm_min,ptm->tm_sec);
	ioctl(fd, LCD_IOCTL_WRITE, &display);*/

	sprintf(timej[j],"%02d:%02d:%02d",(ptm->tm_hour+CCT)%24,ptm->tm_min,ptm->tm_sec);    /* Save time to timej[j] */


	/* Start counting time */
	sscanf(timej[0],"%02d:%02d:%02d",&t1hour,&t1min,&t1sec);		//record the time user first booked




	

	/*Counting how many tickets left */

	switch(r[j]){
	case 'A':
	txt2[a-1].conareanum[0]= txt2[a-1].conareanum[0] - q[j];
	break;
	case 'B':
	txt2[a-1].conareanum[1]= txt2[a-1].conareanum[1] - q[j];
	break;
	case 'C':
	txt2[a-1].conareanum[2]= txt2[a-1].conareanum[2] - q[j];
	break;
	case 'D':
	txt2[a-1].conareanum[3]= txt2[a-1].conareanum[3] - q[j];
	break;
	case 'E':
	txt2[a-1].conareanum[4]= txt2[a-1].conareanum[4] - q[j];
	break;
	}

	j++;

	
	





}//while(1)

	/*  Output Mode */



	/* End counting time*/

	time(&t2);
	ptm2 = gmtime (&t2);	
	countmin = (ptm2->tm_min)-(ptm->tm_min);
	countsec = (ptm2->tm_sec)-(ptm->tm_sec);
	counthour = ((ptm2->tm_hour+CCT)%24)-((ptm->tm_hour+CCT)%24);
	
	/* carrying time */

	if( (ptm->tm_sec) > (ptm2->tm_sec) ){
	
	countsec = countsec + 60 ;
	countmin = countmin - 1;

	}//if
	
	if( (ptm->tm_min) > (ptm2->tm_min) ){
	
	countmin = countmin + 60 ;
	counthour = counthour - 1;

	}//if

	if( ((ptm->tm_hour+CCT)%24) > ((ptm2->tm_hour+CCT)%24) ){
	
	counthour = counthour + 24;

	}//if




	printf("Book tickets using %02d:%02d!\n",countmin,countsec);


	/* 7 seg variables */

	//counting minutes
	char str1[6]="0x";
	char str2[3];
	int  val1;
	sprintf(str2,"%02d",countmin);
	strcat(str1,str2);
	val1=strtol(str1,NULL,16);

	//counting seconds
	char str3[6]="0x";
	char str4[3];
	int  val2;
	sprintf(str4,"%02d",countsec);
	strcat(str3,str4);
	val2=strtol(str3,NULL,16);

	

	
	/* 7 seg */
	seg[5].Mode=_7SEG_MODE_HEX_VALUE;
	seg[5].Value= val1;
	ioctl(fd,_7SEG_IOCTL_SET, &seg[5]);
	seg[6].Mode=_7SEG_MODE_HEX_VALUE;
	seg[6].Value= val2;
	ioctl(fd,_7SEG_IOCTL_SET, &seg[6]);



	/* LED */
	if (n >= 8){
	ioctl(fd,LED_IOCTL_SET,&data[9]);
	}
	else{
	for(i=0;i<n;i++){
	ioctl(fd,LED_IOCTL_BIT_SET,&data[i]);
	}
	}
	
	char booking[20][200];
	/* display on PUTTY */
	for(k=0;k<j;k++){
	for(i=0;i<q[k];i++){
	
	sprintf(booking[k],"%s:%c%d %s\n",p[k],r[k],i,timej[k]);
	strcat(scd,booking[k]);
	
	}
	}

	write(connfd,scd,strlen(scd));



	/* display on LCD */
	ioctl(fd, LCD_IOCTL_CLEAR, NULL);

	for(k=0;k<j;k++){
	for(i=0;i<q[k];i++){
	display.Count = sprintf((char *) display.Msg, "%s:%c%d %s\n",p[k],r[k],i,timej[k]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);
	}
	}//for(k=0;k<j;k++)
	
	
	
	
	ret = ioctl(fd,KEY_IOCTL_CLEAR, key);

	/* Show how many tickets left */


	while(1){


	
	/* Exit if key '#' */
	/*ret = ioctl(fd,KEY_IOCTL_GET_CHAR, &key);
	if((key & 0xFF ) == '#'){
	break;
	}*/

	char showcon[20][200];
	/* Show concert numbers */
	for(k=0;k<connum;k++){


	sprintf(showcon[k],"%d: %s\n",k+1,txt2[k].conname);
	strcat(sed[l],showcon[k]);

	}//for(k=0;k<connum;k++)

	strcat(sed[l],"\n===============\n");

	write(connfd,sed[l],strlen(sed[l]));
	


//write(connfd,,strlen());

	write(connfd,"Press the concert you want to check:",strlen("Press the concert you want to check:"));
	do{
	if ((read(connfd,rcv,BUFSIZE)) == -1){
	   errexit("Error: read()!\n");
	}
	sscanf(rcv," %d",&name1);
	b = name1;
	} while(b > connum);
	//printf("%d\n",b);


	/* display on LCD */
	ioctl(fd, LCD_IOCTL_CLEAR, NULL);

	display.Count = sprintf((char *) display.Msg, "%s:\nA lefts %d\nB lefts %d\nC lefts %d\nD lefts %d\nE lefts %d\n",txt2[b-1].conname,txt2[b-1].conareanum[0],txt2[b-1].conareanum[1],txt2[b-1].conareanum[2],txt2[b-1].conareanum[3],txt2[b-1].conareanum[4]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);

	/* display on PUTTY */
	sprintf(sfd,"%s:\nA lefts %d\nB lefts %d\nC lefts %d\nD lefts %d\nE lefts %d\n===============\n",txt2[b-1].conname,txt2[b-1].conareanum[0],txt2[b-1].conareanum[1],txt2[b-1].conareanum[2],txt2[b-1].conareanum[3],txt2[b-1].conareanum[4]);
	
	write(connfd,sfd,strlen(sfd));




	l++;

	}

	

	






	

fclose(fp1);
fclose(fp2);
return 0;



close(fd);








}//int main
