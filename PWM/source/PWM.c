/*On this project we configure the three TPM modules to work with different frequency
 * and generate two signals per channel with different duty cycles*/
#include <stdio.h>
#include "MKL25Z4.h"

//configure MCG clock to generate 48 MHz to core clock
void MCG_ClockConfigure(void);

//configure pin PTB19 as output with Push-pull
void GPIO_Initialize(void);

//configure TPM0, TPM1 and TPM2 to operate with 1k, 5k and 10k
//also each channel must generate two PWM signals with 20, 50 and 75 Duty Cycle
void TPM_Initialize(void);




int main(void)
{
	MCG_ClockConfigure();
	GPIO_Initialize();
	TPM_Initialize();

    while(1)
    {



    }

    return 0;

}


void MCG_ClockConfigure(void)
{
	//switch from FEI Mode to FEE mode
	OSC0 -> CR |= 0x80;
	MCG -> C2 |= 0x14;
	MCG -> C1 &= ~(0x4); //select external oscillator as FLL's reference clock source
	MCG -> C1 |= 0x9A; //get 31.25 KHz for FLL, enable MCGIRCLK and select External Oscillator

	while(!(MCG -> S & 0x0A)); //external oscillator reference selected and OSC is ready

	//switch from FEE to PBE
	MCG -> C5 |= 0x43; //turn on PLLCKL and select external ocillator prescaler by 4 to get 2 MHz
	MCG -> C6 |= 0x40; //select PLL as main clock source and multiply prescaler frequency by 24 to get 48 MHz

	while(!(MCG -> S & 0x6A)); //wait upon PLL is ready and is the source of PLLS

	//switch from PBE to PEE
	MCG -> C1 &= ~(0x80); //switch to PLL as main clock source

	while(!(MCG -> S & 0x6E)); //wait upon Output frequency is PLL

}


void GPIO_Initialize(void)
{
	//configure pin PTB19 as GPIO
	SIM -> SCGC5 |= (uint32_t)(1 << 10);

	PORTB -> PCR[19] |= (uint32_t)(1 << 8);
	GPIOB -> PSOR |= (uint32_t)(1 << 19);
	GPIOB -> PDDR |= (uint32_t)(1 << 19);

}


void TPM_Initialize(void)
{
	//configure pins PTC1 and PTC2 to operate as TPM0 channels (CH0 and CH1)
	//configure pins PTA1 and PTA2 to operate as TPM2 channels (CH0 and CH1)
	SIM -> SCGC5 |= (uint32_t)(1 << 9);
	SIM -> SCGC5 |= (uint32_t)(1 << 11);

	PORTA -> PCR[1] |= (uint32_t)(0x3 << 8); //select ALT3
	PORTA -> PCR[2] |= (uint32_t)(0x3 << 8); //select ALT3
	PORTC -> PCR[1] |= (uint32_t)(0x4 << 8); //select ALT4
	PORTC -> PCR[2] |= (uint32_t)(0x4 << 8); //select ALT4

	SIM -> SCGC6 |= (0x7 << 24); //enable TPM0
	SIM -> SOPT2 |= (0x0101 << 16); //select MCGPLLCLK divided by two

	TPM0 -> CNT = (uint32_t)(0x0); //clear CNT
	TPM0 -> MOD = (uint32_t)(6001); //period of TPM0 is 1 KHz
	TPM0 -> CONTROLS[0].CnSC|= 0x28; //select Operation Mode for CH0 PWM
	TPM0 -> CONTROLS[0].CnV = 3000; //select 50% duty cycle for CH0
	TPM0 -> CONTROLS[1].CnSC|= 0x28;
	TPM0 -> CONTROLS[1].CnV = 4500; //select 75% duty cycle for CH1

	TPM2 -> CNT = (uint32_t)(0x0);
	TPM2 -> MOD = (uint32_t)(1200);
	TPM2 -> CONTROLS[0].CnSC |= 0x28;
	TPM2 -> CONTROLS[1].CnSC |= 0x28;
	TPM2 -> CONTROLS[0].CnV = 600;
	TPM2 -> CONTROLS[1].CnV = 900;

	TPM0 -> SC |= 0x0A; //select LPTPM clock and prescaler of 4
	TPM2 -> SC |= 0x0A; //select LPTPM clock and prescaler of 4

}
