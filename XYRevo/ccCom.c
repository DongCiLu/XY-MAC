/*
  * Author: lovexy
  * Date: 05/09/2011
  * Version: 2.0
  * A revised version for WCNC 2012.
  */

#include "ccCom.h"

// Device parameter
char devAddr;
volatile char expInt;

void mcuInit()
{
	CLOSEWATCHDOG;	// Close wathdog

	if(CALBC1_8MHZ == 0xFF || CALDCO_8MHZ == 0xFF)
		while(1);

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P3SEL = EMPTY;
	P3DIR |= LEDR | LEDG;
	P3OUT = EMPTY;

	P2DIR = EMPTY;
	P2IES |= BIT0; // falling edge of bit0
	P2IES &= ~BIT1; // rising edge of bit1
	P2IFG = EMPTY;
}

void intEnTrans()
{
	//P2IFG &= ~BIT0;
	P2IE |= BIT0;
}

void intDisTrans()
{
	P2IE &= ~BIT0;
	//P2IFG &= ~BIT0;
}

void intEnWor()
{
	//P2IFG &= ~BIT1;
	P2IE |= BIT1;
}

void intDisWor()
{
	P2IE &= ~BIT1;
	//P2IFG &= ~BIT1;
}

void resetInt()
{
	P2IFG &= ~(BIT0 | BIT1);
}

// Timer Interrupt
#pragma vector=TIMER0_A0_VECTOR
__interrupt void timer_ISR(void)
{
	/* IFG will clear automatically */
	stopTimer();
	McuWakeup;
	expInt = ExpTimer;
}

