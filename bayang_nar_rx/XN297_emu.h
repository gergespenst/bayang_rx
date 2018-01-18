/* 
* XN297_emu.h
*
* Created: 15.01.2018 11:01:37
* Author: USER
*/


#ifndef __XN297_EMU_H__
#define __XN297_EMU_H__

#include "NRF24L01.h"
#include <string.h>

void XN297_SetTXAddr(const uint8_t* addr, uint8_t len);
void XN297_SetRXAddr(const uint8_t* addr, uint8_t len);
void XN297_Configure(uint8_t flags);
uint8_t XN297_WritePayload(uint8_t* msg, uint8_t len);
uint8_t XN297_ReadPayload(uint8_t* msg, uint8_t len);

#endif //__XN297_EMU_H__
