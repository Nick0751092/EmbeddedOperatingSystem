#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

#define VK_S2  1    /* ASCII = '1' */
#define VK_S3  2    /* ASCII = '2' */
#define VK_S4  3    /* ASCII = '3' */
#define VK_S5  10   /* ASCII = 'A' */
#define VK_S6  4
#define VK_S7  5
#define VK_S8  6
#define VK_S9  11
#define VK_S10 7
#define VK_S11 8
#define VK_S12 9
#define VK_S13 12 
#define VK_S14 14   /* ASCII = '*' */
#define VK_S15 0
#define VK_S16 15   /* ASCII = '#' */
#define VK_S17 13


int main ()
{
unsigned short  key ;
int      ret;

int  fd;
lcd_write_info_t display;




if ((fd = open("/dev/lcd" , O_RDWR)) < 0 ) return (-1);

ioctl(fd,KEY_IOCTL_CLEAR, key);
ioctl(fd, LCD_IOCTL_CLEAR, NULL);
while(1) {
    ret = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);
    if (ret < 0) {
      sleep(1);
      continue;
      }
  ret = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
  if ((key & 0xFF) == '#') 
  break;   /* terminate */

  else if ((key & 0xFF) == '1'){
  display.Count = sprintf((char *) display.Msg,"1");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '2'){
  display.Count = sprintf((char *) display.Msg,"2");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '3'){
  display.Count = sprintf((char *) display.Msg,"3");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '4'){
  display.Count = sprintf((char *) display.Msg,"4");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '5'){
  display.Count = sprintf((char *) display.Msg,"5");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '6'){
  display.Count = sprintf((char *) display.Msg,"6");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '7'){
  display.Count = sprintf((char *) display.Msg,"7");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '8'){
  display.Count = sprintf((char *) display.Msg,"8");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '9'){
  display.Count = sprintf((char *) display.Msg,"9");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '0'){
  display.Count = sprintf((char *) display.Msg,"0");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == 'A'){
  display.Count = sprintf((char *) display.Msg,"A");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}
  
  else if ((key & 0xFF) == 'B'){
  display.Count = sprintf((char *) display.Msg,"B");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == 'C'){
  display.Count = sprintf((char *) display.Msg,"C");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == 'D'){
  display.Count = sprintf((char *) display.Msg,"D");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else if ((key & 0xFF) == '*'){
  display.Count = sprintf((char *) display.Msg,"*");
  ioctl(fd , LCD_IOCTL_WRITE, &display );}

  else
  ioctl(fd, LCD_IOCTL_CLEAR, NULL);

}
close(fd);
return 0;
}
