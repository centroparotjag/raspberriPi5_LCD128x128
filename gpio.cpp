#include <wiringPi.h>
#include "gpio.h"
#include <stdio.h>

int gpio_init(void) {

	//wiringPiSetupSys();

	pinMode(DB0, OUTPUT);
	pinMode(DB1, OUTPUT);
	pinMode(DB2, OUTPUT);
	pinMode(DB3, OUTPUT);
	pinMode(DB4, OUTPUT);
	pinMode(DB5, OUTPUT);
	pinMode(DB6, OUTPUT);
	pinMode(DB7, OUTPUT);
	pinMode(RS,  OUTPUT);
	pinMode(E,   OUTPUT);
	pinMode(CS,  OUTPUT);
	pinMode(RES, OUTPUT);

	// set default
	digitalWrite(DB0, LOW);	  // Off
	digitalWrite(DB1, LOW);	  // Off
	digitalWrite(DB2, LOW);	  // Off
	digitalWrite(DB3, LOW);	  // Off
	digitalWrite(DB4, LOW);	  // Off
	digitalWrite(DB5, LOW);	  // Off
	digitalWrite(DB6, LOW);	  // Off
	digitalWrite(DB7, LOW);	  // Off
	digitalWrite(RS,  LOW);	  // Off
	digitalWrite(E,   LOW);	  // Off
	digitalWrite(CS,  LOW);	  // Off
	digitalWrite(RES, LOW);	  // Off

	return 0;
}