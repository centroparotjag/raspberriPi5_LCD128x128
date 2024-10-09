#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include "LC7981.h"
#include "gpio.h"

void E_wr(void) {
	digitalWrite(E, 1);
	delay(1); // ms
	digitalWrite(E, 0);
	delay(1); // ms
}


void wr_port_DB8b_LCD(uint8_t DATA) {
	digitalWrite(DB0, (DATA & 0x01)   );
	digitalWrite(DB1, (DATA & 0x02)>>1);
	digitalWrite(DB2, (DATA & 0x04)>>2);
	digitalWrite(DB3, (DATA & 0x08)>>3);
	digitalWrite(DB4, (DATA & 0x10)>>4);
	digitalWrite(DB5, (DATA & 0x20)>>5);
	digitalWrite(DB6, (DATA & 0x40)>>6);
	digitalWrite(DB7, (DATA & 0x80)>>7);
}


void lcd_write_instriction(uint8_t instruction, uint8_t data) {

	digitalWrite(RS, 1);
	digitalWrite(E, 0);
	delay(1); // ms
	wr_port_DB8b_LCD(instruction);
	E_wr();

	digitalWrite(RS, 0);
	delay(1); // ms
	wr_port_DB8b_LCD(data);
	E_wr();
	delay(1); // ms
}



void init_LC7981(uint8_t mode) {

	// reset LCD

	digitalWrite(RES, LOW);  // Off
	delay(1); // ms
	digitalWrite(RES, HIGH);  // On
	delay(1); // ms


	if (mode == LCD_TEXT) {					// LCD_TEXT

		lcd_write_instriction(0x00, 0x3C);
		lcd_write_instriction(0x01, 0x75);
		lcd_write_instriction(0x02, LCD_TEXT_COLUMNS - 1);
		lcd_write_instriction(0x03, 0x4F);
		lcd_write_instriction(0x04, 0x07);
		lcd_write_instriction(0x08, 0x00);
		lcd_write_instriction(0x09, 0x00);
		lcd_write_instriction(0x0A, 0x00);
		lcd_write_instriction(0x0B, 0x00);

	}
	else {								// LCD_GRAPHIC
		lcd_write_instriction(0x00, 0x32);
		lcd_write_instriction(0x01, 0x07);
		lcd_write_instriction(0x02, (LCD_GRAPHIC_WIDTH / 8) - 1);
		lcd_write_instriction(0x03, 0x4F);
		lcd_write_instriction(0x08, 0x00);
		lcd_write_instriction(0x09, 0x00);
		lcd_write_instriction(0x0A, 0x00);
		lcd_write_instriction(0x0B, 0x00);
	}
}


