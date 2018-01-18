/* 
* NRF24L01.h
*
* Created: 15.01.2018 10:26:40
* Author: USER
*/


#ifndef __NRF24L01_H__
#define __NRF24L01_H__

#include "iface_nrf24l01.h"
#include "hw_defines.h"
/*
* in "hw_defines.h" including spi functions
void SPI_Init();//Инициализация;
uint8_t spi_write(uint8_t byte); //отправка байта;
uint8_t spi_read();//прием байта;
 and 
* CS_INIT
* CS_ON	
* CS_OFF	
* CE_INIT	
* CE_ON	
* CE_OFF	
* HW_DELAY_US(x) 
* macros
*/

uint8_t NRF24L01_WriteReg(uint8_t address, uint8_t data);
void NRF24L01_WriteRegisterMulti(uint8_t address, const uint8_t data[], uint8_t len);

void NRF24L01_Initialize();
void NRF24L01_HW_init();
uint8_t NRF24L01_FlushTx();
uint8_t NRF24L01_FlushRx();
uint8_t Strobe(uint8_t state);
uint8_t NRF24L01_WritePayload(uint8_t *data, uint8_t length);
uint8_t NRF24L01_ReadPayload(uint8_t *data, uint8_t length);
uint8_t NRF24L01_ReadReg(uint8_t reg);
uint8_t NRF24L01_Activate(uint8_t code);
void NRF24L01_SetTxRxMode(enum TXRX_State mode);
uint8_t NRF24L01_Reset();
uint8_t NRF24L01_SetPower(enum TX_Power power);
uint8_t NRF24L01_SetBitrate(uint8_t bitrate);

#endif //__NRF24L01_H__
