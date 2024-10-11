#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include "gpio.h"
#include "LC7981.h"
#include "DS18B20.h"

int main(void)
{
	printf("Raspberry Pi 5 LCD128x128 test 1\n");
	wiringPiSetupSys();
	gpio_init();
	init_LC7981(0);		// 0 - character / 1 - GRAPHIC mode
	printf("END INIT LCD128x128\n");
	//wr_letters();
	//wr_film_test();
	printf("END TEST\n");

	while (true)
	{
		read_t_DS18B20_test();
		delay(1500);
	}
	return 0;
}