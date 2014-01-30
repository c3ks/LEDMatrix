#include <stdint.h>
#include "../../base/ip.h"
#include "../../base/nic.h"

#define TCP_POS_DEST_PORT (IP_POS_DATA + 2)
#define TCP_POS_DATA (IP_POS_DATA+8)


void tcp_generate_packet(char *buffer,
                         uint32_t *dest_ip,
                         unsigned char *dest_mac, unsigned int source_port,
                         unsigned int dest_port, unsigned int data_length);


void tcp_packet_in(char* buffer, unsigned int len);
