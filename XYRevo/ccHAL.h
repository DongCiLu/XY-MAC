/*
  * Author: lovexy
  * Date: 05/09/2011
  * Version: 2.0
  * A revised version for WCNC 2012.
  */

#ifndef CCHAL_H_
#define CCHAL_H_

#include "cc2500.h"
#include "ccSpi.h"
#include "ccCom.h"

/* 
  * Mask of Returns.
  * Neglected in current version.
  */
#define CHIP_RDYn 0x80
#define StateBits 0x70
#define FIFOBits 0x0F

// FIFO Status
#define OoUFlowFlag 0x80

void halResetChip();
void halConfig();

unsigned int getFIFOStatus(char addr);

/* DO NOT USE THE RETURN VALUES OF HAL LAYER IN CURRENT VERSION */
char halWriteReg(char addr, char data);
char halReadReg(char addr);

char halWriteStrobe(char addr);

char halWriteTXBurst(char addr, char *dataBuf, unsigned int cnt);
char halReadRXBurst(char addr, char *retBuf, unsigned int cnt);

#endif 
