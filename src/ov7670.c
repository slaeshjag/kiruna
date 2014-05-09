#include "system/LPC11xx.h"
#include "uart.h"
#include "util.h"
#include "spi.h"


#define SLAVE_ADR_W	0x42
#define SLAVE_ADR_R	0x43

#define SLAVE_SUB_ADR	0x12

unsigned short packet_number;

char ov7670_read_reg(char slave_sub_adr);
void ov7670_write_reg(char slave_sub_adr, char data);

static unsigned char camera_spi(unsigned char data) {
	unsigned char ret;
	util_delay(5);
	LPC_GPIO0->MASKED_ACCESS[0x40] = 0;
	ret = spi_send_recv(data);
	util_delay(5);
	LPC_GPIO0->MASKED_ACCESS[0x40] = ~0;
	return ret;
}

void ov7670_init(void)
{
	/*SPI slave selct pin*/
	LPC_IOCON->PIO0_6 = 0x0;
	LPC_GPIO0->DIR |= 0x40;
	LPC_GPIO0->MASKED_ACCESS[0x40] = 0xFF;
	uart_printf("derp\n");
	
	/*Vsync*/
	LPC_IOCON->PIO0_1 = 0x0;
	LPC_GPIO0->DIR &= ~0x2;
	uart_printf("derp2\n");
	
	// I2C basic configuration at 15.2
	
	LPC_I2C->CONCLR		= 0x6C;		// Clearing: AA, SI, STA and I2EN
	
	LPC_SYSCON->SYSAHBCLKCTRL	|= (1<<5);	//Enables clock for I2C (3.5.14)
	LPC_SYSCON->PRESETCTRL		|= (0x2);	//I2C reset not allowed (3.5.2)

	LPC_IOCON->PIO0_4	= 0x1;		//FUNC: Select I2C function SCL (7.4.11)
	LPC_IOCON->PIO0_5	= 0x1;		//FUNC: Select I2C function SDA (7.4.12)
		
	/*	System clock at 48 MHz, and we want I2C in Fast-Mode (400 kHz).
	 * 	Thus our divider should be 120. HIGH and LOW should be of equal length,
	 * 	so each is set at 60 (15.7.5).
	 */
	LPC_I2C->SCLH	= 60;
	LPC_I2C->SCLL	= 60;
	
	LPC_I2C->CONSET		|= (1<<6);	//I2EN: I2C interface enabled (SDA and SCL not ignored) (15.7.1)
	uart_printf("derp3\n");
	
	
	/******************* CAM SETTINGS ************************/
	
	char temp = ov7670_read_reg(0x0C);		// default 0x00 ???????
	util_delay(5);
	ov7670_write_reg(0x0C, (temp|0b00001000));	// COM3: enable scaling
	
	temp = ov7670_read_reg(0x3E);			// default 0x00 ?????
	util_delay(5);
	ov7670_write_reg(0x3E, (temp|0b00001000));	// COM14: Manual scaling enabled
	uart_printf("derp4\n");
	
	// Scaling parameter where?!
	
	temp = ov7670_read_reg(0x12);					// default 0x00 ?????
	util_delay(5);
	ov7670_write_reg(0x12, ((temp & 0b01000000)|0b00001100));	// COM7: enable QVGA & RGB, ignore reserved
	uart_printf("derp5\n");

	
	temp = ov7670_read_reg(0x40);					// default 0xC0
	util_delay(5);
	ov7670_write_reg(0x40, ((temp & 0b00001111)|0b11010000));	// COM15: enable [00] to [FF], RGB565, ignore reserved
	uart_printf("derp6\n");
	
	temp = ov7670_read_reg(0x8C);			// default 0x00
	util_delay(5);
	ov7670_write_reg(0x8C, (temp & 0b11111101));	// RGB444: RGB565 effective only when RGB444[1] is low, ignore rest
	uart_printf("derp7\n");
}

void ov7670_write(char slave_sub_adr)
{
	int temp;
	
	/******************** START FLAG *************************/
	
	LPC_I2C->CONSET		|= (1<<5);	//Send START flag (S). I2C interface is set to master mode. (15.7.1)
	while ((LPC_I2C->STAT & 0xF8) != 0x08);	//Waiting for start-sent state (0x08). We ignore reserved and 0-bits.
		
	/********************** ADDRESS **************************/

	LPC_I2C->DAT		= SLAVE_ADR_W;	//Send slave address (SLA) 7 bits + data direction bit (W: 0, R: 1) (15.7.3)
	LPC_I2C->CONCLR		= 0x28;		//clears START flag (bit) and Serial Interrupt bit (SI) (15.7.6)
	while(1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x18)	break; 	// (SLA+W) sent, ACKed.
		else if (temp == 0x20)	break; 	// (SLA+W) sent, NACKed.
		else if (temp == 0x38)	break; 	// (SLA+W) sent, arbitration lost.
	}
		
	/******************** SUB-ADDRESS ************************/
	
	LPC_I2C->DAT		= slave_sub_adr;	//Send data (15.7.3)
	LPC_I2C->CONCLR		= (1<<3);		//Clear SI (15.7.6)
	while(1)					//Waiting for ACK from slave confirming received data (15.10.1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x28)	break;	// DATA (sub-adr) sent, ACKed.
		else if (temp == 0x30)	break;	// DATA (sub-adr) sent, NACKed.
		else if (temp == 0x38)	break;	// DATA (sub-adr) sent, arbitration lost.
	}

	/******************** STOP ************************/

	LPC_I2C->CONSET		|= (1<<4);	//Send STOP flag (P). Bit cleared when detected on bus. (15.7.1)
	LPC_I2C->CONCLR		= (1<<3);	//Clear SI (15.7.6)
}

void ov7670_write_reg(char slave_sub_adr, char data)
{
	int temp;
	
	/******************** START FLAG *************************/
	
	LPC_I2C->CONSET		|= (1<<5);	//Send START flag (S). I2C interface is set to master mode. (15.7.1)
	while ((LPC_I2C->STAT & 0xF8) != 0x08);	//Waiting for start-sent state (0x08). We ignore reserved and 0-bits.
	
	/********************** ADDRESS **************************/

	LPC_I2C->DAT		= SLAVE_ADR_W;	//Send slave address (SLA) 7 bits + data direction bit (W: 0, R: 1) (15.7.3)
	LPC_I2C->CONCLR		= 0x28;		//clears START flag (bit) and Serial Interrupt bit (SI) (15.7.6)
	while(1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x18)	break; 	// (SLA+W) sent, ACKed.
		else if (temp == 0x20)	break; 	// (SLA+W) sent, NACKed.
		else if (temp == 0x38)	break; 	// (SLA+W) sent, arbitration lost.
	}
	
	/******************** SUB-ADDRESS ************************/
	
	LPC_I2C->DAT		= slave_sub_adr;	//Send data (15.7.3)
	LPC_I2C->CONCLR		= (1<<3);		//Clear SI (15.7.6)
	while(1)					//Waiting for ACK from slave confirming received data (15.10.1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x28)	break;	// DATA (sub-adr) sent, ACKed.
		else if (temp == 0x30)	break;	// DATA (sub-adr) sent, NACKed.
		else if (temp == 0x38)	break;	// DATA (sub-adr) sent, arbitration lost.
	}
	
	/************************ DATA ***************************/
	
	LPC_I2C->DAT		= data;		//Send data (15.7.3)
	LPC_I2C->CONCLR		= (1<<3);	//Clear SI (15.7.6)
	while(1)				//Waiting for ACK from slave confirming received data (15.10.1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x28)	break;	// DATA sent, ACKed.
		else if (temp == 0x30)	break;	// DATA sent, NACKed.
		else if (temp == 0x38)	break;	// DATA sent, arbitration lost.
	}

	/*********************** STOP ***************************/

	LPC_I2C->CONSET		|= (1<<4);	//Send STOP flag (P). Bit cleared when detected on bus. (15.7.1)
	LPC_I2C->CONCLR		= (1<<3);	//Clear SI (15.7.6)
}

char ov7670_read(int slave_sub_adr)	// This should be used only after a write() (see read_reg()) or a write_reg()
{
	int temp;
	
	/******************** START FLAG *************************/
	
	LPC_I2C->CONSET		|= (1<<5);	//Send START flag (S). I2C interface is set to master mode. (15.7.1)
	while ((LPC_I2C->STAT & 0xF8) != 0x08);	//Waiting for start-sent state (0x08). We ignore reserved and 0-bits.
	
	/********************** ADDRESS **************************/

	LPC_I2C->DAT		= SLAVE_ADR_R;	//Send slave address (SLA) 7 bits + data direction bit (W: 0, R: 1) (15.7.3)
	LPC_I2C->CONCLR		= 0x28;		//clears START flag (bit) and Serial Interrupt bit (SI) (15.7.6)
	while(1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x40) 	break;	// (SLA+R) sent, ACKed.
		else if (temp == 0x48)	break;	// (SLA+R) sent, NACKed.
		else if (temp == 0x38)	break;	// Arbitration lost.
	}
	
	/***************** READ DATA **********************/
	
	LPC_I2C->CONCLR		= (1<<3);	//Clear SI (15.7.6)
	while(1)				//Waiting for ACK from slave confirming received data (15.10.1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x50)	break;	// DATA (sub-adr value) received, ACK sent.
		else if (temp == 0x58)	break;	// DATA (sub-adr value) received, NACK sent.
	}

	/******************** STOP ************************/

	LPC_I2C->CONSET		|= (1<<4);	//Send STOP flag (P). Bit cleared when detected on bus. (15.7.1)
	LPC_I2C->CONCLR		= (1<<3);	//Clear SI (15.7.6)
	
	return LPC_I2C->DAT;
}

char ov7670_read_reg(char slave_sub_adr)
{
	ov7670_write(slave_sub_adr);
	util_delay(5);				// A delay must be set or it will fail in between them (usually set to 5)
	return ov7670_read(slave_sub_adr);
}

void ov7670_fifo_reset() {
	camera_spi(0x0);
}

void ov7670_fifo_unreset() {
	packet_number = 0;
	camera_spi(0xFF);
	camera_spi(0xFF);
}

void ov7670_vsync_reset() {
	while(!LPC_GPIO0->MASKED_ACCESS[0x02]);
	ov7670_fifo_reset();
}

void ov7670_get_data_packet(unsigned char *buf) {
	int i;
	
	buf[0] = packet_number >> 8;
	buf[1] = packet_number;
	
	for(i = 2; i < 16; i++) {
		buf[i] = camera_spi(0xFF);
	}
	
	packet_number++;
}


void ov7670_test() {
	unsigned char buff[16];

	ov7670_fifo_reset();
	util_delay(10000);
	ov7670_fifo_unreset();
	util_delay(10000);
	for (;;) {
		ov7670_get_data_packet(buff);
		uart_send_raw(buff, 16);
	}

	return;
}
