//This header file has all header functions to control SPI peripheral

#ifndef SPI_H
#define SPI_H

#include <stdint.h>
#include "MKL25Z4.h"


void SPI0_Initialize(void);

uint8_t SPI0_Write1Byte(uint8_t data);

uint8_t SPI0_WriteNbytes(uint16_t num, uint8_t datas[]);

void MAX7221_Configure(void);

void MAX7221_Write(uint8_t reg, uint8_t data);


#endif /* SPI_H */
