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
	  int distance=0;
	  u8 reclen=0; 
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	  uart_init(9600);
	  LED_Init();
	  delay_init();
	  printf("ok1\n");
	  TIM3_PWM_Init(899,0);
	  printf("ok3\n");
	  HC05_Init();
	  printf("ok4\n");
	  ultrasonic_Init();               //对超声波模块初始化
	  printf("ok5\n");
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
			temp=MiddleValueFilter(); //中值滤波
			distance=(int)temp;
			LED=!LED;
			printf("distance %fcm\n",temp);
			printf("distance %dcm\n",distance);
			delay_ms(100);
		} 
}
