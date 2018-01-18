/* 
* task_query.h
*
* Created: 28.10.2015 21:04:35
* Author: Nik
*/


#ifndef __TASK_QUERY_H__
#define __TASK_QUERY_H__

#define MAX_TASKS 15
#define AVR

#ifdef STM8
	#include "stm8s_itc.h"
	static  uint8_t __iCliRetVal(void)
	{
		disableInterrupts();
		return 1;
	}
	#define ATOMIC_BLOCK(type) for (  uint8_t __ToDo = __iCliRetVal() ; \
	__ToDo ; __ToDo = 0, enableInterrupts())
#endif
	
#ifdef STM32
	#include <stdint.h>
	static  uint8_t __iCliRetVal(void)
	{
		//disableInterrupts();
		return 1;
	}
	#define ATOMIC_BLOCK(type) for (  uint8_t __ToDo = __iCliRetVal() ; \
	__ToDo ; __ToDo = 0/*, enableInterrupts()*/)
#endif
	
#ifdef AVR
	#include <util/atomic.h>
#endif




typedef void (*TPTR)(void);

extern void Idle();


typedef  struct {
	TPTR task;
	unsigned int start_delay;
	unsigned int  g_netPeriod;
	char run:4;//флаг на запуск задачи в функции Dispatcher
	char real_time:4;//флаг на запуск задачи в обработчике прерывания от таймера дял задач real time
	} TTask;
static TTask TaskQueue[MAX_TASKS];
	


void AddTask(TPTR AddingTask, unsigned int  delay, unsigned int  period);
	//Добавляет задачу в очередь. delay - задержка перед первым выполнением, period - период повторения
void AddRealTimeTask(TPTR AddingTask, unsigned int  delay, unsigned int  period);
	//Добавляет задачу реального времени в очередь. delay - задержка перед первым выполнением, period - период повторения
	//Задача будет выполняться в прерывании таймера по истечении задержки или периода. Задача должны быть короткой!!!
	//Использовать только для задач критичных к временным интервалам
void TimerProcess();
	// Необходимо вызывать данную функцию по таймеру, задержка и период задаются в количестве периодов вызова данной функции
 void Dispatcher();
	//Функция должна крутиться в основном цикле. В ней вызываются по очереди задачи у которых подошло время выполнения
void DeleteTask(TPTR delTask);
   //Удаляет задачу из очереди
#endif //__TASK_QUERY_H__
