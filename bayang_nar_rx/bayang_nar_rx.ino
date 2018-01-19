/*
 * bayang_nar_rx.ino
 *
 * Created: 1/15/2018 10:24:28 AM
 * Author: USER
 */ 

//��������� ������
#define F_CPU 16000000L

#include <avr/interrupt.h>
#include "hw_defines.h"
#include "NRF24L01.h"

#include "bayang_protocol.h"
#include "task_query.h"




////////////////////// PPM CONFIGURATION//////////////////////////
#define channel_number 6  //set the number of channels
//////////////////////////////////////////////////////////////////

uint8_t transmitterID[4];
uint8_t packet[32];
static uint8_t emptyPacketsCount = 0;
int ppm[channel_number];

T_CONTROLDATA recData;

#define NUM_OF_PWM_CHAN 2
#define SPEED_PWM	0
#define LR_PWM		1

#define DIR_PORT_0 PORTC
#define DIR_DDR_0	 DDRC
#define DIR_PIN_00 PINC0
#define DIR_PIN_01 PINC1

#define INIT_DIR_PORT_0() DIR_DDR_0 |= _BV(DIR_PIN_00) | _BV(DIR_PIN_01); DIR_PORT_0 &= ~(_BV(DIR_PIN_00) | _BV(DIR_PIN_01));

#define DIR_PORT_1 PORTC
#define DIR_DDR_1	 DDRC
#define DIR_PIN_10 PINC2
#define DIR_PIN_11 PINC3

#define INIT_DIR_PORT_1() DIR_DDR_1 |= _BV(DIR_PIN_10) | _BV(DIR_PIN_11); DIR_PORT_1 &= ~(_BV(DIR_PIN_10) | _BV(DIR_PIN_11));
uint32_t count = 0;
void ExecComand(){
#if 0
	DEBUGPRINT(count++);
	DEBUGPRINT(" Thr = ");
	DEBUGPRINT(recData.throttle);
	DEBUGPRINT(" YAW = ");
	DEBUGPRINT(recData.yaw);
	DEBUGPRINT(" PIT = ");
	DEBUGPRINT(recData.pitch);
	DEBUGPRINT(" ROLL = ");
	DEBUGPRINT(recData.roll);
	DEBUGPRINT(" T0 = ");
	DEBUGPRINT(recData.trims[0]);
	DEBUGPRINT(" T1 = ");
	DEBUGPRINT(recData.trims[1]);
	DEBUGPRINT('\n');
#endif

	if (recData.pitch > 511)
	{
		SetPwm0((recData.pitch - 512)>>1);
		DIR_PORT_0 |= _BV(DIR_PIN_00);
		DIR_PORT_0 &= ~_BV(DIR_PIN_01);
	}else{
		SetPwm0((511 - recData.pitch)>>1);
	
		DIR_PORT_0 |= _BV(DIR_PIN_01);
		DIR_PORT_0 &= ~_BV(DIR_PIN_00);
	}
	if (recData.roll > 511)
		{
			SetPwm1((recData.roll - 512)>>1);
			DIR_PORT_1 |= _BV(DIR_PIN_10);
			DIR_PORT_1 &= ~_BV(DIR_PIN_11);
			}else{
			SetPwm1((511 - recData.roll)>>1);
			
			DIR_PORT_1 |= _BV(DIR_PIN_11);
			DIR_PORT_1 &= ~_BV(DIR_PIN_10);
		}
	
	
	
}
#define BLINK_PORT	PORTB
#define BLINK_DDR	DDRB
#define BLINK_PIN	PINB1

#define BLINK_INIT() BLINK_DDR |= _BV(BLINK_PIN); BLINK_PORT |= _BV(BLINK_PIN);

void Blink(){
	BLINK_PORT ^= _BV(BLINK_PIN);
}

void RecivePacket(){
	Bayang_recv_packet(&recData);
	//HW_DELAY_MS(1);
	if (recData.throttle==0 && recData.yaw==0 && recData.pitch==0 && recData.roll==0) {
		emptyPacketsCount++;
		if (emptyPacketsCount >= 255) {
			
			emptyPacketsCount = 0;
			// signal lost?
			//TODO: ���������� ������ ��������
			SetPwm0(0);
			SetPwm1(0);
			BLINK_INIT();
			NRF24L01_Initialize();
			HW_DELAY_MS(100);
			Bayang_init(packet,transmitterID);
			
			DeleteTask(Blink);
			AddRealTimeTask(Blink,100,125);
			
			Bayang_bind();
			
			AddTask(Blink,100,500);
			emptyPacketsCount = 0;
		}
		}else{
		
		emptyPacketsCount = 0;
		ExecComand();
	}
}


int main(){
	StartSystemUSTimer();
	InitSystemTimer(TimerProcess);
	ENABLE_INTERRUPT();
	INIT_DBG_PRINT();
	DEBUGPRINT("\nSTART\n");
	
	BLINK_INIT();
	
	INIT_DIR_PORT_0();
	INIT_DIR_PORT_1();
	InitPwm0();
	InitPwm1();
	
	
	
	NRF24L01_Initialize();
	Bayang_init(packet,transmitterID);
	AddRealTimeTask(Blink,100,125);
	Bayang_bind();
	DeleteTask(Blink);
	AddTask(Blink,100,500);
	
	AddTask(RecivePacket,0,1);

	
	
	emptyPacketsCount = 0;
	while (1)
	{
		Dispatcher();
	}
	
	return 0;
}