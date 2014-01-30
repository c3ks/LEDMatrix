
#include "icmp.h"
#include "nic.h"
#include "ip.h"
#include "arp.h"

//#include "enc28j60.h"

//called if icmp packet comes in
void icmp_packet_in(char *buffer,unsigned int len){
	int i;

	//we only support ICMP echo request (ping) packets:
	if (buffer[ICMP_POS_TYPE] == ICMP_TYPE_ECHO_REQUEST){
		//reuse buffer for the ICMP echo reply:

		//*******************************
		//setup the eth packet:
		for(i=0; i<6; i++)
			buffer[i] = buffer[6+i];

		//packet source mac:
		nic_copy_mac_to_buf(&buffer[6]);

		//change type to ECHO REPLY
		buffer[ICMP_POS_TYPE] = ICMP_TYPE_ECHO_REPLY;

		//"calculate" checksum:
		//use quick & dirty hack, we change only the icmp type
		//so we can calculate the new checksum like this:
		if (buffer[ICMP_POS_CHECKSUM+0] > 0xFF-0x08)
			buffer[ICMP_POS_CHECKSUM+1] = buffer[ICMP_POS_CHECKSUM+1] + 1;

		buffer[ICMP_POS_CHECKSUM+0] = buffer[ICMP_POS_CHECKSUM+0] + 0x08;

		//set up dst ip (copy src to dest ip):
		for(i=0; i<4; i++){
			buffer[IP_POS_DST_IP+i] = buffer[IP_POS_SRC_IP+i];
		}

		//set up src ip:
		nic_ip_to_buffer(&buffer[IP_POS_SRC_IP], nic_ip);

		nic_send_packet(buffer,len);
	}
}
