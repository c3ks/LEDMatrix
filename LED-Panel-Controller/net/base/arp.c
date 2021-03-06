/*
,-----------------------------------------------------------------------------------------.
| net/arp
|-----------------------------------------------------------------------------------------
| this file implements a very basic arp protocol
| - we store the last <ARP_TABLE_SIZE> mac<->ip mapping in an table
| - received arp request & answer with reply
| - send arp request & read replay -> add to table
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

#include "arp.h"
#include "nic.h"

struct arp_entry arp_table[ARP_TABLE_SIZE];


//param initialise arp table:
void arp_init(void){
	for(uint8_t i = 0; i < ARP_TABLE_SIZE; i++){
		arp_table[i].ip  = 0;
	}
}





uint8_t arp_search_by_ip(uint32_t ip){
	uint8_t i;
	for (i = 0; i < ARP_TABLE_SIZE; i++){
		if (arp_table[i].ip == ip) {
			return i;
		}

	}
	//if we get here, ip is unknown

	return 255;
}


void arp_add_mac2ip(char *buffer, uint32_t *ip){


	if (arp_search_by_ip(*ip) != 255)
		return;

	uint8_t c;

	for(uint8_t i = 1; i < ARP_TABLE_SIZE; i++){
		arp_table[i-1].ip  = arp_table[i].ip;

		for (c=0; c<6; c++)
			arp_table[i-1].mac[c] = arp_table[i].mac[c];
	}

	//set ip
	arp_table[ARP_TABLE_SIZE - 1].ip = *ip;

	//set mac:
	for (c = 0; c < 6; c++){
		arp_table[ARP_TABLE_SIZE - 1].mac[c] = buffer[c];
	}
}


unsigned int nic_buffer_to_unsigned_int(char *buffer) {
	unsigned int res = ((unsigned int) buffer[20] << 8) | buffer[21];
	return res;
}

//decode arp packet & send reply
void arp_packet_in(char *buffer, unsigned int len){
	uint32_t src_ip;

	if (nic_buffer_to_ip(&buffer[ARP_POS_DST_IP]) == nic_ip){

		//is this an arp request ?
		if (nic_buffer_to_unsigned_int(buffer) == ARP_OP_REQUEST){
			//is this arp request for us ?

			//get entry pos in arp table.
			src_ip = nic_buffer_to_ip(&buffer[ARP_POS_SRC_IP]);


			arp_add_mac2ip(&buffer[ARP_POS_SRC_MAC], &src_ip);

			//setup the eth packet:

			//packet target mac:
			for (uint8_t c = 0; c < 6; c++){
				buffer[c] = buffer[ARP_POS_SRC_MAC+c];
				buffer[ARP_POS_DST_MAC+c] = buffer[ARP_POS_SRC_MAC+c];
			}
			//packet source mac:

			nic_copy_mac_to_buf(&buffer[6]);
			nic_copy_mac_to_buf(&buffer[ARP_POS_SRC_MAC]);

			//now setup the arp packet:
			//change OP to REPLY
			buffer[ARP_POS_OP+0] = ARP_OP_REPLY>>8 & 0xFF;
			buffer[ARP_POS_OP+1] = ARP_OP_REPLY    & 0xFF;

			//setup source ip:
			nic_ip_to_buffer(&buffer[ARP_POS_SRC_IP], nic_ip);
			//setup target ip:
			nic_ip_to_buffer(&buffer[ARP_POS_DST_IP], src_ip);

			//send packet
			nic_send_packet(buffer, len);


		} else if (nic_buffer_to_unsigned_int(buffer) == ARP_OP_REPLY) {
		//handle ARP REPLY:

		//is this for us ?


			uint32_t ip = nic_buffer_to_ip(&buffer[ARP_POS_SRC_IP]);
			arp_add_mac2ip(&buffer[ARP_POS_SRC_MAC], &ip);

		}
	}
}


//send an arp request
void arp_send_request(char *buffer, uint32_t *dest_ip)
{
	//generate ethernet packet:
	for (uint8_t c = 0; c < 6; c++){
		//set destination mac address:
		buffer[c]   = 0xFF; //BROADCAST!
		//set source mac address:
		buffer[c+6] = nic_mac[c];

		//set target mac to 0xFF:
		buffer[ARP_POS_DST_MAC+c] = 0xFF;
	}

	//set ethernet packettype to ARP
	buffer[12] = 0x08;
	buffer[13] = 0x06;

	//set hardware type to ethernet:
	buffer[ARP_POS_HARDWARE_TYPE+0] = 0x00;
	buffer[ARP_POS_HARDWARE_TYPE+1] = 0x01;

	//set protocol to IP:
	buffer[ARP_POS_PROTOCOL+0] = 0x08;
	buffer[ARP_POS_PROTOCOL+1] = 0x00;

	//set hw size:
	buffer[ARP_POS_HARDWARE_SIZE] = 0x06;

	//set protocol size
	buffer[ARP_POS_PROTOCOL_SIZE] = 0x04;

	//set ARP type to REQUEST
	buffer[ARP_POS_OP+0] = ARP_OP_REQUEST>>8;
	buffer[ARP_POS_OP+1] = ARP_OP_REQUEST;

	//setup sender mac & ip:
	nic_copy_mac_to_buf(&buffer[ARP_POS_SRC_MAC]);
	nic_ip_to_buffer(&buffer[ARP_POS_SRC_IP], nic_ip);

	//set target ip:
	nic_ip_to_buffer(&buffer[ARP_POS_DST_IP], *dest_ip);

	//send arp request:
	nic_send_packet(buffer, 14+28);
}

uint8_t arpGetMac(uint32_t* ip, unsigned char* mac, char* buffer) {

	unsigned long *server_ip;

	//target ip in the same network ?!
	if ((*ip & netmask_bits_to_netmask(nic_netmask_bits)) != (nic_ip & netmask_bits_to_netmask(nic_netmask_bits))){
		server_ip = &nic_gateway_ip;
	}else{
		server_ip = ip;
	}

	uint8_t arp_idx = arp_search_by_ip(*server_ip);

	if (arp_idx == 255){
		//target ip (gateway) unknown
		//->send arp request and exit,
		//this is called later again and we hopefully know the mac then

		if (buffer != 0x00)
			arp_send_request(buffer, server_ip);

		return 0;
	}



	mac[0] = arp_table[arp_idx].mac[0];
	mac[1] = arp_table[arp_idx].mac[1];
	mac[2] = arp_table[arp_idx].mac[2];
	mac[3] = arp_table[arp_idx].mac[3];
	mac[4] = arp_table[arp_idx].mac[4];
	mac[5] = arp_table[arp_idx].mac[5];

	return 1;
}


