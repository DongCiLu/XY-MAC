#include "ccHAL.h"

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
	halWriteReg(TI_CC2500_SYNC1, 0xD3);
	halWriteReg(TI_CC2500_SYNC0, 0x91);
	halWriteReg(TI_CC2500_PKTLEN, 0x3C);
	halWriteReg(TI_CC2500_PKTCTRL1, 0x4E);// *4E - 04
	halWriteReg(TI_CC2500_PKTCTRL0, 0x05);
	halWriteReg(TI_CC2500_ADDR, devAddr);
	halWriteReg(TI_CC2500_CHANNR, 0x00);
	halWriteReg(TI_CC2500_FSCTRL1, 0x06);
	halWriteReg(TI_CC2500_FSCTRL0, 0x00);
	halWriteReg(TI_CC2500_FREQ2, 0x5D);
	halWriteReg(TI_CC2500_FREQ1, 0x93);
	halWriteReg(TI_CC2500_FREQ0, 0xB1);
	halWriteReg(TI_CC2500_MDMCFG4, 0x79);
	halWriteReg(TI_CC2500_MDMCFG3, 0x93);
	halWriteReg(TI_CC2500_MDMCFG2, 0x03);// *01 - 03
	halWriteReg(TI_CC2500_MDMCFG1, 0x22);
	halWriteReg(TI_CC2500_MDMCFG0, 0xF8);
	halWriteReg(TI_CC2500_DEVIATN, 0x44);
	halWriteReg(TI_CC2500_MCSM2 , 0x09); // *09 - 07: 07 is not accpetable, but 09 works, 01 won't work, at the same time the PQT = 0, why?
	halWriteReg(TI_CC2500_MCSM1 , 0x03); // *TX to RX, RX to IDLE, no CCA
	halWriteReg(TI_CC2500_MCSM0 , 0x08);
	/*
	if(devType == DevTrans)
		halWriteReg(TI_CC2500_MCSM0 , 0x08);
	else
		halWriteReg(TI_CC2500_MCSM0 , 0x38);
	*/
	halWriteReg(TI_CC2500_FOCCFG, 0x16);
	halWriteReg(TI_CC2500_BSCFG, 0x6C);
	halWriteReg(TI_CC2500_AGCCTRL2, 0x43);
	halWriteReg(TI_CC2500_AGCCTRL1, 0x40); 
	halWriteReg(TI_CC2500_AGCCTRL0, 0x91); 
	halWriteReg(TI_CC2500_WOREVT1, 0x43);
	halWriteReg(TI_CC2500_WOREVT0, 0xB5);
	halWriteReg(TI_CC2500_WORCTRL, 0x38);
	halWriteReg(TI_CC2500_FREND1, 0x56);
	halWriteReg(TI_CC2500_FREND0, 0x10);
	halWriteReg(TI_CC2500_FSCAL3, 0xA9);
	halWriteReg(TI_CC2500_FSCAL2, 0x0A);
	halWriteReg(TI_CC2500_FSCAL1, 0x00);
	halWriteReg(TI_CC2500_FSCAL0, 0x11);
	halWriteReg(TI_CC2500_PATABLE, 0xFF); 
	/* !!!!!!!
	 * due to the implementation error in halWriteReg
	 * (address 0x3E is disabled), 
	 * the configuration of output power is ignored,
	 * so default value C6 is used, 
	 * which stand for -12dB
	 */

	/* ----------------Generated-------------- */
}

void halResetChip(void)
{
    // Toggle chip select signal
    SpiClose;
    timerIssuer(5);
    SpiOpen;
    timerIssuer(5);
    SpiClose;
    timerIssuer(5);

    // Send SRES command
    SpiOpen;
    SpiReady;
    spiAccessByte(TI_CC2500_SRES);

    // Wait for chip to finish internal reset
    SpiReady;
    SpiClose;
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
