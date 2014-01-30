
#include "ip.h"
#include "nic.h"



volatile unsigned int ip_identification;

unsigned int nethelp_checksum(char *buffer, unsigned int len, unsigned long csum32){
	unsigned int  res16 = 0x0000;
	unsigned char data_hi;
	unsigned char data_lo;

	//loop through buffer
	while(len > 1){
		//read data_hi/lo from pointer:
		data_hi = *buffer++;
		data_lo = *buffer++;

		//generate 16bit var from both:
		res16 = ((data_hi << 8) + data_lo);

		//add to csum
		csum32 = csum32 + res16;

		//decrement length (we have read 2 bytes from ptr)
		len -=2;
	}

	//is len odd -> add again last hi value & lo = 0
	if(len > 0){
		//read data_hi/lo from pointer & generate 16bit var:
		data_hi = *buffer;
		res16   = data_hi<<8;

		//add to csum
		csum32 = csum32 + res16;
	}

	//to complement calc:
	csum32 = ((csum32 & 0x0000FFFF)+ ((csum32 & 0xFFFF0000) >> 16));
	csum32 = ((csum32 & 0x0000FFFF)+ ((csum32 & 0xFFFF0000) >> 16));
	res16  =~(csum32 & 0x0000FFFF);

	return (res16);
}

void ip_generate_packet(char *buffer,
                        uint32_t *dest_ip,
                        unsigned char *dest_mac,
                        unsigned char ip_packettype,
                        unsigned int data_length){
	unsigned int res16;
	unsigned int c;

	//datalength = datalength + ip header:
	data_length = data_length + (IP_HEADER_LENGTH<<2);

	//generate ethernet packet:
	for (c=0; c<6; c++){
		//set destination mac address:
		buffer[c]   = dest_mac[c];
		//set source mac address:
		buffer[c+6] = nic_mac[c];
	}

	//set ethernet packettype to IP
	buffer[12] = 0x08;
	buffer[13] = 0x00;

	//now generate ip packet:

	//set ipv4 & set length:
	buffer[IP_POS_VERSION] = IP_V4 | IP_HEADER_LENGTH;

	//set TOS to default 0x00 (?)
	buffer[IP_POS_TOS] = 0x00;

	//set total length:
	buffer[IP_POS_PKTLEN+0] = (data_length>>8)&0xFF;
	buffer[IP_POS_PKTLEN+1] = (data_length   )&0xFF;

	//set packet identification
	buffer[IP_POS_IDENTIFICATIION+0] = (ip_identification>>8)&0xFF;
	buffer[IP_POS_IDENTIFICATIION+1] = (ip_identification   )&0xFF;
	ip_identification++;

	//set dont fragment flags:
	buffer[IP_POS_FRAGMENTATION_FLAGS+0] = 0x40;
	buffer[IP_POS_FRAGMENTATION_FLAGS+1] = 0x00;

	//set TTL to 128
	buffer[IP_POS_TTL] = 0x80;

	//set ip packettype to tcp/udp/...
	buffer[IP_POS_IPTYPE] = ip_packettype;

	//set dest ip:
	nic_ip_to_buffer(&buffer[IP_POS_DST_IP], *dest_ip);

	//set source ip:
	nic_ip_to_buffer(&buffer[IP_POS_SRC_IP], nic_ip);

	//set checksum to zero before running crc calc:
	buffer[IP_POS_CHECKSUM+0] = 0x00;
	buffer[IP_POS_CHECKSUM+1] = 0x00;

	//calculate checksum:
	res16 = nethelp_checksum(&buffer[IP_POS_VERSION], IP_HEADER_LENGTH<<2, 0);

	//store checksum:
	buffer[IP_POS_CHECKSUM+0] = (res16>>8)&0xFF;
	buffer[IP_POS_CHECKSUM+1] = res16 & 0xFF;

	//finished

}

