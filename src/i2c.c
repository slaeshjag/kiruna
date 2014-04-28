#include "system/LPC11xx.h"
#include "uart.h"


#define SLAVE_ADR_W	0x42
#define SLAVE_ADR_R	0x43

#define SLAVE_SUB_ADR	0x12


void i2c_init(void)
{
	// I2C basic configuration at 15.2
	
	// Initialize clock pin (SCL)
	LPC_IOCON->PIO0_4		&= ~(0x307);	//I2CMODE: Standard mode/Fast-mode I2C. Also 0 FUNC bits (7.4.11)
	LPC_IOCON->PIO0_4		|= 0x1;		//FUNC: Select I2C function SCL (7.4.11)
	LPC_SYSCON->SYSAHBCLKCTRL	|= (1<<5);	//Enables clock for I2C (3.5.14)
	
	/*	System clock at 48 MHz, and we want I2C in Fast-Mode (400 kHz).
	 * 	Thus our divider should be 120. HIGH and LOW should be of equal length,
	 * 	so each is set at 60 (15.7.5).
	 */
	LPC_I2C->SCLH	= 60;
	LPC_I2C->SCLL	= 60;

	
	// Initialize data pin (SDA)
	LPC_IOCON->PIO0_5	&= ~(0x303);	//I2CMODE: Standard mode/Fast-mode I2C. Also zero some FUNC bits (7.4.12)
	LPC_IOCON->PIO0_5	|= 0x1;		//FUNC: Select I2C function SDA (7.4.12)	
	
	
	LPC_SYSCON->PRESETCTRL		|= (0x2);	//I2C reset not allowed (3.5.2)
	LPC_I2C->CONSET			|= (1<<6);	//I2EN: I2C interface enabled (15.7.1)
	
	/*When I2EN is “0”, the SDA and SCL input signals are ignored, the I2C block is in the “not
	addressed” slave state, and the STO bit is forced to “0”.*/
	
	/* Master Transmit mode: (15.8.1)
	The STA, STO and SI bits must be 0. The SI Bit is cleared by writing 1 to the
	SIC bit in the CONCLR register. THe STA bit should be cleared after writing the 
	slave address.
	*/

}

void w_2_phases(int slave_adr_w, int slave_sub_adr)
{
	int temp;
	
	/******************** START FLAG *************************/
	
	LPC_I2C->CONSET		|= (1<<5);	//Send START flag (S). I2C interface is set to master mode. (15.7.1)
	while ((LPC_I2C->STAT & 0xF8) != 0x08);	//Waiting for start-sent state (0x08). We ignore reserved and 0-bits.
	
	/********************** ADDRESS **************************/

	LPC_I2C->DAT		= slave_adr_w;	//Send slave address (SLA) 7 bits + data direction bit (W: 0, R: 1) (15.7.3)
	LPC_I2C->CONCLR		= 0x28;		//clears START flag (bit) and Serial Interrupt bit (SI) (15.7.6)
	while(1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x18)
		{
			uart_printf("(SLA+W) sent, ACKed.\n");
			break;
		}
		else if (temp == 0x20)
		{
			uart_printf("(SLA+W) sent, NACKed.\n");
			break;
		}
		else if (temp == 0x38)
		{
			uart_printf("(SLA+W) sent, arbitration lost.\n");
			break;
		}
	}
	
	/******************** SUB-ADDRESS ************************/
	
	LPC_I2C->DAT		= slave_sub_adr;	//Send data (15.7.3)
	LPC_I2C->CONCLR		= (1<<3);		//Clear SI (15.7.6)
	while(1)					//Waiting for ACK from slave confirming received data (15.10.1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x28)
		{
			uart_printf("DATA (sub-adr) sent, ACKed.\n");
			break;
		}
		else if (temp == 0x30)
		{
			uart_printf("DATA (sub-adr) sent, NACKed.\n");
			break;
		}
		else if (temp == 0x38)
		{
			uart_printf("DATA (sub-adr) sent, arbitration lost.\n");
			break;
		}
	}

}

void i2c_send(void)
{
	int temp;

	LPC_I2C->CONSET		|= (1<<5);	//Send START flag (S). I2C interface is set to master mode. (15.7.1)
	while ((LPC_I2C->STAT & 0xF8) != 0x08);	//Waiting for start-sent state (0x08). We ignore reserved and 0-bits.
	
	LPC_I2C->DAT		= SLAVE_ADR_W;	//Send slave address (SLA) 7 bits + data direction bit (W: 0, R: 1) (15.7.3)
	LPC_I2C->CONCLR		= 0x28;		//clears START flag (bit) and Serial Interrupt bit (SI) (15.7.6)
	while(1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x18)
		{
			uart_printf("(SLA+W) sent, ACKed.\n");
			break;
		}
		else if (temp == 0x20)
		{
			uart_printf("(SLA+W) sent, NACKed.\n");
			break;
		}
		else if (temp == 0x38)
		{
			uart_printf("(SLA+W) sent, arbitration lost.\n");
			break;
		}
	}
	
	/*********************************************************/
	
	LPC_I2C->DAT		= SLAVE_SUB_ADR;	//Send data (15.7.3)
	LPC_I2C->CONCLR		= (1<<3);		//Clear SI (15.7.6)
	while(1)					//Waiting for ACK from slave confirming received data (15.10.1)
	{
		temp = (LPC_I2C->STAT & 0xF8);
		if (temp == 0x28)
		{
			uart_printf("DATA (sub-adr) sent, ACKed.\n");
			break;
		}
		else if (temp == 0x30)
		{
			uart_printf("DATA (sub-adr) sent, NACKed.\n");
			break;
		}
		else if (temp == 0x38)
		{
			uart_printf("DATA (sub-adr) sent, arbitration lost.\n");
			break;
		}
	}
	
	/********** Repeat following to send all data ************/
	
	LPC_I2C->DAT		= 0x06;		//Send data (15.7.3)
	LPC_I2C->CONCLR		= (1<<3);	//Clear SI (15.7.6)
	while(LPC_I2C->STAT & 0xF8) != 0x28);	//Waiting for ACK from slave confirming received data (15.10.1)
	
	/*********************************************************/
	
	LPC_I2C->CONSET		|= (1<<4);	//Send STOP flag (P). Bit cleared when detected on bus. (15.7.1)
	LPC_I2C->CONCLR		= (1<<3);	//Clear SI (15.7.6)
}


