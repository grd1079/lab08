/*
 * File:        uart.c
 * Purpose:     Provide UART routines for serial IO
 *
 * Notes:		
 *
 */

//#include "MK64F12.h"
#include "uart.h"
#define BAUD_RATE 9600      //default baud rate 
#define SYS_CLOCK 20485760 //default system clock (see DEFAULT_SYSTEM_CLOCK  in system_MK64F12.c)

void uart_init()
{
//define variables for baud rate and baud rate fine adjust
uint16_t ubd, brfa;

//Enable clock for UART
	SIM_SCGC4 |= (SIM_SCGC4_UART0_MASK|SIM_SCGC4_UART3_MASK);		//UART0 & UART3
	
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;		//RX & TX
 
//Configure the port control register to alternative 3 (which is UART mode for K64)
	PORTB_PCR16 |= PORT_PCR_MUX(3);		// UART0_RX
	PORTB_PCR17 |= PORT_PCR_MUX(3);		// UART0_TX
	
	PORTB_PCR10 |= PORT_PCR_MUX(3);		// UART3_RX
	PORTB_PCR11 |= PORT_PCR_MUX(3);		// UART3_TX

/*Configure the UART for establishing serial communication*/	

//Disable transmitter and receiver until proper settings are chosen for the UART module
	UART0_C2 &= ~UART_C2_TE_MASK;	// UART0 
	UART0_C2 &= ~UART_C2_RE_MASK;
	
	UART3_C2 &= ~UART_C2_TE_MASK; // UART3
	UART3_C2 &= ~UART_C2_RE_MASK;

//Select default transmission/reception settings for serial communication of UART by clearing the control register 1
	UART0_C1 &= 0x00;
	
	UART3_C1 &= 0x00;

//UART Baud rate is calculated by: baud rate = UART module clock / (16 × (SBR[12:0] + BRFD))
//13 bits of SBR are shared by the 8 bits of UART0_BDL and the lower 5 bits of UART0_BDH 
//BRFD is dependent on BRFA, refer Table 52-234 in K64 reference manual
//BRFA is defined by the lower 4 bits of control register, UART0_C4 

//calculate baud rate settings: ubd = UART module clock/16* baud rate
	ubd = (uint16_t)((SYS_CLOCK)/(BAUD_RATE * 16));  

//clear SBR bits of BDH
	UART0_BDH &= ~UART_BDH_SBR_MASK;	
	
	UART3_BDH &= ~UART_BDH_SBR_MASK;
	
//distribute this ubd in BDH and BDL	
	UART0_BDH |= UART_BDH_SBR(ubd >> 8);	//shift over 8 bits to make sure the first 8 bits are correctly being written to BDH
	UART0_BDL |= UART_BDL_SBR(ubd);

	UART3_BDH |= UART_BDH_SBR(ubd >> 8);	//shift over 8 bits to make sure the first 8 bits are correctly being written to BDH
	UART3_BDL |= UART_BDL_SBR(ubd);
//BRFD = (1/32)*BRFA 
//make the baud rate closer to the desired value by using BRFA
	brfa = (((SYS_CLOCK*32)/(BAUD_RATE * 16)) - (ubd * 32));

//write the value of brfa in UART0_C4
	UART0_C4 |= UART_C4_BRFA(brfa);
	
	UART3_C4 |= UART_C4_BRFA(brfa);
	
//Enable transmitter and receiver of UART
	UART0_C2 |= UART_C2_TE_MASK;
	UART0_C2 |= UART_C2_RE_MASK;

	UART3_C2 |= UART_C2_TE_MASK;
	UART3_C2 |= UART_C2_RE_MASK;
}

void LED_Init(void){
	// Enable clocks on Ports B and E for LED timing
	SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK;
	//SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	// Configure the Signal Multiplexer for GPIO
  PORTB_PCR22 = PORT_PCR_MUX(1);	//Red
	PORTB_PCR21 = PORT_PCR_MUX(1);	//Blue
	//PORTE_PCR26 = PORT_PCR_MUX(1);	//Green

	// Switch the GPIO pins to output mode
	GPIOB_PDDR |= RED_LED | BLUE_LED;
	//GPIOE_PDDR |= GREEN_LED;

	// Turn off the LEDs
  GPIOB_PSOR |= RED_LED | BLUE_LED;
	//GPIOE_PSOR |= GREEN_LED;
}

uint8_t uart0_getchar()
{
/* Wait until there is space for more data in the receiver buffer*/

	while( (UART_S1_RDRF_MASK&UART0_S1) == 0 )				// Get the RDRF bit
	{	
		continue;
	}
	/* Return the 8-bit data from the receiver */
	return UART0_D;
}

void uart0_putchar(char ch)
{
/* Wait until transmission of previous bit is complete */

	while( (UART_S1_TDRE_MASK&UART0_S1) == 0 )	// Get the TDRE bit
	{
		continue;
	}
	UART0_D = ch;
}


void uart0_put(char *ptr_str){
	/*use putchar to print string*/
	while(*ptr_str)
	{
		uart0_putchar(*ptr_str++);
	}
}
	uint8_t uart3_getchar()
{
/* Wait until there is space for more data in the receiver buffer*/

	while( (UART_S1_RDRF_MASK&UART3_S1) == 0 )				// Get the RDRF bit
	{	
		continue;
	}
	/* Return the 8-bit data from the receiver */
	return UART3_D;
}

void uart3_putchar(char ch)
{
/* Wait until transmission of previous bit is complete */

	while( (UART_S1_TDRE_MASK&UART3_S1) == 0 )	// Get the TDRE bit
	{
		continue;
	}
	UART3_D = ch;
}


void uart3_put(char *ptr_str){
	/*use putchar to print string*/
	while(*ptr_str)
	{		
		uart3_putchar(*ptr_str++);
	}
}
void uart0_putNumU(int n){
//	while(i)
//	{
//		uart0_putchar(i++);
//	}
	if (n < 0) {
        uart0_putchar('-');
        n = -n;
    }
    if (n / 10 != 0)
        uart0_putNumU(n / 10);
    uart0_putchar((n % 10) + '0');
}

