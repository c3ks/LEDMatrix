#include "nic.h"
#include "arp.h"
#include "ip.h"
#include "icmp.h"

#include "../tcp/base/tcp.h"

#include "../udp/base/udp.h"

#include <string.h>

#include "../../dev/enc28j60/enc28j60.h"

char nic_mac[6];
uint32_t nic_ip;
uint32_t nic_gateway_ip;
uint8_t nic_netmask_bits;
uint8_t nic_ip_valid;
uint8_t nic_up;

f_sendpacket_t nic_sendfunction;
f_receivepacket_t nic_receivefunction;
f_setmac_t nic_setmacfunction;


/*
 * Kleiner Hack, damit vor dem Paket Platz für ein rfm Header ist
 */
char nic_buffer[NIC_BUFFERSIZE];


void nic_set_mac(char* mac) {
	for (uint8_t i = 0; i < 6; i++) {
		nic_mac[i] = mac[i];
	}

	nic_setmacfunction(mac);
}

void nic_set_ip(void) {

	nic_ip = NIC_IP;
	nic_gateway_ip = NIC_GATEWAY;
	nic_netmask_bits = NIC_NETMASK_BITS;
	nic_ip_valid = 1;
}

void nic_init(f_sendpacket_t sfunction, f_receivepacket_t rfunction,
		f_setmac_t smfunction) {

	nic_sendfunction = sfunction;
	nic_receivefunction = rfunction;
	nic_setmacfunction = smfunction;

	char mac[6] = NIC_MAC;
	nic_set_mac(mac);

#ifdef NIC_CONFIG_STATIC
	nic_set_ip();
	nic_up = 1;
#else
	nic_ip_valid = 0;
	nic_up = 1;
#endif

	arp_init();

}

uint32_t netmask_bits_to_netmask(uint8_t bits) {
	return ~(((uint32_t) 1 << (32 - bits)) - 1);
}

inline void nic_process_local(char *buffer, uint16_t len) {

	uint16_t type = (buffer[12] << 8) + buffer[13];
	unsigned char ip_protocol;


	switch (type) {

		case (ETHERNET_TYPE_ARP):

			arp_packet_in(buffer, len);
			break;

		case (ETHERNET_TYPE_IP):





			ip_protocol = buffer[IP_POS_IPTYPE];
			switch (ip_protocol) {

				case (IP_PROTOCOL_ICMP):
					icmp_packet_in(buffer, len);
					break;

				case (IP_PROTOCOL_UDP):

					udp_packet_in(buffer, len);
					break;

				case (IP_PROTOCOL_TCP):
					tcp_packet_in(buffer, len);
					break;

			}
			break;

		}
}

inline void nic_doEvents(char *receivebuffer) {
	//is there a packet ?
	uint16_t len = enc28j60_receive_packet(receivebuffer, NIC_BUFFERSIZE);
	//led(1,0);
	//led(0,0);
	if (nic_up && (len > 0)) {

		//
		//if ((memcmp(receivebuffer + ETHERNET_POS_DST_MAC, nic_mac, 6) == 0)) {

			//led(0,1);

		/*
			uint32_t src_ip = nic_buffer_to_ip(&receivebuffer[IP_POS_SRC_IP]);
			if ((((src_ip ^ nic_ip) & netmask_bits_to_netmask(nic_netmask_bits)) == 0x00000000) &&
					(src_ip != 0x00000000) &&
					(nic_ip != 0x00000000)) {
				arp_add_mac2ip(&receivebuffer[ETHERNET_POS_SRC_MAC], &src_ip);

			}
*/

			nic_process_local(receivebuffer, len);



	}
}

void nic_send_packet(char *buffer, unsigned int len) {
	nic_sendfunction(buffer, len);
}


uint32_t nic_buffer_to_ip(char *buffer) {
	uint32_t res = ((uint32_t) buffer[0] << 24) | ((uint32_t) buffer[1] << 16)
			| ((uint32_t) buffer[2] << 8) | ((uint32_t) buffer[3] << 0);
	return res;
}

void nic_ip_to_buffer(char *buffer, uint32_t ip) {
	buffer[0] = ip >> 24 & 0xFF;
	buffer[1] = ip >> 16 & 0xFF;
	buffer[2] = ip >> 8 & 0xFF;
	buffer[3] = ip >> 0 & 0xFF;
}

void nic_copy_mac_to_buf(char *buffer) {
	for (uint8_t i = 0; i < 6; i++) {
		buffer[i] = nic_mac[i];
	}
}

