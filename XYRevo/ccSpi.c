/*
  * Author: lovexy
  * Date: 05/09/2011
  * Version: 2.0
  * A revised version for WCNC 2012.
  */

#include "ccSpi.h"
#include "cc2500.h"
#include "ccCom.h"

void spiConfig()
{
	P3SEL |= BIT1 | BIT2 | BIT3; 
	P3DIR |= BIT0 | BIT1 | BIT3; 
	
	SpiClose;
	
	UCB0CTL1 = UCSWRST;	// set UCSWRST to config SPI port
	
	UCB0CTL0 = EMPTY;	
	UCB0CTL0 |= UCCKPH | UCSYNC | UCMST | UCMSB;	
	
	UCB0CTL1 = EMPTY;	
	UCB0CTL1 |= UCSSEL1 | UCSSEL0;	
	
	UCB0BR0 = 0x10;	
	UCB0BR1 = 0x00;
	
	UCB0CTL1 &= ~UCSWRST;	//clear UCSWRST to release spi for operation
	
	return;
}

char spiAccessByte(char buf)
{
	UCB0TXBUF = buf;
	while(UCB0STAT & UCBUSY);
	return UCB0RXBUF;
}
