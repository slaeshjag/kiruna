TOOLCHAIN	= arm-none-eabi-
CC		= $(TOOLCHAIN)gcc
AS		= $(TOOLCHAIN)as
LD		= $(TOOLCHAIN)ld
OBJCOPY		= $(TOOLCHAIN)objcopy

LPC21ISP	= lpc21isp

RM		= rm -f
CP		= cp
MV		= mv
MKDIR		= mkdir

CFILES		= $(wildcard *.c)
ASFILES		= $(wildcard *.s)
OBJFILES	= $(CFILES:.c=.o)
OBJFILES	+=$(ASFILES:.s=.o)
LINKSCRIPT	= link.ld
BINFILE		= autokorg

TERM-DEVICE	= /dev/ttyUSB0
##UART-BAUDRATE	= 115200
TERM-BAUDRATE	= 115200
UART-BAUDRATE	= 230400
#CHIP-XTAL	= 60000
CHIP-XTAL	= 48000

ASFLAGS		= -mthumb -mcpu=cortex-m0
CFLAGS		= -O3 -Wall $(ASFLAGS) -D__NEWLIB__ -DUART_BAUD_RATE=$(UART-BAUDRATE) -DSYSTEM_CLOCK=$(CHIP-XTAL)000
CFLAGS		+= -DDEBUG
LDFLAGS		= $(CFLAGS) -nostartfiles

.PHONY: all clean install install-term extract-code

all: $(OBJFILES) system/startup.o
	$(CC) $(LDFLAGS) system/startup.c $(OBJFILES) -o $(BINFILE).elf -T $(LINKSCRIPT)

extract-code: all
	$(OBJCOPY) -O ihex $(BINFILE).elf $(BINFILE).hex

install-term: extract-code
	$(LPC21ISP) -control -term $(BINFILE).hex $(TERM-DEVICE) $(TERM-BAUDRATE) $(CHIP-XTAL)

install: extract-code
	$(LPC21ISP) -control $(BINFILE).hex $(TERM-DEVICE) $(TERM-BAUDRATE) $(CHIP-XTAL)

clean:
	$(RM) $(OBJFILES) system/startup.o $(BINFILE).hex $(BINFILE).elf

system/startup.o:
	$(CC) $(CFLAGS) -c -o $@ $*.c

%.o: %.c %.h
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(AS) $(ASFLAGS) -o $@ $<
