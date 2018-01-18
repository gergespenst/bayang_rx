/* 
 * task_query.cpp
 *
 * Created: 28.10.2015 21:04:34
 * Author: Nik
 */

#include "task_query.h"

unsigned char QueueTail = 0;

void AddTask(TPTR AddingTask, unsigned int delay, unsigned int period) {

	if (!AddingTask)
		return;

	for (unsigned char i = 0; i < QueueTail; i++) {
		if (TaskQueue[i].task == AddingTask) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				TaskQueue[i].g_netPeriod = period;
				TaskQueue[i].start_delay = delay;
				TaskQueue[i].run = 0;
				TaskQueue[i].real_time = 0;
			}
			return;
		}
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (QueueTail < MAX_TASKS) {
			TaskQueue[QueueTail].task = AddingTask;
			TaskQueue[QueueTail].run = 0;
			TaskQueue[QueueTail].real_time = 0;
			TaskQueue[QueueTail].g_netPeriod = period;
			TaskQueue[QueueTail].start_delay = delay;
			QueueTail++;
		}
	}

}

void AddRealTimeTask(TPTR AddingTask, unsigned int delay, unsigned int period) { //TODO: Не очень хорошо полностью копировать функцию кроме одной строчки
	if (!AddingTask)
		return;

	for (unsigned char i = 0; i < QueueTail; i++) {
		if (TaskQueue[i].task == AddingTask) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				TaskQueue[i].g_netPeriod = period;
				TaskQueue[i].start_delay = delay;
				TaskQueue[i].run = 0;
				TaskQueue[i].real_time = 1;
			}
			return;
		}
	}

	ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
	{
		if (QueueTail < MAX_TASKS) {
			TaskQueue[QueueTail].task = AddingTask;
			TaskQueue[QueueTail].run = 0;
			TaskQueue[QueueTail].real_time = 1;
			TaskQueue[QueueTail].g_netPeriod = period;
			TaskQueue[QueueTail].start_delay = delay;
			QueueTail++;
		}
	}

}

void DeleteTask(TPTR delTask) {
	for (unsigned char i = 0; i < QueueTail; i++) {
		if (TaskQueue[i].task == delTask) {
			ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
			{
				if (i != (QueueTail - 1)) {
					TaskQueue[i].task = TaskQueue[QueueTail - 1].task;
					TaskQueue[i].start_delay =
							TaskQueue[QueueTail - 1].start_delay;
					TaskQueue[i].g_netPeriod =
							TaskQueue[QueueTail - 1].g_netPeriod;
					TaskQueue[i].run = TaskQueue[QueueTail - 1].run;
					TaskQueue[i].real_time = TaskQueue[QueueTail - 1].real_time;
				}
				QueueTail--;
			}
		}
	}
}

void Dispatcher() {
	volatile TTask * tempTask;
	for (unsigned char i = 0; i < QueueTail; i++) {
		tempTask = &TaskQueue[i];
		if ((tempTask->run == 1) && (tempTask->real_time == 0)) {
			(*(tempTask->task))();
			if (tempTask->g_netPeriod == 0) {
				DeleteTask(tempTask->task);
			} else {
				tempTask->run = 0;
				if (!tempTask->start_delay) {
					tempTask->start_delay = tempTask->g_netPeriod - 1;
				}
			}

		}
	}
}

void TimerProcess() {
	for (unsigned char i = 0; i < QueueTail; i++) {
		if (TaskQueue[i].start_delay == 0) {
			if (TaskQueue[i].real_time == 1) //если стоит флаг реалтайм то выполняем функцию в прерывании
					{
				//ATOMIC_BLOCK(ATOMIC_RESTORESTATE)
				{
					(*(TaskQueue[i].task))();

					if (TaskQueue[i].g_netPeriod == 0) {
						DeleteTask(TaskQueue[i].task);
					} else {
						TaskQueue[i].run = 0;
						if (!TaskQueue[i].start_delay) {
							TaskQueue[i].start_delay = TaskQueue[i].g_netPeriod
									- 1;
						}
					}
				}
			} else
				TaskQueue[i].run = 1;

		} else
			TaskQueue[i].start_delay--;
	}
}

