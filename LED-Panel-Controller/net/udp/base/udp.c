
#include "udp.h"


#ifdef _CONFIG_USE_NTP_CLIENT
	#include "../ntp-client/ntp_client.h"
#endif

#ifdef _CONFIG_USE_DHCP_CLIENT
	#include "../dhcp-client/dhcp_client.h"
#endif

#include "../../../panel-controller.h"

uint16_t udp_get_src_port(char* buffer) {
	return (buffer[UDP_POS_SRC_PORT] << 8) + buffer[UDP_POS_SRC_PORT+1];
}

uint16_t udp_get_dest_port(char* buffer) {
	return (buffer[UDP_POS_DEST_PORT] << 8) + buffer[UDP_POS_DEST_PORT+1];
}

void udp_packet_in(char* buffer, unsigned int len) {

	uint16_t port = (buffer[UDP_POS_DEST_PORT]<<8) + buffer[UDP_POS_DEST_PORT+1];
	uint32_t dest_ip = nic_buffer_to_ip(&buffer[IP_POS_DST_IP]);

	switch (port){

//#ifdef _CONFIG_USE_DHCP_CLIENT
		case(68):

			dhcp_get_packet(buffer,len);
		break;
//#endif

		case(1020):
			if (nic_ip == dest_ip)
				panel_get_text_packet(buffer, len);
			break;

		case(1021):
			if (nic_ip == dest_ip)
				panel_get_binary_packet(buffer, len);
			break;

#ifdef _CONFIG_USE_NTP_CLIENT
		//NTP Reply
		case(40123):
			if (nic_ip == dest_ip)
				ntp_client_decode_packet(buffer, len);
		break;
#endif



		//Nordlicht Connector
		//case(17086):
		//	if (nic_ip == dest_ip)
		//		nordlicht_connector_get_packet(buffer, len);
		//break;
	}
}










//generate an udp packet.
//
//usage:
// - put udp data at position UDP_POS_DATA
// - call udp_generate_packet with all neccessary fields and
//   datalength=how many databytes to tx
// - call nic_send_packet(buffer) to TX

void udp_generate_packet(char *buffer,
                         uint32_t *dest_ip,
                         unsigned char *dest_mac, unsigned int source_port,
                         unsigned int dest_port, unsigned int data_length){

	//datalength = udpheader + datalen
	data_length = data_length + 8;

	//generate an ip packet (includes ethernet header)
	ip_generate_packet(buffer, dest_ip, dest_mac, IP_PROTOCOL_UDP, data_length);

	//setup soure/dest port:
	buffer[IP_POS_DATA + 0] = (source_port>>8);
	buffer[IP_POS_DATA + 1] = (source_port   );
	buffer[IP_POS_DATA + 2] = (  dest_port>>8);
	buffer[IP_POS_DATA + 3] = (  dest_port   );

	//setup data length of udp
	buffer[IP_POS_DATA + 4] = (data_length>>8);
	buffer[IP_POS_DATA + 5] = (data_length   );

	//store checksum: (0x0000 -> checksum disable)
	buffer[IP_POS_DATA + 6] = 0x00;
	buffer[IP_POS_DATA + 7] = 0x00;

	//calculate checksum now: (for ip header + udp header + data !!!
	//add pseudo header (sourceip+targetip + 0x00 + PROTOCOL + LEN) after udp data:

}

