/*
  * Author: lovexy
  * Date: 05/09/2011
  * Version: 2.0
  * A revised version for WCNC 2012.
  */

#ifndef CCNETP_H_
#define CCNETP_H_

#include "msp430f2132.h"
#include "cc2500.h"
#include "ccSpi.h"
#include "ccHAL.h"
#include "ccCom.h"
#include "string.h"

/* General Output Mask */
#define WorInt (P2IFG & BIT1) //GDO2
#define TransInt (P2IFG & BIT0) //GDO0

/* Carrier Sense Threshold and RSSI related */
#define CSThreshold 0x07
#define MinimalRSSI 0x80

/* Chk and Sample threshold */
/* Still have room for optimization, leave for later */
#define AckDctPeriod 20
#define AckDctThre 5
#define SamplePeriod 60
#define SampleThre 5

/* 
  * Timings of the transmission:
  * Packet duration: 6ms (15 Bytes under 20kbps) / 2ms (15 Bytes under 60kbps) 
  * Ack detection duration: 0.92ms (30 ticks with 32768Hz crystal oscillator)
  * Sampling period: 1.77ms (58 ticks with 32768Hz crystal oscillator) / 2.62ms (86 ticks with 32768Hz crystal oscillator)
  * Sleeping period: 500ms
  */
#define TimeOut (MSecond*20)
/* Can not exceed 255 due to implementation constrants, multiply by 2 */
#define PreambleDuration 135

/* packet format */
#define PayloadLen 2
#define AppDataLen 2
#define MaxPktLen 20
struct packetFormat 
{
	struct packetHeader
	{
		char destAddr;
		char srcAddr;
		/* 
		  * indicates the number of remaining short preamble packets, 
		  * Ack should be the same as the original packet.
		  */
		char packetID; 
		char packetType;
	} header;
	
	char payload[PayloadLen];
};

/* Packet Validation */
#define BroadcastAddr 0x00
#define InvalidPktLen 0

/* exam related */
extern unsigned int RAwake;
extern unsigned int RTotal;
extern unsigned int GCnt[7];

enum PacketType
{
	Ack,
	SPreamble
};

enum RadioState
{
	RadioInvalid,
	RadioIdle,
	RadioTX,
	RadioRX
};

extern volatile char radioState;

void dutyCycle();
void radioCaliberation();
char sendSP(struct packetFormat *pkt, unsigned int *pktLen);
char sampleTheChannel(struct packetFormat *pkt, unsigned int *pktLen);

#endif
