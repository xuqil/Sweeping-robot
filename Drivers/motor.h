#ifndef __MOTOR_H
#define __MOTOR_H
#include "sys.h"
#include "stm32f10x_tim.h"

#define left1 PAout(6)
#define left2 PAout(7)
#define right1 PBout(0)
#define right2 PBout(1)

void TIM3_PWM_Init(u16 arr,u16 psc );
void run_init(void);
void right_init(void);
void left_init(void);
void back_init(void);
void stod_init(void);

#endif
