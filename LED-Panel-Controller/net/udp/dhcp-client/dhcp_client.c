#include "dhcp_client.h"
#include <avr/pgmspace.h>
#include <string.h>

#include "../../base/nic.h"
#include "../base/udp.h"

PROGMEM char DHCP_Basic_Content[] = { 0x01,0x01,0x06,0x00 };
PROGMEM char DHCP_Transaction_ID[] = {0x17,0x04,0x19,0x89 };
PROGMEM char DHCP_Magic_Cookie[] = { 0x63,0x82,0x53,0x63 };
PROGMEM char DHCP_Option_Discover[] = { 0x35, 0x01, 0x01 };
PROGMEM char DHCP_Option_Request[] = { 0x35, 0x01, 0x03 };

char *dhcp_name;

uint16_t dhcp_add_option(char *buffer, uint16_t current_pos, PGM_P option) {
	uint16_t length = pgm_read_byte(option+1) + 2;
	memcpy_P(buffer + UDP_POS_DATA + current_pos, option, length);
	return current_pos + length;
}

uint16_t dhcp_add_name_option(char *buffer, uint16_t current_pos, char *name) {
	uint16_t length = strlen(name);
	buffer[UDP_POS_DATA + current_pos] = 0x0c;
	buffer[UDP_POS_DATA + current_pos + 1] = length;
	memcpy(buffer + UDP_POS_DATA + current_pos + 2, name, buffer[UDP_POS_DATA + current_pos + 1]);
	return current_pos + length + 2;
}

uint16_t dhcp_add_iprequest_option(char *buffer, uint16_t current_pos, uint32_t ip) {
	buffer[UDP_POS_DATA + current_pos] = 0x32;
	buffer[UDP_POS_DATA + current_pos + 1] = 0x04;
	nic_ip_to_buffer(&buffer[UDP_POS_DATA + current_pos + 2], ip);
	return current_pos + 6;
}

uint16_t dhcp_add_serveridentifier_option(char *buffer, uint16_t current_pos, uint32_t ip) {
	buffer[UDP_POS_DATA + current_pos] = 0x36;
	buffer[UDP_POS_DATA + current_pos + 1] = 0x04;
	nic_ip_to_buffer(&buffer[UDP_POS_DATA + current_pos + 2], ip);
	return current_pos + 6;
}

void dhcp_init(char* name) {
	dhcp_name = name;
}

uint16_t lease_renew_time = 0;

uint32_t schneller_timer;
void dhcp_doEvents(char *buffer) {

	if (schneller_timer++ == 1000) {
		lease_renew_time--;
		schneller_timer = 0;
	}

	if (!lease_renew_time) {
		dhcp_send_discover(buffer);
		lease_renew_time = 15;
	}

}

void dhcp_generate_base_packet(char *buffer, uint16_t length)
{
	unsigned char c;

	uint32_t dest_ip = 0xFFFFFFFF;
	unsigned char dest_mac[] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };
	uint32_t tmp_nic_ip;

	tmp_nic_ip = nic_ip;
	nic_ip = 0x00000000;
	udp_generate_packet(buffer, &dest_ip, dest_mac, 68, 67, length);
	nic_ip = tmp_nic_ip;

	//Initialise as DHCP + ID
	memcpy_P(&buffer[UDP_POS_DATA+0], DHCP_Basic_Content, 4);
	memcpy_P(&buffer[UDP_POS_DATA+4], DHCP_Transaction_ID, 4);

	//Some Zeros
	for(c = 8; c < 235; c++)
		buffer[UDP_POS_DATA+c] = 0x00;

	for (c=0; c<6; c++)
		//set source mac address:
		buffer[UDP_POS_DATA+c+28] = nic_mac[c];



	//Magic Cookie - alles klar?!
	memcpy_P(&buffer[UDP_POS_DATA+236], DHCP_Magic_Cookie, 4);
}

void dhcp_send_discover_packet(char *buffer) {

	uint8_t length = 241;

	//Discover Option
	length += 3;

	if (dhcp_name != 0x00) {
		length += 2 + strlen(dhcp_name);
	}


	dhcp_generate_base_packet(buffer, length);
	uint16_t current_pos = 240;

	current_pos = dhcp_add_option(buffer, current_pos, DHCP_Option_Discover);

	if (dhcp_name != 0x00)
		current_pos = dhcp_add_name_option(buffer, current_pos, dhcp_name);

	//end option
	buffer[UDP_POS_DATA+current_pos] = 0xff;
}

uint16_t dhcp_generate_discover_packet(char *buffer) {

	uint16_t length = 241;

	//Discover Option
	length += 3;

	if (dhcp_name != 0x00) {
		length += 2 + strlen(dhcp_name);
	}


	dhcp_generate_base_packet(buffer, length);
	uint16_t current_pos = 240;

	current_pos = dhcp_add_option(buffer, current_pos, DHCP_Option_Discover);

	if (dhcp_name != 0x00)
		current_pos = dhcp_add_name_option(buffer, current_pos, dhcp_name);

	//end option
	buffer[UDP_POS_DATA+current_pos] = 0xff;

	return current_pos + 1;
}

uint16_t dhcp_generate_request_packet(char *buffer, uint32_t request_ip, uint32_t server_ip) {

	uint16_t length = 241;

	//Request
	length += 3;

	//request ip
	length += 6;

	//server identifier
	length += 6;

	if (dhcp_name != 0x00) {
		length += 2 + strlen(dhcp_name);
	}


	dhcp_generate_base_packet(buffer, length);
	uint16_t current_pos = 240;

	current_pos = dhcp_add_option(buffer, current_pos, DHCP_Option_Request);

	current_pos = dhcp_add_iprequest_option(buffer, current_pos, request_ip);

	current_pos = dhcp_add_serveridentifier_option(buffer, current_pos, server_ip);

	if (dhcp_name != 0x00)
		current_pos = dhcp_add_name_option(buffer, current_pos, dhcp_name);

	//end option
	buffer[UDP_POS_DATA+current_pos] = 0xff;

	return current_pos + 1;
}


void dhcp_get_packet(char *buffer, unsigned int len)
{
	//No Reply
	if (buffer[UDP_POS_DATA] != 0x02)
		return;

	//not our ID
	if (memcmp_P(buffer + UDP_POS_DATA + 4, DHCP_Transaction_ID, 4))
		return;

	//DHCP Offer
	if ((buffer[UDP_POS_DATA+240] == 0x35)
			&& (buffer[UDP_POS_DATA+241] == 0x01)
			&& (buffer[UDP_POS_DATA+242] == 0x02))
	{

		uint32_t server_ip;
		uint32_t offered_ip;

		//Rescue offered data
		server_ip = nic_buffer_to_ip(&buffer[IP_POS_SRC_IP]);
		offered_ip = nic_buffer_to_ip(&buffer[UDP_POS_DATA+16]);

		uint16_t length = dhcp_generate_request_packet(buffer, offered_ip, server_ip);
		nic_send_packet(buffer, 42+length);
	}

	//DHCP Ack
	else if ((buffer[UDP_POS_DATA+240] == 0x35)
			&& (buffer[UDP_POS_DATA+241] == 0x01)
			&& (buffer[UDP_POS_DATA+242] == 0x05))
	{

		uint32_t offered_ip = 0;
		uint32_t subnet_mask = 0;
		uint32_t router_ip = 0;
		uint32_t ntp_ip = 0;

		offered_ip = nic_buffer_to_ip(&buffer[UDP_POS_DATA+16]);

		lease_renew_time = 60*60;

		uint8_t c = 0;

		while(buffer[UDP_POS_DATA+240+c] != 0xff)
		{
			if (buffer[UDP_POS_DATA+240+c] == 0x03)
					router_ip = nic_buffer_to_ip(&buffer[UDP_POS_DATA+242+c]);

			if (buffer[UDP_POS_DATA+240+c] == 0x01)
					subnet_mask = nic_buffer_to_ip(&buffer[UDP_POS_DATA+242+c]);

			if (buffer[UDP_POS_DATA+240+c] == 0x2a)
					ntp_ip = nic_buffer_to_ip(&buffer[UDP_POS_DATA+242+c]);

			if ((buffer[UDP_POS_DATA+240+c] == 0x33)
					&& (buffer[UDP_POS_DATA+240+c] == 0x3a)
					&& (buffer[UDP_POS_DATA+240+c] == 0x3b)) {

				uint32_t lease_t;
				switch (buffer[UDP_POS_DATA+240+c+1]) {
				case 1:
					lease_t = buffer[UDP_POS_DATA+240+c+2];
					break;

				case 2:
					lease_t = ((uint32_t)buffer[UDP_POS_DATA+240+c+2]<<8)
							+ (uint32_t)buffer[UDP_POS_DATA+240+c+3];
					break;

				case 3:
					lease_t = ((uint32_t)buffer[UDP_POS_DATA+240+c+2]<<16)
							+ ((uint32_t)buffer[UDP_POS_DATA+240+c+3]<<8)
							+ (uint32_t)buffer[UDP_POS_DATA+240+c+4];
					break;

				case 4:
					lease_t = ((uint32_t)buffer[UDP_POS_DATA+240+c+2]<<24)
							+ ((uint32_t)buffer[UDP_POS_DATA+240+c+3]<<16)
							+ ((uint32_t)buffer[UDP_POS_DATA+240+c+4]<<8)
							+ (uint32_t)buffer[UDP_POS_DATA+240+c+5];
					break;

				default:
					lease_t = 60*60;
					break;
				}

				if (lease_t < lease_renew_time)
					lease_renew_time = lease_t;

			}

			c += buffer[UDP_POS_DATA+241+c] + 2;
		}

		nic_ip = offered_ip;
		nic_gateway_ip = router_ip;
		uint8_t i;
		for(i = 31; (i > 0 ) && (subnet_mask & (1<<i)); i--) {

		}
		nic_netmask_bits = 32 - i;

		nic_ip_valid = 1;

	}

}



void dhcp_send_discover(char *buffer)
{

	uint16_t length = dhcp_generate_discover_packet(buffer);
	nic_send_packet(buffer, 42+length);
}




