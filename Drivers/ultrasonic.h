#ifndef __ULTRASONIC_H
#define __ULTRASONIC_H
#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stm32f10x_tim.h"

#define   ULT_GPIO 	GPIOA

#define   Tx_GPIO 	GPIO_Pin_1		   //Tx！！！>PA.1 
#define   Rx_GPIO	GPIO_Pin_0		   // Rx！！！！>PA.0


void ultrasonic_Init(void);
float get_distance(u32 TIME);
float MiddleValueFilter(void); 

#endif
