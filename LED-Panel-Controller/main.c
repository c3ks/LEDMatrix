/*
 * main.c
 *
 *  Created on: 27.05.2010
 *      Author: steene
 */

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "graphic-lcd.h"

#include "dev/enc28j60/enc28j60.h"
#include "net/base/nic.h"
#include "net/udp/dhcp-client/dhcp_client.h"
#include "net/udp/ntp-client/ntp_client.h"

#include "net/udp/base/udp.h"
#include "net/base/arp.h"

#include "dev/soft-spi/spi-master.h"
#include "panel-controller.h"

uint8_t enable = 0;

void spi_init(void) {

	SPSR = (1<<SPI2X);
	SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPIE) | (1<<DORD);
}

void timer_init(void) {
	TCCR0 = (1<<WGM01) | (1<<CS02) | (1<<CS00); // | (1<<FOC0);
	TIMSK = (1<<OCIE0);
	TCNT0 = 0;
	OCR0 = 5;

}

volatile char *ptr;
volatile uint8_t col_nr = 0;
volatile uint8_t row_nr = 0;


ISR(SPI_STC_vect) {

	//Solange die Spaltennummer nicht 0 ist
	//runterzählen und weiter Daten in die SPI schieben
	if (col_nr) {
		SPDR = ~(*ptr++);
		col_nr--;

	//Wenn Spaltennummer gleich 0
	//die aktuelle Zeilennummer aktivieren ()
	} else {

		//Eins dieser Pins aktiviert das Latch an den Seriell->Parallel Wandlern
		//bei dem anderen habe ich grad keine Ahnung
		PORTD = (1<<6);
		PORTD = (1<<6) | (1<<5);

		//PortB wird offensichtlich zur Übertragung der Zeile genutzt
		PORTB = row_nr;
		PORTD = 0;
		row_nr = (row_nr + 1);
	}
}


ISR(TIMER0_COMP_vect) {

	if (row_nr < 0x10) {


		ptr = (lcd_content + (row_nr * 32));

		col_nr = 31;
		SPDR = ~(*(ptr++));


	} else {

		row_nr = (row_nr + 1);

		if (row_nr >= brightness)
			row_nr = 0;
	}
}


char* dhcpname = "ledpanel";





int main(void) {
	DDRB |= 0xFF;
	DDRD |= (1<<5) | (1<<6);

	PORTD = (1<<6);

	uint16_t tmp = 512;

	while(tmp--) {
		lcd_content[tmp] = 0x00;
	}


	spi_init();

	soft_spi_init();

	enc28j60_init();

	nic_init(enc28j60_send_packet, enc28j60_receive_packet, enc28j60_set_mac);

	dhcp_init(dhcpname);

	timer_init();

	sei();


	//char nic_buffer[600];

	while(1) {

		nic_doEvents(nic_buffer);
		dhcp_doEvents(nic_buffer);

	}
}
