#include "sys.h"
#include "led.h"
#include "motor.h"
#include "delay.h"
#include "hc05.h"
#include "usart2.h"			 	 
#include "string.h"	
#include "usart.h"
#include "ultrasonic.h"
extern u32	TIME;	//输入捕获值	


int main(void)
{
	  float temp;	 //测量值
	  int distance=0, flag=0;
	  u8 reclen=0, sendcnt=0; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  uart_init(9600);
	  LED_Init();
	  delay_init();
	  TIM3_PWM_Init(899,0);
	  HC05_Init();
	  ultrasonic_Init();               //对超声波模块初始化
    while(1)
		{	
			
			if(USART2_RX_STA&0X8000)			//接收到一次数据了
			{		
				reclen=USART2_RX_STA&0X7FFF;	//得到数据长度
				USART2_RX_BUF[reclen]=0;	 	//加入结束符		
				
				if(strcmp((const char*)USART2_RX_BUF,"F")==0)
				{
					run_init();//前进
					flag=1;
				}
				
				
				if(strcmp((const char*)USART2_RX_BUF,"B")==0)
				{
					back_init();//后退
					flag=1;
				}
				
				if(strcmp((const char*)USART2_RX_BUF,"L")==0)
				{
					left_init();//左转
					flag=1;
				}
				
				if(strcmp((const char*)USART2_RX_BUF,"R")==0)
				{
					right_init();//右转
					flag=1;
				}
				
				if(strcmp((const char*)USART2_RX_BUF,"S")==0)
				{
					stod_init();//停止
					flag=0;
				}
				
				USART2_RX_STA=0;
				
			}
			temp=MiddleValueFilter(); //中值滤波
			distance=(int)temp;
			sendcnt = (u8)temp;
			u2_printf("%d cm\r\n",sendcnt);
			if((distance <= 10)&&flag)
			{
				stod_init();
				delay_ms(1000);
				left_init();
				delay_ms(500);
				run_init();
			}
			LED=!LED;
			delay_ms(100);
		} 
}
