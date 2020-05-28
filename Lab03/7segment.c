#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

int main(){
_7seg_info_t data;
int  fd,ret,i;
int  dec=9;

if ((fd = open("/dev/lcd" , O_RDWR)) < 0 ) 
    {
    printf("open /dev/lcd error\n");
    return (-1);
    }

ioctl(fd, _7SEG_IOCTL_ON,NULL);

data.Mode = _7SEG_MODE_HEX_VALUE;
data.Which = _7SEG_D5_INDEX;
data.Value = 0xC ;
ioctl(fd , _7SEG_IOCTL_SET ,&data);
sleep(1);
/*
data.Mode = _7SEG_MODE_PATTERN;
data.Which = _7SEG_D5_INDEX ;/*| _7SEG_D8_INDEX;
data.Value = 0x3f ;
ioctl(fd , _7SEG_IOCTL_SET ,&data);
sleep(1);*/

ioctl(fd , _7SEG_IOCTL_OFF , NULL);
close(fd);
}
