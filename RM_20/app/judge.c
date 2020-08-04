#include "judge.h"
#include "string.h"
#include "usart.h"



#if JUDGE_VERSION == JUDGE_2020
/*****************数据结构体定义**********************/
frame_header											FrameHeader;		//帧头

ext_game_state_t									Game_State;
ext_game_result_t									Game_Result;
ext_game_robot_HP_t               RobotHP;
ext_dart_status_t									Dart_Status;
ext_event_data_t									Event_Data;
ext_supply_projectile_action_t		Supply_Projectile_Action;
ext_referee_warning_t							Referee_Warning;
ext_dart_remaining_time_t					Dart_Remaining_Time;
ext_game_robot_status_t						Game_Robot_Status;
ext_power_heat_data_t							Power_Heat_Data;
ext_game_robot_pos_t							Robot_Position;
ext_buff_t												Buff;
ext_aerial_robot_energy_t					Aerial_Robot_Energy;
ext_robot_hurt_t									Robot_Hurt;
ext_shoot_data_t									Shoot_Data;
ext_bullet_remaining_t						Bullet_Remaining;
ext_rfid_status_t									RFID_Status;
ext_dart_client_cmd_t							Dart_Client;

ext_SendClientDraw_t							Judgesend_Data;
ext_SendClientDelete_t						Delete_data;
ext_SendClientStrings_t						Judgesend_strings_Data;	
/******************************************************/

int Color;//当前机器人的阵营
uint8_t Robot_Self_ID;//当前机器人的ID
uint16_t Judge_Client_ID;//发送者机器人对应的客户端ID


/**
  * @brief  读取裁判数据函数，串口中断函数中直接调用进行读取
  * @param  对应串口的缓存区数据
  * @retval 是否对正误判断做处理
  * @attention  在此判断帧头和CRC校验,无误再写入数据，不重复判断帧头
  */
int Judge_Read_Data(uint8_t *ReadFromUsart)
{
	uint8_t  retval_tf = FALSE; //数据真实性标志位,每次读取时都默认为FALSE
	uint16_t judge_length;			//数据字节长度
	int      CmdID=0;				
	if(ReadFromUsart == NULL)
	{
		return -1;
	}
	
	memcpy(&FrameHeader, ReadFromUsart, LEN_HEADER);  //写入帧头
	//判断帧头数据是否为0xA5
	if(ReadFromUsart[ SOF ] == JUDGE_FRAME_HEADER)
	{
		//帧头CRC8校验
		if (Verify_CRC8_Check_Sum( ReadFromUsart, LEN_HEADER ) == TRUE)
		{
			//统计一帧数据长度,用于CR16校验
			judge_length = ReadFromUsart[ DATA_LENGTH ] + LEN_HEADER + LEN_CMDID + LEN_TAIL;;

			//帧尾CRC16校验
			if(Verify_CRC16_Check_Sum(ReadFromUsart,judge_length) == TRUE)
			{
				retval_tf = TRUE;//都校验过了则说明数据可用
				
				CmdID = (ReadFromUsart[6] << 8 | ReadFromUsart[5]);
				//解析数据命令码,将数据拷贝到相应结构体中(注意拷贝数据的长度)
				switch(CmdID)
				{
					case ID_game_state:     //0x0001
						memcpy(&Game_State, (ReadFromUsart + DATA), LEN_game_state);
					break;
					
					case ID_game_result:    //0x0002
						memcpy(&Game_Result, (ReadFromUsart + DATA), LEN_game_result);
					break;
					
					case ID_robot_HP:       //0x0003
						memcpy(&RobotHP, (ReadFromUsart + DATA), LEN_robot_HP);
					break;
					
					case ID_darts_status:		//0x0004
						memcpy(&Dart_Status, (ReadFromUsart + DATA), LEN_darts_status);
					break;
					
					case ID_event_data:    	 //0x0101
						memcpy(&Event_Data, (ReadFromUsart + DATA), LEN_event_data);
					break;
					
					case ID_supply_action:   //0x0102
						memcpy(&Supply_Projectile_Action, (ReadFromUsart + DATA), LEN_supply_action);
					break;
					
					case ID_judge_warning:  	//0x0104
						memcpy(&Referee_Warning, (ReadFromUsart + DATA), LEN_judge_warning);
					break;
					
					case ID_dart_remaining_time:  //0x0105
						memcpy(&Dart_Remaining_Time, (ReadFromUsart + DATA), LEN_darts_remaining_tim);
					break;
					
					case ID_robot_status:     //0x0201
						memcpy(&Game_Robot_Status, (ReadFromUsart + DATA), LEN_robot_status);
					break;
					
					case ID_robot_power:      //0x0202
						memcpy(&Power_Heat_Data, (ReadFromUsart + DATA), LEN_robot_power);
					break;
					
					case ID_robot_position:   //0x0203
						memcpy(&Robot_Position, (ReadFromUsart + DATA), LEN_robot_position);
					break;
					
					case ID_robot_buff:      	//0x0204
						memcpy(&Buff, (ReadFromUsart + DATA), LEN_robot_buff);
					break;
					
					case ID_AerialRobotEnergy:   //0x0205
						memcpy(&Aerial_Robot_Energy, (ReadFromUsart + DATA), LEN_AerialRobotEnergy);
					break;
					
					case ID_robot_hurt:      		//0x0206
						memcpy(&Robot_Hurt, (ReadFromUsart + DATA), LEN_robot_hurt);
					
//						if(RobotHurt.hurt_type == 0)//非装甲板离线造成伤害
//						{	Hurt_Data_Update = TRUE;	}//装甲数据每更新一次则判定为受到一次伤害
					break;
					
					case ID_shoot_data:      			//0x0207
						memcpy(&Shoot_Data, (ReadFromUsart + DATA), LEN_shoot_data);
					break;
					
					case ID_bullet_remaining:     //0x0208
						memcpy(&Bullet_Remaining, (ReadFromUsart + DATA), LEN_bullet_remaining);
					break;
					
					case ID_RFID_status:      		//0x0209
						memcpy(&RFID_Status, (ReadFromUsart + DATA), LEN_RFID_status);
					break;
					
					case ID_dart_client:      		//0x020A
						memcpy(&Dart_Client, (ReadFromUsart + DATA), LEN_dart_client);
					break;
					
//					case ID_robot_interract:      			//0x0207
//						memcpy(&ShootData, (ReadFromUsart + DATA), LEN_shoot_data);
//						JUDGE_ShootNumCount();//发弹量统
//					break;
				}
			}
		}
		//首地址加帧长度,指向CRC16下一字节,用来判断是否为0xA5,用来判断一个数据包是否有多帧数据
		if(*(ReadFromUsart + sizeof(FrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL) == 0xA5)
		{
			//如果一个数据包出现了多帧数据,则再次读取
			Judge_Read_Data(ReadFromUsart + sizeof(FrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL);
		}
	}
	return retval_tf;
}
	

/**
	* @brief  发送自定义数据到电脑客户端
  * @param  void
  * @retval void
  * @attention  数据打包,打包完成后通过串口发送到裁判系统
  */
#define send_max_len     200
unsigned char CliendTxBuffer[send_max_len];
void judge_send_task(void const *argu)
{
	
//	determine_ID();//判断发送者ID和其对应的客户端ID
	
	Judgesend_Data.txFrameHeader.SOF = 0xA5;
	Judgesend_Data.txFrameHeader.DataLength = sizeof(ext_judgesend_custom_header_t) + sizeof(graphic_data_struct_t);
	Judgesend_Data.txFrameHeader.Seq = 0;
	memcpy(CliendTxBuffer, &Judgesend_Data.txFrameHeader, sizeof(frame_header));//写入帧头数据
	Append_CRC8_Check_Sum((uint8_t *)CliendTxBuffer, sizeof(frame_header));//写入帧头CRC8校验码
	
	Judgesend_Data.CmdID = ID_robot_interract;	

	Judgesend_Data.drawFrameHeader.send_ID 	 = 3;//发送者的ID(步兵红3)
	Judgesend_Data.drawFrameHeader.receiver_ID = 0x0103;//客户端的ID，只能为发送者机器人对应的客户端(步兵红3)
//	Judgesend_Data.drawFrameHeader.send_ID 	 = Judge_Client_ID;//发送者的ID
//	Judgesend_Data.drawFrameHeader.receiver_ID = Judge_Client_ID;//客户端的ID，只能为发送者机器人对应的客户端
	
	/*- 自定义内容 -*/
	Judgesend_Data.drawFrameHeader.data_cmd_id = CLIENT_DRAW_1_GRAPH_CMD_ID;//客户端绘制一个图形,官方固定
//	strcpy(Judgesend_Data.clientDraw.graphic_name,"abc");
	memcpy(&Judgesend_Data.clientDraw.graphic_name, "abc", sizeof("abc")); //命名
	Judgesend_Data.clientDraw.operate_tpye=1;
	Judgesend_Data.clientDraw.graphic_tpye=2;
	Judgesend_Data.clientDraw.layer=1;
	Judgesend_Data.clientDraw.color=2;
//	Judgesend_Data.clientDraw.start_angle=0;
//	Judgesend_Data.clientDraw.end_angle=0;
	Judgesend_Data.clientDraw.width=10;
	Judgesend_Data.clientDraw.start_x=540;
	Judgesend_Data.clientDraw.start_y=540;
	Judgesend_Data.clientDraw.radius=100;
//	Judgesend_Data.clientDraw.end_x=0;
//	Judgesend_Data.clientDraw.end_y=0;	
	/*--------------*/
	
	//打包写入数据段
	memcpy(	
			CliendTxBuffer + 5, 
			(uint8_t*)&Judgesend_Data.CmdID, 
			(sizeof(Judgesend_Data.CmdID)+ sizeof(Judgesend_Data.drawFrameHeader)+ sizeof(Judgesend_Data.clientDraw))
		  );				
	Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(Judgesend_Data));//写入数据段CRC16校验码	
	HAL_UART_Transmit_DMA(&huart3,CliendTxBuffer,sizeof(Judgesend_Data));

}


/**
  * @brief  判断自己红蓝方
  * @param  void
  * @retval RED   BLUE
  * @attention  数据打包,打包完成后通过串口发送到裁判系统
  */
int determine_red_blue(void)
{
	Robot_Self_ID = Game_Robot_Status.robot_id;//读取当前机器人ID
	
	if(Game_Robot_Status.robot_id > 100)
	{
		return BLUE;
	}
	else 
	{
		return RED;
	}
}
/**
  * @brief  判断自身ID，选择客户端ID
  * @param  void
  * @retval RED   BLUE
  * @attention  数据打包,打包完成后通过串口发送到裁判系统
  */
void determine_ID(void)
{
	Color = determine_red_blue();
	if(Color == BLUE)
	{
		Judge_Client_ID = 0x0164 + (Robot_Self_ID-100);//计算客户端ID
	}
	else if(Color == RED)
	{
		Judge_Client_ID = 0x0100 + Robot_Self_ID;//计算客户端ID
	}
}

/**
  * @brief  客户端删除图形
  * @param  类型选择：0: 空操作；
	*										1: 删除图层；
	*										2: 删除所有；
  * @param  图层选择：0~9
  * @attention  
  */
void data_pack_delete(type_graphic_delete_e type,uint8_t layer)
{
	Judgesend_Data.drawFrameHeader.data_cmd_id = CLIENT_DELETE_GRAPH_CMD_ID;
	Delete_data.operate_tpye = type;
	Delete_data.layer = layer;
}
/**
  * @brief  客户端绘制字符
  * @param  字体大小
  * @param  字符长度
  * @param  线条宽度
  * @param  起点x坐标
  * @param  起点y坐标
  * @param  字符
  * @attention  
  */
void data_pack_code()
{
	Judgesend_Data.drawFrameHeader.data_cmd_id = CLIENT_WRITE_STRINGS_CMD_ID;
}
#endif
