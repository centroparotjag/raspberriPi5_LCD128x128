#ifndef LC7981_H_
#define LC7981_H_

#define LCD_TEXT 0				    //!< Constant for text mode
#define LCD_TEXT_LINES   16		//!< Number of lines in text mode
#define LCD_TEXT_COLUMNS 32      //!< Number of columns in text mode 24 32

#define LCD_GRAPHIC 1				//!< Constant for graphic mode
#define LCD_GRAPHIC_WIDTH  256		//!< Horizontal display size in pixels
#define LCD_GRAPHIC_HEIGHT 128		//!< Vertical display size in pixels
#define PIXEL_ON 1					//!< see lcd_plot_pixel
#define PIXEL_OFF 0					//!< see lcd_plot_pixel

void init_LC7981(uint8_t mode);
void wr_port_DB8b_LCD(uint8_t DATA);
void lcd_write_instriction(uint8_t instruction, uint8_t data);
void E_wr(void);
void lcd_clear();
void wr_letter(uint8_t row, uint8_t column, uint8_t letter);
void wr_text_in_character_mode(uint8_t row, uint8_t column, char* str);
void wr_GRAPHIC(uint8_t* img);



//    test foo
void wr_letter_test(void);
void wr_film_test(void);


#endif // !LC7981_H_#pragma once
