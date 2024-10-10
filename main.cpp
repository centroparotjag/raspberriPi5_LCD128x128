#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include "gpio.h"
#include "LC7981.h"



int main(void)
{
	printf("Raspberry Pi 5 LCD128x128 test 1\n");
	wiringPiSetupSys();
	gpio_init();

	init_LC7981(0);

	printf("END INIT LCD128x128\n");

	wr_letters();

	printf("END TEST\n");

	while (true)
	{





	}
	return 0;
}