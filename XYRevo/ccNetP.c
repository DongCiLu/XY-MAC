/*
  * Author: lovexy
  * Date: 05/09/2011
  * Version: 2.0
  * A revised version for WCNC 2012.
  */

#include "ccNetP.h"

volatile char radioState;

static char waitForTrans();
static char sendPacket(struct packetFormat *pkt, unsigned int *pktLen);
static char checkIdle(int period, int threshold);
static void prepareForRecv();
static char packetHandler(struct packetFormat *pkt, unsigned int *pktLen);

char waitForTrans()
{
	expInt = ExpInvalid;

	setTimer(TimeOut);
	intEnTrans();
	switch(radioState)
	{
		case RadioTX:
			while((expInt != ExpTx) && (expInt != ExpTimer))
				McuSleep;
			break;
		case RadioRX:
			while((expInt != ExpRx) && (expInt != ExpTimer))
				McuSleep;
			break;
		default:
			return ErrUnexpectRadioState;
	}
	intDisTrans();
	stopTimer();

	if(expInt == ExpTimer)
	{
		if(radioState == RadioTX)
			return ErrTOTrans;
		else
			return ErrTORecv;
	}
	else
		return True;
}

char sendPacket(struct packetFormat *pkt, unsigned int *pktLen)
{
	char ret = ErrBeg;

	halWriteReg(TI_CC2500_TXFIFO, *pktLen);
	halWriteTXBurst(TI_CC2500_TXFIFO, (char *)pkt, *pktLen);

	do
	{
		halWriteStrobe(TI_CC2500_STX);
		timerIssuer(MinIntvl);
	} while(halReadReg(TI_CC2500_MARCSTATE) != MARCTX);
	radioState = RadioTX;

	ret = waitForTrans();

	while(halReadReg(TI_CC2500_MARCSTATE) != MARCRX) 
	{
		halWriteStrobe(TI_CC2500_SRX);
		timerIssuer(MinIntvl);
	}
	radioState = RadioRX;

	return ret;
}

char checkIdle(int period, int threshold)
{
	int busy = 0;
	char testContent[SamplePeriod] = {0};

	while(halReadReg(TI_CC2500_MARCSTATE) != MARCRX)
	{
		halWriteStrobe(TI_CC2500_SRX);
		timerIssuer(MinIntvl);
	}
	radioState = RadioRX;

	int ix;
	for(ix = 0; ix < period; ix++)
	{
		testContent[ix] = halReadReg(TI_CC2500_RSSI);
		if((signed char)testContent[ix] > (signed char)CSThreshold)
		{
			if(++busy > threshold)
				break;
		}
		else /* Outlier detection */
			busy = 0;
	}

	if(busy > threshold)
		return False; // busy
	else
		return True; // idle
}

void radioCaliberation()
{
	while(halReadReg(TI_CC2500_MARCSTATE) != MARCIDLE)
	{
		halWriteStrobe(TI_CC2500_SIDLE);
		timerIssuer(MinIntvl); 
	}
	radioState = RadioIdle;

	halWriteStrobe(TI_CC2500_SCAL);
	timerIssuer(MSecond);
}

void prepareForRecv()
{
	while(halReadReg(TI_CC2500_MARCSTATE) != MARCIDLE)
	{
		halWriteStrobe(TI_CC2500_SIDLE);
		timerIssuer(MinIntvl);
	}
	radioState = RadioIdle;

	halWriteStrobe(TI_CC2500_SFRX);
	timerIssuer(MinIntvl);

	do
	{
		halWriteStrobe(TI_CC2500_SRX);
		timerIssuer(MinIntvl);
	} while(halReadReg(TI_CC2500_MARCSTATE) != MARCRX);
	radioState = RadioRX;
}

void dutyCycle()
{
	halWriteStrobe(TI_CC2500_SWOR);
	intEnWor();
	McuSleep;
	intDisWor();
	radioState = RadioRX;
}

char packetHandler(struct packetFormat *pkt, unsigned int *pktLen)
{
	char fifoLen = getFIFOStatus(TI_CC2500_RXBYTES);
	while(fifoLen != getFIFOStatus(TI_CC2500_RXBYTES))
		fifoLen = getFIFOStatus(TI_CC2500_RXBYTES);

	while(halReadReg(TI_CC2500_MARCSTATE) != MARCIDLE)
	{
		halWriteStrobe(TI_CC2500_SIDLE);
		timerIssuer(MinIntvl);
	}
	radioState = RadioIdle;

	*pktLen = halReadReg(TI_CC2500_RXFIFO);
	if((*pktLen == InvalidPktLen) || (*pktLen > MaxPktLen))
	{
		/* Invalid packet length, dump the RX FIFO, only for test */
		halReadRXBurst(TI_CC2500_RXFIFO, (char *)pkt, MAXPKTLEN);

		halWriteStrobe(TI_CC2500_SFRX);
		return ErrRecvPktLen;
	}
	halReadRXBurst(TI_CC2500_RXFIFO, (char *)pkt, *pktLen + AppDataLen);

	/* Flush the FIFO for the next receiving */
	halWriteStrobe(TI_CC2500_SFRX);
	return True;
}

char sendSP(struct packetFormat *pkt, unsigned int *pktLen)
{
	char ret = ErrBeg;
	unsigned int cnt = 2 * (pkt->header.packetID);
	char recvAck = False;

	radioCaliberation();
	prepareForRecv();
	while(cnt--)
	{
		pkt->header.packetID--;
		/* 
		  * Braodcast does not expect Acks.
		  * Continue sending imediately without detect the channel
		  */
		if((pkt->header.destAddr != BroadcastAddr) && \
				(checkIdle(AckDctPeriod, AckDctThre) != True))
		{
			if(waitForTrans() != True)
			{
				/* No ack found, false positive, reset Rx state and flush the RXFIFO*/
				GCnt[0]++;
				prepareForRecv();
				continue;
			}
			/* An ack packet has beed detected. */
			recvAck = True;
			break;
		}
		if((ret = sendPacket(pkt, pktLen)) != True)
			return ret;
	}

	if(pkt->header.destAddr == BroadcastAddr)
		return ErrNoBPacket; 

	else if((pkt->header.destAddr != BroadcastAddr) && !recvAck) 
		/* No ack is received for Unicast. */
		return ErrTargetNotResp;
	
	else /* An ack is received. */
	{
		if((ret = packetHandler(pkt, pktLen)) == True)
		{
			if(pkt->header.packetType == Ack)
				return True; /* Unicast, recv ack and back to IDLE */
			else 
				return ErrUnexpectAck;
		}
		else
			return ret; 
	}

}

char sampleTheChannel(struct packetFormat *pkt, unsigned int *pktLen)
{
	char ret = ErrBeg;
	
	/* always come from WOR event, so do not need to prepare for receiving */
	if(checkIdle(SamplePeriod, SampleThre) != True) 
	{
		if(waitForTrans() == True)
		{
			if((ret = packetHandler(pkt, pktLen)) == True)
			{
				if((pkt->header.destAddr != BroadcastAddr))
				{
					pkt->header.destAddr = pkt->header.srcAddr;
					pkt->header.srcAddr = devAddr;
					pkt->header.packetType = Ack;

					pkt->payload[0] = RTotal;
					pkt->payload[1] = RAwake;
					
					ret = sendPacket(pkt, pktLen);
					return ret; /* Unicast, send ack and return to IDLE */
				}
				else 
					return ErrNoBPacket; 
			}
			else
				return ret;
		}
		else
			return ErrFalsePositive;
	}
	else
	{
		radioCaliberation();

		return WarnNonPktSensed;
	}
}

// Port2 Interrupt
#pragma vector=PORT2_VECTOR
__interrupt void port2_ISR(void)
{
	/* should reset the corresponding IFG manually */
	if(TransInt)
	{
		if(radioState == RadioTX)
		{
			expInt = ExpTx;
			P2IFG &= ~BIT0;
			McuWakeup;
		}
		else if(radioState == RadioRX)
		{
			expInt = ExpRx;
			P2IFG &= ~BIT0;
			McuWakeup;
		}
		else
		{
			expInt = ExpInvalid;
			P2IFG = EMPTY;
		}
	}
	else if(WorInt)
	{
		P2IFG &= ~BIT1;
		McuWakeup;
		expInt = ExpWor;
	}
	else
	{
		P2IFG = EMPTY;
		expInt = ExpInvalid;
	}
}
