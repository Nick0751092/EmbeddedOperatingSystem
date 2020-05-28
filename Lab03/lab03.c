#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"


int main ()
{

unsigned short  key , Count , data1 , data2 ;
int      retval;
int      ret;
int      fd;
int      i ,j , k ;
int      p;
int 	 dec  ;
int      dec1[p];
char     s[20]={};        /* save the keypad */        /*zeroing the array s[]*/
int	 bin[4];          /* binary representation */

if ((fd = open("/dev/lcd" , O_RDWR)) < 0 ) 
    {
    printf("open /dev/lcd error\n");
    return (-1);
    }

lcd_write_info_t display;
lcd_full_image_info_t display1;
_7seg_info_t data;

while(1){

   ret = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);
    if (ret < 0) {
      sleep(1);
      continue;
      }

  i = 0;

  do{
  ret = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
  if (ret == 0) {
     s[i] = key;
     display.Count = sprintf((char *) display.Msg,"%c",key);
     ioctl(fd , LCD_IOCTL_WRITE, &display );
     i++; 
      }     

  }while((key & 0xFF) != '#');

  p=i-1;


  ioctl(fd, LCD_IOCTL_CLEAR, NULL);      /* Clean LCD */

  data.Mode = _7SEG_MODE_PATTERN;
  data.Which = _7SEG_ALL ;
  data.Value = 0x00000000 ;
  ioctl(fd , _7SEG_IOCTL_SET ,&data);  

        /*     LED      */


 /* LED off */
 data1 = LED_ALL_OFF;
 ioctl(fd, LED_IOCTL_SET, &data1);
 sleep(1);







 /* First LED D13-D16 ON */
 data2 = LED_D13_INDEX ;
 ioctl(fd, LED_IOCTL_BIT_SET, &data2);

 data2 = LED_D14_INDEX ;
 ioctl(fd, LED_IOCTL_BIT_SET, &data2);

 data2 = LED_D15_INDEX ;
 ioctl(fd, LED_IOCTL_BIT_SET, &data2);
 
 data2 = LED_D16_INDEX ;
 ioctl(fd, LED_IOCTL_BIT_SET, &data2);
  
 

  /* data1 = LED_ALL_OFF;
  ioctl(fd, LED_IOCTL_SET, &data1);*/


k=0;

while(1){


 for(j=0;j<i-1;j++)    /*   Get s[j] without '#' , then j < i-1  */
 {
  ioctl(fd , _7SEG_IOCTL_OFF , NULL); 

  /*Hexadecimal representation with  7  segments on D6  */

/*  ioctl(fd, _7SEG_IOCTL_ON,NULL);
  data.Which = _7SEG_D6_INDEX;
  ioctl(fd , _7SEG_IOCTL_SET ,&data);*/



  /*  Binary representation with D13-D16  */
/*  if ( bin[0] == 1){
  data2 = LED_D13_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}
  if ( bin[1] == 1){
  data2 = LED_D14_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}
  if ( bin[2] == 1){
  data2 = LED_D15_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}
  if ( bin[3] == 1){
  data2 = LED_D16_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}*/


  /* binary representation for s[j] */
  if (s[j] == '0'){
  bin[0]=0; bin[1]=0; bin[2]=0; bin[3]=0; 
  dec = 0;  
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x0;
  }

  else if (s[j] == '1'){
  bin[0]=1; bin[1]=0; bin[2]=0; bin[3]=0; 
  dec = 1;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x1;
  }

  else if (s[j] == '2'){
  bin[0]=0; bin[1]=1; bin[2]=0; bin[3]=0; 
  dec = 2;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x2;
  }

  else if (s[j] == '3'){
  bin[0]=1; bin[1]=1; bin[2]=0; bin[3]=0; 
  dec = 3;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x3;
  }

  else if (s[j] == '4'){
  bin[0]=0; bin[1]=0; bin[2]=1; bin[3]=0; 
  dec = 4;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x4;
  }

  else if (s[j] == '5'){
  bin[0]=1; bin[1]=0; bin[2]=1; bin[3]=0; 
  dec = 5;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x5;
  }

  else if (s[j] == '6'){
  bin[0]=0; bin[1]=1; bin[2]=1; bin[3]=0; 
  dec = 6;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x6;
  }

  else if (s[j] == '7'){
  bin[0]=1; bin[1]=1; bin[2]=1; bin[3]=0; 
  dec = 7;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x7;
  }

  else if (s[j] == '8'){
  bin[0]=0; bin[1]=0; bin[2]=0; bin[3]=1; 
  dec = 8;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x8;
  }

  else if (s[j] == '9'){
  bin[0]=1; bin[1]=0; bin[2]=0; bin[3]=1; 
  dec = 9;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x9;
  }

  else if (s[j] == 'A'){
  bin[0]=0; bin[1]=1; bin[2]=0; bin[3]=1; 
  dec = 10;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xa;
  }

  else if (s[j] == 'B'){
  bin[0]=1; bin[1]=1; bin[2]=0; bin[3]=1; 
  dec = 11;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xb;
  }

  else if (s[j] == 'C'){
  bin[0]=0; bin[1]=0; bin[2]=1; bin[3]=1; 
  dec = 12;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xc;
  }

  else if (s[j] == 'D'){
  bin[0]=1; bin[1]=0; bin[2]=1; bin[3]=1; 
  dec = 13;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xd;
  }

  else if (s[j] == '*'){
  bin[0]=0; bin[1]=1; bin[2]=1; bin[3]=1; 
  dec = 14;
  data.Mode = _7SEG_MODE_PATTERN;
  data.Value = 0x80;
  }

  else {
  bin[0]=1; bin[1]=1; bin[2]=1; bin[3]=1; 
  dec = 15;
  data.Mode = _7SEG_MODE_PATTERN;
  data.Value = 0x00;
  }



  /*  Hexadecimal representation with  7  segments on D5   */

  ioctl(fd, _7SEG_IOCTL_ON,NULL);
  data.Which = _7SEG_D5_INDEX;
  ioctl(fd , _7SEG_IOCTL_SET ,&data);
 



  /*Binary representation with D9-D12*/
  if ( bin[0] == 1){
  data2 = LED_D9_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}
  if ( bin[1] == 1){
  data2 = LED_D10_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}
  if ( bin[2] == 1){
  data2 = LED_D11_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}
  if ( bin[3] == 1){
  data2 = LED_D12_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}


  printf("%d " , k);

  printf("%d " , j);
  printf("%d  ", dec ); 
  printf("%d",bin[0]);printf("%d",bin[1]);printf("%d",bin[2]);printf("%d\n",bin[3]);
  


/* binary representation for s[j-1] */
  if (s[(k-1)%(i-1)] == '0'){
  bin[0]=0; bin[1]=0; bin[2]=0; bin[3]=0; 
  dec = 0;  
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x0;
  }

  else if (s[(k-1)%(i-1)] == '1'){
  bin[0]=1; bin[1]=0; bin[2]=0; bin[3]=0; 
  dec = 1;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x1;
  }

  else if (s[(k-1)%(i-1)] == '2'){
  bin[0]=0; bin[1]=1; bin[2]=0; bin[3]=0; 
  dec = 2;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x2;
  }

  else if (s[(k-1)%(i-1)] == '3'){
  bin[0]=1; bin[1]=1; bin[2]=0; bin[3]=0; 
  dec = 3;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x3;
  }

  else if (s[(k-1)%(i-1)] == '4'){
  bin[0]=0; bin[1]=0; bin[2]=1; bin[3]=0; 
  dec = 4;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x4;
  }

  else if (s[(k-1)%(i-1)] == '5'){
  bin[0]=1; bin[1]=0; bin[2]=1; bin[3]=0; 
  dec = 5;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x5;
  }

  else if (s[(k-1)%(i-1)] == '6'){
  bin[0]=0; bin[1]=1; bin[2]=1; bin[3]=0; 
  dec = 6;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x6;
  }

  else if (s[(k-1)%(i-1)] == '7'){
  bin[0]=1; bin[1]=1; bin[2]=1; bin[3]=0; 
  dec = 7;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x7;
  }

  else if (s[(k-1)%(i-1)] == '8'){
  bin[0]=0; bin[1]=0; bin[2]=0; bin[3]=1; 
  dec = 8;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x8;
  }

  else if (s[(k-1)%(i-1)] == '9'){
  bin[0]=1; bin[1]=0; bin[2]=0; bin[3]=1; 
  dec = 9;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x9;
  }

  else if (s[(k-1)%(i-1)] == 'A'){
  bin[0]=0; bin[1]=1; bin[2]=0; bin[3]=1; 
  dec = 10;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xa;
  }

  else if (s[(k-1)%(i-1)] == 'B'){
  bin[0]=1; bin[1]=1; bin[2]=0; bin[3]=1; 
  dec = 11;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xb;
  }

  else if (s[(k-1)%(i-1)] == 'C'){
  bin[0]=0; bin[1]=0; bin[2]=1; bin[3]=1; 
  dec = 12;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xc;
  }

  else if (s[(k-1)%(i-1)] == 'D'){
  bin[0]=1; bin[1]=0; bin[2]=1; bin[3]=1; 
  dec = 13;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xd;
  }

  else if (s[(k-1)%(i-1)] == '*'){
  bin[0]=0; bin[1]=1; bin[2]=1; bin[3]=1; 
  dec = 14;
  data.Mode = _7SEG_MODE_PATTERN;
  data.Value = 0x80;
  }

  else {
  bin[0]=1; bin[1]=1; bin[2]=1; bin[3]=1; 
  dec = 15;
  data.Mode = _7SEG_MODE_PATTERN;
  data.Value = 0x00;
  }



  /*  Hexadecimal representation with  7  segments on D6   */

  ioctl(fd, _7SEG_IOCTL_ON,NULL);
  data.Which = _7SEG_D6_INDEX;
  ioctl(fd , _7SEG_IOCTL_SET ,&data);
 

  /*  Binary representation with D13-D16  */
  if ( bin[0] == 1){
  data2 = LED_D13_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}
  if ( bin[1] == 1){
  data2 = LED_D14_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}
  if ( bin[2] == 1){
  data2 = LED_D15_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}
  if ( bin[3] == 1){
  data2 = LED_D16_INDEX ;
  ioctl(fd, LED_IOCTL_BIT_SET, &data2);}





/* binary representation for s[j-2] */
  if (s[(k-2)%(i-1)] == '0'){
  bin[0]=0; bin[1]=0; bin[2]=0; bin[3]=0; 
  dec = 0;  
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x0;
  }

  else if (s[(k-2)%(i-1)] == '1'){
  bin[0]=1; bin[1]=0; bin[2]=0; bin[3]=0; 
  dec = 1;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x1;
  }

  else if (s[(k-2)%(i-1)] == '2'){
  bin[0]=0; bin[1]=1; bin[2]=0; bin[3]=0; 
  dec = 2;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x2;
  }

  else if (s[(k-2)%(i-1)] == '3'){
  bin[0]=1; bin[1]=1; bin[2]=0; bin[3]=0; 
  dec = 3;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x3;
  }

  else if (s[(k-2)%(i-1)] == '4'){
  bin[0]=0; bin[1]=0; bin[2]=1; bin[3]=0; 
  dec = 4;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x4;
  }

  else if (s[(k-2)%(i-1)] == '5'){
  bin[0]=1; bin[1]=0; bin[2]=1; bin[3]=0; 
  dec = 5;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x5;
  }

  else if (s[(k-2)%(i-1)] == '6'){
  bin[0]=0; bin[1]=1; bin[2]=1; bin[3]=0; 
  dec = 6;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x6;
  }

  else if (s[(k-2)%(i-1)] == '7'){
  bin[0]=1; bin[1]=1; bin[2]=1; bin[3]=0; 
  dec = 7;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x7;
  }

  else if (s[(k-2)%(i-1)] == '8'){
  bin[0]=0; bin[1]=0; bin[2]=0; bin[3]=1; 
  dec = 8;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x8;
  }

  else if (s[(k-2)%(i-1)] == '9'){
  bin[0]=1; bin[1]=0; bin[2]=0; bin[3]=1; 
  dec = 9;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x9;
  }

  else if (s[(k-2)%(i-1)] == 'A'){
  bin[0]=0; bin[1]=1; bin[2]=0; bin[3]=1; 
  dec = 10;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xa;
  }

  else if (s[(k-2)%(i-1)] == 'B'){
  bin[0]=1; bin[1]=1; bin[2]=0; bin[3]=1; 
  dec = 11;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xb;
  }

  else if (s[(k-2)%(i-1)] == 'C'){
  bin[0]=0; bin[1]=0; bin[2]=1; bin[3]=1; 
  dec = 12;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xc;
  }

  else if (s[(k-2)%(i-1)] == 'D'){
  bin[0]=1; bin[1]=0; bin[2]=1; bin[3]=1; 
  dec = 13;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xd;
  }

  else if (s[(k-2)%(i-1)] == '*'){
  bin[0]=0; bin[1]=1; bin[2]=1; bin[3]=1; 
  dec = 14;
  data.Mode = _7SEG_MODE_PATTERN;
  data.Value = 0x80;
  }

  else {
  bin[0]=1; bin[1]=1; bin[2]=1; bin[3]=1; 
  dec = 15;
  data.Mode = _7SEG_MODE_PATTERN;
  data.Value = 0x00;
  }



  /*  Hexadecimal representation with  7  segments on D7   */

  ioctl(fd, _7SEG_IOCTL_ON,NULL);
  data.Which = _7SEG_D7_INDEX;
  ioctl(fd , _7SEG_IOCTL_SET ,&data);
 


/* binary representation for s[j-3] */
  if (s[(k-3)%(i-1)] == '0'){
  bin[0]=0; bin[1]=0; bin[2]=0; bin[3]=0; 
  dec = 0;  
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x0;
  }

  else if (s[(k-3)%(i-1)] == '1'){
  bin[0]=1; bin[1]=0; bin[2]=0; bin[3]=0; 
  dec = 1;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x1;
  }

  else if (s[(k-3)%(i-1)] == '2'){
  bin[0]=0; bin[1]=1; bin[2]=0; bin[3]=0; 
  dec = 2;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x2;
  }

  else if (s[(k-3)%(i-1)] == '3'){
  bin[0]=1; bin[1]=1; bin[2]=0; bin[3]=0; 
  dec = 3;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x3;
  }

  else if (s[(k-3)%(i-1)] == '4'){
  bin[0]=0; bin[1]=0; bin[2]=1; bin[3]=0; 
  dec = 4;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x4;
  }

  else if (s[(k-3)%(i-1)] == '5'){
  bin[0]=1; bin[1]=0; bin[2]=1; bin[3]=0; 
  dec = 5;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x5;
  }

  else if (s[(k-3)%(i-1)] == '6'){
  bin[0]=0; bin[1]=1; bin[2]=1; bin[3]=0; 
  dec = 6;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x6;
  }

  else if (s[(k-3)%(i-1)] == '7'){
  bin[0]=1; bin[1]=1; bin[2]=1; bin[3]=0; 
  dec = 7;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x7;
  }

  else if (s[(k-3)%(i-1)] == '8'){
  bin[0]=0; bin[1]=0; bin[2]=0; bin[3]=1; 
  dec = 8;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x8;
  }

  else if (s[(k-3)%(i-1)] == '9'){
  bin[0]=1; bin[1]=0; bin[2]=0; bin[3]=1; 
  dec = 9;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0x9;
  }

  else if (s[(k-3)%(i-1)] == 'A'){
  bin[0]=0; bin[1]=1; bin[2]=0; bin[3]=1; 
  dec = 10;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xa;
  }

  else if (s[(k-3)%(i-1)] == 'B'){
  bin[0]=1; bin[1]=1; bin[2]=0; bin[3]=1; 
  dec = 11;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xb;
  }

  else if (s[(k-3)%(i-1)] == 'C'){
  bin[0]=0; bin[1]=0; bin[2]=1; bin[3]=1; 
  dec = 12;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xc;
  }

  else if (s[(k-3)%(i-1)] == 'D'){
  bin[0]=1; bin[1]=0; bin[2]=1; bin[3]=1; 
  dec = 13;
  data.Mode = _7SEG_MODE_HEX_VALUE;
  data.Value = 0xd;
  }

  else if (s[(k-3)%(i-1)] == '*'){
  bin[0]=0; bin[1]=1; bin[2]=1; bin[3]=1; 
  dec = 14;
  data.Mode = _7SEG_MODE_PATTERN;
  data.Value = 0x80;
  }

  else {
  bin[0]=1; bin[1]=1; bin[2]=1; bin[3]=1; 
  dec = 15;
  data.Mode = _7SEG_MODE_PATTERN;
  data.Value = 0x00;
  }



  /*  Hexadecimal representation with  7  segments on D8   */

  ioctl(fd, _7SEG_IOCTL_ON,NULL);
  data.Which = _7SEG_D8_INDEX;
  ioctl(fd , _7SEG_IOCTL_SET ,&data);



 
  sleep(1);
  data1 = LED_ALL_OFF;
  ioctl(fd, LED_IOCTL_SET, &data1);

  k++;


 }   /*for s[j]*/






}/*while(1)*/



} /*while(1)*/
close(fd);
}





