
#include <stdint.h>

void soft_spi_init(void);
unsigned char spi_transfer_byte(unsigned char data);
void spi_write_word(uint16_t data);
