#ifndef _NIC_H_
#define _NIC_H_
#include <stdint.h>
#include "config.h"

extern uint32_t nic_ip;
extern char  nic_mac[6];
extern uint32_t nic_gateway_ip;
extern uint8_t nic_netmask_bits;
extern uint32_t ntp_server_ip;
extern uint8_t nic_ip_valid;
extern char nic_buffer[NIC_BUFFERSIZE];

typedef void (*f_sendpacket_t)(char*, unsigned int);
typedef unsigned int (*f_receivepacket_t)(char* , unsigned int);
typedef void (*f_setmac_t)(char*);

void nic_init(f_sendpacket_t, f_receivepacket_t, f_setmac_t);
void nic_save(void);
void nic_load(void);
void nic_doEvents(char *buffer);
uint32_t netmask_bits_to_netmask(uint8_t);


void nic_copy_mac_to_buf(char * buffer);
unsigned int nic_buffer_to_unsigned_int(char *buffer);
uint32_t nic_buffer_to_ip(char *buffer);
void nic_ip_to_buffer(char *buffer, uint32_t ip);
void nic_send_packet(char *buffer, unsigned int len);

#define nic_seq_to_buffer(b,s) nic_ip_to_buffer(b,s)
#define nic_buffer_to_seq(b) nic_buffer_to_ip(b)

#endif

