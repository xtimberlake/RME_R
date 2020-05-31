/** 
  * @file judge_send_task.c
  * @version 1.0
  * @date    2019.5.4
  * @brief 
  *	@author link
	
	
	* @version 1.9
  * @date    May 24th, 2020
  * @brief   添加己方交互，客户端UI发送函数
	*	@author  Haoyun Yan
  */
#include "judge_send_task.h"
#include "judge_unpack_task.h"
#include "comm_task.h"
#include "bsp_uart.h"
#include "cmsis_os.h"
#include "judgement_info.h"
#include "protocol.h"
#include "data_fifo.h"
#include "string.h"
#include "stdio.h"


      //send data to judge system timer
void judge_send_task(void const *argu)
{

		data_packet_pack(ROBOT_INTERACTIVE_ID,
				             STU_CUSTOM_DATA_CMD_ID,
				             judge_recv_mesg.robot_state_data.robot_id,
				             Stu_Client_ID,
				             (uint8_t *)&judge_send_mesg.show_in_client_data,
				             sizeof(client_custom_data_t),
										 DN_REG_ID);
    send_packed_fifo_data(&judge_txdata_fifo, DN_REG_ID);
		judge_send_mesg.show_in_client_data.data1 =  0;
		judge_send_mesg.show_in_client_data.data2 =  0;
	  judge_send_mesg.show_in_client_data.data3 =  0;
		judge_send_mesg.show_in_client_data.masks =  0;

}
uint8_t* protocol_packet_pack(uint16_t cmd_id, uint8_t *p_data, uint16_t len, uint8_t sof, uint8_t *tx_buf)
{
  //memset(tx_buf, 0, 100);
  static uint8_t seq;
  
  uint16_t frame_length = HEADER_LEN + CMD_LEN + len + CRC_LEN; //统计一帧长度
  frame_header_t *p_header = (frame_header_t*)tx_buf; //帧头 5 byte
  
  p_header->sof          = sof;
  p_header->data_length  = len;
  
  
  if (sof == UP_REG_ID) //包序号处理
  {
    if (seq++ >= 255)
      seq = 0;
    
    p_header->seq = seq;
  }
  else
  {
    p_header->seq = 0;
  }
  
  //附加CRC校验码
  memcpy(&tx_buf[HEADER_LEN], (uint8_t*)&cmd_id, CMD_LEN);
  append_crc8_check_sum(tx_buf, HEADER_LEN);
  memcpy(&tx_buf[HEADER_LEN + CMD_LEN], p_data, len);
  append_crc16_check_sum(tx_buf, frame_length);
  
  return tx_buf;
}

/*

	brief：裁判系统串口信息打包
  params：

[1]cmd_id:    0x0301

头结构定义： 6 byte
[2]data_cmd:  动作内容id:即选择己方机器人通信，还是在客户端画图，删图，写字符串
[3]robot_id:  发送者ID
[4]client_id: 接受者ID

[5]p_data:    数据
[6]len:       数据长度，最大113（不包括头结构）
[7]sof:       0xA5

*/


void data_packet_pack(uint16_t cmd_id,uint16_t data_cmd_id,uint16_t robot_id,uint16_t client_id,
                      uint8_t *p_data, uint16_t len, uint8_t sof)
{
  uint8_t tx_buf[PROTOCAL_FRAME_MAX_SIZE];
  uint8_t data_buf[DATA_MAX_SIZE];
  uint16_t frame_length = HEADER_LEN + CMD_LEN + DATA_HEADER_LEN + len + CRC_LEN;
	

  memcpy(&data_buf[0], (uint8_t*)&data_cmd_id, 2);
	memcpy(&data_buf[2], (uint8_t*)&robot_id, 2);
	memcpy(&data_buf[4], (uint8_t*)&client_id, 2);
	memcpy(&data_buf[6], p_data, len);
	
  protocol_packet_pack(cmd_id, data_buf, DATA_HEADER_LEN+len, sof, tx_buf);
  
  /* use mutex operation */

  if (sof == DN_REG_ID)
	{
    fifo_s_puts(&judge_txdata_fifo, tx_buf, frame_length);
	}
    return ;
}

uint32_t send_packed_fifo_data(fifo_s_t *pfifo, uint8_t sof)
{
  uint8_t  tx_buf[JUDGE_FIFO_BUFLEN];

  
  uint32_t fifo_count = fifo_used_count(pfifo); //统计一帧数据长度，最大128
  
  if (fifo_count)
  {
    fifo_s_gets(pfifo, tx_buf, fifo_count);
    
 if (sof == DN_REG_ID)
	    HAL_UART_Transmit(&JUDGE_HUART,tx_buf,fifo_count, 100); //tx
    else
      return 0;
  }
  
  return fifo_count;
}

/*
  brief：客户端删除图形 
  params in：
[1]类型选择："type_delete_layer" or "type_delete_all" or "type_delete_nop"
[2]图层选择：0~9

  ret：status
*/
Send_Judge_StatusTypedef client_delete_graphs(type_graphic_delete_e type, uint8_t layer)
{
	uint16_t frame_length;
	
	judge_send_mesg.graphic_delete_data.operate_tpye = type;
	judge_send_mesg.graphic_delete_data.layer = layer;
	
	
	data_packet_pack(ROBOT_INTERACTIVE_ID,                                 
				             CLIENT_DELETE_GRAPH_CMD_ID,
				             judge_recv_mesg.robot_state_data.robot_id,
				             Stu_Client_ID,
				             (uint8_t *)&judge_send_mesg.graphic_delete_data,
				             sizeof(ext_client_custom_graphic_delete_t),     
										 DN_REG_ID);
  
	frame_length = HEADER_LEN + CMD_LEN + sizeof(ext_student_interactive_header_data_t) +sizeof(ext_client_custom_graphic_delete_t) + CRC_LEN;
										 
	if(frame_length == sizeof(judge_txdata_fifo) )  //校验帧大小
	{
		send_packed_fifo_data(&judge_txdata_fifo, DN_REG_ID);
		return SEND_JUDGE_OK;
	}
	else
		return SEND_JUDGE_ERROR;
}

/*
  brief：定义一条直线
  params in：
[1]线条命名/索引号
[2]起点x坐标：start_x 
[3]起点y坐标：start_y 
[4]终点x坐标：end_x
[5]终点y坐标：end_y
[6]线条宽度：width
[7]图层选择：0~9
[8]颜色选择：9种颜色选择 graphic_color_e

  ret： graphic_data_struct_t
*/
 graphic_data_struct_t client_define_line(uint8_t graphic_name[3],
                                          uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
  graphic_data_struct_t res;
	memcpy(&res, graphic_name, sizeof(graphic_name[3])); //命名
	res.graphic_tpye = GRAPHIC_TYPE_LINE ;
	
	res.operate_tpye = 1; //默认添加
	res.layer = layer;
	res.color = color;
	res.width = width;
	res.start_x = start_x;
	res.start_y = start_y;
	
	res.end_x = end_x;
	res.end_y = end_y;
	
	return res;
}

/*
  brief：定义一个矩形
  params in：
[1]线条命名/索引号
[2]起点x坐标：start_x 
[3]起点y坐标：start_y 
[4]对顶角x坐标：end_x
[5]对顶角y坐标：end_y
[6]线条宽度：width
[7]图层选择：0~9
[8]颜色选择：9种颜色选择 graphic_color_e

  ret： graphic_data_struct_t
*/
 graphic_data_struct_t client_define_rect(uint8_t graphic_name[3],
                                          uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
  graphic_data_struct_t res;
	memcpy(&res, graphic_name, sizeof(graphic_name[3])); //命名
	res.graphic_tpye = GRAPHIC_TYPE_RECT ;
	
	res.operate_tpye = 1; //默认添加
	res.layer = layer;
	res.color = color;
	res.width = width;
	
	res.start_x = start_x;
	res.start_y = start_y;
	
	res.end_x = end_x;
	res.end_y = end_y;
	
	return res;
}

/*
  brief：定义一个正圆
  params in：
[1]线条命名/索引号
[2]圆心x坐标：point_x
[3]圆心y坐标：point_y
[4]半径：     radius
[5]线条宽度：width
[6]图层选择：0~9
[7]颜色选择：9种颜色选择 graphic_color_e

  ret： graphic_data_struct_t
*/
 graphic_data_struct_t client_define_circle(uint8_t graphic_name[3],
                                          uint16_t point_x,uint16_t point_y,uint16_t radius,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
  graphic_data_struct_t res;
	memcpy(&res, graphic_name, sizeof(graphic_name[3])); //命名
	res.graphic_tpye = GRAPHIC_TYPE_CIRCLE ;
	
	res.operate_tpye = 1; //默认添加
	res.layer = layer;
	res.color = color;
	
	res.width = width;
	
	res.start_x = point_x;
	res.start_y = point_y;
	res.radius = radius;
	
	return res;
}

/*
  brief：定义一条圆弧
  params in：
[1]线条命名/索引号
[2]圆心x坐标：point_x
[3]圆心y坐标：point_y
[4]x半轴长度：x_axis
[5]y半轴长度：y_axis
[6]起始角度： start_angle
[7]终止角度： end_angle
[8]线条宽度：width
[9]图层选择：0~9
[10]颜色选择：9种颜色选择 graphic_color_e

  ret： graphic_data_struct_t
*/
 graphic_data_struct_t client_define_arc(uint8_t graphic_name[3],
                                          uint16_t point_x,uint16_t point_y,uint16_t x_axis, uint16_t y_axis,
                                          uint16_t start_angle, uint16_t end_angle,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
  graphic_data_struct_t res;
	memcpy(&res, graphic_name, sizeof(graphic_name[3])); //命名
	res.graphic_tpye = GRAPHIC_TYPE_ARC ;
	res.operate_tpye = 1; //默认添加
	
	res.layer = layer;
	res.color = color;
	res.width = width;
	
	res.start_x = point_x;
	res.start_y = point_y;
	res.end_x   = x_axis;
	res.end_y   = y_axis;
	res.start_angle = start_angle;
  res.end_angle   = end_angle;
	
	return res;
}

/*
  brief：定义一个浮点数
  params in：
[1]线条命名/索引号
[2]圆心x坐标：point_x
[3]圆心y坐标：point_y
[4]半径：     radius
[5]线条宽度：width
[6]图层选择：0~9
[7]颜色选择：9种颜色选择 graphic_color_e

  ret： graphic_data_struct_t
*/





/*
  brief：客户端画直线 
  params in：
[1]线条命名/索引号
[2]起点x坐标：start_x 
[3]起点y坐标：start_y 
[4]终点x坐标：end_x
[5]终点y坐标：end_y
[6]线条宽度：width
[7]图层选择：0~9
[8]颜色选择：9种颜色选择 graphic_color_e

  ret：status
*/
Send_Judge_StatusTypedef client_draw_line(uint8_t graphic_name[3],
                                          uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
	uint16_t frame_length;
	
	memset(&judge_send_mesg.graphic_data, 0, sizeof(graphic_data_struct_t)); //初始化图形数据
	
	judge_send_mesg.graphic_data = client_define_line(graphic_name,
                                           start_x, start_y, end_x, end_y,
                                           width,  layer,  color);
	
	judge_send_mesg.graphic_data.operate_tpye = 1; //添加
	
  data_packet_pack(ROBOT_INTERACTIVE_ID,                                 
				             CLIENT_DRAW_1_GRAPH_CMD_ID,
				             judge_recv_mesg.robot_state_data.robot_id,
				             Stu_Client_ID,
				             (uint8_t *)&judge_send_mesg.graphic_data,
				             sizeof(graphic_data_struct_t),     
										 DN_REG_ID);
										 
	frame_length = HEADER_LEN + CMD_LEN + sizeof(ext_student_interactive_header_data_t) +sizeof(graphic_data_struct_t) + CRC_LEN;		
										 
	if(frame_length == sizeof(judge_txdata_fifo) )  //校验帧大小
	{
		send_packed_fifo_data(&judge_txdata_fifo, DN_REG_ID);
		return SEND_JUDGE_OK;
	}
	else
		return SEND_JUDGE_ERROR;									 

}


