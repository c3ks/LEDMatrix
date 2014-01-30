#include "spi-master.h"

#include <avr/io.h>

#include <util/delay.h>

void soft_spi_init(void) {

	//SPI init
	// initialize I/O
	//sck = hi

	//MOSI = PA3
	//MISO = PA4
	//SCK = PA5
	//CS = PA6
	//RESET = PA7

	PORTA |= (1<<5);

	DDRA |= (1<<3) | (1<<5);
	DDRA &= ~(1<<4);

	PORTA &= ~(1<<3) & ~(1<<4) & ~(1<<5);


}

//read a byte (adds a extra dummy read if (address&0x80)
inline unsigned char spi_transfer_byte(unsigned char data) {
	unsigned char res = 0x00;

	uint8_t i = 8;

	while(i--) {

		_delay_us(0.5);

		if (data & 0x80) {
			PORTA |= (1<<3);
		} else {
			PORTA &= ~(1<<3);
		}

		_delay_us(0.5);

		data <<= 1;

		PORTA |= (1<<5);

		_delay_us(0.5);

		res <<= 1;

		if (PINA & (1<<4)) {
			res |= 0x01;
		}

		_delay_us(0.5);

		PORTA &= ~(1<<5);

		_delay_us(0.5);
	}


	return res;
}

//spi write word
inline void spi_write_word(uint16_t data) {
	//unsigned char res = 0;

	spi_transfer_byte(data >> 8);
	spi_transfer_byte(data);

}
