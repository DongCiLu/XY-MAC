#ifndef CC2500_H_
#define CC2500_H_

// Configuration Registers
#define TI_CC2500_IOCFG2 0x00 // GDO2 output pin configuration
#define TI_CC2500_IOCFG1 0x01 // GDO1 output pin configuration
#define TI_CC2500_IOCFG0 0x02 // GDO0 output pin configuration
#define TI_CC2500_FIFOTHR 0x03 // RX FIFO and TX FIFO thresholds
#define TI_CC2500_SYNC1 0x04 // Sync word, high byte
#define TI_CC2500_SYNC0 0x05 // Sync word, low byte
#define TI_CC2500_PKTLEN 0x06 // Packet length
#define TI_CC2500_PKTCTRL1 0x07 // Packet automation control
#define TI_CC2500_PKTCTRL0 0x08 // Packet automation control
#define TI_CC2500_ADDR 0x09 // Device address
#define TI_CC2500_CHANNR 0x0A // Channel number
#define TI_CC2500_FSCTRL1 0x0B // Frequency synthesizer control
#define TI_CC2500_FSCTRL0 0x0C // Frequency synthesizer control
#define TI_CC2500_FREQ2 0x0D // Frequency control word, high byte
#define TI_CC2500_FREQ1 0x0E // Frequency control word, middle byte
#define TI_CC2500_FREQ0 0x0F // Frequency control word, low byte
#define TI_CC2500_MDMCFG4 0x10 // Modem configuration
#define TI_CC2500_MDMCFG3 0x11 // Modem configuration
#define TI_CC2500_MDMCFG2 0x12 // Modem configuration
#define TI_CC2500_MDMCFG1 0x13 // Modem configuration
#define TI_CC2500_MDMCFG0 0x14 // Modem configuration
#define TI_CC2500_DEVIATN 0x15 // Modem deviation setting
#define TI_CC2500_MCSM2 0x16 // Main Radio Cntrl State Machine config
#define TI_CC2500_MCSM1 0x17 // Main Radio Cntrl State Machine config
#define TI_CC2500_MCSM0 0x18 // Main Radio Cntrl State Machine config
#define TI_CC2500_FOCCFG 0x19 // Frequency Offset Compensation config
#define TI_CC2500_BSCFG 0x1A // Bit Synchronization configuration
#define TI_CC2500_AGCCTRL2 0x1B // AGC control
#define TI_CC2500_AGCCTRL1 0x1C // AGC control
#define TI_CC2500_AGCCTRL0 0x1D // AGC control
#define TI_CC2500_WOREVT1 0x1E // High byte Event 0 timeout
#define TI_CC2500_WOREVT0 0x1F // Low byte Event 0 timeout
#define TI_CC2500_WORCTRL 0x20 // Wake On Radio control
#define TI_CC2500_FREND1 0x21 // Front end RX configuration
#define TI_CC2500_FREND0 0x22 // Front end TX configuration
#define TI_CC2500_FSCAL3 0x23 // Frequency synthesizer calibration
#define TI_CC2500_FSCAL2 0x24 // Frequency synthesizer calibration
#define TI_CC2500_FSCAL1 0x25 // Frequency synthesizer calibration
#define TI_CC2500_FSCAL0 0x26 // Frequency synthesizer calibration
#define TI_CC2500_RCCTRL1 0x27 // RC oscillator configuration
#define TI_CC2500_RCCTRL0 0x28 // RC oscillator configuration
#define TI_CC2500_FSTEST 0x29 // Frequency synthesizer cal control
#define TI_CC2500_PTEST 0x2A // Production test
#define TI_CC2500_AGCTEST 0x2B // AGC test
#define TI_CC2500_TEST2 0x2C // Various test settings
#define TI_CC2500_TEST1 0x2D // Various test settings
#define TI_CC2500_TEST0 0x2E // Various test settings

// Strobe commands
#define TI_CC2500_SRES 0x30 // Reset chip.
#define TI_CC2500_SFSTXON 0x31 // Enable/calibrate freq synthesizer
#define TI_CC2500_SXOFF 0x32 // Turn off crystal oscillator.
#define TI_CC2500_SCAL 0x33 // Calibrate freq synthesizer & disable
#define TI_CC2500_SRX 0x34 // Enable RX.
#define TI_CC2500_STX 0x35 // Enable TX.
#define TI_CC2500_SIDLE 0x36 // Exit RX / TX
#define TI_CC2500_SAFC 0x37 // AFC adjustment of freq synthesizer
#define TI_CC2500_SWOR 0x38 // Start automatic RX polling sequence
#define TI_CC2500_SPWD 0x39 // Enter pwr down mode when CSn goes hi
#define TI_CC2500_SFRX 0x3A // Flush the RX FIFO buffer.
#define TI_CC2500_SFTX 0x3B // Flush the TX FIFO buffer.
#define TI_CC2500_SWORRST 0x3C // Reset real time clock.
#define TI_CC2500_SNOP 0x3D // No operation.

// Status registers
#define TI_CC2500_PARTNUM 0x30 // Part number
#define TI_CC2500_VERSION 0x31 // Current version number
#define TI_CC2500_FREQEST 0x32 // Frequency offset estimate
#define TI_CC2500_LQI 0x33 // Demodulator estimate for link quality
#define TI_CC2500_RSSI 0x34 // Received signal strength indication
#define TI_CC2500_MARCSTATE 0x35 // Control state machine state
#define TI_CC2500_WORTIME1 0x36 // High byte of WOR timer
#define TI_CC2500_WORTIME0 0x37 // Low byte of WOR timer
#define TI_CC2500_PKTSTATUS 0x38 // Current GDOx status and packet status
#define TI_CC2500_VCO_VC_DAC 0x39 // Current setting from PLL cal module
#define TI_CC2500_TXBYTES 0x3A // Underflow and # of bytes in TXFIFO
#define TI_CC2500_RXBYTES 0x3B // Overflow and # of bytes in RXFIFO
#define TI_CC2500_NUM_RXBYTES 0x7F // Mask "# of bytes" field in _RXBYTES

//Main Radio Control State Machine State
#define StateRx 0x0D
#define StateTxRx_SETTLING 0x10
#define StateFSTXON 0x12
#define StateTx 0x13
#define StateRxTx_SETTLING 0x15

// Other memory locations
#define TI_CC2500_PATABLE 0x3E
#define TI_CC2500_TXFIFO 0x3F
#define TI_CC2500_RXFIFO 0x3F

// Masks for appended status bytes
#define TI_CC2500_LQI_RX 0x01 // Position of LQI byte
#define TI_CC2500_CRC_OK 0x80 // Mask "CRC_OK" bit within LQI byte

// Definitions to support burst/single access:
#define TI_CC2500_WRITE_SINGLE 0x00 //单一写入
#define TI_CC2500_WRITE_BURST 0x40 //突发写入
#define TI_CC2500_READ_SINGLE 0x80 //单一访问
#define TI_CC2500_READ_BURST 0xC0 //突发访问

#endif /*CC2500_H_*/

