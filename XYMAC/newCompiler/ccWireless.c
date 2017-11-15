/* A Revised Version */

//#include "msp430x21x2.h"
#include "msp430f2132.h"
#include "ccCom.h"
#include "ccSpi.h"
#include "ccHAL.h"
#include "ccTrans.h"
//#include "signal.h"
#include "legacymsp430.h"
#include "string.h"

/* Experiment Parameters */
#define TransmitterAddr 0x07
#define ReceiverBaseAddr 0xA0
#define ReceiverCnt 2
#define ReceiverNum 0
#define BroadcastAddr 0x00

void GlobalParaInit();
void sysInit();
int selfChk();
void errorQ();
void generatePacket(char *dataBuf);
void generateAck(char *dataBuf);

char dataBuf[FIFOLEN];
unsigned int dataLen = 0;
unsigned int ret = ErrBeg;

int main()
{
	memset(dataBuf, '\0', FIFOLEN);
	GlobalParaInit();
	sysInit();
	if(!selfChk())
		errorQ();

	while(1)
	{
		if(devType == DevTrans)
		{
			/*
			int ix;
			for(ix = 9; ix > 0; ix--)
				timerIssuer(Second);
			*/
			timerIssuer(Second * 0.7);
			generatePacket(dataBuf);
			/*
			if(dataBuf[2] == 35)
				errorQ();
			*/
			halWriteStrobe(TI_CC2500_SCAL);
			timerIssuer(MSecond);
			ret = send(dataBuf, &dataLen);
			if(ret && (dataLen > 0))
			{
				totalCnt += (unsigned char)dataBuf[PayloadBeg];
				testCnt += (unsigned char)dataBuf[PayloadBeg + 1];
				errCnt += (unsigned char)dataBuf[PayloadBeg + 2];
				P3OUT ^= LED2;
			}
			else if(ret > ErrBeg)
			{
				P3OUT ^= LED1;
				if(ret == ErrDestAddr)
					continue;
				/* FIFO dump */
				halReadRXBurst(TI_CC2500_RXFIFO, dataBuf, dataLen);
			}
			memset(dataBuf, '\0', FIFOLEN);
			dataLen = 0;
		}
		else if(devType == DevRecv)
		{
			halWriteStrobe(TI_CC2500_SWOR);
			intEnWor();
			McuSleep;
			intDisWor();
			if(expInt == ExpWor)
			{
				totalCnt++;
				radioState = RadioRx;
				ret = sampleTheChannel(dataBuf, &dataLen);
				if(ret && (dataLen > 0))
				{
					testCnt++;
					generateAck(dataBuf);
					if(recv(dataBuf, &dataLen));
					{
						P3OUT ^= LED2;
					}
					memset(dataBuf, '\0', FIFOLEN);
					dataLen = 0;
				}
				else if(ret > ErrBeg)
				{
					if(ret == ErrDestAddr)
					{
						errCnt++;
						continue;
					}
					else if(ret == ErrTimeOut) 
						continue;
					P3OUT ^= LED1;
					/* FIFO dump */
					halReadRXBurst(TI_CC2500_RXFIFO, dataBuf, dataLen);
				}
				halWriteStrobe(TI_CC2500_SCAL);
				timerIssuer(MSecond); //wait Calibrate being finished
			}
		}
	}
			
	errorQ();
	/* never reach here! */
	return 0;
}

void GlobalParaInit()
{
	devType = DevTrans;
	if(devType == DevTrans)
		devAddr = TransmitterAddr;
	else
		devAddr = ReceiverBaseAddr | ReceiverNum;
	expInt = ExpInvalid;
	radioState = RadioInvalid;
	totalCnt = 0;
	testCnt = 0;
	errCnt = 0;
}

void generatePacket(char *dataBuf)
{
	static char packetID = 0;

	dataBuf[DestMac] = ReceiverBaseAddr | (packetID % ReceiverCnt);
	dataBuf[SrcMac] = TransmitterAddr;
	dataBuf[PacketID] = packetID++;
	dataBuf[PacketType] = Data;

	memset(dataBuf + Header, PayloadContent, DataPayload);
}

void generateAck(char *dataBuf)
{
	char tmp = 0xFF;
	tmp = dataBuf[DestMac];
	dataBuf[DestMac] = dataBuf[SrcMac];
	dataBuf[SrcMac] = tmp;
	dataBuf[PacketType] = Ack;

	/* attach the status in ack */
	dataBuf[PayloadBeg] = (unsigned char)totalCnt;
	dataBuf[PayloadBeg + 1] = (unsigned char)testCnt;
	dataBuf[PayloadBeg + 2] = (unsigned char)errCnt;

	/* reset the status */
	totalCnt = 0;
	testCnt = 0;
	errCnt = 0;
}

void sysInit()
{
	mcuInit();
	spiConfig();
	halResetChip();
	halConfig();
	resetInt();
	halWriteStrobe(TI_CC2500_SIDLE);
	halWriteStrobe(TI_CC2500_SCAL);
	radioState = RadioIdle;
	IntOn;
}

int selfChk()
{
	if(halReadReg(TI_CC2500_ADDR) == devAddr)
	{
		P3OUT |= LED1 | LED2;
		timerIssuer(Second);
		P3OUT &= ~(LED1 | LED2);

		return True;
	}
	else
		return False;
}

void errorQ()
{
	while(1)
	{
		P3OUT ^= LED1;
		timerIssuer(Second/10);
	}
}


