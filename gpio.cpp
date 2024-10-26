#include <wiringPi.h>
#include "gpio.h"
#include <stdio.h>

void gpio_init(void) {
	// display WG128128B (SANYO LC7981)
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

	// button
	pinMode(BUTTON, INPUT);
	pullUpDnControl(BUTTON, PUD_UP);

	// DS18B20
	pinMode(DQ, INPUT);
	pullUpDnControl(DQ, PUD_UP);

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

	delay(100);
}


int status_button = 0;

int button_read(void) {
	int i;
	int filter = 0xff;

	//--------------- 0 - BUTTON pressed --------------
	i = 0;
	while (i < filter && digitalRead(BUTTON) == 0) { i++; }
	if (i == filter && status_button == 0) {
		status_button = 1;
		return 0;			// button = 0
	}


	//--------------- 1 - BUTTON unpressed --------------
	i = 0;
	while (i < filter && digitalRead(BUTTON) == 1) { i++; }
	if (i == filter && status_button == 1) {
		status_button = 0;
		return 1;			// button = 0
	}

	//--------------- 
	if (digitalRead(BUTTON) == 0) {
		return 0xF0;		// released
	}
	else {
		return 0x0F;		// pinched
	}

}