/* 
* spi_drv.cpp
*
* Created: 15.01.2018 10:29:40
* Author: USER
*/


#include "spi_drv.h"
#include <util/atomic.h>

void SPI_Init()//Инициализация
{

     DDR_SPI &= ~( (1<<SPI_MOSI) | (1<<SPI_MISO)  | (1<<SPI_SCK) );
     DDR_SPI |= ( (1<<SPI_MOSI)  | (1<<SPI_SCK) | (1<<SPI_SS) );
 	
     SPCR = ((1<<SPE)|               // SPI Enable
     (0<<SPIE)|              // SPI Interupt Disable
     (0<<DORD)|              // Data Order (0:MSB first / 1:LSB first)
     (1<<MSTR)|              // Master/Slave select
     (0<<SPR1)|(0<<SPR0)|    // SPI Clock Rate
     (0<<CPOL)|              // Clock Polarity (0:SCK low / 1:SCK hi when idle)
     (0<<CPHA));             // Clock Phase (0:leading / 1:trailing edge sampling)
 
     SPSR = (0<<SPI2X); // double spi speed b
}

uint8_t spi_write(uint8_t byte) //отправка байта
{
	//ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    SPDR = byte;
    while((SPSR & (1<<SPIF)) == 0);
	//}
    return SPDR;
	
}

// read one byte from MISO
uint8_t spi_read()//прием байта
{
        SPDR = 0xFF;
        while((SPSR & (1<<SPIF)) == 0);
        return SPDR;
}



