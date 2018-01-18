/*
 * hw_defines.cpp
 *
 * Created: 15.01.2018 13:32:32
 *  Author: USER
 */ 

#include "hw_defines.h"

void StartSystemUSTimer(){
	// Timer/Counter 2 initialization
	// Clock source: System Clock
	// Clock value: 16000,000 kHz
	// Mode: Normal top=0xFF
	// OC2A output: Disconnected
	// OC2B output: Disconnected
	ASSR=0x00;
	TCCR2A=0x00;
	TCCR2B=0x01;
	TCNT2=0xBF;
	OCR2A=0x29;
	OCR2B=0x00;
	TIMSK2=0x02;

}

static uint32_t usec,msec;

ISR(TIMER2_COMPA_vect){
	usec++;
	TCNT2 = 0;
	
}

uint32_t micros(){
	return usec;
}

uint32_t millis(){
	return (usec/240 );
}

void InitPwm0(){
// для таймера 1
// 	TCCR1A = _BV(COM1A1) | _BV(WGM10);
// 	TCCR1B = _BV(WGM12) |_BV(CS12);// _BV(CS11)  | _BV(CS10);
// 	OCR1A = 0x0000;
//для таймера 0
	TCCR0A = _BV(COM0A1)  | _BV(WGM00) | _BV(WGM01)| _BV(COM0B1);
	TCCR0B = _BV(CS01) | _BV(CS00);
	OCR0A = 0;
	OCR0B = 0;
	
	PWM0_DDR |= _BV(PWM0_PIN);
}

void SetPwm0(uint16_t value){
	OCR0A = value;
}

void InitPwm1()
{
		OCR0B = 0;
		PWM1_DDR |= _BV(PWM1_PIN);

}

void SetPwm1( uint16_t value )
{
	OCR0B = value;
}
typedef void (*TPTR)(void);
TPTR pIsrFunc = NULL;

void InitSystemTimer( TPTR  pFunc)
{
	pIsrFunc = pFunc;
	TIMSK0 |= _BV(TOIE0);
}

ISR(TIMER0_OVF_vect){
	if (pIsrFunc != NULL)
	{
		(*pIsrFunc)();
	}
}
