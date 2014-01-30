/*
 * graphic-lcd.h
 *
 *  Created on: 06.12.2009
 *      Author: steene
 */

#ifndef GRAPHICLCD_H_
#define GRAPHICLCD_H_


#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>


#define lcd_left 0x40
#define lcd_right 0x20
#define lcd_command 0x00
#define lcd_data 0x80

volatile char lcd_content[512];


void lcd_init(void);
void lcd_clear(void);
void lcd_update(void);
void lcd_switch(uint8_t on);

void lcd_hline(uint8_t x1, uint8_t x2, uint8_t y);
void lcd_vline(uint8_t y1, uint8_t y2, uint8_t x);

void lcd_frame(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void lcd_rect(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t color);
void lcd_pclear(uint8_t x, uint8_t y);
void lcd_pset(uint8_t x, uint8_t y);
void lcd_pput(uint8_t x, uint8_t y, uint8_t color);

uint8_t lcd_char(uint16_t val, uint8_t x, uint8_t y);
void lcd_string(char *str, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);
void lcd_string_P(PGM_P str, uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2);

void lcd_string_scroll_update(char *str);

void lcd_hex(uint8_t val, uint8_t x, uint8_t y);

void lcd_picture(PGM_P picture, uint8_t x, uint8_t y);

void lcd_write(uint8_t val, uint8_t rscs);

uint8_t lcd_color(uint8_t x, uint8_t y);

#endif /* GRAPHICLCD_H_ */
