/*
 * graphic-lcd.c
 *
 *  Created on: 06.12.2009
 *      Author: steene
 */


#include "graphic-lcd.h"

volatile char lcd_content[512];


const uint8_t charset[] PROGMEM = {
				0x02, 0x00, 0x00, //' '
				0x02, 0x5F, 0x00, //'!'
				0x04, 0x03, 0x00, 0x03, 0x00, //'"'
				0x06, 0x14, 0x7F, 0x14, 0x7F, 0x14, 0x00, //'#'
				0x06, 0x24, 0x2A, 0x7F, 0x2A, 0x12, 0x00, //'$'
				0x06, 0x23, 0x13, 0x08, 0x64, 0x62, 0x00, //'%'
				0x06, 0x36, 0x49, 0x55, 0x22, 0x50, 0x00, //'&'
				0x06, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, //'''

				0x06, 0x00, 0x1C, 0x22, 0x41, 0x00, 0x00, //'('
				0x06, 0x00, 0x41, 0x22, 0x1C, 0x00, 0x00, //')'
				0x06, 0x14, 0x08, 0x3E, 0x08, 0x14, 0x00, //'*'
				0x06, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, //'+'
				0x06, 0x00, 0x00, 0x50, 0x30, 0x00, 0x00, //','
				0x06, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, //'-'
				0x06, 0x00, 0x00, 0x60, 0x60, 0x00, 0x00, //'.'
				0x06, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, //'/'

				0x06, 0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, //'0'
				0x06, 0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, //'1'
				0x06, 0x42, 0x61, 0x51, 0x49, 0x46, 0x00, //'2'
				0x06, 0x21, 0x41, 0x45, 0x4B, 0x31, 0x00, //'3'
				0x06, 0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, //'4'
				0x06, 0x27, 0x45, 0x45, 0x45, 0x39, 0x00, //'5'
				0x06, 0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, //'6'
				0x06, 0x03, 0x01, 0x71, 0x09, 0x07, 0x00, //'7'
				0x06, 0x36, 0x49, 0x49, 0x49, 0x36, 0x00, //'8'
				0x06, 0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, //'9'

				0x06, 0x00, 0x00, 0x36, 0x36, 0x00, 0x00, //':'
				0x06, 0x00, 0x00, 0x56, 0x36, 0x00, 0x00, //';'
				0x06, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, //'<'
				0x06, 0x14, 0x14, 0x14, 0x14, 0x14, 0x00, //'='
				0x06, 0x41, 0x22, 0x14, 0x08, 0x00, 0x00, //'>'
				0x06, 0x02, 0x01, 0x51, 0x09, 0x06, 0x00, //'?'
				0x06, 0x32, 0x49, 0x79, 0x41, 0x3E, 0x00, //'@'

				0x06, 0x7E, 0x11, 0x11, 0x11, 0x7E, 0x00, //A
				0x06, 0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, //B
				0x06, 0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, //C
				0x06, 0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00, //D
				0x06, 0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, //E
				0x06, 0x7F, 0x09, 0x09, 0x09, 0x01, 0x00, //F
				0x06, 0x3E, 0x41, 0x49, 0x49, 0x7A, 0x00, //G
				0x06, 0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, //H
				0x06, 0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, //I
				0x06, 0x20, 0x40, 0x41, 0x3F, 0x01, 0x00, //J
				0x06, 0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, //K
				0x06, 0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, //L
				0x06, 0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00, //M
				0x06, 0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00, //N
				0x06, 0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, //O
				0x06, 0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, //P
				0x06, 0x3E, 0x41, 0x51, 0x21, 0x5E, 0x00, //Q
				0x06, 0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, //R
				0x06, 0x46, 0x49, 0x49, 0x49, 0x31, 0x00, //S
				0x06, 0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, //T
				0x06, 0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, //U
				0x06, 0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00, //V
				0x06, 0x3F, 0x40, 0x38, 0x40, 0x3F, 0x00, //W
				0x06, 0x63, 0x14, 0x08, 0x14, 0x63, 0x00, //X
				0x06, 0x07, 0x08, 0x70, 0x08, 0x07, 0x00, //Y
				0x06, 0x61, 0x51, 0x49, 0x45, 0x43, 0x00, //Z

				0x06, 0x00, 0x7F, 0x41, 0x41, 0x00, 0x00, //' ' <= '['
				0x06, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, //' ' <= '\'
				0x06, 0x00, 0x41, 0x41, 0x7F, 0x00, 0x00, //' ' <= ']'
				0x06, 0x04, 0x02, 0x01, 0x02, 0x04, 0x00, //' ' <= '^'
				0x06, 0x40, 0x40, 0x40, 0x40, 0x40, 0x00, //' ' <= '_'
				0x06, 0x00, 0x01, 0x02, 0x04, 0x00, 0x00, //' ' <= '`'

				0x06, 0x20, 0x54, 0x54, 0x54, 0x78, 0x00, //a
				0x06, 0x7F, 0x48, 0x44, 0x44, 0x38, 0x00, //b
				0x06, 0x38, 0x44, 0x44, 0x44, 0x20, 0x00, //c
				0x06, 0x38, 0x44, 0x44, 0x48, 0x7F, 0x00, //d
				0x06, 0x38, 0x54, 0x54, 0x54, 0x18, 0x00, //e
				0x06, 0x08, 0x7E, 0x09, 0x01, 0x02, 0x00, //f
				0x06, 0x0C, 0x52, 0x52, 0x52, 0x3C, 0x00, //g
				0x06, 0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, //h
				0x06, 0x00, 0x44, 0x7D, 0x40, 0x00, 0x00, //i
				0x06, 0x20, 0x40, 0x44, 0x3D, 0x00, 0x00, //j
				0x06, 0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, //k
				0x06, 0x00, 0x41, 0x7F, 0x40, 0x00, 0x00, //l
				0x06, 0x78, 0x08, 0x30, 0x08, 0x70, 0x00, //m
				0x06, 0x78, 0x10, 0x08, 0x08, 0x70, 0x00, //n
				0x06, 0x30, 0x48, 0x48, 0x48, 0x30, 0x00, //o
				0x06, 0x7C, 0x14, 0x14, 0x14, 0x08, 0x00, //p
				0x06, 0x08, 0x14, 0x14, 0x18, 0x7C, 0x00, //q
				0x06, 0x7C, 0x08, 0x04, 0x04, 0x08, 0x00, //r
				0x06, 0x48, 0x54, 0x54, 0x54, 0x20, 0x00, //s
				0x06, 0x04, 0x3F, 0x44, 0x40, 0x20, 0x00, //t
				0x06, 0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00, //u
				0x06, 0x1C, 0x20, 0x40, 0x20, 0x1C, 0x00, //v
				0x06, 0x3C, 0x40, 0x38, 0x40, 0x3C, 0x00, //w
				0x06, 0x44, 0x28, 0x10, 0x28, 0x44, 0x00, //x
				0x06, 0x0C, 0x50, 0x50, 0x50, 0x3C, 0x00, //y
				0x06, 0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, //z
				0x06, 0x00, 0x08, 0x36, 0x41, 0x00, 0x00, //'{'
				0x06, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00, //'|'
				0x06, 0x00, 0x41, 0x36, 0x08, 0x00, 0x00, //'}'
				0x06, 0x08, 0x04, 0x08, 0x10, 0x08, 0x00, //'~'

				 };



void lcd_clear(void) {

	lcd_rect(0, 0, 255, 15, 0);
}



void lcd_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color) {

	uint8_t x_int, y_int;

	if (color) {
		for(x_int = x1; x_int < x2; x_int++) {
			for(y_int = y1; y_int < y2; y_int++) {
				lcd_pset(x_int, y_int);
			}
		}
	} else {
		for(x_int = x1; x_int < x2; x_int++) {
			for(y_int = y1; y_int < y2; y_int++) {
				lcd_pclear(x_int, y_int);
			}
		}
	}
}


void lcd_pset(uint8_t x, uint8_t y) {

	lcd_content[y*32 + (x/8)] |= (1<<(x%8));
}

void lcd_pclear(uint8_t x, uint8_t y) {

	lcd_content[y*32 + (x/8)] &= ~(1<<(x%8));
	//lcd_content[x + ((y/8)*128)] &= ~(1<<(y%8));
}

uint8_t lcd_color(uint8_t x, uint8_t y) {

	return (lcd_content[y*32 + (x/8)] & (1<<(x%8))) != 0;
	//return ((lcd_content[x + ((y/8)*128)] & (1<<(y%8))) != 0);
}

inline void lcd_pput(uint8_t x, uint8_t y, uint8_t color) {

	if (color)
		lcd_pset(x,y);
	else
		lcd_pclear(x,y);
}

void lcd_frame(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {

	uint8_t x_int, y_int;
	for(x_int = x1; x_int < x2; x_int++) {
		lcd_pset(x_int, y1);
		lcd_pset(x_int, y2);
	}

	for(y_int = y1; y_int <= y2; y_int++) {
		lcd_pset(x1, y_int);
		lcd_pset(x2, y_int);
	}
}

void lcd_hline(uint8_t x1, uint8_t x2, uint8_t y) {

	uint8_t x_int;
	for(x_int = x1; x_int < x2; x_int++)
		lcd_pset(x_int, y);

}

void lcd_vline(uint8_t y1, uint8_t y2, uint8_t x) {

	uint8_t y_int;
	for(y_int = y1; y_int < y2; y_int++)
		lcd_pset(x, y_int);
}


uint8_t lcd_char(uint16_t val, uint8_t x, uint8_t y) {

	uint8_t x_int,y_int;

	if (val < 32)
		val = 0;
	else if (val < 128)
		val -= 32;
	else {
		switch(val) {
			case 128: // €
				val = 100;
				break;

			case 167: // §

			case 181: // µ

			case 196: // Ä

			case 214: // Ö

			case 220: // Ü

			case 223: // ß

			case 228: // ä

			case 246: // ö

			case 252: // ü
				break;
		}
	}

	uint8_t charwidth = 0;
	uint16_t charptr = 0;
	while(val--) {
		charptr += pgm_read_byte(&charset[charptr]) + 1;
	}
	charwidth = pgm_read_byte(&charset[charptr++]);

	for(x_int = 0; x_int < charwidth; x_int++) {
		uint8_t charpx = pgm_read_byte(&charset[charptr++]);
		for(y_int = 0; y_int < 8; y_int++) {
			if (0x01 & charpx)
				lcd_pset(x + x_int, y + y_int);

			charpx >>=1;
		}
	}
	return charwidth;
}

/*
void lcd_string_scroll_update(char *str) {
	for(uint8_t x = 0; x < 128; x++) {
		for(uint8_t y = 55; y >= 8; y--) {
			lcd_pput(x,y,lcd_color(x,y - 8));
		}
		for(uint8_t y = 0; y < 8; y++) {
			lcd_pput(x,y,0);
		}
	}

	lcd_string(str, 0, 0, 127, 15);
	lcd_update();
}
*/

void lcd_string(char *str, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {
	uint8_t curr_x = x1;
	while(*str) {
		lcd_char(*(str++), curr_x, y1);
		curr_x += 6;
		if (curr_x > x2) {
			curr_x = x1;
			y1 += 8;

			if (y1 > y2)
				return;
		}
	}
}


void lcd_string_P(PGM_P str, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2) {

	while(pgm_read_byte(str)) {
		lcd_char(pgm_read_byte(str++), x1, y1);
		x1 += 6;
		if (x1 > x2) {
			x1 = 0;
			y1 += 8;

			if (y1 > y2)
				return;
		}
	}
}


void lcd_picture(PGM_P picture, uint8_t x, uint8_t y) {
	uint8_t size_x = pgm_read_byte(picture++);
	uint8_t size_y = pgm_read_byte(picture++);

	uint8_t x_int, y_int;
	uint8_t bit_count = 8;
	uint8_t img = 0;

	for(x_int = x; x_int < (x + size_x); x_int++) {
		for(y_int = y; y_int < (y + size_y); y_int++) {
			if (bit_count == 8) {
				bit_count = 0;
				img = pgm_read_byte(picture++);
			}

			lcd_pput(x_int, y_int, 0x01 & (img>>(7-bit_count)));
			bit_count++;
		}
	}
}


void lcd_dec(uint8_t val) {

	lcd_char('0' + (val/100), 0, 0);
	val %= 100;
	lcd_char('0' + (val/10), 0, 0);
	val %= 10;
	lcd_char('0' + val, 0, 0);
}

void lcd_hex(uint8_t val, uint8_t x, uint8_t y) {

	uint8_t tmp = (val >> 4) & 0x0F;

	if (tmp > 9)
		lcd_char((tmp - 10) + 'A', x, y);
	else
		lcd_char(tmp + '0', x, y);

	tmp = val & 0x0F;

	if (tmp > 9)
		lcd_char((tmp - 10) + 'A', x+6, y);
	else
		lcd_char(tmp + '0', x+6, y);

}