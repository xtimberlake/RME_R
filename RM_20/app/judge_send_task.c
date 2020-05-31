/** 
  * @file judge_send_task.c
  * @version 1.0
  * @date    2019.5.4
  * @brief 
  *	@author link
	
	
	* @version 1.9
  * @date    May 24th, 2020
  * @brief   ��Ӽ����������ͻ���UI���ͺ���
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
  
  uint16_t frame_length = HEADER_LEN + CMD_LEN + len + CRC_LEN; //ͳ��һ֡����
  frame_header_t *p_header = (frame_header_t*)tx_buf; //֡ͷ 5 byte
  
  p_header->sof          = sof;
  p_header->data_length  = len;
  
  
  if (sof == UP_REG_ID) //����Ŵ���
  {
    if (seq++ >= 255)
      seq = 0;
    
    p_header->seq = seq;
  }
  else
  {
    p_header->seq = 0;
  }
  
  //����CRCУ����
  memcpy(&tx_buf[HEADER_LEN], (uint8_t*)&cmd_id, CMD_LEN);
  append_crc8_check_sum(tx_buf, HEADER_LEN);
  memcpy(&tx_buf[HEADER_LEN + CMD_LEN], p_data, len);
  append_crc16_check_sum(tx_buf, frame_length);
  
  return tx_buf;
}

/*

	brief������ϵͳ������Ϣ���
  params��

[1]cmd_id:    0x0301

ͷ�ṹ���壺 6 byte
[2]data_cmd:  ��������id:��ѡ�񼺷�������ͨ�ţ������ڿͻ��˻�ͼ��ɾͼ��д�ַ���
[3]robot_id:  ������ID
[4]client_id: ������ID

[5]p_data:    ����
[6]len:       ���ݳ��ȣ����113��������ͷ�ṹ��
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

  
  uint32_t fifo_count = fifo_used_count(pfifo); //ͳ��һ֡���ݳ��ȣ����128
  
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
  brief���ͻ���ɾ��ͼ�� 
  params in��
[1]����ѡ��"type_delete_layer" or "type_delete_all" or "type_delete_nop"
[2]ͼ��ѡ��0~9

  ret��status
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
										 
	if(frame_length == sizeof(judge_txdata_fifo) )  //У��֡��С
	{
		send_packed_fifo_data(&judge_txdata_fifo, DN_REG_ID);
		return SEND_JUDGE_OK;
	}
	else
		return SEND_JUDGE_ERROR;
}

/*
  brief������һ��ֱ��
  params in��
[1]��������/������
[2]���x���꣺start_x 
[3]���y���꣺start_y 
[4]�յ�x���꣺end_x
[5]�յ�y���꣺end_y
[6]������ȣ�width
[7]ͼ��ѡ��0~9
[8]��ɫѡ��9����ɫѡ�� graphic_color_e

  ret�� graphic_data_struct_t
*/
 graphic_data_struct_t client_define_line(uint8_t graphic_name[3],
                                          uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
  graphic_data_struct_t res;
	memcpy(&res, graphic_name, sizeof(graphic_name[3])); //����
	res.graphic_tpye = GRAPHIC_TYPE_LINE ;
	
	res.operate_tpye = 1; //Ĭ�����
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
  brief������һ������
  params in��
[1]��������/������
[2]���x���꣺start_x 
[3]���y���꣺start_y 
[4]�Զ���x���꣺end_x
[5]�Զ���y���꣺end_y
[6]������ȣ�width
[7]ͼ��ѡ��0~9
[8]��ɫѡ��9����ɫѡ�� graphic_color_e

  ret�� graphic_data_struct_t
*/
 graphic_data_struct_t client_define_rect(uint8_t graphic_name[3],
                                          uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
  graphic_data_struct_t res;
	memcpy(&res, graphic_name, sizeof(graphic_name[3])); //����
	res.graphic_tpye = GRAPHIC_TYPE_RECT ;
	
	res.operate_tpye = 1; //Ĭ�����
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
  brief������һ����Բ
  params in��
[1]��������/������
[2]Բ��x���꣺point_x
[3]Բ��y���꣺point_y
[4]�뾶��     radius
[5]������ȣ�width
[6]ͼ��ѡ��0~9
[7]��ɫѡ��9����ɫѡ�� graphic_color_e

  ret�� graphic_data_struct_t
*/
 graphic_data_struct_t client_define_circle(uint8_t graphic_name[3],
                                          uint16_t point_x,uint16_t point_y,uint16_t radius,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
  graphic_data_struct_t res;
	memcpy(&res, graphic_name, sizeof(graphic_name[3])); //����
	res.graphic_tpye = GRAPHIC_TYPE_CIRCLE ;
	
	res.operate_tpye = 1; //Ĭ�����
	res.layer = layer;
	res.color = color;
	
	res.width = width;
	
	res.start_x = point_x;
	res.start_y = point_y;
	res.radius = radius;
	
	return res;
}

/*
  brief������һ��Բ��
  params in��
[1]��������/������
[2]Բ��x���꣺point_x
[3]Բ��y���꣺point_y
[4]x���᳤�ȣ�x_axis
[5]y���᳤�ȣ�y_axis
[6]��ʼ�Ƕȣ� start_angle
[7]��ֹ�Ƕȣ� end_angle
[8]������ȣ�width
[9]ͼ��ѡ��0~9
[10]��ɫѡ��9����ɫѡ�� graphic_color_e

  ret�� graphic_data_struct_t
*/
 graphic_data_struct_t client_define_arc(uint8_t graphic_name[3],
                                          uint16_t point_x,uint16_t point_y,uint16_t x_axis, uint16_t y_axis,
                                          uint16_t start_angle, uint16_t end_angle,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
  graphic_data_struct_t res;
	memcpy(&res, graphic_name, sizeof(graphic_name[3])); //����
	res.graphic_tpye = GRAPHIC_TYPE_ARC ;
	res.operate_tpye = 1; //Ĭ�����
	
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
  brief������һ��������
  params in��
[1]��������/������
[2]Բ��x���꣺point_x
[3]Բ��y���꣺point_y
[4]�뾶��     radius
[5]������ȣ�width
[6]ͼ��ѡ��0~9
[7]��ɫѡ��9����ɫѡ�� graphic_color_e

  ret�� graphic_data_struct_t
*/





/*
  brief���ͻ��˻�ֱ�� 
  params in��
[1]��������/������
[2]���x���꣺start_x 
[3]���y���꣺start_y 
[4]�յ�x���꣺end_x
[5]�յ�y���꣺end_y
[6]������ȣ�width
[7]ͼ��ѡ��0~9
[8]��ɫѡ��9����ɫѡ�� graphic_color_e

  ret��status
*/
Send_Judge_StatusTypedef client_draw_line(uint8_t graphic_name[3],
                                          uint16_t start_x,uint16_t start_y,uint16_t end_x,uint16_t end_y,
                                          uint16_t width, uint8_t layer, graphic_color_e color)
{
	uint16_t frame_length;
	
	memset(&judge_send_mesg.graphic_data, 0, sizeof(graphic_data_struct_t)); //��ʼ��ͼ������
	
	judge_send_mesg.graphic_data = client_define_line(graphic_name,
                                           start_x, start_y, end_x, end_y,
                                           width,  layer,  color);
	
	judge_send_mesg.graphic_data.operate_tpye = 1; //���
	
  data_packet_pack(ROBOT_INTERACTIVE_ID,                                 
				             CLIENT_DRAW_1_GRAPH_CMD_ID,
				             judge_recv_mesg.robot_state_data.robot_id,
				             Stu_Client_ID,
				             (uint8_t *)&judge_send_mesg.graphic_data,
				             sizeof(graphic_data_struct_t),     
										 DN_REG_ID);
										 
	frame_length = HEADER_LEN + CMD_LEN + sizeof(ext_student_interactive_header_data_t) +sizeof(graphic_data_struct_t) + CRC_LEN;		
										 
	if(frame_length == sizeof(judge_txdata_fifo) )  //У��֡��С
	{
		send_packed_fifo_data(&judge_txdata_fifo, DN_REG_ID);
		return SEND_JUDGE_OK;
	}
	else
		return SEND_JUDGE_ERROR;									 

}


