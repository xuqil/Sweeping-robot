#include "sys.h"
#include "led.h"
#include "motor.h"
#include "delay.h"
#include "hc05.h"
#include "usart2.h"			 	 
#include "string.h"	
#include "usart.h"


int main(void)
{
	  u8 reclen=0; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);
	  LED_Init();
	  delay_init();
	  TIM3_PWM_Init(899,0);
		uart_init(9600);
	  HC05_Init();
    while(1)
		{	

			if(USART2_RX_STA&0X8000)			//接收到一次数据了
			{		
				reclen=USART2_RX_STA&0X7FFF;	//得到数据长度
				USART2_RX_BUF[reclen]=0;	 	//加入结束符		
				
				if(strcmp((const char*)USART2_RX_BUF,"F")==0)run_init();//前进
				
				
				if(strcmp((const char*)USART2_RX_BUF,"B")==0)back_init();//后退
				
				if(strcmp((const char*)USART2_RX_BUF,"L")==0)left_init();//左转
				
				if(strcmp((const char*)USART2_RX_BUF,"R")==0)right_init();//右转
				
				if(strcmp((const char*)USART2_RX_BUF,"S")==0)stod_init();//停止
				
				USART2_RX_STA=0;
				
			}
			LED=!LED;
			delay_ms(100);
		} 
}
