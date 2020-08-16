/** 
  * @file     bsp_TOF.c
  * @version  v1.0
  * @date     2020.1.10
	*
  * @brief    ��ȡ����ģ����Ϣ��*���ڲ�����Ϊ115200
	*
  *
  */

#include "bsp_TOF.h"
#include "bsp_uart.h"

uint8_t TOF[2];//TOF����ģ�鴦�����ֵ

/**
  * @brief  ����TOF����ģ����Ϣ
  * @param  uint8_t *receive_buff
  * @retval  int DIST
  */
	
//	uint16_t TOF_GetData(uint8_t *receive_buff)
//{	
//	int DIST=0;
//	DIST=receive_buff[2]|receive_buff[3]<<8;
//	return DIST;
//}	

/**
  * @brief  ����TOF����ģ����Ϣ(ά������)
  * @param  uint8_t *receive_buff
  * @retval  int DIST
  */
	uint16_t TOF_GetData(uint8_t *receive_buff)
{	
	int DIST=0;
	for(int i=30;i<40;i++)
	{
		if(receive_buff[i]>=48&&receive_buff[i]<=57)
		{
			DIST=DIST*10+(receive_buff[i]-48);
		}			
	}
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
	

