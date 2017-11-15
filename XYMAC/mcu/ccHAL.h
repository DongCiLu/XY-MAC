#ifndef CCHAL_H_
#define CCHAL_H_

#include "cc2500.h"
#include "ccSpi.h"
#include "ccCom.h"

// Mask of Returns.
#define CHIP_RDYn 0x80
#define StateBits 0x70
#define FIFOBits 0x0F

// FIFO Status
#define OoUFlowFlag 0x80

void halResetChip();

void halConfig();

char halWriteReg(char addr, char data);
char halReadReg(char addr);

char halWriteStrobe(char addr);

char halWriteTXBurst(char addr, char *dataBuf, unsigned int cnt);
char halReadRXBurst(char addr, char *retBuf, unsigned int cnt);

unsigned int getFIFOStatus(char type);

#endif 
