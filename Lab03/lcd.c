#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"


int main ()
{

int  fd;
lcd_write_info_t display;


if ((fd = open("/dev/lcd" , O_RDWR)) < 0 ) 
    {
    printf("open /dev/lcd error\n");
    return (-1);
    }

ioctl(fd,LCD_IOCTL_CLEAR, NULL);

display.Count = sprintf((char *) display.Msg, "FUCK YOU\n");

ioctl(fd, LCD_IOCTL_WRITE, &display);

ioctl(fd, LCD_IOCTL_CUR_GET, &display);
printf("The cursor position is at (x,y) = (%d,%d)\n",
       display.CursorX, display.CursorY);


close(fd);
return 0;
}
