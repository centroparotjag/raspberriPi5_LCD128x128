#include "DS18B20.h"
#include <stdio.h>
#include <stdint.h>
#include "gpio.h"
#include <wiringPi.h>
#include "LC7981.h"

 //====================================== GPIO ============================================
unsigned char DQ_read(void) {
	char r = digitalRead(DQ);
	//printf("%x", r);

	if (r == 0) {
		return 0;
	}
	return 1;
}

void DQ_OUT(unsigned char Out) {
	if (Out) {
		pinMode(DQ, INPUT);		//input 
	}
	else {
		pinMode(DQ, OUTPUT);	//OUTput
		digitalWrite(DQ, LOW);	  // 0
	}
}

//========================================================================================
//---------------------- PRESENCE -----------------------------
unsigned char DS18B20_RESET_PRESENCE(void)
{
	unsigned char STATUS_PRESENCE;
	delay(1);
	DQ_OUT(0);
	delay(1);
	DQ_OUT(1);
	delayMicroseconds(60);				// 60u
	
	STATUS_PRESENCE = DQ_read();
	delay(1);
	return STATUS_PRESENCE;
}

//----------------- MASTER WRITE SLOT ------------------------
void WRITE_SLOT(unsigned char SLOT) {

	DQ_OUT(0);

	if (SLOT)	//////////// 1 ///////////   --|_|---------
	{
		delayMicroseconds(2);			// 5u
		DQ_OUT(1);
		delayMicroseconds(62); 			// 62u
	}
	else		//////////// 0 ///////////	--|_________|--
	{
		delayMicroseconds(65); 			// 65u
		DQ_OUT(1);
		delayMicroseconds(5);			// 5u
	}
}

//---------- MASTER READ SLOT ---------------------------------------
unsigned char READ_SLOT(void) {
	unsigned char READ = 0;
	DQ_OUT(0);
	delayMicroseconds(5);				// 5u (78i = 10us)
	DQ_OUT(1);
	delayMicroseconds(5);				// 5u (78i = 10us)
	READ = DQ_read();
	delayMicroseconds(70);				// 70u (78i = 10us)
	return READ;
}

//----------- MASTER READ BYTE (Little endian) ----------------------
unsigned char READ_BYTE(void) {
	unsigned char BYTE = 0;
	for (unsigned char i = 0; i < 8; ++i)
	{
		BYTE |= ((READ_SLOT()) << i);
	}
	return BYTE;
}

//----------- MASTER WRITE BYTE (Little endian) ----------------------
void WRITE_BYTE(unsigned char DATA) {
	unsigned char arr[9] = { 0 };

	for (uint8_t i = 0; i < 8; ++i)
	{
		arr[i] = (DATA >> i) & 0x01;
	}



	for (uint8_t i = 0; i < 8; ++i)
	{
		WRITE_SLOT(arr[i]);
	}
}

//----------------------- CRC --------------------------------------------------
unsigned char dsCRC8(const unsigned char* addr, unsigned char len) {			//CRC 1-WIRE DS18B20

	unsigned char crc = 0;
	while (len--)
	{
		unsigned char inbyte = *addr++;
		for (unsigned char i = 8; i; i--)
		{
			unsigned char mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) crc ^= 0x8C;
			inbyte >>= 1;
		}
	}
	return crc;
}

unsigned char SCRATCHPAD[9];
//---------------------- READ SCRATCHPAD --------------------------------------
unsigned int SCRATCHPAD_READ(void) {

	unsigned char i, SCRATCHPAD_i[9];

	WRITE_BYTE(SKIP_ROM);					// DQ, DATA
	WRITE_BYTE(READ_SCRATCHPAD);			// DQ, DATA

	//-------- READ 9 BYTE SCRATCHPAD --------
	for (i = 0; i < 9; ++i) {
		SCRATCHPAD_i[i] = READ_BYTE();	// DQ
	}

	//---- comparison of the CRC --------
	if (SCRATCHPAD_i[8] == dsCRC8(SCRATCHPAD_i, 8))
	{
		for (i = 0; i < 9; ++i) {
			SCRATCHPAD[i] = SCRATCHPAD_i[i];	// Copy SCRATCHPAD to in external buffer
		}

		unsigned int T_src = 0;
		T_src = SCRATCHPAD[0];
		T_src |= SCRATCHPAD[1] << 8;

		//SCRATCHPAD[1] = 0xaa;

		//printf("SCRATCHPAD [0]=0x%02x, [1]=0x%02x, [2]=0x%02x, [3]=0x%02x, [4]=0x%02x, [5]=0x%02x, [6]=0x%02x, [7]=0x%02x\n",
		//	SCRATCHPAD[0], SCRATCHPAD[1], SCRATCHPAD[2], SCRATCHPAD[3], SCRATCHPAD[4], SCRATCHPAD[5], SCRATCHPAD[6], SCRATCHPAD[7]);

		return T_src;								// CRC - OK (0x01)

	}
	else
	{
		for (i = 0; i < 9; ++i) {
			SCRATCHPAD[i] = 0;					// RESET DATA in external SCRATCHPAD buffer
		}
		return 0;							// CRC - ERROR (0x00)
	}
}

//----------------------- TEST AND WRITE DEFAULT STATE SRAM -------------
unsigned char TEST_AND_WRITE_DEFAULT_SRAM(void) {
	unsigned char i, CONFIG_RESOLUTION;
	CONFIG_RESOLUTION = RESOLUTION_12b;			// SET DEFAULT RESOLUTION
	if (DS18B20_RESET_PRESENCE()) {
		return ERROR;							// error
	}
	else {}

	//--------------- READ SCRATCHPAD ----------------------------
	i = 0;
	while (SCRATCHPAD_READ() == 0 && i < 10) { i++; }

	//-------------- check resolution in SRAM --------------------
	if (SCRATCHPAD[4] == CONFIG_RESOLUTION) {	// Config Register
		return OK;
	}
	else {}

	//================= NEW SEQUENCE WRITE SCRATCHPAD ===================
	DS18B20_RESET_PRESENCE();

	WRITE_BYTE(SKIP_ROM);					// DQ, DATA
	WRITE_BYTE(WRITE_SCRATCHPAD);			// DQ, DATA

	WRITE_BYTE(0x4B);						// DQ, Th (default manufacture). SRAM 0x02
	WRITE_BYTE(0x46);						// DQ, Tl (default manufacture). SRAM 0x03
	WRITE_BYTE(CONFIG_RESOLUTION);			// DQ, config. 				     SRAM 0x04

	DS18B20_RESET_PRESENCE();

	if (!SCRATCHPAD_READ()) {				//READ SCRATCHPAD and CRC CHECK
		return 255;
	}
	else {}

	//-------------- check resolution in SRAM --------------------
	if (SCRATCHPAD[4] != CONFIG_RESOLUTION) {	// Config. Register
		return 254;
	}
	else {}

	//================== SAVE ROM in SRAM ==============================
	DS18B20_RESET_PRESENCE();
	WRITE_BYTE(SKIP_ROM);					// DQ, DATA
	WRITE_BYTE(COPY_SCRATCHPAD);			// DQ, DATA

	delayMicroseconds(15);

	return OK;							   // OK write SRAM
}

//*********************** READ ROM 64b *********************************************

//unsigned char ROM [9] = {};
//
//unsigned char READ_ROM_64 (void){
	//unsigned char i, ROM_i [9];
//
	//if (DS18B20_RESET_PRESENCE ()) {
		//return ERROR;						// error
	//}
	//else {}
//
	//WRITE_BYTE (READ_ROM);					// DQ, DATA
//
	//-------- READ 9 BYTE ROM --------
	//for (i = 0; i < 8; ++i) {
		//ROM_i [i] = READ_BYTE (  );			// DQ
	//}
//
	//---- comparison of the CRC --------
	//if (ROM_i[7] == dsCRC8(ROM_i, 7))
	//{
		//for (i = 0; i < 8; ++i) {
			//ROM [i] = ROM_i [i];			// Copy ROM to in external buffer
		//}
		//return OK;							// CRC - OK (0x01)
	//}
	//else
	//{
		//for (i = 0; i < 9; ++i) {
			//ROM [i] = 0;					// RESET DATA in external SCRATCHPAD buffer
		//}
		//return ERROR;						// CRC - ERROR (0x00)
	//}
//}

//*********************** READ TEMPERATURE ******************************************

unsigned char FLAG_convert = 0;
unsigned char internal_T = 0;
unsigned int T_src;

void time_out_convert_t(unsigned char  timer, unsigned char  conversial_period) {
	 

	if (!(timer % conversial_period) && FLAG_convert == 0)
	{
		TEST_AND_WRITE_DEFAULT_SRAM();
		delay(1);
		start_t_convert_DS12B20();
		internal_T = timer + 1;
		FLAG_convert = 1;
	}

	if (timer == internal_T)
	{
		T_src = read_t_after_convert_DS12B20();
		FLAG_convert = 0;
		internal_T = 0;
	}
}


unsigned char start_t_convert_DS12B20(void) {
	//--------- CONVERT T -----------------------------------
	if (DS18B20_RESET_PRESENCE()) {
		return 0;					// error
	}
	else {}

	WRITE_BYTE(SKIP_ROM);				// DQ, DATA  0xCC
	WRITE_BYTE(CONVERT_T);				// DQ, DATA 0x44
	return 1;
}

unsigned int read_t_after_convert_DS12B20(void) {
	//---------- READ T ------------------------------------
	if (DS18B20_RESET_PRESENCE()) {
		return 0;							// error
	}
	else {}

	return SCRATCHPAD_READ();
}



unsigned int READ_TEMPERATURE(void) {

	//--------- CONVERT T -----------------------------------
	if (DS18B20_RESET_PRESENCE()) {
		return ERROR;							// error
	}
	else {}

	WRITE_BYTE(SKIP_ROM);					// DQ, DATA
	WRITE_BYTE(CONVERT_T);					// DQ, DATA
	delay(751);

	//---------- READ T ------------------------------------
	if (DS18B20_RESET_PRESENCE()) {
		return ERROR;							// error
	}
	else {}

	return SCRATCHPAD_READ();
}



//========================================================================
void print_t_DS18B20_in_terminal(int mode_lcd) {
	unsigned int T_src = READ_TEMPERATURE();
	char s = 0;
	float Td;

	char sign = T_src >> 15;
	unsigned long long T = T_src & 0x07FF;

	if (sign) {
		T = (0x0800 - T);
		s = '-';
	}
	else {
		wr_text_in_character_mode(0, 0, "+");
		s = '+';
	}

	Td = T;
	Td *= 0.0625;

	printf("Thread \"Worker\": t = %c%.4f\n", s, Td);

	char ptr[200] = {0};

	 sprintf (ptr, "t = %c%.4f\n", s, Td);

	 if (!mode_lcd) {

	 }
	 else {
		 init_LC7981(0);		// 0 - character / 1 - GRAPHIC mode
		 wr_text_in_character_mode(0, 3, ptr);
	 }



}

void read_t_DS18B20_test(void) {
	uint16_t S2, S1, S0;

	unsigned int T_src = READ_TEMPERATURE();

	//T_src = 0x024A;			// 36.6

	char sign = T_src >> 15;
	unsigned long long T = T_src & 0x07FF;

	if (sign) {
		T = (0x0800 - T);
		wr_text_in_character_mode(0, 0, "-");
	}
	else {
		wr_text_in_character_mode(0, 0, "+");
	}

	T *= 625;
	T /= 1000;

	if (T > 1000)
	{
		T /= 10;
		S0 = T % 10;
		T /= 10;
		S1 = T % 10;
		T /= 10;
		S2 = T % 10;

		wr_letter(0, 1, S2 + 0x30);
		wr_letter(0, 2, S1 + 0x30);
		wr_letter(0, 3, S0 + 0x30);
		wr_letter(0, 4, 0xDF);
		wr_letter(0, 5, 'C');
	}
	else {
		S0 = T % 10;
		T /= 10;
		S1 = T % 10;
		T /= 10;
		S2 = T % 10;

		if (S2 > 0) {
			wr_letter(0, 1, S2 + 0x30);
		}

		wr_letter(0, 2, S1 + 0x30);
		wr_letter(0, 3, '.');
		wr_letter(0, 4, S0 + 0x30);
		wr_letter(0, 5, 0xDF);
		wr_letter(0, 6, 'C');
	}
}