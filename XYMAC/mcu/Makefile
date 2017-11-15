# Makefile for CCWireless_Release_2_0
# 12/03/2010
# lovexy

CC=msp430-gcc
CFLAG= -g -O2 -I. -Wall
MCU=-mmcu=msp430x2132
OBJ=ccWireless.o ccSpi.o ccCom.o ccHAL.o ccTrans.o

all : ${OBJ} ccWireless.elf

ccWireless.elf : ${OBJ}
	${CC} ${CFLAG} ${MCU} ${OBJ} -o $@

${OBJ} : %.o : %.c  
	${CC} -c ${CFLAG} ${MCU} $< -o $@

.PHONY : clean
clean:
	rm ${OBJ} ccWireless.elf
