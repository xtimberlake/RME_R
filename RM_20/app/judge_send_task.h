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
  * @brief  添加交互信息，客户定UI定义
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
雷达站最大上行、下行速度：     5120字节每秒
哨兵机器人上行、下行速度：     5120字节每秒
其他机器人上行、下行最大速度： 3720字节每秒
安全起见，请合理安排带宽，发送周期不得小于25ms 和 36ms
*/


typedef enum 
{
  SEND_JUDGE_OK       = 0x01U,
  SEND_JUDGE_ERROR    = 0x00U,
	
} Send_Judge_StatusTypedef;

/*
	breif：数据段的内容 ID
*/
typedef enum
{
	CLIENT_INTERACTIVE_CMD_ID      = 0x0233 , //交互数据，可在0x0200~0x02ff选取，具体ID含义由参赛队自定义
	CLIENT_DELETE_GRAPH_CMD_ID     = 0x0100 , //客户端删除图形
	CLIENT_DRAW_1_GRAPH_CMD_ID     = 0x0101 , //客户端绘制1个图形
	CLIENT_DRAW_2_GRAPHS_CMD_ID    = 0x0102 , //客户端绘制2个图形
	CLIENT_DRAW_5_GRAPHS_CMD_ID    = 0x0103 , //客户端绘制5个图形
	CLIENT_DRAW_7_GRAPHS_CMD_ID    = 0x0104 , //客户端绘制7个图形
	CLIENT_WRITE_STRINGS_CMD_ID    = 0x0103 , //客户端绘制字符串图形
	
}client_data_cmd_e;

//图形删除
typedef __packed struct
{
uint8_t operate_tpye; 
uint8_t layer; 
} ext_client_custom_graphic_delete_t;

//图形删除类型
typedef enum
{
	type_delete_nop   = 0,
	type_delete_layer = 1,
	type_delete_all   = 2,
	
}type_graphic_delete_e;


//图形数据 15 byte表示一张图
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

