#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <time.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"
#define  CCT (+8)

int main(int argc,char *argv[]){

	/* Open device /dev/lcd */
	int fd , ret , retval ;

	if((fd=open("/dev/lcd",O_RDWR)) < 0) 
	{
	  printf("Open /dev/lcd failed.\n");
	  exit(-1);
	}

	/* variables */
	int  b;
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
	while(1){
	
	ret = ioctl(fd,KEY_IOCTL_CHECK_EMTPY, &key);
	if(ret<0){
	sleep(1);
	continue;
	}
	/* Output mode if key '#' */
	ret = ioctl(fd,KEY_IOCTL_GET_CHAR, &key);
	if((key & 0xFF ) == '#'){
	break;
	}

	do{
	ret = ioctl(fd,KEY_IOCTL_GET_CHAR, &key);
	a = (key&0xff) -'0' ;
	}while( a > connum );

	ioctl(fd,LCD_IOCTL_CLEAR, NULL);
	
	sprintf(p[j],"%s",txt2[a-1].conname);

	display.Count = sprintf((char *) display.Msg, "%s:\nA lefts %d\nB lefts %d\nC lefts %d\nD lefts %d\nE lefts %d\n",txt2[a-1].conname,txt2[a-1].conareanum[0],txt2[a-1].conareanum[1],txt2[a-1].conareanum[2],txt2[a-1].conareanum[3],txt2[a-1].conareanum[4]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);

	display.Count = sprintf((char *) display.Msg, "===============\n");
	ioctl(fd, LCD_IOCTL_WRITE, &display);
	


	/* Tickets client booked */

	for(k=0;k<j;k++){
	display.Count = sprintf((char *) display.Msg, "%s/%c/%d  ",p[k],r[k],q[k]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);
	}//for(k=0;k<j;k++)


	display.Count = sprintf((char *) display.Msg, "\n===============\n");
	ioctl(fd, LCD_IOCTL_WRITE, &display);


	/* Booking tickets */
	display.Count = sprintf((char *) display.Msg, "ticket:");
	ioctl(fd, LCD_IOCTL_WRITE, &display);

	do{
	ret = ioctl(fd,KEY_IOCTL_GET_CHAR, &key);
	}while((key & 0xFF ) != 'A' && (key & 0xFF ) != 'B' && (key & 0xFF ) != 'C' && (key & 0xFF ) != 'D' && (key & 0xFF ) != 'E');

	
	r[j] = (key&0xff);
	display.Count = sprintf((char *) display.Msg, "%c\n",r[j]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);






	display.Count = sprintf((char *) display.Msg, "number:");
	ioctl(fd, LCD_IOCTL_WRITE, &display);


	do{
	ret = ioctl(fd,KEY_IOCTL_GET_CHAR, &key);
	}while((key & 0xFF ) != '1' && (key & 0xFF ) != '2'  && (key & 0xFF ) != '3' && (key & 0xFF ) != '4' && (key & 0xFF ) != '5' && (key & 0xFF ) != '6' && (key & 0xFF ) != '7' && (key & 0xFF ) != '8' && (key & 0xFF ) != '9');
	

	q[j]= (key&0xff) -'0';
	n= n + q[j];
	
	display.Count = sprintf((char *) display.Msg, "%d\n",q[j]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);


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
	
	if( (ptm->tm_min)> (ptm2->tm_min) ){
	
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
	

	/* LCD */
	ioctl(fd, LCD_IOCTL_CLEAR, NULL);

	for(k=0;k<j;k++){
	for(i=0;i<q[k];i++){
	display.Count = sprintf((char *) display.Msg, "%s:%c%d %s\n",p[k],r[k],i,timej[k]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);
	}
	}//for(k=0;k<j;k++)
	
	sleep(5);
	
	
	ret = ioctl(fd,KEY_IOCTL_CLEAR, key);

	/* Show how many tickets left */


	while(1){

	ret = ioctl(fd,KEY_IOCTL_CHECK_EMTPY, &key);
	if(ret<0){
	sleep(1);
	continue;
	}
	
	/* Exit if key '#' */
	ret = ioctl(fd,KEY_IOCTL_GET_CHAR, &key);
	if((key & 0xFF ) == '#'){
	break;
	}


	do{
	ret = ioctl(fd,KEY_IOCTL_GET_CHAR, &key);
	b = (key & 0xFF) - '0';
	} while(b > connum);

	ioctl(fd, LCD_IOCTL_CLEAR, NULL);

	display.Count = sprintf((char *) display.Msg, "%s:\nA lefts %d\nB lefts %d\nC lefts %d\nD lefts %d\nE lefts %d\n",txt2[b-1].conname,txt2[b-1].conareanum[0],txt2[b-1].conareanum[1],txt2[b-1].conareanum[2],txt2[b-1].conareanum[3],txt2[b-1].conareanum[4]);
	ioctl(fd, LCD_IOCTL_WRITE, &display);

	}

	

	






	

fclose(fp1);
fclose(fp2);
return 0;



close(fd);








}//int main
