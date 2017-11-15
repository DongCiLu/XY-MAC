#ifndef CCSPI_H_
#define CCSPI_H_

//#include "msp430f2132.h"
#include "msp430x21x2.h"

/*
 * p3.0 connects to CSn
 * p3.1 connects SI
 * p3.2 connects to SO
 * p3.3 connects to SCLK
 */
#define CSn	0x01
#define MOSI 	0x02
#define MISO 	0x04
#define SCLK 	0x08

#define SpiOpen		(P3OUT &= ~CSn)
#define SpiReady	while(P3IN & MISO)
#define SpiClose	(P3OUT |= CSn)

void spiConfig();
char spiAccessByte(char addr);

#endif 
