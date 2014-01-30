#include "timer.h"

#include "net/udp/ntp-client/ntp_client.h"
#include "net/base/nic.h"



//01.01.2010 - 00:00 Uhr
volatile uint64_t timestamp = 0xBBF81DFF4407E200;
volatile uint8_t drift = 0xc0;

void ttimer_init(void)
{
	TCCR1A = 0;
	TCCR1B = (1<<WGM12) | (1<<CS10);
	OCR1A = 1000; //302

	TIMSK = (1<<OCIE1A);
}

uint32_t last_ntp_refresh = 0;
uint8_t ntp_wait = 0;

void timer_timestamp_set(uint64_t new_timestamp) {

	int64_t offset = new_timestamp - timestamp;

	cli();
	timestamp = new_timestamp;
	sei();

	if (offset > 2147483) {
		drift += 10;
		//led(2,1);
		//led(3,0);
	} else if (offset < -2147483) {
		drift -= 10;
		//led(2,0);
		//led(3,1);
	} else {
		//led(2,0);
		//led(3,0);
	}

	last_ntp_refresh = timestamp>>32;
}



void timer_doEvents(void) {
	//Alle 5 Sekunden testen

	uint32_t secstamp = timestamp>>32;

	if (((secstamp - last_ntp_refresh) > (30)) || (secstamp < 65536))
	{
		uint8_t low_ts = secstamp;
		uint8_t timediff = (ntp_wait <= low_ts) ? (low_ts - ntp_wait) : (((uint16_t)low_ts + 255) - (uint16_t)ntp_wait);

		if (timediff > 15) {
			ntp_wait = low_ts;
			ntp_client_start_request(nic_buffer);
		}

		if ((secstamp - last_ntp_refresh) > (3*60)) {
			last_ntp_refresh = secstamp - (1*60);
			//syslog_P(LOG_SEVERITY_INFORMATIONAL, LOG_FACILITY_LOCAL1, PSTR("ntpclient"), PSTR("response timeout"));
		}
	}
}

//I want 15,258789063us

ISR(TIMER1_COMPA_vect)
{

	asm volatile(




			"lds r30, timestamp + 0\n"
			"lds r31, drift\n"
			"add r30, r31\n"
			"sts timestamp + 0, r30\n"

			"lds r30, timestamp + 1\n"
			"ldi r31, 0x47\n"
			"adc r30, r31\n"
			"sts timestamp + 1, r30\n"

			"lds r30, timestamp + 2\n"
			"ldi r31, 0x03\n"
			"adc r30, r31\n"
			"sts timestamp + 2, r30\n"

			"brcc 0f\n"

			"lds r30, timestamp + 3\n"
			"lds r31, timestamp + 4\n"
			"adiw r30, 0x01\n"
			"sts timestamp + 3, r30\n"
			"sts timestamp + 4, r31\n"
			"brcc 0f\n"

			"lds r30, timestamp + 5\n"
			"lds r31, timestamp + 6\n"
			"adiw r30, 0x01\n"
			"sts timestamp + 5, r30\n"
			"sts timestamp + 6, r31\n"
			"brcc 0f\n"

			"lds r30, timestamp + 7\n"
			"ldi r31, 0x00\n"
			"adiw r30, 0x01\n"
			"sts timestamp + 7, r30\n"
			"0:\n"

			:
			:
			: "r30", "r31"

	);

	//timestamp += 65536;
}



