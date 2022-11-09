/*
 * timer.h
 *
 *  Created on: Oct 12, 2022
 *      Author: phamv
 */

#ifndef INC_TIMER_H_
#define INC_TIMER_H_

#include "main.h"
#include "scheduler.h"

#define TIMER_CYCLE 10

extern int timer1_flag, timer2_flag, timer3_flag;

void setTimer1(int);
void setTimer2(int);
void setTimer3(int);
void timer_run(void);

#endif /* INC_TIMER_H_ */
