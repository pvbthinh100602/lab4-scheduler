/*
 * scheduler.c
 *
 *  Created on: Oct 31, 2022
 *      Author: phamv
 */
#include "scheduler.h"

struct sTask *pHead = 0;
struct sTask *pTail = 0;
struct sTask SCH_tasks_G[SCH_MAX_TASKS];

uint32_t realloc_index = SCH_MAX_TASKS;

void SCH_Init(void){
	unsigned char i;
	for(i = 0; i < SCH_MAX_TASKS; i++){
		SCH_Delete_Task(i);
	}
}

void SCH_Update(void){
//	uint32_t index;
//	for(index = 0; index < SCH_MAX_TASKS; index++){
//		if(SCH_tasks_G[index].pTask){
//			if(SCH_tasks_G[index].Delay > 0){
//				SCH_tasks_G[index].Delay--;
//			} else {
//				SCH_tasks_G[index].RunMe += 1;
//				SCH_tasks_G[index].Delay = SCH_tasks_G[index].Period;
//			}
//		}
//	}
	uint8_t timeout_flag = 0;
	struct sTask* pCur = pHead;
	while(pCur != 0){
		if(pCur->Delay > 0){
			if(timeout_flag == 0) pCur->Delay--;
			else break;
		} else {
			pCur->RunMe += 1;
			timeout_flag = 1;
		}
		pCur = pCur->pNext;
	}
//	struct sTask* pCur = pHead->pNext;
//	if(timeout_flag == 1){
//		while(pCur != 0 && pCur->Delay == 0){
//			pCur->RunMe += 1;
//		}
//	}
}

uint32_t SCH_Add_Task(void(*pFunc)(), uint32_t delay, uint32_t period){
//	HAL_GPIO_TogglePin(GPIOB, LED_CHECK_Pin);
	uint32_t waiting = 0;
	uint8_t index = 0;
	if(realloc_index == SCH_MAX_TASKS){
		while(SCH_tasks_G[index].pTask != 0 && index < SCH_MAX_TASKS) index++;
		if(index == SCH_MAX_TASKS){
			return SCH_MAX_TASKS;
		}
	} else {
		index = realloc_index;
		realloc_index = SCH_MAX_TASKS;
	}

	if(pHead == 0){
		pHead = &SCH_tasks_G[index];
		pTail = pHead;
		pHead->Delay = delay;
		pHead->pNext = 0;
	} else{
		struct sTask* pCur = pHead;
		struct sTask* pPrev = 0;
		while(pCur != 0){
			waiting += pCur->Delay;
			if(waiting > delay) break;
			pPrev = pCur;
			pCur = pCur->pNext;
		}
		if(waiting <= delay){
			SCH_tasks_G[index].Delay = delay - waiting;
			pTail->pNext = &SCH_tasks_G[index];
			pTail = pTail->pNext;
			pTail->pNext = 0;
		} else {
			SCH_tasks_G[index].Delay = delay - (waiting - pCur->Delay);
			SCH_tasks_G[index].pNext = pCur;
			if(pPrev == 0){
				pHead = &SCH_tasks_G[index];
			} else {
				pPrev->pNext = &SCH_tasks_G[index];
			}
			while(pCur != 0){
				pCur->Delay -= SCH_tasks_G[index].Delay;
				pCur = pCur->pNext;
			}
		}
	}
	SCH_tasks_G[index].pTask = pFunc;
	SCH_tasks_G[index].Period = period;
	SCH_tasks_G[index].RunMe = 0;
	SCH_tasks_G[index].TaskID = index;
	return index;
}

void SCH_Dispatch_Tasks(void){
//	for(int index = 0; index < SCH_MAX_TASKS; index++){
//		if(SCH_tasks_G[index].RunMe > 0){
//			SCH_tasks_G[index].RunMe--;
//			(*SCH_tasks_G[index].pTask)();
//			if(SCH_tasks_G[index].Period == 0) SCH_Delete_Task(index);
//		}
//	}
	if((pHead != 0) && (pHead->RunMe > 0)){
		(*pHead->pTask)();
		pHead->RunMe -= 1;
		struct sTask* pDel = pHead;
		pHead = pHead->pNext;
		if(pHead->Period == 0) SCH_Delete_Task(pDel->TaskID);
		else {
			realloc_index = pDel->TaskID;
			SCH_Add_Task(pDel->pTask, pDel->Period, pDel->Period);
		}

	}
	SCH_Go_To_Sleep();
}

uint8_t SCH_Delete_Task(uint32_t index){
	if(SCH_tasks_G[index].pTask == 0){
		return 0;
	} else {
		pHead = pHead->pNext;
		SCH_tasks_G[index].pTask = 0;
		SCH_tasks_G[index].Delay = 0;
		SCH_tasks_G[index].Period = 0;
		SCH_tasks_G[index].RunMe = 0;
		SCH_tasks_G[index].pNext = 0;
		SCH_tasks_G[index].TaskID = 0;
		return 1;
	}
}

void SCH_Go_To_Sleep(void){
	HAL_SuspendTick();
	HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
	HAL_ResumeTick();
}
