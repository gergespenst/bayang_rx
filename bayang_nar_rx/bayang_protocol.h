/* 
* bayang_protocol.h
*
* Created: 15.01.2018 11:07:09
* Author: USER
*/


#ifndef __BAYANG_PROTOCOL_H__
#define __BAYANG_PROTOCOL_H__

#include "hw_defines.h"
#include "NRF24L01.h"
#include "XN297_emu.h"

#define BAYANG_BIND_COUNT       1000
#define BAYANG_PACKET_PERIOD    1000
#define BAYANG_INITIAL_WAIT    50
#define BAYANG_PACKET_SIZE      15
#define BAYANG_RF_NUM_CHANNELS  4
#define BAYANG_RF_BIND_CHANNEL  0
#define BAYANG_ADDRESS_LENGTH   5

#define AUXNUMBER 5
#define CH_FLIP 0
#define CH_EXPERT 1
#define CH_HEADFREE 2
#define CH_RTH 3
#define CH_INV 4


#define RF_POWER TX_POWER_158mW

enum{
	// flags going to packet[2]
	BAYANG_FLAG_RTH      = 0x01,
	BAYANG_FLAG_HEADLESS = 0x02,
	BAYANG_FLAG_FLIP     = 0x08,
	BAYANG_FLAG_VIDEO    = 0x10,
	BAYANG_FLAG_SNAPSHOT = 0x20,
};

enum{
	// flags going to packet[3]
	BAYANG_FLAG_INVERT   = 0x80,
};

typedef struct controlData {
	uint16_t throttle;
	uint16_t yaw;
	uint16_t pitch;
	uint16_t roll;
	uint8_t trims[4];
	uint8_t ch_flip:1;
	uint8_t ch_expert:1;
	uint8_t ch_headfree:1;
	uint8_t ch_rth:1;
	uint8_t ch_inv:1;
	uint8_t ch_none:3;
	
} T_CONTROLDATA;
void Bayang_init(uint8_t* p_packet,uint8_t* p_trans_id);
void Bayang_bind();
void Bayang_recv_packet(controlData* data);
void setNextChannel(void);
bool checkPacket(uint8_t *packet);
#endif //__BAYANG_PROTOCOL_H__
