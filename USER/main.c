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
	  int distance=0, flag=0, change=0;
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
			static int l=500, r=500;
			
			if(USART2_RX_STA&0X8000)			//接收到一次数据了
			{		
				reclen=USART2_RX_STA&0X7FFF;	//得到数据长度
				USART2_RX_BUF[reclen]=0;	 	//加入结束符		
				
				if(strcmp((const char*)USART2_RX_BUF,"F")==0)
				{
					//run_init();//前进
					run_init_pwm(l,r);
					u2_printf("run!\r\n");
					flag=1;
					change=1;
				}
				
				
				if(strcmp((const char*)USART2_RX_BUF,"B")==0)
				{
					//back_init();//后退
					back_init_pwm(l,r);
					u2_printf("back!\r\n");
					flag=1;
					change=2;
				}
				
				if(strcmp((const char*)USART2_RX_BUF,"L")==0)
				{
					//left_init();//左转
					left_init_pwm(l,r);
					u2_printf("left!\r\n");
					flag=1;
					change=3;
				}
				
				if(strcmp((const char*)USART2_RX_BUF,"R")==0)
				{
					//right_init();//右转
					right_init_pwm(l,r);
					u2_printf("right!\r\n");
					flag=1;
					change=4;
				}
				
				if(strcmp((const char*)USART2_RX_BUF,"S")==0)
				{
					stod_init();//停止
					u2_printf("stop!\r\n");
					flag=0;
					l=500;
					r=500;
					change=0;
				}
				if((strcmp((const char*)USART2_RX_BUF,"A")==0)&&flag)
				{
					if(l<700&&r<700)
					{
						l+=50;
						r+=50;
					}
					if(change==1)run_init_pwm(l,r);
					else if(change==2)back_init_pwm(l,r);
					else if(change==3)left_init_pwm(l,r);
					else if(change==4)right_init_pwm(l,r);
				}
				if((strcmp((const char*)USART2_RX_BUF,"D")==0)&&flag)
				{
					if(l>100&&r>100)
					{
						l-=50;
						r-=50;
					}
					if(change==1)run_init_pwm(l,r);
					else if(change==2)back_init_pwm(l,r);
					else if(change==3)left_init_pwm(l,r);
					else if(change==4)right_init_pwm(l,r);
				}
				USART2_RX_STA=0;
				
			}
			temp=MiddleValueFilter(); //中值滤波
			distance=(int)temp;
			sendcnt = (u8)temp;
			u2_printf("%d cm\r\n",sendcnt);
			if((distance <= 5)&&flag)
			{
				stod_init();
				u2_printf("stop!\r\n");
				delay_ms(1000);
				//left_init();
				left_init_pwm(l,r);
				u2_printf("left!\r\n");
				delay_ms(500);
				//run_init();
				run_init_pwm(l,r);
				u2_printf("run!\r\n");
			}
			if((distance > 5)&&(distance <= 10)&&flag)run_init_pwm(200,200);
			if((distance > 10)&&(distance <= 15)&&flag)run_init_pwm(300,300);
			if((distance > 15)&&(distance <= 20)&&flag)run_init_pwm(400,400);
			LED=!LED;
			delay_ms(100);
		} 
}
