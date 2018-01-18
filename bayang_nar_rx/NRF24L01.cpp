/* 
* NRF24L01.cpp
*
* Created: 15.01.2018 10:26:40
* Author: USER
*/


#include "NRF24L01.h"

static uint8_t rf_setup;


uint8_t NRF24L01_WriteReg(uint8_t address, uint8_t data)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    CS_OFF;
    spi_write(address | W_REGISTER);
	__asm__ __volatile__("nop");
	spi_write(data);
    CS_ON;
	}
    return 1;
}

void NRF24L01_WriteRegisterMulti(uint8_t address, const uint8_t data[], uint8_t len)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    HW_DELAY_US(5);
    CS_OFF;
    spi_write(address | W_REGISTER);
    for(uint8_t i=0;i<len;i++){
        spi_write(data[i]);
	}
    CS_ON;
    HW_DELAY_US(5);
	}
}

void NRF24L01_Initialize()
{	
	SPI_Init();
	CS_INIT;
	CE_INIT;
	CE_ON;
	CS_ON;
	NRF24L01_Reset();
    rf_setup = 0x0F;
}
void NRF24L01_HW_init(){
		SPI_Init();
		CS_INIT;
		CE_INIT;
		CE_ON;
		CS_ON;
		NRF24L01_Reset();
}

uint8_t NRF24L01_FlushTx()
{
	
    return Strobe(FLUSH_TX);
}

uint8_t NRF24L01_FlushRx()
{
    return Strobe(FLUSH_RX);
}

uint8_t Strobe(uint8_t state)
{uint8_t result;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    
    CS_OFF;
    result = spi_write(state);
    CS_ON;
}
    return result;
}

uint8_t NRF24L01_WritePayload(uint8_t *data, uint8_t length)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    CE_OFF;
    CS_OFF;
    spi_write(W_TX_PAYLOAD); 
    for(uint8_t i=0; i<length; i++)
        spi_write(data[i]);
    CS_ON;
    CE_ON; // transmit
	}
    return 1;
}

uint8_t NRF24L01_ReadPayload(uint8_t *data, uint8_t length)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    uint8_t i;
    CS_OFF;
    spi_write(R_RX_PAYLOAD); // Read RX payload
    for (i=0;i<length;i++) {
        data[i]=spi_write(0xFF);
    }
    CS_ON;
	}
    return 1;
}

uint8_t NRF24L01_ReadReg(uint8_t reg)
{	uint8_t data;
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    CS_OFF;
	spi_write(reg);
	__asm__ __volatile__("nop");
     data = spi_write(0xFF);
    CS_ON;
	}
    return data;
}

uint8_t NRF24L01_Activate(uint8_t code)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    CS_OFF;
    spi_write(ACTIVATE);
    spi_write(code);
    CS_ON;
	}
    return 1;
}

void NRF24L01_SetTxRxMode(enum TXRX_State mode)
{
	ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    if(mode == TX_EN) {
        CE_OFF;
        NRF24L01_WriteReg(NRF24L01_07_STATUS, (1 << NRF24L01_07_RX_DR)    //reset the flag(s)
                                            | (1 << NRF24L01_07_TX_DS)
                                            | (1 << NRF24L01_07_MAX_RT));
        NRF24L01_WriteReg(NRF24L01_00_CONFIG, (1 << NRF24L01_00_EN_CRC)   // switch to TX mode
                                            | (1 << NRF24L01_00_CRCO)
                                            | (1 << NRF24L01_00_PWR_UP));
        HW_DELAY_US(130);
        CE_ON;
    } else if (mode == RX_EN) {
        CE_OFF;
        NRF24L01_WriteReg(NRF24L01_07_STATUS, 0x70);        // reset the flag(s)
        NRF24L01_WriteReg(NRF24L01_00_CONFIG, 0x0F);        // switch to RX mode
        NRF24L01_WriteReg(NRF24L01_07_STATUS, (1 << NRF24L01_07_RX_DR)    //reset the flag(s)
                                            | (1 << NRF24L01_07_TX_DS)
                                            | (1 << NRF24L01_07_MAX_RT));
        NRF24L01_WriteReg(NRF24L01_00_CONFIG, (1 << NRF24L01_00_EN_CRC)   // switch to RX mode
                                            | (1 << NRF24L01_00_CRCO)
                                            | (1 << NRF24L01_00_PWR_UP)
                                            | (1 << NRF24L01_00_PRIM_RX));
        HW_DELAY_US(130);
        CE_ON;
    } else {
        NRF24L01_WriteReg(NRF24L01_00_CONFIG, (1 << NRF24L01_00_EN_CRC)); //PowerDown
        CE_OFF;
    }
	}
}

uint8_t NRF24L01_Reset()
{
    NRF24L01_FlushTx();
    NRF24L01_FlushRx();
    uint8_t status1 = Strobe(0xFF); // NOP
    uint8_t status2 = NRF24L01_ReadReg(0x07);
    NRF24L01_SetTxRxMode(TXRX_OFF);
    return (status1 == status2 && (status1 & 0x0f) == 0x0e);
}

uint8_t NRF24L01_SetPower(enum TX_Power power)
{
    rf_setup = (rf_setup & 0xF9) | ((power & 0x03) << 1);
    return NRF24L01_WriteReg(NRF24L01_06_RF_SETUP, rf_setup);
}

uint8_t NRF24L01_SetBitrate(uint8_t bitrate)
{
    // Note that bitrate 250kbps (and bit RF_DR_LOW) is valid only
    // for nRF24L01+. There is no way to programmatically tell it from
    // older version, nRF24L01, but the older is practically phased out
    // by Nordic, so we assume that we deal with with modern version.

    // Bit 0 goes to RF_DR_HIGH, bit 1 - to RF_DR_LOW
    rf_setup = (rf_setup & 0xD7) | ((bitrate & 0x02) << 4) | ((bitrate & 0x01) << 3);
    return NRF24L01_WriteReg(NRF24L01_06_RF_SETUP, rf_setup);
}
