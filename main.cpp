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
		//print_t_DS18B20_in_terminal(0);
		i++;
		sleep(10);		// 10s period measurement T
	}

	return NULL;
}


int main(void)
{

	#include "img_data.h"

	printf("Raspberry Pi 5 LCD128x128 test 1\n");
	wiringPiSetupSys();
	gpio_init();
	delay(200);
	init_LC7981(1);		// 0 - character / 1 - GRAPHIC mode

	copy_buf(true, false, image_menu1);

	wr_text_font10x16_grafic_mode(57, 40, "TEST!");

	
	pthread_t th1;
	pthread_create(&th1, NULL, worker, NULL);

	
	while (1)
	{
		

		if (button_read() == 0) {
			printf("main button press. flag_button = %d\n", flag_button);
			if (flag_button) {
				flag_button = 0;


			}
			else {
				flag_button = 1;

				

			}
		}
		usleep(50000);	// 50ms
	}

	return 0;
}
