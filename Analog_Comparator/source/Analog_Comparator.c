/*This project is an example of how to configrue Analog Comparator in three cases
 * First case is in Continuos Mode
 * Second case is in Filter Mode
 * Third case is in Widow Mode
 */
#include <stdio.h>
#include <stdint.h>
#include "MKL25Z4.h"

#define _Core_Clock 48000000

void MCG_ClockInitialize(void);

void SysTick_Initialize(void);

void AnalogComparator_Initialize(void);

void GPIO_Initialize(void);

void Delay_ms(uint16_t time);



int main(void)
{
	MCG_ClockInitialize();
	SysTick_Initialize();
	GPIO_Initialize();
	AnalogComparator_Initialize();



	while(1)
	{
		if(CMP0 -> SCR & 0x01)
			GPIOB -> PCOR |= (uint32_t)(1 << 18);
		else
			GPIOB -> PSOR |= (uint32_t)(1 << 18);


	}

    return 0;
}


void MCG_ClockInitialize(void)
{
	//switch from FEI to FBE

	//Enable External Osc
	OSC0 -> CR |= 0x80;

	//select High Frequency, Low Gain, External Osc requested and Fast internal Clock as FLL reference
	MCG -> C2 |= 0x15;
	MCG -> C1 |= 0x90; //switch to External Osc and get 31.25 kHz to FLL
	while(!(MCG -> S & 0x1B));

	//switch from FBE to PBE
	//enable PLL and divide external clock by 4
	MCG -> C5 |= 0x43;
	//select PLL output as MCG source
	MCG -> C6 |= 0x40;
	while(!(MCG -> S & 0x7B));

	//switch from PBE to PEE
	MCG -> C1 &= ~(1 << 8);
	while(!(MCG -> S & 0x7F));

}


void SysTick_Initialize(void)
{
	//turn on SysTick timer and load to get overflows each 1 ms
	SysTick -> CTRL |= (uint32_t)(0x04);
	SysTick -> CTRL &= ~(uint32_t)(1 << 16);
	SysTick -> LOAD = (uint32_t)(_Core_Clock/1000);
	SysTick -> CTRL |= (uint32_t)(0x01);

}


void AnalogComparator_Initialize(void)
{
	//Enable CMP clock
	SIM -> SCGC4 |= (uint32_t)(1 << 19);

	//configure CMP as
	CMP0 -> DACCR = 39; //select DAC output to 2.0625 v
	CMP0 -> MUXCR |= 0x07; //select IN0 Plus Input and DAC as Negative Input
	CMP0 -> DACCR |= (0x3 << 6); //select VDD as reference and enable DAC
	CMP0 -> CR1 |= 0x03;
	CMP0 -> SCR |= 0x06; //clear Falling and Rising edge detections

}


void GPIO_Initialize(void)
{
	//configure pin PTB18 and PTB19 as digital outputs, PTC6 as IN0 input and PTE0 as CMP0_Out
	SIM -> SCGC5 |= (uint32_t)(1 << 10) | (uint32_t)(1 << 11);

	PORTB -> PCR[18] |= (uint32_t)(1 << 8);
	PORTB -> PCR[19] |= (uint32_t)(1 << 8);
	//PORTE -> PCR[0] |= (uint32_t)(0x5 << 8);
	GPIOB -> PSOR |= (uint32_t)(0x03 << 18); //set on High PTB18 and PTB19
	GPIOB -> PDDR |= (uint32_t)(0x3 << 18); //set up PTB18 and PTB19 as outputs

}


void Delay_ms(uint16_t time)
{
	SysTick -> VAL = (uint32_t)(0x00);

	for(uint16_t i = 0; i < time; i++)
	{
		while(!(SysTick -> CTRL & (uint32_t)(1 << 16)));

		SysTick -> CTRL &= ~(uint32_t)(1 << 16);

	}

}
