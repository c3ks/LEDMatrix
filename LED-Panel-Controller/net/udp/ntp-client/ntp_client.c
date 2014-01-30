/*
,-----------------------------------------------------------------------------------------.
| net/ntp_client
|-----------------------------------------------------------------------------------------
| this file describes a very simple ntp client.
| - it does not check the UDP checksum
| - it does not really do the ntp sequence, it justs sends ONE request
|   and uses the reply timestamp as valid clock (-> network delay -> wrong clock)
|   (i do not really need a very accurate clock...)
|
| Author   : Simon Schulz / avr{AT}auctionant.de
|
|
|
|-----------------------------------------------------------------------------------------
| License:
| This program is free software; you can redistribute it and/or modify it under
| the terms of the GNU General Public License as published by the Free Software
| Foundation; either version 2 of the License, or (at your option) any later
| version.
| This program is distributed in the hope that it will be useful, but
|
| WITHOUT ANY WARRANTY;
|
| without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
| PURPOSE. See the GNU General Public License for more details.
|
| You should have received a copy of the GNU General Public License along with
| this program; if not, write to the Free Software Foundation, Inc., 51
| Franklin St, Fifth Floor, Boston, MA 02110, USA
|
| http://www.gnu.de/gpl-ger.html
`-----------------------------------------------------------------------------------------*/
#include "ntp_client.h"

#include "../../base/arp.h"

#include "../../../timer.h"

#include <string.h>

char ntp_state = 0;



void (*ntp_set_time)(uint64_t time);
uint32_t ntp_server;

int64_t ntp_offset[4];
uint64_t ntp_delay[4];

void ntp_client_init(uint32_t server_ip, void (*function)(uint64_t time)) {
	ntp_set_time = function;
	ntp_server = server_ip;
}


void ntp_client_start_request(char *buffer) {
	ntp_state = 0;
	ntp_client_send_request(buffer);
}

void ntp_client_send_request(char *buffer){
	unsigned int i;

	//set NTP flags (client)
	buffer[NTP_POS_FLAGS] = 0xE3;

	//set peer clock stratum to unspecified
	buffer[NTP_POS_STRATUM] = 0x00;

	//set poll intervall to 4 (=16sec)
	buffer[NTP_POS_POLL_INTERVAL] = 0x04;

	//set precision (??)
	buffer[NTP_POS_PRECISION] = 0xFA;

	//clear data buffer:
	//-> we only need to set some values because many data is 0x00
	for(i=NTP_POS_ROOT_DELAY; i<NTP_POS_LAST_DATA; i++)
		buffer[i] = 0x00;

	//set root delay to 1s -> 0x00 0x01 0x00 0x00
	buffer[NTP_POS_ROOT_DELAY+1] = 0x01; //rest is zero, see above

	//set clock dispersion to 1s -> 0x00 0x01 0x00 0x00
	buffer[NTP_POS_CLOCK_DISP+1] = 0x01; //rest is zero, see above

	//reference clock id = 0x00 0x00 0x00 0x00
	//reference clock update time NULL = 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	//originate timestamp         NULL= 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00
	//receive timestamp           NULL= 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00


	for(uint8_t i = 0; i < 8; i++) {
		buffer[NTP_POS_ORG_TIMESTAMP+i] = ((char*)&timestamp)[7-i];
		buffer[NTP_POS_TX_TIMESTAMP+i] = ((char*)&timestamp)[7-i];
	}
	//tx timestamp:
	//send NULL ... MAYBE add timestamp calc from localclock here
	//tx timestamp           NULL= 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00


	unsigned char mac[6];
	if (arpGetMac(&ntp_server, mac, buffer)) {

		//no need to be called again, we know the mac address of gateway
		udp_generate_packet(buffer, &ntp_server, mac, 40123, IP_PORT_NTP, NTP_DATALENGTH);

		//send packet:
		nic_send_packet(buffer, 14+(IP_HEADER_LENGTH<<2)+8+NTP_DATALENGTH);

	}
}

//decode an ntp packet & extract time data
//this is called after ip+udp decidet this packet is valid & is for us.
//-> ip+udp checks are done before (!)
void ntp_client_decode_packet(char *buffer, unsigned int len){

	//check for valid flags, should be:
	//- NTP Version 4
	//- Server Mode
	//-> ignore Leap indicator

	if ((buffer[NTP_POS_FLAGS]&0x3F) != NTP_FLAG_NTP4_SERVER)
		return;


	//extract timestamp
	//NOTE: because we do not really need the exact clock, we
	//      only use the TX timestamp and hope that the transfer
	//      did not take too long -> maybe fix this later
	//      (and we dont use the microseconds...)


	//Ich machs ordentlich:
	//ORG - REF => ?
	//TX-RF => Process-Delay


	//(TX - REF) - (ORG - RX)
	/*
	 * org - rx => Cl/Sv-Diff + Send
	 *
	 * tx - ref =>
	 */

	uint64_t ts;


	int64_t* offset_tmp = ntp_offset + ntp_state;
	uint64_t* delay_tmp = ntp_delay + ntp_state;

	*delay_tmp = timestamp;


	for(uint8_t i = 0; i < 8; i++) {
		((char*)&ts)[7-i] = buffer[NTP_POS_RX_TIMESTAMP+i];
	}
	*offset_tmp = (int64_t)ts;

	for(uint8_t i = 0; i < 8; i++) {
		((char*)&ts)[7-i] = buffer[NTP_POS_ORG_TIMESTAMP+i];
	}
	*offset_tmp -= (int64_t)ts;
	*delay_tmp -= (uint64_t)ts;

	for(uint8_t i = 0; i < 8; i++) {
		((char*)&ts)[7-i] = buffer[NTP_POS_RX_TIMESTAMP+i];
	}
	*delay_tmp += (uint64_t)ts;

	for(uint8_t i = 0; i < 8; i++) {
		((char*)&ts)[7-i] = buffer[NTP_POS_TX_TIMESTAMP+i];
	}
	*offset_tmp += (int64_t)ts;
	*delay_tmp -= (uint64_t)ts;

	*offset_tmp -= timestamp;

	*offset_tmp >>=1;

	ntp_state++;

	if (ntp_state == 4) {
		uint64_t diff_tmp = ~0; //0xFFFFFFFFFFFFFFFF;
		for(uint8_t i = 0; i < 4; i++) {
			if (ntp_delay[i] < diff_tmp) {
				diff_tmp = ntp_delay[i];
				offset_tmp = &ntp_offset[i];
			}
		}



		//(ntp_set_time)((timestamp + (0xFFFFFFFF * 1000)));
		timer_timestamp_set(timestamp + *offset_tmp);


		uint32_t dest_ip = IP(172,18,101,121);
		unsigned char dest_mac[6];

		if (arpGetMac(&dest_ip, dest_mac, nic_buffer)) {

			udp_generate_packet(nic_buffer, &dest_ip, dest_mac, 55000, 55001, 8);
			memcpy(&nic_buffer[UDP_POS_DATA], offset_tmp, 8);
			nic_send_packet(nic_buffer, 42+8);
		}

	} else {
		ntp_client_send_request(buffer);
	}

	//kurz unsere eigentliche Zeit aktualisieren

}

//lookup table
PROGMEM char NTP_CLIENT_MONTH_TO_DAYS[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

//convert unix timestamp to date/time.
//returns: year,month,day,hour,min,sec (year ist realyear-1970)
//
//inspired by code posted on mikrocontroller.net by peter dannegger:
// see http://www.mikrocontroller.net/forum/read-4-140631.html#new
void ntp_client_timestamp_to_time(unsigned long timestamp, unsigned char *time){
	unsigned char tmp;
	unsigned char tmp_year;
	unsigned int tmp_day;
	unsigned char leap400;

	//Convert from UTC to GMT+1
	timestamp += 60*60;


	//Sommerzeit...
	timestamp += 60*60;

	//calculate seconds:
	time[NTP_TIME_SEC] = timestamp % 60;
	timestamp = timestamp / 60;

	//calculate minutes:
	time[NTP_TIME_MIN] = timestamp % 60;
	timestamp = timestamp / 60;

	//calculate hours:
	time[NTP_TIME_HOUR] = timestamp % 24;
	timestamp = timestamp / 24;

	tmp_year = 0;
  leap400 = 4;

	//find day of year:
	while(1){
		tmp_day = 365;

		//leap year ?! (only check 3yr exception)
		//FIXME: add 400yr exception handling !
		if((tmp_year & 0x03)==0){
			tmp_day = 365 + 1; //yes!

			//100/400 yr exception:
			if(tmp_year == 0 || tmp_year == 100 || tmp_year == 200) //100yr exc
				if (--leap400)
					tmp_day = 365;
		}

		//is the rest smaller than the next whole year ?
		if( timestamp < tmp_day )
			break; //yes -> exit

		//substract daycount of current year from timestamp
		timestamp -= tmp_day;

		//increase year:
		tmp_year++;
	}

	//tmp_year is now (Ynow-1900)
	time[NTP_TIME_YEAR] = tmp_year;

	//timestamp is now how many days of the next year have
	//passed. -> calc month & day
	if ((tmp_day == 365) && timestamp > (30+28))
		timestamp++; //no leap year ! -> skip 29th feb

	//31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
	//calculate month:
	PGM_P mtd_ptr = NTP_CLIENT_MONTH_TO_DAYS;
	for(tmp=1; tmp<12; tmp++){
		timestamp -= pgm_read_byte(mtd_ptr++);
		if (timestamp < pgm_read_byte(mtd_ptr))
			break;
	}

	//days are now in timestamp
	time[NTP_TIME_DAY] = timestamp;

	//current month is in tmp
	time[NTP_TIME_MONTH] = tmp+1;

	//finished
}

uint16_t get_daytime(uint32_t timestamp) {
	unsigned char time[6];
	ntp_client_timestamp_to_time(timestamp, time);
	return time[NTP_TIME_MIN] + time[NTP_TIME_HOUR] * 60;
}
