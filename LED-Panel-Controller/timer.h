#include <avr/io.h>
#include <avr/interrupt.h>



extern volatile uint64_t timestamp;

void timer_init(void);
void timer_doEvents(void);
void timer_timestamp_set(uint64_t new_timestamp);
