#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]){

int fd;
	if((fd = open("/dev/mydev", O_RDWR)) < 0) {
		printf("Error open /dev/mydev\n");
		exit(-1);
	}

char map[16] = {
0b1111110, // 0
0b0110000, // 1
0b1101101, // 2
0b1111001, // 3
0b0110011, // 4
0b1011011, // 5
0b1011111, // 6
0b1110000, // 7
0b1111111, // 8
0b1111011, // 9
0b1110111, // A,a
0b0011111, // B,b
0b1001110, // C,c
0b0111101, // D,d
0b1001111, // E,e
0b1000111 // F,f
};

int	    val,num;
int         buff[7]={0};
char	    buf[8]={0};
int 	    i;

	/*num is the number of string's character ex:argv[1]=12369 , num=5 */
		num=strlen(argv[1]);  

	while(1) {

		for(i=0 ; i < num ;i++){
		sleep(3);
		 
		printf("%c ", argv[1][i]);       /*argv[] is a string , argv[1][i] is a character*/

		char str1[3]="0x";
       		char str2[1] = {argv[1][i]} ;
		int  j;
		strcat(str1, str2); 
		//printf("%c%c%c ",str1[0],str1[1],str1[2]);   
		val = strtol (str1,NULL,16);               /* 0x0~0xF to integer 0~15 */
		//printf("%d ",val);
		 
		j = map[val];
		//printf("map[%d]=%d ",val,j);        /* map[val] is a  integer */
		
		/*transfer integer j into binary representation*/
		buff[0]=j/64;
		buff[1]=(j%64)/32;
		buff[2]=(j%32)/16;
		buff[3]=(j%16)/8;
		buff[4]=(j%8)/4;
		buff[5]=(j%4)/2;
		buff[6]=(j%2)/1;
		
		printf("buf=[%d %d %d %d %d %d %d]\n",buff[0],buff[1],buff[2],buff[3],buff[4],buff[5],buff[6]);

		/*int buff[] transfer into char buf[]*/
		buf[0]=buff[0]+'\0';
		buf[1]=buff[1]+'\0';
		buf[2]=buff[2]+'\0';
		buf[3]=buff[3]+'\0';
		buf[4]=buff[4]+'\0';
		buf[5]=buff[5]+'\0';
		buf[6]=buff[6]+'\0';

		/*printf("buf=[%c %c %c %c %c %c %c %c]\n",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);*/


		if( write(fd, buf, 7) < 0) {
			printf("Error write %s\n",argv[3]);
			exit(-1);
		}
		
		
		
		}
	}//while(1)





close(fd);
return 0;

}//int main
