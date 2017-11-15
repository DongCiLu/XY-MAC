/*
  * Author: lovexy
  * Date: 05/09/2011
  * Version: 2.0
  * A revised version for WCNC 2012.
  */

#ifndef CCCOM_H
#define CCCOM_H

#include "msp430f2132.h"
#include "signal.h"

/* Common */
#define BYTELEN 8
#define FIFOLEN	64
#define True 0x01
#define False 0x00
#define CLOSEWATCHDOG	(WDTCTL = WDTPW + WDTHOLD)

/* MAX possible packet length, 1 byte packetlen, 2 bytes append data */
#define MAXPKTLEN (FIFOLEN - 1 - 2)
#define BUFFLEN	64
#define EMPTY 0x00

/* Timer */
#define MinIntvl 3 /* 100 us */
#define MSecond 33
#define Second 32768

/* SR defs */
#define McuSleep do{ __bis_SR_register(LPM3_bits + GIE); } while(0)
#define McuWakeup do{ __bic_SR_register_on_exit(LPM3_bits); } while(0)

/*
 * Following commands not working, i don't know why
 * #define IntOff do{ __bic_SR_register(GIE); } while(0) 
 * #define IntOn do{ __bis_SR_register(GIE); } while(0)
 */

/* LEDs */
#define LEDR BIT6
#define LEDG BIT7

enum ExpectIntType
{
	ExpInvalid,
	ExpTimer,
	ExpTx,
	ExpRx,
	ExpWor
};

enum ErrCode
{
	WarnBeg = 0x30,
	WarnSetContinue,
	WarnNonPktSensed,

	ErrBeg = 0x70, /* 112 */
	ErrUnexpectRadioState, /* 113 */
	ErrTOTrans, /* 114 */
	ErrTORecv, /* 115 */
	ErrNoBPacket, /* 116, broadcast is not included in current version  */
	ErrTargetNotResp, /* 117 */
	ErrRecvPktLen, /* 118 */
	ErrRecvPktType, /* 119 */
	ErrFalsePositive, /* 120 */
	ErrUnexpectAck, /* 121 */
	ErrUnexpectPkt, /* 122 */
	ErrUnexpectState /* 123 */
};

extern char devAddr;
extern volatile char expInt;

/* 
 * Timer.
 * 30us for a cycle
 * max cycle is 65535, approximate 2s
 * no overflow protection
 */

#define setTimer(cycles) do{ \
	TA0CCTL0 =  CCIE; \
	TA0CCR0 = (cycles); \
	TA0CTL = TASSEL_1 | MC_1; \
}while(0)

#define timerIssuer(cycles) do{ \
	setTimer(cycles); \
	McuSleep; \
}while(0)

#define stopTimer() do{ TA0CTL = MC_0; }while(0)

// MCU initialization
void mcuInit();

// Interrupts
void intEnTrans();
void intDisTrans();
void intEnWor();
void intDisWor();
void resetInt();

#endif 
