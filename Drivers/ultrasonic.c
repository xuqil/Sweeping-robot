#include "ultrasonic.h"
#include "delay.h"
#include "usart.h"

#define N 13  //中位值滤波参数
u8 i=0;
u32 TIME;	//输入捕获值	 记录TIM->CNT的值
/********************************************************************************
这里使用的是定时器2进行捕获！
自动重装值为0	 分频数为  72	
T=(arr*(psc+1))/(36M*2)
此处72分频，采样频率为1M
Tx———>PA.1     Rx————>PA.0
********************************************************************************/
TIM_ICInitTypeDef  TIM2_ICInitStructure;

void ultrasonic_Init()
{	 
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//x=TIMx
 	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //x=GPIOx
//------------------------RX,ECHO----------------------------------	
	GPIO_InitStructure.GPIO_Pin  = Rx_GPIO;  		//RX，ECHO
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;  //下拉输入  
	GPIO_Init(ULT_GPIO, &GPIO_InitStructure);
	GPIO_ResetBits(ULT_GPIO,Rx_GPIO);		 
//------------------------TX,TRIG----------------------------------
	GPIO_InitStructure.GPIO_Pin  = Tx_GPIO;         //TX,Trig      
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;   
	GPIO_Init(ULT_GPIO, &GPIO_InitStructure);
//---------------------------TIMx输入捕获参数--------------------------------------	
	TIM_TimeBaseStructure.TIM_Period = 0XFFFF;//自动重装值 
	TIM_TimeBaseStructure.TIM_Prescaler =71; 	//预分频,T=(arr*(psc+1))/(36M*2)   
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);	 //x=TIMx
//-----------------------------TIMx输入捕获参数------------------------------------------- 
//PA0--TIM2_CH1	 PA1--TIM2_CH2  PA2--TIM2_CH3  PA3--TIM2_CH4
	TIM2_ICInitStructure.TIM_Channel = TIM_Channel_1; //选择通道,TIM_Channel_x=TIMx_CHx
	TIM2_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	//上升沿捕获
	TIM2_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; //映射到TI1上
	TIM2_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 //配置输入分频,不分频 
	TIM2_ICInitStructure.TIM_ICFilter = 0x00;//IC1F=0000 配置输入滤波器 不滤波
	TIM_ICInit(TIM2, &TIM2_ICInitStructure);  //x=TIMx
//---------------------------------------NVIC初始化------------------------------------------ 
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; //TIMx_IRQn=TIMx
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2; //先占优先级2级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; //从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器 
	
	TIM_ITConfig(TIM2,TIM_IT_Update|TIM_IT_CC1,ENABLE);//允许更新中断 ,允许CCxIE捕获中断,CHx=TIM_IT_CCx	
	
   	TIM_Cmd(TIM2,ENABLE ); 	//使能,x=TIMx  
}    	 
////定时器5中断服务程序	 
void TIM2_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM2, TIM_IT_CC1) != RESET)//发生捕获,x=TIMx,TIM_IT_CCx= TIM_CHx
		if(i==0)
		{
			 TIM_SetCounter(TIM2,0);	 //计数清零,x=TIMx
			 TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Falling); //下降沿触发,x=TIMx,CHx=TIM_OCxPolarityConfig
			 i=1;				//上升沿到来
		}
		else
		{
			TIME=TIM_GetCounter(TIM2);	//获得捕获值,x=TIMx 即高电平时间
			TIM_OC1PolarityConfig(TIM2,TIM_ICPolarity_Rising);//上降沿触发,x=TIMx,CHx=TIM_OCxPolarityConfig
			i=0;				//下降沿到来 			
		}
		TIM_ClearITPendingBit(TIM2, TIM_IT_CC1|TIM_IT_Update);//清除中断标志，x=TIMx,TIM_IT_CCx= TIM_CHx
}



float MiddleValueFilter() //中值滤波
{ 
  u8 i,j,k; 
  float temp; 	   //排序中间参数
  float ArrDataBuffer[N]; 
  for(i=0; i<N; i++) //一次采集N个数据放入数组中 
  { 
    ArrDataBuffer[i]=get_distance(TIME); 
    delay_ms(1); 
  } 
  for(j=0; j<=N-1; j++) //采样值由小到大排列 
  { 
    for(k=0; k<=N-j-1; k++) 
    { 
      if(ArrDataBuffer[k]>ArrDataBuffer[k+1]) 
      { 
        temp=ArrDataBuffer[k]; 
        ArrDataBuffer[k]=ArrDataBuffer[k+1]; 
        ArrDataBuffer[k+1]=temp; 
      } 
    } 
  } 
  return(ArrDataBuffer[(N-1)/2]);//取中间值 
} 

float get_distance(u32 TIME)	
{
		float distance; 
		GPIO_SetBits(ULT_GPIO,Tx_GPIO);//送>10US的高电平
		delay_ms(1);
		GPIO_ResetBits(ULT_GPIO,Tx_GPIO);	//发出请求
		distance=(float)TIME*340/20000;	//得到距离
		return distance;	 
}
