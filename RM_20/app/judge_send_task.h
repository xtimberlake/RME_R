/**
  * @file judge_unpack_task.h
  * @version 1.0
  * @date 
  *
  * @brief  
  *
  *	@author link
	
	
	* @version 1.9
  * @date May 25th, 2020
  * @brief  ��ӽ�����Ϣ���ͻ���UI����
  *	@author Haoyun Yan
  *
  */
#ifndef __JUDGE_SEND_TASK_H__
#define __JUDGE_SEND_TASK_H__

#include "usart.h"
#include "data_fifo.h"
#include "protocol.h"



#define JUDGE_SEND_PERIOD 150
/*
�״�վ������С������ٶȣ�     5120�ֽ�ÿ��
�ڱ����������С������ٶȣ�     5120�ֽ�ÿ��
�������������С���������ٶȣ� 3720�ֽ�ÿ��
��ȫ�����������Ŵ����������ڲ���С��25ms �� 36ms
*/


typedef enum 
{
  SEND_JUDGE_OK       = 0x01U,
  SEND_JUDGE_ERROR    = 0x00U,
	
} Send_Judge_StatusTypedef;

/*
	breif�����ݶε����� ID
*/
typedef enum
{
	CLIENT_INTERACTIVE_CMD_ID      = 0x0233 , //�������ݣ�����0x0200~0x02ffѡȡ������ID�����ɲ������Զ���
	CLIENT_DELETE_GRAPH_CMD_ID     = 0x0100 , //�ͻ���ɾ��ͼ��
	CLIENT_DRAW_1_GRAPH_CMD_ID     = 0x0101 , //�ͻ��˻���1��ͼ��
	CLIENT_DRAW_2_GRAPHS_CMD_ID    = 0x0102 , //�ͻ��˻���2��ͼ��
	CLIENT_DRAW_5_GRAPHS_CMD_ID    = 0x0103 , //�ͻ��˻���5��ͼ��
	CLIENT_DRAW_7_GRAPHS_CMD_ID    = 0x0104 , //�ͻ��˻���7��ͼ��
	CLIENT_WRITE_STRINGS_CMD_ID    = 0x0103 , //�ͻ��˻����ַ���ͼ��
	
}client_data_cmd_e;

//ͼ��ɾ��
typedef __packed struct
{
uint8_t operate_tpye; 
uint8_t layer; 
} ext_client_custom_graphic_delete_t;

//ͼ��ɾ������
typedef enum
{
	type_delete_nop   = 0,
	type_delete_layer = 1,
	type_delete_all   = 2,
	
}type_graphic_delete_e;


//ͼ������ 15 byte��ʾһ��ͼ
typedef __packed struct
{ 
uint8_t graphic_name[3]; 
uint32_t operate_tpye:3; 
uint32_t graphic_tpye:3; 
uint32_t layer:4; 
uint32_t color:4; 
uint32_t start_angle:9;
uint32_t end_angle:9;
uint32_t width:10; 
uint32_t start_x:11; 
uint32_t start_y:11; 
uint32_t radius:10; 
uint32_t end_x:11; 
uint32_t end_y:11; 
} graphic_data_struct_t;

typedef enum
{ 
	GRAPHIC_MIAN_COLOR = 0,
	GRAPHIC_YELLOW     = 1,
	GRAPHIC_GREEN      = 2,
	GRAPHIC_ORANGE     = 3,
	GRAPHIC_PURPLE     = 4,
	GRAPHIC_PINK       = 5,
	GRAPHIC_CYAN       = 6,
	GRAPHIC_BALCK      = 7,
	GRAPHIC_WHITE      = 8,
	
} graphic_color_e;

typedef enum
{ 
	GRAPHIC_TYPE_LINE     = 0,
	GRAPHIC_TYPE_RECT     = 1,
	GRAPHIC_TYPE_CIRCLE   = 2,
	GRAPHIC_TYPE_OVAL     = 3,
	GRAPHIC_TYPE_ARC      = 4,
	GRAPHIC_TYPE_FLOAT    = 5,
	GRAPHIC_TYPE_INT      = 6,
	GRAPHIC_TYPE_STRINGS  = 7,
	
} graphic_type_e;

void judge_send_task(void const *argu);
void data_packet_pack(uint16_t cmd_id,uint16_t data_cmd_id,uint16_t robot_id,uint16_t client_id,
                      uint8_t *p_data, uint16_t len, uint8_t sof);
uint32_t send_packed_fifo_data(fifo_s_t *pfifo, uint8_t sof);


#endif

