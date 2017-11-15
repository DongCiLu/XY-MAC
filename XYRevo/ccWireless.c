/*
  * Author: lovexy
  * Date: 05/09/2011
  * Version: 2.0
  * A revised version for WCNC 2012.
  */

#include "msp430f2132.h"

#include "cc2500.h"
#include "ccCom.h"
#include "ccSpi.h"
#include "ccHAL.h"
#include "ccNetP.h"

#include "signal.h"
#include "string.h"

#define ccTrans
//#define ccRecv
#define DevAddr 0xA0
#define TotalReceiver 7

/* Exam related */
unsigned int GCnt[7];
unsigned int TSuc;
unsigned int TTotal;
unsigned int RAwake;
unsigned int RTotal;
unsigned int RSuc;

void GlobalParaInit();
void sysInit();
int selfChk();
void errorQ(char ret); /* the parameter is used to save the err type */

void delay(int secends);
void genExperimentPkt(struct packetFormat *pkt, unsigned int *pktLen);

int main()
{
	char ret = ErrBeg;

	struct packetFormat pkt;
	unsigned int pktLen;

	memset(&pkt, 0x00, sizeof pkt);
	GlobalParaInit();
	sysInit();
	if(!selfChk())
		errorQ(ret);

	while(1)
	{
#if defined(ccTrans)
		delay(10);
		if(TTotal++ == 35)
			break;
		P3OUT ^= LEDG;
		genExperimentPkt(&pkt, &pktLen);
		switch(ret = sendSP(&pkt, &pktLen))
		{
			case True:
				RTotal += pkt.payload[0];
				RAwake += pkt.payload[1];
				TSuc++;
				P3OUT ^= LEDR;
				break;	/* Succeed, Continuing next loop */
				
			case ErrTOTrans:
				GCnt[1]++;
				break;
			case ErrTargetNotResp:
				GCnt[2]++;
				break;
			case ErrRecvPktLen:
				GCnt[3]++;
				break; /* Neglectable Error, Continuing next loop */
						
			case ErrUnexpectAck:
			case ErrNoBPacket:
			default:
				errorQ(ret); /* Critical Error, Alerting */
				break;
		}
#elif defined(ccRecv)
		dutyCycle();
		RTotal++;
		P3OUT ^= LEDG;
		switch(ret = sampleTheChannel(&pkt, &pktLen))
		{
			case True:
				RTotal = 0;
				RAwake= 0; /* Reset Status */
				RSuc++;
				P3OUT ^= LEDR;
				break;	/* Succeed, Continuing next loop */

			case ErrRecvPktLen:	/* Major reason: address mismatch */	
			case ErrFalsePositive:	/* Major reason: noise */
			case ErrTOTrans:
				RAwake++;
			case WarnNonPktSensed:	
				break; /* Neglectable Error, Continuing next loop */
			
			case ErrNoBPacket:
			default:
				errorQ(ret); /*Critical Error, Alerting */
				break;
		}
#else /* tests */
		if((signed char)0x00 > (signed char)0xBC)
			P3OUT |= LEDR | LEDG;
#endif
	}

	errorQ(ret);
	/* never reach here */
	return 0;
}

void delay(int seconds)
{
	int ix;
	for(ix = 0; ix < seconds; ix++)
		timerIssuer(Second);
}

void genExperimentPkt(struct packetFormat *pkt, unsigned int *pktLen)
{
	static char destAddr = 0x00;

	if(++destAddr > TotalReceiver)
		destAddr = 0x01;

	pkt->header.destAddr = destAddr;
	pkt->header.srcAddr = devAddr;
	pkt->header.packetID = PreambleDuration;
	pkt->header.packetType = SPreamble;

	pkt->payload[0] = 0x07;

	*pktLen = sizeof(struct packetFormat);
}

void GlobalParaInit()
{
	int ix = 0;
	for(ix = 0; ix < 7; ix++)
		GCnt[ix] = 0;
	TTotal = 0;
	TSuc = 0;
	RAwake = 0;
	RTotal = 0;
	RSuc = 0;
	devAddr = DevAddr;
	expInt = ExpInvalid;
	radioState = RadioInvalid;
}

void sysInit()
{
	mcuInit();
	spiConfig();
	halResetChip();
	halConfig();
	resetInt();
	radioCaliberation();
}

int selfChk()
{
	if(halReadReg(TI_CC2500_ADDR) == devAddr)
	{
		P3OUT |= LEDR | LEDG;
		timerIssuer(Second);
		P3OUT &= ~(LEDR | LEDG);
		timerIssuer(Second);

		return True;
	}
	else
		return False;
}

void errorQ(char ret)
{
	while(ret)
	{
		P3OUT ^= LEDR | LEDG;
		timerIssuer(Second/10);
	}
}


