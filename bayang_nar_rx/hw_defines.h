/*
 * hw_defines.h
 *
 * Created: 15.01.2018 10:46:11
 *  Author: USER
 */ 


#ifndef HW_DEFINES_H_
#define HW_DEFINES_H_

#include "spi_drv.h"
#include <util/atomic.h>
#define F_CPU 16000000UL
#include <avr/delay.h>
#include <avr/io.h>
#include <inttypes.h>
#include <avr/interrupt.h>
#include <HardwareSerial.h>

//Пины для выбора чипа по SPI и линии контроля за чипом NRF
#define CS_PIN	PINB0
#define CS_PORT PORTB
#define CS_DIR_PORT DDRB

#define CE_PIN  PINB2
#define CE_PORT PORTB
#define CE_DIR_PORT	DDRB

// Служебные макросы
// #define PortReg(port) (*(port))
// #define DirReg(port) (*((port) - 1))
// #define PinReg(port) (*((port) - 2))

#ifndef _BV
	#define _BV(x) (1<<(x))
#endif
//Макросы управления линиями
#define CS_INIT		CS_DIR_PORT |= _BV(CS_PIN)
#define CS_ON		CS_PORT |= _BV(CS_PIN)
#define CS_OFF		CS_PORT &= ~_BV(CS_PIN)

#define CE_INIT		CE_DIR_PORT |= _BV(CE_PIN)
#define CE_ON		CE_PORT |= _BV(CE_PIN)
#define CE_OFF		CE_PORT &= ~_BV(CE_PIN)

//реализация задержек
#define HW_DELAY_US(x) _delay_us((x))

#define HW_DELAY_MS(x) _delay_ms((x))

//макрос отладочного вывода
#define INIT_DBG_PRINT() Serial.begin(115200)
#define DEBUGPRINT(x) Serial.print((x))

//разрешение прерываний
#define ENABLE_INTERRUPT() sei()
//системные функции времени
uint32_t micros();
uint32_t millis();
void StartSystemUSTimer();
//аппаратный ШИМ
#define PWM0_PIN  PIND6
#define PWM0_PORT PORTD
#define PWM0_DDR  DDRD

void InitPwm0();
void SetPwm0(uint16_t value);

#define PWM1_PIN  PIND5
#define PWM1_PORT PORTD
#define PWM1_DDR  DDRD

void InitPwm1();
void SetPwm1(uint16_t value);
//системный таймер с интервалом в 0.97 мс
void InitSystemTimer(void(*)(void));

#endif /* HW_DEFINES_H_ */