#include <stdint.h>
#include "../../base/ip.h"
#include "../../base/nic.h"

#include "../../../config.h"

#define UDP_POS_DEST_PORT (IP_POS_DATA + 2)
#define UDP_POS_SRC_PORT (IP_POS_DATA)
#define UDP_POS_DATA (IP_POS_DATA+8)

#ifdef _CONFIG_RECEIVER
void udp_packet_in(char* buffer, unsigned int len);
#endif

#ifdef _CONFIG_SENDER
void udp_generate_packet(char *buffer,
                         uint32_t *dest_ip,
                         unsigned char *dest_mac, unsigned int source_port,
                         unsigned int dest_port, unsigned int data_length);
#endif

uint16_t udp_get_src_port(char* buffer);
uint16_t udp_get_dest_port(char* buffer);

