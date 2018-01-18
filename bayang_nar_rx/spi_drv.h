/* 
* spi_drv.h
*
* Created: 15.01.2018 10:29:40
* Author: USER
* ��������� ��������� ������� spi
*/


#ifndef __SPI_DRV_H__
#define __SPI_DRV_H__
#include <inttypes.h>
#include <avr/io.h>

#define PORT_SPI  PORTB
#define DDR_SPI   DDRB
#define SPI_MISO  PB4
#define SPI_MOSI  PB3
#define SPI_SCK   PB5
#define SPI_SS	PB2


void SPI_Init();//�������������;
uint8_t spi_write(uint8_t byte); //�������� �����;
uint8_t spi_read();//����� �����;

#endif //__SPI_DRV_H__
