/* A Revised Version! */
#ifndef CCTRANS_H_
#define CCTRANS_H_

#include "msp430x21x2.h"
#include "cc2500.h"
#include "ccSpi.h"
#include "ccHAL.h"
#include "ccCom.h"

// General Output Mask
#define WorInt (P2IFG & BIT1) //GDO2
#define TransInt (P2IFG & BIT0) //GDO0

// Carrier Sense Threshold
#define CSThreshold 0x00

// Timings of the transmission, the unit is RSSIRate except RSSIRate
#define RSSIRate (MSecond/6)
#define SamplePeriod 15 
#define TimeOut (MSecond*30)

// packet format
#define DestMac 0
#define SrcMac 1
#define PacketID 2
#define PacketType 3
#define PayloadBeg 4
#define Header 4
#define FlagPayload 1
#define AckPayload 3
#define DataPayload 33
#define PayloadContent 0x17

// times
#define UniDuration 80

enum RadioState
{
	RadioInvalid,
	RadioIdle,
	RadioTx,
	RadioRx,
};

enum Type
{
	Flag = 0x01,
	Data = 0x02,
	Ack = 0x04,
	BroadCast= 0x08
};

volatile char radioState;

unsigned int critical waitForTrans();

unsigned int sendPacket(char *dataBuf, unsigned int dataLen);
unsigned int send(char *dataBuf, unsigned int *dataLen);
unsigned int recv(char *dataBuf, unsigned int *dataLen);
unsigned int test(char *dataBuf, unsigned int *dataLen);

unsigned int sampling();
unsigned int sampleTheChannel(char *dataBuf, unsigned int *dataLen);

#endif
