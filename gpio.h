#ifndef MAIN_H_
#define MAIN_H_

	// LCD 128x128 pins. BCM num
	#define	DB0	14 
	#define	DB1	15 
	#define	DB2	18 		

	#define	DB3	23
	#define	DB4	24

	#define	DB5	25
	#define	DB6	8
	#define	DB7	7

	#define	RS	12
	#define	E	16
	#define	CS	20
	#define	RES	21
	//-------------------
	#define BUTTON 26 
	#define DQ 4		// DS18B20


	void gpio_init(void);
	int button_read(void);



#endif // !MAIN_H_
