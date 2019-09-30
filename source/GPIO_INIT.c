#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "GPIO_INIT.h"

unsigned int *gpioPtr(void)
{
	int fdgpio = open("/dev/gpiomem", O_RDWR);
	
	if (fdgpio <0) {
		printf("unable to open");
	}
	
	unsigned int *gpio = (unsigned int *)mmap(0,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fdgpio,0);
	
	printf("gpio at pointer %p\n", gpio);
	
	return gpio;
}
