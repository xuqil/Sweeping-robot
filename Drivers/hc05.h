#ifndef __HC05_H
#define __HC05_H	 
#include "sys.h" 


#define HC05_KEY  	PCout(4) 	//蓝牙控制KEY信号
#define HC05_LED  	PAin(4)		//蓝牙连接状态信号
  
u8 HC05_Init(void);
void HC05_CFG_CMD(u8 *str);
u8 HC05_Get_Role(void);
u8 HC05_Set_Cmd(u8* atstr);	   
#endif  

