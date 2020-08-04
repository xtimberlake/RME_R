#include "judge.h"
#include "string.h"
#include "usart.h"



#if JUDGE_VERSION == JUDGE_2020
/*****************���ݽṹ�嶨��**********************/
frame_header											FrameHeader;		//֡ͷ

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

int Color;//��ǰ�����˵���Ӫ
uint8_t Robot_Self_ID;//��ǰ�����˵�ID
uint16_t Judge_Client_ID;//�����߻����˶�Ӧ�Ŀͻ���ID


/**
  * @brief  ��ȡ�������ݺ����������жϺ�����ֱ�ӵ��ý��ж�ȡ
  * @param  ��Ӧ���ڵĻ���������
  * @retval �Ƿ�������ж�������
  * @attention  �ڴ��ж�֡ͷ��CRCУ��,������д�����ݣ����ظ��ж�֡ͷ
  */
int Judge_Read_Data(uint8_t *ReadFromUsart)
{
	uint8_t  retval_tf = FALSE; //������ʵ�Ա�־λ,ÿ�ζ�ȡʱ��Ĭ��ΪFALSE
	uint16_t judge_length;			//�����ֽڳ���
	int      CmdID=0;				
	if(ReadFromUsart == NULL)
	{
		return -1;
	}
	
	memcpy(&FrameHeader, ReadFromUsart, LEN_HEADER);  //д��֡ͷ
	//�ж�֡ͷ�����Ƿ�Ϊ0xA5
	if(ReadFromUsart[ SOF ] == JUDGE_FRAME_HEADER)
	{
		//֡ͷCRC8У��
		if (Verify_CRC8_Check_Sum( ReadFromUsart, LEN_HEADER ) == TRUE)
		{
			//ͳ��һ֡���ݳ���,����CR16У��
			judge_length = ReadFromUsart[ DATA_LENGTH ] + LEN_HEADER + LEN_CMDID + LEN_TAIL;;

			//֡βCRC16У��
			if(Verify_CRC16_Check_Sum(ReadFromUsart,judge_length) == TRUE)
			{
				retval_tf = TRUE;//��У�������˵�����ݿ���
				
				CmdID = (ReadFromUsart[6] << 8 | ReadFromUsart[5]);
				//��������������,�����ݿ�������Ӧ�ṹ����(ע�⿽�����ݵĳ���)
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
					
//						if(RobotHurt.hurt_type == 0)//��װ�װ���������˺�
//						{	Hurt_Data_Update = TRUE;	}//װ������ÿ����һ�����ж�Ϊ�ܵ�һ���˺�
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
//						JUDGE_ShootNumCount();//������ͳ
//					break;
				}
			}
		}
		//�׵�ַ��֡����,ָ��CRC16��һ�ֽ�,�����ж��Ƿ�Ϊ0xA5,�����ж�һ�����ݰ��Ƿ��ж�֡����
		if(*(ReadFromUsart + sizeof(FrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL) == 0xA5)
		{
			//���һ�����ݰ������˶�֡����,���ٴζ�ȡ
			Judge_Read_Data(ReadFromUsart + sizeof(FrameHeader) + LEN_CMDID + FrameHeader.DataLength + LEN_TAIL);
		}
	}
	return retval_tf;
}
	

/**
	* @brief  �����Զ������ݵ����Կͻ���
  * @param  void
  * @retval void
  * @attention  ���ݴ��,�����ɺ�ͨ�����ڷ��͵�����ϵͳ
  */
#define send_max_len     200
unsigned char CliendTxBuffer[send_max_len];
void judge_send_task(void const *argu)
{
	
//	determine_ID();//�жϷ�����ID�����Ӧ�Ŀͻ���ID
	
	Judgesend_Data.txFrameHeader.SOF = 0xA5;
	Judgesend_Data.txFrameHeader.DataLength = sizeof(ext_judgesend_custom_header_t) + sizeof(graphic_data_struct_t);
	Judgesend_Data.txFrameHeader.Seq = 0;
	memcpy(CliendTxBuffer, &Judgesend_Data.txFrameHeader, sizeof(frame_header));//д��֡ͷ����
	Append_CRC8_Check_Sum((uint8_t *)CliendTxBuffer, sizeof(frame_header));//д��֡ͷCRC8У����
	
	Judgesend_Data.CmdID = ID_robot_interract;	

	Judgesend_Data.drawFrameHeader.send_ID 	 = 3;//�����ߵ�ID(������3)
	Judgesend_Data.drawFrameHeader.receiver_ID = 0x0103;//�ͻ��˵�ID��ֻ��Ϊ�����߻����˶�Ӧ�Ŀͻ���(������3)
//	Judgesend_Data.drawFrameHeader.send_ID 	 = Judge_Client_ID;//�����ߵ�ID
//	Judgesend_Data.drawFrameHeader.receiver_ID = Judge_Client_ID;//�ͻ��˵�ID��ֻ��Ϊ�����߻����˶�Ӧ�Ŀͻ���
	
	/*- �Զ������� -*/
	Judgesend_Data.drawFrameHeader.data_cmd_id = CLIENT_DRAW_1_GRAPH_CMD_ID;//�ͻ��˻���һ��ͼ��,�ٷ��̶�
//	strcpy(Judgesend_Data.clientDraw.graphic_name,"abc");
	memcpy(&Judgesend_Data.clientDraw.graphic_name, "abc", sizeof("abc")); //����
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
	
	//���д�����ݶ�
	memcpy(	
			CliendTxBuffer + 5, 
			(uint8_t*)&Judgesend_Data.CmdID, 
			(sizeof(Judgesend_Data.CmdID)+ sizeof(Judgesend_Data.drawFrameHeader)+ sizeof(Judgesend_Data.clientDraw))
		  );				
	Append_CRC16_Check_Sum(CliendTxBuffer,sizeof(Judgesend_Data));//д�����ݶ�CRC16У����	
	HAL_UART_Transmit_DMA(&huart3,CliendTxBuffer,sizeof(Judgesend_Data));

}


/**
  * @brief  �ж��Լ�������
  * @param  void
  * @retval RED   BLUE
  * @attention  ���ݴ��,�����ɺ�ͨ�����ڷ��͵�����ϵͳ
  */
int determine_red_blue(void)
{
	Robot_Self_ID = Game_Robot_Status.robot_id;//��ȡ��ǰ������ID
	
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
  * @brief  �ж�����ID��ѡ��ͻ���ID
  * @param  void
  * @retval RED   BLUE
  * @attention  ���ݴ��,�����ɺ�ͨ�����ڷ��͵�����ϵͳ
  */
void determine_ID(void)
{
	Color = determine_red_blue();
	if(Color == BLUE)
	{
		Judge_Client_ID = 0x0164 + (Robot_Self_ID-100);//����ͻ���ID
	}
	else if(Color == RED)
	{
		Judge_Client_ID = 0x0100 + Robot_Self_ID;//����ͻ���ID
	}
}

/**
  * @brief  �ͻ���ɾ��ͼ��
  * @param  ����ѡ��0: �ղ�����
	*										1: ɾ��ͼ�㣻
	*										2: ɾ�����У�
  * @param  ͼ��ѡ��0~9
  * @attention  
  */
void data_pack_delete(type_graphic_delete_e type,uint8_t layer)
{
	Judgesend_Data.drawFrameHeader.data_cmd_id = CLIENT_DELETE_GRAPH_CMD_ID;
	Delete_data.operate_tpye = type;
	Delete_data.layer = layer;
}
/**
  * @brief  �ͻ��˻����ַ�
  * @param  �����С
  * @param  �ַ�����
  * @param  �������
  * @param  ���x����
  * @param  ���y����
  * @param  �ַ�
  * @attention  
  */
void data_pack_code()
{
	Judgesend_Data.drawFrameHeader.data_cmd_id = CLIENT_WRITE_STRINGS_CMD_ID;
}
#endif
