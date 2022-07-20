//This project is an example of how to configure SPI module in Master Half Duplex Mode
#include <stdio.h>
#include "MKL25Z4.h"
#include "SPI.h"

#define _System_Clock 48000000

void MCG_ClockInitialize(void);

void SysTick_Initialize(void);

void Delay_us(uint16_t time);

void GPIO_Initialize(void);




int main(void)
{
	MCG_ClockInitialize();
	SysTick_Initialize();
	GPIO_Initialize();
	SPI0_Initialize();



    while(1)
    {
    	MAX7221_Configure();
    	Delay_us(1000);

    }


    return 0;

}


void MCG_ClockInitialize(void)
{
	//switch from FEI to FBE mode
	OSC0 -> CR |= 0x80;
	MCG -> C2 |= 0x15; //select High frequency, low Gain, request external osc and Fast Internal Clock
	MCG -> C1 &= (1 << 2); //select External Osc for FLL
	MCG -> C1 |= (uint32_t)(0x9A); //select external Osc, divide Osc by 256 and enable MCGIRCLK
	while(!(MCG -> S & 0x02));
	while(MCG -> S & 0x10 );
	while(!(MCG -> S & 0x01));
	while(!(MCG -> S & 0x08));

	//switch from FBE to PBE
	MCG -> C5 |= 0x43; //enable PLL clock and divide Osc by 4
	MCG -> C6 |= 0x60; //select PLL as MCGOUT and external clock failure
	while(!(MCG -> S & 0x40));
	while(!(MCG -> S & 0x20));

	//switch from PBE  to PEE
	MCG -> C1 &= ~(1 << 7);
	while(!(MCG -> S & 0x0C));

}



void SysTick_Initialize(void)
{
	SysTick -> CTRL |= (uint32_t)(0x4); //select system clock as clock source
	SysTick -> CTRL &= ~(1 << 16);
	SysTick -> LOAD = _System_Clock/1000000; //load to oveflows each 1 us
	SysTick -> CTRL |= (uint32_t)(0x1); //turn on SysTick

}


void Delay_us(uint16_t time)
{
	SysTick -> VAL = (uint32_t)(0x0);

	for(uint16_t i = 0; i < time; i++)
	{
		while(!(SysTick -> CTRL & (1 << 16)));

		SysTick -> CTRL &= ~(1 << 16);

	}

}


void GPIO_Initialize(void)
{
	//enable PORTB and PORTC clock
	SIM -> SCGC5 |= (uint32_t)(0x3 << 10);

	//configure pin PTB19 as digital output
	PORTB -> PCR[19] |= (uint32_t)(1 << 8);
	GPIOB -> PSOR |= (uint32_t)(1 << 19);
	GPIOB -> PDDR |= (uint32_t)(1 << 19);

	//configure SPI pins, PTC4, PTC5, PTC6 and PTC7 (SS, SCK, MOSI and MISO)
	PORTC -> PCR[4] |= (uint32_t)(0x2 << 8);
	PORTC -> PCR[5] |= (uint32_t)(0x2 << 8);
	PORTC -> PCR[6] |= (uint32_t)(0x2 << 8);
	PORTC -> PCR[7] |= (uint32_t)(0x2 << 8);

}
