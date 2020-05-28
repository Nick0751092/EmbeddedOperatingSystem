#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"


int main(int argc , char *argv[])
{ 
  int fd;
  int retval;
  unsigned short data;

if ((fd = open("/dev/lcd" , O_RDWR)) < 0 ) 
    {
    printf("open /dev/lcd error\n");
    return (-1);
    }

data = LED_ALL_ON;
ioctl(fd, LED_IOCTL_SET, &data);
printf("Turn on all LED lamps\n");
sleep(1);

data = LED_ALL_OFF;
ioctl(fd, LED_IOCTL_SET, &data);
printf("Turn off all LED lamps\n");
sleep(1);

data = LED_D9_INDEX ;
ioctl(fd, LED_IOCTL_BIT_SET, &data);
printf("Turn on D9 \n");
sleep(1);

data = LED_D9_INDEX ;
ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
printf("Turn off D9 \n");
sleep(1);


close(fd);
return 0;
}
