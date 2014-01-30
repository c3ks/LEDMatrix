/*
 * panel-controller.c
 *
 *  Created on: 18.06.2010
 *      Author: steene
 */

#include "panel-controller.h"

#include "net/udp/base/udp.h"
#include "graphic-lcd.h"

#include <avr/pgmspace.h>

volatile uint8_t brightness = 16;

void panel_get_binary_packet(char *buffer, unsigned int len) {
	uint16_t j = 0;
	for(uint16_t i = UDP_POS_DATA; i < len; i++) {
		lcd_content[j++] = buffer[i];
	}
}

void parse_command(char* buffer, uint16_t *pos, uint8_t *color) {
	if (!strncmp_P(buffer, PSTR("\\1"), 2)) {
		(*pos) += 2;
		(*color) = 0;
	} else if (!strncmp_P(buffer, PSTR("\\2"), 2)) {
		(*pos) += 2;
		(*color) = 1;
	} else if (!strncmp_P(buffer, PSTR("\\3"), 2)) {
		(*pos) += 2;
		(*color) = 2;
	} else if (!strncmp_P(buffer, PSTR("\\-"), 2)) {
		if ((buffer[2] >= '0') && (buffer[2] <= '9')) {
			brightness = 16+(buffer[2] - '0')*10;
		}
		(*pos) += 3;
	} else {
		(*pos) += 1;
	}

}

void panel_get_text_packet(char *buffer, unsigned int len) {

	buffer[len] = 0;

	uint16_t current_pos = UDP_POS_DATA;
	uint8_t x = 0;
	uint8_t y = 0;
	uint8_t color = 0;

	lcd_clear();

	while(current_pos < len) {
		if (buffer[current_pos] == '\\' ) {
			parse_command(buffer+current_pos, &current_pos, &color);
		} else {
			switch(color) {
			case 0:
				x += lcd_char(buffer[current_pos++], x, y);
				break;
			case 1:
				x += lcd_char(buffer[current_pos++], x+128, y);
				break;
			case 2:
				lcd_char(buffer[current_pos], x, y);
				x += lcd_char(buffer[current_pos++], x+128, y);
				break;
			}
			if (x > 125) {
				x = 0;
				y += 8;

				if (y > 14)
					return;
			}
		}
	}
/*
	if (!strncmp_P(&buffer[UDP_POS_DATA], PSTR("text:"), 5)) {
		if (!strncmp_P(&buffer[UDP_POS_DATA+5], PSTR("green,"), 6)) {
			lcd_clear();
			lcd_string(&buffer[UDP_POS_DATA+11], 0, 0, 120, 16);
		}

		if (!strncmp_P(&buffer[UDP_POS_DATA+5], PSTR("red,"), 4)) {
			lcd_clear();
			lcd_string(&buffer[UDP_POS_DATA+9], 128, 0, 250, 16);

		}

		if (!strncmp_P(&buffer[UDP_POS_DATA+5], PSTR("orange,"), 7)) {
			lcd_clear();
			lcd_string(&buffer[UDP_POS_DATA+12], 0, 0, 120, 16);
			lcd_string(&buffer[UDP_POS_DATA+12], 128, 0, 250, 16);

		}
	}

	if (!strncmp_P(&buffer[UDP_POS_DATA], PSTR("graphic:"), 8)) {

	}

	if (!strncmp_P(&buffer[UDP_POS_DATA], PSTR("command:"), 8)) {
		if (!strncmp_P(&buffer[UDP_POS_DATA+8], PSTR("clear"), 5)) {
			lcd_rect(0, 0, 255, 16, 0);
		}

	}
*/
}
