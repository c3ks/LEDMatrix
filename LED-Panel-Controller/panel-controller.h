/*
 * panel-controller.h
 *
 *  Created on: 18.06.2010
 *      Author: steene
 */

#ifndef PANELCONTROLLER_H_
#define PANELCONTROLLER_H_

#include <stdint.h>

void panel_get_text_packet(char *buffer, unsigned int len);
void panel_get_binary_packet(char *buffer, unsigned int len);
volatile uint8_t brightness;

#endif /* PANELCONTROLLER_H_ */
