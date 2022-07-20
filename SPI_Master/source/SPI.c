//This source file has all declaration functions to control SPI peripheral

#include "SPI.h"


void SPI0_Initialize(void)
{
	SIM -> SCGC4 |= (uint32_t)(1 << 22);

	//select Master Mode and Clock Polarity Active Low, CHPA first edge and SS by hardware
	SPI0 -> C1 |= 0x1E;

	//enable MODFEN bit and MOSI and MISO pins to send datas
	SPI0 -> C2 |= 0x10;

	//configure baud rate to 1 Mbps
	SPI0 -> BR = 0x51;

	//enable SPI0
	SPI0 -> C1 |= 0x40;

}


uint8_t SPI0_Write1Byte(uint8_t data)
{
	uint8_t status = 0x00;

	while(!(SPI0 -> S & 0x20)); //wait until buffer is empty

	SPI0 -> D = data;

	while(!(SPI0 -> S & 0x20))
	{
		if(SPI0 -> S & 0x10) //if detect a fault in SS
		{
			status = 0x01;
			break;

		}

	}

	return status;

}


uint8_t SPI0_WriteNbytes(uint16_t num, uint8_t datas[])
{
	uint8_t status = 0x00;

	while(!(SPI0 -> S & 0x20));

	for(uint16_t i = 0; i < num; i++)
	{
		SPI0 -> D = datas[i];

		while(!(SPI0 -> S & 0x20))
		{
			if(SPI0 -> S & 0x10)
			{
				status = 0x01;
				break;

			}

		}

	}

	return status;

}


void MAX7221_Configure(void)
{
	uint8_t datos[10] = {0x0B, 0x07, 0x0A, 0x01, 0x09, 0x01, 0x0C, 0xFF, 0x0F, 0x01};

	for(uint8_t i = 0; i < 10; i++)
	{
		SPI0_Write1Byte(datos[i]);
		i++;
		SPI0_Write1Byte(datos[i]);
		Delay_us(10);
	}

}


void MAX7221_Write(uint8_t reg, uint8_t data)
{
	uint8_t datos[2];

	datos[0] = reg;
	datos[1] = data;

	SPI0_WriteNbytes(2, datos);

}
