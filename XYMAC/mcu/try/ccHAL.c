#include "ccHAL.h"

void halResetChip(void)
{
    // Toggle chip select signal
    SpiClose;
    timerIssuer(5 * MSecond);
    SpiOpen;
    timerIssuer(5 * MSecond);
    SpiClose;
    timerIssuer(7 * MSecond);

    // Send SRES command
    SpiOpen;
    SpiReady;
    spiAccessByte(TI_CC2500_SRES);

    // Wait for chip to finish internal reset
    SpiReady;
    SpiClose;
}

void halConfig()
{
	// Write register settings, refer to regSet(doc) for details
	if(devType == DevTrans)
		halWriteReg(TI_CC2500_IOCFG2, 0x2E); 
	else
		halWriteReg(TI_CC2500_IOCFG2, 0x25); 
	halWriteReg(TI_CC2500_IOCFG1, 0x2E); 
	halWriteReg(TI_CC2500_IOCFG0, 0x06);
	halWriteReg(TI_CC2500_FIFOTHR, 0x0D);
	halWriteReg(TI_CC2500_SYNC1, 0x17);
	halWriteReg(TI_CC2500_SYNC0, 0x77);
	halWriteReg(TI_CC2500_PKTLEN, 0x3C);
	halWriteReg(TI_CC2500_PKTCTRL1, 0x4E);
	halWriteReg(TI_CC2500_PKTCTRL0, 0x05);
	halWriteReg(TI_CC2500_ADDR, devAddr);
	halWriteReg(TI_CC2500_MDMCFG4, 0x79);
	halWriteReg(TI_CC2500_MDMCFG3, 0x79);
	//halWriteReg(TI_CC2500_MDMCFG2, 0x05);
	halWriteReg(TI_CC2500_MDMCFG2, 0x15);
	halWriteReg(TI_CC2500_MDMCFG1, 0x22);
	halWriteReg(TI_CC2500_MCSM2 , 0x19);
	halWriteReg(TI_CC2500_MCSM1 , 0x23); // TX to RX, RX to IDLE
	if(devType == DevTrans)
		halWriteReg(TI_CC2500_MCSM0 , 0x04);
	else
		halWriteReg(TI_CC2500_MCSM0 , 0x34);
	halWriteReg(TI_CC2500_AGCCTRL2, 0x01);
	halWriteReg(TI_CC2500_AGCCTRL1, 0x49); 
	halWriteReg(TI_CC2500_WOREVT1, 0x43);
	halWriteReg(TI_CC2500_WOREVT0, 0xB5);
	halWriteReg(TI_CC2500_WORCTRL, 0x38);
	halWriteReg(TI_CC2500_FREND0, 0x10);
	halWriteReg(TI_CC2500_PATABLE, 0xFF);

	/* ----------------Generated-------------- */
	halWriteReg(TI_CC2500_FSCTRL1, 0x06);
	halWriteReg(TI_CC2500_FREQ2, 0x5D);
	halWriteReg(TI_CC2500_FREQ1, 0x93);
	halWriteReg(TI_CC2500_FREQ0, 0xB1);
	halWriteReg(TI_CC2500_DEVIATN, 0x44);
	halWriteReg(TI_CC2500_FSCAL1, 0x00);
	halWriteReg(TI_CC2500_FSCAL0, 0x11);
}

char halWriteReg(char addr, char data)
{
	char ret = 0xFF;
	
	if((addr <= 0x2F) || (addr == 0x3F))
	{
		SpiOpen;
		SpiReady;
		ret = spiAccessByte(addr + TI_CC2500_WRITE_SINGLE);
		ret = spiAccessByte(data);
		SpiClose;
	}
	
	return ret;
}

char halReadReg(char addr)
{
	char ret = 0xFF;
	
	if((addr <= 0x2F) || (addr == 0x3F))
	{
		SpiOpen;
		SpiReady;
		ret = spiAccessByte(addr + TI_CC2500_READ_SINGLE);
		ret = spiAccessByte(TI_CC2500_SNOP);
		SpiClose;
	}
	else if((0x30 <= addr) && (addr <= 0x3D))
	{
		SpiOpen;
		SpiReady;
		ret = spiAccessByte(addr + TI_CC2500_READ_BURST);
		ret = spiAccessByte(TI_CC2500_SNOP);
		SpiClose;
	}
	
	return ret;
}

char halWriteStrobe(char addr)
{
	char ret = 0xFF;
	
	if((0x30 <= addr) && (addr <= 0x3D))
	{
		SpiOpen;
		SpiReady;
		ret = spiAccessByte(addr);
		SpiClose;
	}
	
	return ret;
}

char halWriteTXBurst(char addr, char *dataBuf, unsigned int cnt)
{
	char ret = 0xFF;
	
	if((addr == TI_CC2500_TXFIFO) && (cnt <= (FIFOLEN - getFIFOStatus(TI_CC2500_TXBYTES))))
	{
		SpiOpen;
		SpiReady;
		ret = spiAccessByte(addr + TI_CC2500_WRITE_BURST);
		
		int ix;
		for(ix = 0; ix < cnt; ix++)
		{
			ret = spiAccessByte(dataBuf[ix]);
		}
		SpiClose;
	}
		
	return ret;	
}

char halReadRXBurst(char addr, char *retBuf, unsigned int cnt)
{
	char ret = 0xFF;
	
	if((addr == TI_CC2500_RXFIFO) && (cnt < FIFOLEN)) // one byte for payload length
	{
		SpiOpen;
		SpiReady;
		ret = spiAccessByte(addr + TI_CC2500_READ_BURST);
		
		int ix;
		for(ix = 0; ix < cnt; ix++)
		{
			retBuf[ix] = spiAccessByte(TI_CC2500_SNOP);
		}
		SpiClose;
	}
	
	return ret;
}

unsigned int getFIFOStatus(char type)
{
	char ret = 0xFF;
	
	if((type == TI_CC2500_TXBYTES) || (type == TI_CC2500_RXBYTES))
	{
		SpiOpen;
		SpiReady;
		ret = spiAccessByte(type + TI_CC2500_READ_BURST);
		ret = spiAccessByte(TI_CC2500_SNOP) & ~OoUFlowFlag;
		SpiClose;
	}
	
	return ret;
}
