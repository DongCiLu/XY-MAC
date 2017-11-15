#ifndef COMMON_H_
#define COMMON_H_

#include "msp430f2132.h"
//#include "msp430x21x2.h"
//#include "signal.h"
#include "legacymsp430.h"

// Common
#define BYTELEN 8
#define FIFOLEN	128
#define EMPTY 0x00

// Timer
#define MSecond 33
#define Second 32768

// SR defs
#define McuSleep do{ __bis_SR_register(LPM3_bits + GIE); } while(0)
#define IntOff do{ __bic_SR_register(GIE); } while(0) 
#define IntOn do{ __bis_SR_register(GIE); } while(0)

// LEDs
#define LED1 BIT6
#define LED2 BIT7

#define True 0x01
#define False 0x00

#define CLOSEWATCHDOG	(WDTCTL = WDTPW + WDTHOLD)

// Communications
#define BroadcastAddr 0x00

enum DeviceType
{
	DevInvalid,
	DevTest,
	DevTrans,
	DevRecv
};

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
	ErrBeg = 0x30,
	ErrInvLen = 0x31,
	ErrTimeOut = 0x32,
	ErrChnlBsy = 0x33,
	ErrPktType = 0x34,
	ErrDestAddr = 0x35,
};

char devAddr;
char destAddr;
char devType;
volatile char expInt;
unsigned int totalCnt;
unsigned int testCnt;
unsigned int errCnt;
char testStat;

/* 
 * 30us for a cycle
 * max cycle is 65535, approximate 2s
 * no overflow protection
 */
void timerIssuer(unsigned int cycles); 

void mcuInit();

// Interrupts
void intEnTrans();
void intDisTrans();
void intEnWor();
void intDisWor();
void resetInt();
#endif 
