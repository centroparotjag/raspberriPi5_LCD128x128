#include <wiringPi.h>
#include <stdio.h>
#include <stdint.h>
#include "LC7981.h"
#include "gpio.h"

void E_wr(void) {
	digitalWrite(E, 1);
	delayMicroseconds(1);
	digitalWrite(E, 0);
	delayMicroseconds(1);
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
	delayMicroseconds(1);
	wr_port_DB8b_LCD(instruction);
	E_wr();

	digitalWrite(RS, 0);
	delayMicroseconds(1);
	wr_port_DB8b_LCD(data);
	E_wr();
	delayMicroseconds(1);
}

uint8_t lcd_mode; 		//!< Currently used mode, can be text or graphic.
uint8_t lcd_curline;	//!< Current line. Used for scrolling in text mode.

void init_LC7981(uint8_t mode) {
	lcd_mode = mode;

	// reset LCD
	digitalWrite(RES, LOW);  // Off
	delay(1); // ms
	digitalWrite(RES, HIGH);  // On
	delay(1); // ms

	if (mode == LCD_TEXT) {					// LCD_TEXT
		lcd_write_instriction(0x00, 0x3C);	// Mode control
		lcd_write_instriction(0x01, 0x75);	// setting the character pitch
		lcd_write_instriction(0x02, LCD_TEXT_COLUMNS - 1);	// number of character
		lcd_write_instriction(0x03, 0x20);	// time devision number 0x4F
		lcd_write_instriction(0x04, 0x07);	// cursor position
		lcd_write_instriction(0x08, 0x00);	// display start lower address
		lcd_write_instriction(0x09, 0x00);	// display start upper address
		lcd_write_instriction(0x0A, 0x00);	// cursor start lower address
		lcd_write_instriction(0x0B, 0x00);	// cursor start upper address
	}
	else {									// LCD_GRAPHIC
		lcd_write_instriction(0x00, 0x32);	// Mode control
		lcd_write_instriction(0x01, 0x07);	// setting the character pitch
		lcd_write_instriction(0x02, (LCD_GRAPHIC_WIDTH / 8) - 1);	// number of character
		lcd_write_instriction(0x03, 0x30);	// time devision number 0x4F
		lcd_write_instriction(0x08, 0x00);	// display start lower address
		lcd_write_instriction(0x09, 0x00);	// display start upper address
		lcd_write_instriction(0x0A, 0x00);	// cursor start lower address
		lcd_write_instriction(0x0B, 0x00);	// cursor start upper address
	}
	lcd_clear();
}


/**
 * Clears the display by setting the whole memory to 0.
 */
void lcd_clear() {
	uint16_t i;
	if (lcd_mode == LCD_TEXT) {
		lcd_write_instriction(0x0A, 0x00);	// cursor start lower address
		lcd_write_instriction(0x0B, 0x00);	// cursor start upper address
		for (i = 1; i <= LCD_TEXT_LINES * LCD_TEXT_COLUMNS; i++) {
			lcd_write_instriction(0x0C, 'c');	// writing display data
		}
		lcd_write_instriction(0x0A, 0x00);	// cursor start lower address
		lcd_write_instriction(0x0B, 0x00);	// cursor start upper address
		lcd_curline = 0;
	}
	else {
		lcd_write_instriction(0x0A, 0x00);	// cursor start lower address
		lcd_write_instriction(0x0B, 0x00);	// cursor start upper address
		for (i = 0; i < ((LCD_GRAPHIC_WIDTH * LCD_GRAPHIC_HEIGHT) / 8); i++) {
			lcd_write_instriction(0x0C, 0x81);	// writing display data				//(uint8_t)i & 0xff
		}
		lcd_write_instriction(0x0A, 0x00);	// cursor start lower address
		lcd_write_instriction(0x0B, 0x00);	// cursor start upper address
	}
}
