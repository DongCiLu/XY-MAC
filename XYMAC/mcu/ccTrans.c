/* A Revised Version! */
#include "ccTrans.h"

unsigned int critical waitForTrans()
{
	expInt = ExpInvalid;

	TACCTL0 |=  CCIE; // CCIFG Enable
	TACCR0 = TimeOut; // Set Goal
	TACTL = TASSEL_1 | MC_1; // ACLK, UP Mode

	intEnTrans();
	switch(radioState)
	{
		case RadioTx:
			while((expInt != ExpTx) && (expInt != ExpTimer))
				McuSleep;
			break;
		case RadioRx:
			while((expInt != ExpRx) && (expInt != ExpTimer))
				McuSleep;
			break;
	}
	intDisTrans();

	TACTL = MC_0; // Stop timer.

	if(expInt != ExpTimer)
		return True;
	else
		return False;
}

unsigned int sendPacket(char *dataBuf, unsigned int dataLen)
{
	unsigned int ret = 0xFF;

	halWriteReg(TI_CC2500_TXFIFO, dataLen);
	halWriteTXBurst(TI_CC2500_TXFIFO, dataBuf, dataLen);

	halWriteStrobe(TI_CC2500_STX);

	radioState = RadioTx;
	ret = waitForTrans();
	radioState = RadioRx;

	return ret;
}

unsigned int checkIdle()
{
	int ix;
	int busy = 0;

	timerIssuer(1);
	for(ix = 0; ix < 20; ix++)
	{
		if(halReadReg(TI_CC2500_RSSI) > (char)CSThreshold)
			busy++;
	}

	if(busy > 5)
		return False; // busy
	else
		return True; // idle
}

unsigned int send(char *dataBuf, unsigned int *dataLen)
{
	unsigned int times = UniDuration; 
	char recvAck = False;

	halWriteStrobe(TI_CC2500_SFRX);
	timerIssuer(5);

	halWriteStrobe(TI_CC2500_SRX);
	timerIssuer(MSecond/4);

	while(times--)
	{
		if(!checkIdle())
		{
			if(!waitForTrans())
				continue;
			recvAck = True;
			break;
		}

		if(!sendPacket(dataBuf, Header + FlagPayload))
			return ErrTimeOut;
	}

	if(!recvAck) // No ack is received.
	{
		if(!(++times))
			return True;
		else
			return False;
	}
	else // An ack is received.
	{
		*dataLen = halReadReg(TI_CC2500_RXFIFO);
		if(*dataLen == 0)
			return ErrDestAddr;
		if(*dataLen > Header + AckPayload)
			return ErrInvLen;
		halReadRXBurst(TI_CC2500_RXFIFO, dataBuf, *dataLen);
		if(dataBuf[PacketType] != Ack)
			return ErrPktType;
		return True;
	}
}

unsigned int test(char *dataBuf, unsigned int *dataLen)
{
	dataBuf[PacketID]++;
	halWriteStrobe(TI_CC2500_SCAL);
	timerIssuer(MSecond);

	while(1)
	{
		sendPacket(dataBuf, Header + FlagPayload);

		if(waitForTrans())
			P3OUT ^= LED2;
	}

	return True;
}

unsigned int sampling()
{
	int ix;
	int busy = 0;
	for(ix = 0; ix < SamplePeriod; ix++)
	{
		if(halReadReg(TI_CC2500_RSSI) > (char)CSThreshold)
			busy++;
		timerIssuer(RSSIRate);
	}

	if(busy > SamplePeriod / 3)
		return False; // busy
	else
		return True; // idle
}

unsigned int sampleTheChannel(char *dataBuf, unsigned int *dataLen)
{
	if(!sampling()) // approximate 4 ms
	{
		if(waitForTrans())
		{
			*dataLen = halReadReg(TI_CC2500_RXFIFO);
			if(*dataLen == 0)
				return ErrDestAddr;
			if(*dataLen > Header + FlagPayload)
				return ErrInvLen;
			halReadRXBurst(TI_CC2500_RXFIFO, dataBuf, *dataLen);
			if((dataBuf[PacketType] != Flag) && (dataBuf[PacketType] != BroadCast))
				return ErrPktType;
			return True;
		}
		else
			return ErrTimeOut;
	}
	else
	{
		halWriteStrobe(TI_CC2500_SIDLE);
		timerIssuer(5);
		halWriteStrobe(TI_CC2500_SFRX);
		timerIssuer(5);
		return halReadReg(TI_CC2500_MARCSTATE);
	}
	return ErrBeg;
}

unsigned int recv(char *dataBuf, unsigned int *dataLen)
{
	/* Do not sample the channel, send directly */
	if(!sendPacket(dataBuf, Header + AckPayload))
		return ErrTimeOut;
	return True;
}

// Port2 Interrupt
interrupt (PORT2_VECTOR) wakeup port2_ISR(void)
{
	/* should reset the corresponding IFG manually */
	if(TransInt)
	{
		if(radioState == RadioTx)
		{
			expInt = ExpTx;
			P2IFG &= ~BIT0;
			return;
		}
		else if(radioState == RadioRx)
		{
			expInt = ExpRx;
			P2IFG &= ~BIT0;
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
		expInt = ExpWor;
	}
	else
	{
		P2IFG = EMPTY;
		expInt = ExpInvalid;
	}
}
