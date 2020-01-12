/** 
  * @file     bsp_TOF.c
  * @version  v1.0
  * @date     2020.1.10
	*
  * @brief    读取激光模块信息，*串口波特率为115200
	*
  *	@author   YYQ
  *
  */

#include "bsp_TOF.h"
#include "bsp_uart.h"

uint16_t TOF[2];//TOF激光模块处理后获得值
int i;

/**
  * @brief  处理TOF激光模块信息
  * @param  uint8_t *receive_buff
  * @retval  int DIST
  */
	
	uint16_t TOF_GetData(uint8_t *receive_buff)
{	
	int DIST=0;
	DIST=receive_buff[2]|receive_buff[3]<<8;
	return DIST;
}	
//	uint16_t TOF_GetData(uint8_t *receive_buff)
//{	
//	int DIST=0;
//	i=0;
//	while(i<=20)
//	{
//		if(receive_buff[i]>=48&&receive_buff[i]<=57)
//		{
//			DIST=DIST*10+(receive_buff[i]-48);
//		}
//		i++;
//	}
//	return DIST;
//}
	
//uint16_t TOF_GetData(uint8_t *receive_buff) //。。。。。。
//{	
//	uint16_t a,b,c,d,e;
//	a=receive_buff[0],b=receive_buff[1],c=receive_buff[2],d=receive_buff[3];
//	
//	if(d>=48&&d<=57)
//	{
//		e=d+10*c+100*b+1000*a-53328;
//	}
//	else if(c>=48&&c<=57)
//	{
//		e=c+10*b+100*a-5328;
//	}
//	else if(b>=48&&b<=57)
//	{
//		e=b+10*a-528;
//	}
//	else if(a>=48&&a<=57)
//	{
//		e=a-48;
//	}
//	return e;
//}


