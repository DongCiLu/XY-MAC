#include "ccCom.h"

void mcuInit()
{
	CLOSEWATCHDOG;	// Close wathdog
	IntOff;

	if(CALBC1_8MHZ == 0xFF || CALDCO_8MHZ == 0xFF)
		while(1);

	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;

	P3SEL = EMPTY;
	P3DIR |= LED1 | LED2;
	P3OUT = EMPTY;

	P2DIR = EMPTY;
	P2IES |= BIT0; // falling edge of bit0
	P2IES &= ~BIT1; // rising edge of bit1
	P2IFG = EMPTY;
}

void timerIssuer(unsigned int cycles)
{
	TACCTL0 |=  CCIE; // CCIFG Enable
	TACCR0 = cycles; // Set Goal
	TACTL = TASSEL_1 | MC_1; // ACLK, UP Mode

	McuSleep;
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
interrupt (TIMER0_A0_VECTOR) wakeup TimerISR()
{
	/* IFG will clear automatically */
	TA0CTL = MC_0;
	expInt = ExpTimer;
}

