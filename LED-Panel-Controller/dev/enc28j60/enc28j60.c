/*
 * enc28j60.c
 *
 *  Created on: 09.11.2008
 *      Author: steene
 */

#include "enc28j60.h"

#include "../soft-spi/spi-master.h"

volatile unsigned char enc28j60_current_bank;
volatile unsigned int  enc28j60_next_packet_ptr;
unsigned char enc28j60_revision;

//store enc28j60 config in program memory
//syntax: <REGx>, <REGVALx>, <REGy>, <REGVALy>,...

#include <avr/pgmspace.h>
unsigned char enc28j60_config[] PROGMEM = {
	//setup bank0 (config stored in progmem, see above)
	//tx buffer:
	ENC28J60_REG_ETXSTL, lo8(ENC28J60_TX_BUFFER_START), //start lo
	ENC28J60_REG_ETXSTH, hi8(ENC28J60_TX_BUFFER_START), //start hi
	ENC28J60_REG_ETXNDL, lo8(ENC28J60_TX_BUFFER_END  ), //end lo
	ENC28J60_REG_ETXNDH, hi8(ENC28J60_TX_BUFFER_END  ), //end hi
	//rx buffer
	ENC28J60_REG_ERXSTL, lo8(ENC28J60_RX_BUFFER_START), //start lo
	ENC28J60_REG_ERXSTH, hi8(ENC28J60_RX_BUFFER_START), //start hi
	ENC28J60_REG_ERXNDL, lo8(ENC28J60_RX_BUFFER_END  ), //end lo
	ENC28J60_REG_ERXNDH, hi8(ENC28J60_RX_BUFFER_END  ), //end hi
	//rx ptr:
	//ENC28J60_REG_ERDPTL, lo8(ENC28J60_RX_BUFFER_START+1),
	//ENC28J60_REG_ERDPTH, hi8(ENC28J60_RX_BUFFER_START+1),

	//setup bank2: (see microchip datasheet p.36)
	//1.) clear the MARST bit in MACON2.
	ENC28J60_REG_MACON2, 	0x00,
	//2.) mac rx enable, activate pause control frame support
	ENC28J60_REG_MACON1, 	((1<<ENC28J60_BIT_MARXEN)|(1<<ENC28J60_BIT_RXPAUS)|(1<<ENC28J60_BIT_TXPAUS)),
	//3.) setup MACON3: auto padding of small packets, add crc, enable frame length check:
	ENC28J60_REG_MACON3, 	((1<<ENC28J60_BIT_PADCFG0)|(1<<ENC28J60_BIT_TXCRCEN)|(1<<ENC28J60_BIT_FRMLNEN)),
	//4.) dont set up MACON4 (use default)
	//5.) setup maximum framelenght to 1518:
	ENC28J60_REG_MAMXFLL, lo8(1518),
	ENC28J60_REG_MAMXFLH, hi8(1518),
	//6.) set up back-to-back gap: 0x15 for full duplex / 0x12 for half duplex
	ENC28J60_REG_MABBIPG, 0x12, //half duplex
	//7.) setup non-back-to-back gap: use 0x12
	ENC28J60_REG_MAIPGL, 	0x12,
	//8.) setup non-back-to-back gap high byte: 0x0C for half duplex:
	ENC28J60_REG_MAIPGH, 	0x0C, //half duplex
	//9.) dont change MACLCON1+2 / MACLCON2 might be changed for networks with long wires !

	//setup bank3:
	//10.) programm mac address: BYTE BACKWARD !
	ENC28J60_REG_MAADR5, 0x00, //NIC_MAC0,
	ENC28J60_REG_MAADR4, 0x00, //NIC_MAC1,
	ENC28J60_REG_MAADR3, 0x00, //NIC_MAC2,
	ENC28J60_REG_MAADR2, 0x00, //NIC_MAC3,
	ENC28J60_REG_MAADR1, 0x00, //NIC_MAC4,
	ENC28J60_REG_MAADR0, 0x00, //NIC_MAC5

	ENC28J60_REG_ERXFCON, 0x80 | 0x20 | 0x01
	};



//read a byte (adds a extra dummy read if (address&0x80)
inline unsigned char enc28j60_spi_read_byte(unsigned char op, unsigned char address){
	unsigned char res;
	//select enc28j60
	ENC28J60_CS_LO();

	spi_transfer_byte(op | (address & 0x1F));
	if (address & 0x80)
		spi_transfer_byte(0x00);

	res = spi_transfer_byte(0x00);

	//deselect enc28j60
	ENC28J60_CS_HI();

	return res;
}

//spi write word
inline void enc28j60_spi_write_word(unsigned char hi, unsigned char lo){
	//unsigned char res = 0;

	//select enc28j60
	ENC28J60_CS_LO();

	spi_transfer_byte(hi);
	spi_transfer_byte(lo);

	//deselect enc28j60
	ENC28J60_CS_HI();
}



//switch to given bank (only if necessary)
inline void enc28j60_select_bank(unsigned char bank){
	//check if this is really a new bank
	if ((bank & 0x60) != enc28j60_current_bank){
		//store bank info
		enc28j60_current_bank = (bank & 0x60);

		//clear bank sel bits:
		enc28j60_spi_write_word((ENC28J60_OP_BFC | ENC28J60_REG_ECON1),((1<<ENC28J60_BIT_ECON1_BSEL0) | (1<<ENC28J60_BIT_ECON1_BSEL1)));
		//setup bank selection bits:
		enc28j60_spi_write_word((ENC28J60_OP_BFS | ENC28J60_REG_ECON1),enc28j60_current_bank>>5);
	}
}


//read address
inline unsigned char enc28j60_read_address(unsigned char address){
	//select bank:
	enc28j60_select_bank(address);

	//and read the data:
	return enc28j60_spi_read_byte(ENC28J60_OP_READ_CTRL_REG, address);
}

//write address
inline void enc28j60_write_address(unsigned char address, unsigned char data){
	//select bank:
	enc28j60_select_bank(address);

	//and write data
	enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (address & 0x1F)), data);
}

//write phy register
inline void enc28j60_write_phy(unsigned char address, unsigned int data){
	//see microchip datasheet p.21
	//set address to MIREGADR:
	enc28j60_write_address(ENC28J60_REG_MIREGADR, address);

	//send the data
	enc28j60_write_address(ENC28J60_REG_MIWRL, data&0xFF);
	enc28j60_write_address(ENC28J60_REG_MIWRH, data>>8  );


	//wait until PHY write finished:
	while(enc28j60_read_address(ENC28J60_REG_MISTAT) & (1<<ENC28J60_BIT_MISTAT_BUSY)){}
}

//read phy register
inline unsigned int enc28j60_read_phy(unsigned char address){
	//see microchip datasheet p.21
	//set address to MIREGADR:
	enc28j60_write_address(ENC28J60_REG_MIREGADR, address);

	//lesen einleiten
	enc28j60_write_address(ENC28J60_REG_MICMD, 0x01);

	//wait until PHY read finished:
	while(enc28j60_read_address(ENC28J60_REG_MISTAT) & (1<<ENC28J60_BIT_MISTAT_BUSY)){}

	//lesebit wieder löschen (wer auch immer sich das ausgedacht hat
	enc28j60_write_address(ENC28J60_REG_MICMD, 0x00);

	unsigned int value;

	//send the data
	value = enc28j60_read_address(ENC28J60_REG_MIRDL);
	value |= enc28j60_read_address(ENC28J60_REG_MIRDH) << 8;

	return value;

}



//initialise spi & enc28j60
void enc28j60_init(void) {
	unsigned char i;

	//set bank to invalid value -> bank set will update in any case:
	enc28j60_current_bank = 0xFF;

	//set up port directions:
	ENC28J60_DDR |= (1<<ENC28J60_PIN_CS)|(1<<ENC28J60_PIN_RESET);

	//deselect enc28j60:
	ENC28J60_CS_HI();

	//reset enc28j60:
	ENC28J60_RESET_LO();
	_delay_ms(10);
	ENC28J60_RESET_HI();

	//after reset we have to wait at least 50us !
	_delay_us(60);
	//workaround for bad chip revisions: wait >1ms (wait 10ms...)
	_delay_ms(10);

	//wait for ready flag
	while(!(enc28j60_read_address(ENC28J60_REG_ESTAT) & 0x01)){};


	//set up rx pointer:
	enc28j60_next_packet_ptr = ENC28J60_RX_BUFFER_START;
	//enc28j60_next_packet_ptr = 0x0602;

	//copy config from progmem to enc28j60:
	for(i=0; i<2*16; i+=2){
		enc28j60_write_address(pgm_read_byte(&enc28j60_config[i+0]),pgm_read_byte(&enc28j60_config[i+1]));
	}



	//setup phy:
	enc28j60_write_phy(ENC28J60_PHY_PHCON2, (1<<ENC28J60_BIT_HDLDIS)); //=no loopback of transmitted frames

	//enable interrups
	enc28j60_write_address(ENC28J60_REG_EIE, (1<<6)|(1<<7));

	//enable rx
	//enc28j60_write_address(ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_RXEN));
	enc28j60_spi_write_word(ENC28J60_OP_BFS|ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_RXEN));

	//set up leds: LEDA: link status, LEDB: RX&TX activity
	enc28j60_write_phy(ENC28J60_PHY_PHLCON, 0x0472);

	//read silicon revision
	enc28j60_revision = enc28j60_read_address(ENC28J60_REG_EREVID);
}

void enc28j60_set_mac(char* mac) {
	enc28j60_write_address(ENC28J60_REG_MAADR5, mac[0]);
	enc28j60_write_address(ENC28J60_REG_MAADR4, mac[1]);
	enc28j60_write_address(ENC28J60_REG_MAADR3, mac[2]);
	enc28j60_write_address(ENC28J60_REG_MAADR2, mac[3]);
	enc28j60_write_address(ENC28J60_REG_MAADR1, mac[4]);
	enc28j60_write_address(ENC28J60_REG_MAADR0, mac[5]);
}


//read enc28j60 buffer
inline void enc28j60_read_buffer(char *buffer, unsigned int len){
	//select enc28j60
	ENC28J60_CS_LO();


	spi_transfer_byte(ENC28J60_OP_READ_BUF_MEM);

	//read data & store in buffer:
	while(len--){
		*buffer++ = spi_transfer_byte(0x00);
	}

	//deselect enc28j60
	ENC28J60_CS_HI();
}

//write enc28j60 buffer
inline void enc28j60_write_buffer(char *buffer, unsigned int len){
	//select enc28j60
	ENC28J60_CS_LO();

	spi_transfer_byte(ENC28J60_OP_WRITE_BUF_MEM);

	while(len--){
		//send data
		spi_transfer_byte(*buffer++);
	}

	//deselect enc28j60
	ENC28J60_CS_HI();
}



void enc28j60_send_packet(char *buffer, unsigned int len){
	unsigned int tmp;
	//still sending ?
	//wait max 50*10 = 500ms:
	for(tmp=0; tmp<500; tmp++){
		if (!(enc28j60_read_address(ENC28J60_REG_ECON1) & (1<<ENC28J60_BIT_TXRTS))){
			//send finished -> tx packet
			break;
		}
		//tx not finished yet, wait 10ms
		_delay_ms(1);
	}

	//send anyway...

	//setup write pointer:
	//enc28j60_write_address(ENC28J60_REG_EWRPTL, (ENC28J60_TX_BUFFER_START&0xFF));
	enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_EWRPTL & 0x1F)), (ENC28J60_TX_BUFFER_START&0xFF));
	//enc28j60_write_address(ENC28J60_REG_EWRPTH, (ENC28J60_TX_BUFFER_START)>>8);
	enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_EWRPTH & 0x1F)), (ENC28J60_TX_BUFFER_START)>>8);


	//set tx end pointer to [start+len]:
	//enc28j60_write_address(ENC28J60_REG_ETXNDL, (ENC28J60_TX_BUFFER_START+len)&0xFF);
	enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_ETXNDL & 0x1F)), (ENC28J60_TX_BUFFER_START+len)&0xFF);
	//enc28j60_write_address(ENC28J60_REG_ETXNDH, (ENC28J60_TX_BUFFER_START+len)>>8);
	enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_ETXNDH & 0x1F)), (ENC28J60_TX_BUFFER_START+len)>>8);

	//start buffer write command
	enc28j60_spi_write_word(ENC28J60_OP_WRITE_BUF_MEM, 0x00);

	//copy buffer to enc28j60:
	enc28j60_write_buffer(buffer, len);

	//bad silicon workaround:
	//reset tx logic:
	enc28j60_spi_write_word(ENC28J60_OP_BFS | ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_TXRST));
	enc28j60_spi_write_word(ENC28J60_OP_BFC | ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_TXRST));

	//activate transmission
	enc28j60_spi_write_word(ENC28J60_OP_BFS | ENC28J60_REG_ECON1, (1<<ENC28J60_BIT_TXRTS)|(1<<ENC28J60_BIT_RXEN));
}

unsigned int enc28j60_receive_packet(char *buffer, unsigned int maxlen){
	unsigned int rxstat;
	unsigned int len;


	//packet in buffer ?
	if ((enc28j60_read_address(ENC28J60_REG_EIR) & (1<<ENC28J60_BIT_PKTIF)) == 0){
		//double check!
		//errata says that PKTIF does not work as it should
		//->check packetcount too:
		if (enc28j60_read_address(ENC28J60_REG_EPKTCNT) == 0)
			return 0;
	}

	//set read pointer to next packet;
	enc28j60_write_address(ENC28J60_REG_ERDPTL, (enc28j60_next_packet_ptr));
	//enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_ERDPTL & 0x1F)), (enc28j60_next_packet_ptr));
	enc28j60_write_address(ENC28J60_REG_ERDPTH, (enc28j60_next_packet_ptr)>>8);
	//enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_ERDPTH & 0x1F)), (enc28j60_next_packet_ptr)>>8);

	//now read the transmit status vector
	//read next packet ptr
	enc28j60_next_packet_ptr  = enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0);
	enc28j60_next_packet_ptr |= enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0)<<8;


	//read packet length
	len  = enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0);
	len |= enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0)<<8;

	//read rx stat
	rxstat  = enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0);
	rxstat |= enc28j60_spi_read_byte(ENC28J60_OP_READ_BUF_MEM, 0)<<8;

	//limit read bytecount
	if (len>maxlen)
		len = maxlen;

	//don't read crc
	len -= 4;

	//tranfer packet from enc28j60 to our buffer
	enc28j60_read_buffer(buffer,len);

	//mark packet as processed (free mem)

	//ERRATA says we need to check packet pointer:

	// (enc28j60_next_packet_ptr - 1 < ENC28J60_RX_BUFFER_START) ||    Überflüssig, weil BUFFER_START = 0
	//===================================================================================================

	if ((enc28j60_next_packet_ptr - 1 > ENC28J60_RX_BUFFER_END)){
		enc28j60_write_address(ENC28J60_REG_ERXRDPTL, lo8(ENC28J60_RX_BUFFER_END));
		enc28j60_write_address(ENC28J60_REG_ERXRDPTH, hi8(ENC28J60_RX_BUFFER_END));
		//enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_ERXRDPTL & 0x1F)), lo8(ENC28J60_RX_BUFFER_END));
		//enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_ERXRDPTH & 0x1F)), hi8(ENC28J60_RX_BUFFER_END));
	}else{
		enc28j60_write_address(ENC28J60_REG_ERXRDPTL, lo8(enc28j60_next_packet_ptr- 1));
		enc28j60_write_address(ENC28J60_REG_ERXRDPTH, hi8(enc28j60_next_packet_ptr- 1));
		//enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_ERXRDPTL & 0x1F)), lo8(enc28j60_next_packet_ptr- 1));
		//enc28j60_spi_write_word((ENC28J60_OP_WRITE_CTRL_REG | (ENC28J60_REG_ERXRDPTH & 0x1F)), hi8(enc28j60_next_packet_ptr- 1));
	}

	//decrement packet counter:
	enc28j60_spi_write_word(ENC28J60_OP_BFS|ENC28J60_REG_ECON2, (1<<ENC28J60_BIT_PKTDEC));

	//return length
	return len;
}




