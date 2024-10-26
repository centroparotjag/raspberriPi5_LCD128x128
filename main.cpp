#include <wiringPi.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "gpio.h"
#include "LC7981.h"
#include "DS18B20.h"


int flag_button = 0;

void* worker(void* data)
{
	int i = 0;
	while (1)
	{
		start_t_convert_DS12B20();
		printf("Thread \"Worker\": start convert t (DS18B20) = %d\n", i);
		usleep(751000);	// 751mS
		print_t_DS18B20_in_terminal(flag_button);
		i++;
		sleep(10);
	}

	printf("Thread worker done!\n");
	return NULL;
}


int main(void)
{
	printf("Raspberry Pi 5 LCD128x128 test 1\n");
	wiringPiSetupSys();
	gpio_init();
	delay(200);
	init_LC7981(1);		// 0 - character / 1 - GRAPHIC mode
	printf("END INIT LCD128x128\n");
	wr_film_test();

	pthread_t th1;
	pthread_create(&th1, NULL, worker, NULL);

	
	while (1)
	{
		
		if (button_read() == 0) {
			printf("main button press. flag_button = %d\n", flag_button);

			if (flag_button) {
				init_LC7981(1);		// 0 - character / 1 - GRAPHIC mode
				wr_film_test();
				flag_button = 0;
			}
			else {
				flag_button = 1;
				print_t_DS18B20_in_terminal(flag_button);
			}

		}
		usleep(50000);
	}

	return 0;
}


//int main(void)
//{
//	//printf("Raspberry Pi 5 LCD128x128 test 1\n");
//	//wiringPiSetupSys();
//	//gpio_init();
//	//init_LC7981(1);		// 0 - character / 1 - GRAPHIC mode
//	//printf("END INIT LCD128x128\n");
//	////wr_letters();
//	//wr_film_test();
//	////printf("END TEST\n");
//
//
//
//
//
//	//while (true)
//	//{
//	//	//read_t_DS18B20_test();
//	//	//delay(1500);
//	//}
//	return 0;
//}