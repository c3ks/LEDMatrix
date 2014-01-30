

#include "tcp.h"

void tcp_packet_in(char* buffer, unsigned int len) {

	uint16_t port = (buffer[TCP_POS_DEST_PORT]<<8) + buffer[TCP_POS_DEST_PORT+1];
	//uint32_t dest_ip = nic_buffer_to_ip(&buffer[IP_POS_DST_IP]);



	switch (port){

		case(68):
		//	dhcp_get_packet(buffer,len);
		break;

		//NTP Reply
		case(40123):
			//if (nic_ip == dest_ip)
			//	ntp_client_decode_packet(buffer, len);
		break;

		//Display Command
		case(5000):
			//if (nic_ip == dest_ip)
			//get_udp(nic_buffer, len);
		break;
	}
}
