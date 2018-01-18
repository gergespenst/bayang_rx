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
	char run:4;//���� �� ������ ������ � ������� Dispatcher
	char real_time:4;//���� �� ������ ������ � ����������� ���������� �� ������� ��� ����� real time
	} TTask;
static TTask TaskQueue[MAX_TASKS];
	


void AddTask(TPTR AddingTask, unsigned int  delay, unsigned int  period);
	//��������� ������ � �������. delay - �������� ����� ������ �����������, period - ������ ����������
void AddRealTimeTask(TPTR AddingTask, unsigned int  delay, unsigned int  period);
	//��������� ������ ��������� ������� � �������. delay - �������� ����� ������ �����������, period - ������ ����������
	//������ ����� ����������� � ���������� ������� �� ��������� �������� ��� �������. ������ ������ ���� ��������!!!
	//������������ ������ ��� ����� ��������� � ��������� ����������
void TimerProcess();
	// ���������� �������� ������ ������� �� �������, �������� � ������ �������� � ���������� �������� ������ ������ �������
 void Dispatcher();
	//������� ������ ��������� � �������� �����. � ��� ���������� �� ������� ������ � ������� ������� ����� ����������
void DeleteTask(TPTR delTask);
   //������� ������ �� �������
#endif //__TASK_QUERY_H__
