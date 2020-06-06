/****************************************************************************
 *  Copyright (C) 2018 RoboMaster.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program. If not, see <http://www.gnu.org/licenses/>.
 ***************************************************************************/
/** @file judgement_info.h
 *  @version 1.0
 *  @date Feb 2019
 *
 *  @brief the information from judgement system
 *
 *  @copyright 2019 DJI RoboMaster. All rights reserved.
 *
 */

#ifndef __JUDGEMENT_INFO_H__
#define __JUDGEMENT_INFO_H__

#include "stm32f4xx_hal.h"
#include "judge_send_task.h"


#define JUDGE_FIFO_BUFLEN 500
#define S2019 2019
#define S2020 2020

#define SEASON_SELECT S2020




#if SEASON_SELECT == S2019

/** 
  * @brief  judgement data command id
  */
typedef enum
{
  GAME_STATE_INFO_ID      = 0x0001, //比赛状态数据，1Hz 周期发送
  GAME_RESULT_ID          = 0x0002, //比赛结果数据，比赛结束后
  GAME_ROBOT_LIVE_ID      = 0x0003, //比赛机器人存活数据，1Hz 发送
	
  FIELD_EVENT_DATA_ID     = 0x0101, //场地事件数据，事件改变后
	FIELD_SUPPY_ACTION_ID   = 0x0102, //场地补给站动作标识数据，动作改变后发送
	FIELD_SUPPY_BOOKING_ID  = 0X0103, //场地补给站预约子弹数据，由参赛队发送
	
  ROBOT_STATE_DATA_ID     = 0x0201, //机器人状态数据,10Hz 周期发送
  REAL_POWER_HEAT_DATA_ID = 0x0202, //实时功率热量数据，50Hz 周期发送
	ROBOT_POS_DATA_ID       = 0x0203, //机器人位置数据，10Hz 发送
	ROBOT_GAIN_BUFF_ID      = 0x0204, //机器人增益数据,增益状态改变后发送
	DRONE_ENERGY_DATA_ID    = 0X0205, //空中机器人能量状态数据，10Hz 周期发送，只有空中机器人主控发送
	REAL_BLOOD_DATA_ID      = 0X0206, //伤害状态数据，伤害发生后发送
  REAL_SHOOT_DATA_ID      = 0X0207, //实时射击数据，子弹发射后发送
	
  ROBOT_INTERACTIVE_ID    = 0x0301, //机器人间交互数据 、客户端自定义数据，上限 10Hz
	STU_CUSTOM_DATA_CMD_ID  = 0xD180, //客户端自定义数据内容ID
} judge_data_id_e;

typedef enum
{
	//Red
  Hero_R1     = 1,
	Engineer_R2 = 2,
	Standard_R3 = 3,
	Standard_R4 = 4,
	Standard_R5 = 5,
	Aerial_R6   = 6,
	Sentry_R7   = 7,
	//Blue
	Hero_B1     = 11,
	Engineer_B2 = 12,
	Standard_B3 = 13,
	Standard_B4 = 14,
	Standard_B5 = 15,
	Aerial_B6   = 16,
	Sentry_B7   = 17,
} robot_id_e;

typedef enum
{
	//Red
  Hero_CLient_R1     = 0x0101,
	Engineer_CLient_R2 = 0x0102,
	Standard_CLient_R3 = 0x0103,
	Standard_CLient_R4 = 0x0104,
	Standard_CLient_R5 = 0x0105,
	Aerial_CLient_R6   = 0x0106,
	//Blue
	Hero_CLient_B1     = 0x0111,
	Engineer_CLient_B2 = 0x0112,
	Standard_CLient_B3 = 0x0113,
	Standard_CLient_B4 = 0x0114,
	Standard_CLient_B5 = 0x0115,
	Aerial_CLient_B6   = 0x0116,
} stu_client_id_e;

//0x001
typedef __packed struct
{
 uint8_t game_type : 4;
 uint8_t game_progress : 4;
 uint16_t stage_remain_time;
} ext_game_state_t;

//0x002
typedef __packed struct
{
 uint8_t winner;
} ext_game_result_t;

//0x003
typedef __packed struct
{
 uint16_t robot_legion;
} ext_game_robot_survivors_t;

//0x0101
typedef __packed struct
{
 uint32_t event_type;
} ext_event_data_t;


//0x0102
typedef __packed struct
{
 uint8_t supply_projectile_id; 
 uint8_t supply_robot_id; 
 uint8_t supply_projectile_step; 
 uint8_t supply_projectile_num;
} ext_supply_projectile_action_t;


//0x0103
typedef __packed struct
{
  uint8_t supply_projectile_id;
  uint8_t supply_robot_id; 
  uint8_t supply_num;
} ext_supply_projectile_booking_t;


typedef __packed struct
{
 uint8_t robot_id;
 uint8_t robot_level;
 uint16_t remain_HP;
 uint16_t max_HP;
 uint16_t shooter_heat0_cooling_rate;
 uint16_t shooter_heat0_cooling_limit;
 uint16_t shooter_heat1_cooling_rate;
 uint16_t shooter_heat1_cooling_limit;
 uint8_t mains_power_gimbal_output : 1;
 uint8_t mains_power_chassis_output : 1;
 uint8_t mains_power_shooter_output : 1;
} ext_game_robot_state_t;

//0x0202
typedef __packed struct
{
 uint16_t chassis_volt; 
 uint16_t chassis_current; 
 float chassis_power; 
 uint16_t chassis_power_buffer; 
 uint16_t shooter_heat0; 
 uint16_t shooter_heat1; 
} ext_power_heat_data_t;

//0x0203
typedef __packed struct
{
 float x;
 float y;
 float z;
 float yaw;
} ext_game_robot_pos_t;

//0x0204
typedef __packed struct
{ 
 uint8_t power_rune_buff;    // bit 1：枪口热量冷却加速
}ext_buff_musk_t;

//0x0205
typedef __packed struct
{
 uint8_t energy_point;
 uint8_t attack_time;
} aerial_robot_energy_t;

//0x0206
typedef __packed struct
{
 uint8_t armor_id : 4;
 uint8_t hurt_type : 4;
} ext_robot_hurt_t;

//0x0207
typedef __packed struct
{
 uint8_t bullet_type;
 uint8_t bullet_freq;
 float bullet_speed;
} ext_shoot_data_t;

//0x0301
typedef __packed struct
{
 uint16_t data_cmd_id;
 uint16_t send_ID;
 uint16_t receiver_ID;
}ext_student_interactive_header_data_t;

//cmd_id:0x0301。内容ID:0xD180
typedef __packed struct
{
 float data1;
 float data2;
 float data3;
 uint8_t masks;
}client_custom_data_t;



typedef __packed struct
{
 uint16_t data_cmd_id;
 uint16_t send_id;
	uint16_t receiver_id;
	
}ext_student_interative_header_data_t;
/** 
  * @brief  the data structure receive from judgement
  */
typedef struct
{
  ext_game_state_t                game_information;      //0x0001
	ext_game_result_t               game_result_data;      //0x0002
	ext_game_robot_survivors_t      robot_survivors_data;  //0x0003
	
	ext_event_data_t                field_event_data;      //0x0101
	ext_supply_projectile_action_t  supply_action_data;    //0x0102
	ext_supply_projectile_booking_t supply_booking_data;   //0x0103

	ext_game_robot_state_t          robot_state_data;      //0x0201
	ext_power_heat_data_t           power_heat_data;       //0x0202
	ext_game_robot_pos_t            robot_pos_data;        //0x0203
	ext_buff_musk_t                 get_buff_data;         //0x0204
	aerial_robot_energy_t           drone_energy_data;     //0x0205
	ext_robot_hurt_t                blood_changed_data;    //0x0206
	ext_shoot_data_t                real_shoot_data;       //0x0207     
	
  ext_student_interactive_header_data_t  stu_interactive_data; //0x0301
	
} receive_judge_t;
/** 
  * @brief  the data structure send to judgement
  */
typedef struct
{
	client_custom_data_t                   show_in_client_data ;
	
  ext_student_interactive_header_data_t  stu_interactive_data;
} send_judge_t;

/* data send (forward) */
/* data receive */
extern receive_judge_t                       judge_recv_mesg;
extern client_custom_data_t                  client_custom_data;
extern ext_student_interactive_header_data_t student_interactive_header_id;
extern send_judge_t                          judge_send_mesg;
extern uint16_t                              Stu_Client_ID;
void  judgement_data_handler(uint8_t *p_frame);

#elif SEASON_SELECT == S2020
/** 

   @brief Season 2020
   @version 0.9
   @date May 5th,2020
 
 */
 
 /*接口协议cmd_id */
typedef enum
{
  GAME_STATE_INFO_ID      = 0x0001, //比赛状态数据，1Hz 周期发送
  GAME_RESULT_ID          = 0x0002, //比赛结果数据，比赛结束后
  GAME_ROBOT_HP_ID        = 0x0003, //比赛机器人血量数据，1Hz 周期发送
	
	DART_LAUNCHING_ID       = 0x0004, //飞镖发射状态，飞镖发射时发送
	AI_CHALLENGE_STATE_ID   = 0x0005, //人工智能挑战赛加成与惩罚状态，1Hz周期发送
	
  FIELD_EVENT_DATA_ID     = 0x0101, //场地事件数据，1Hz 周期发送
	FIELD_SUPPY_ACTION_ID   = 0x0102, //场地补给站动作标识数据，动作改变后发送
	//FIELD_SUPPY_BOOKING_ID  = 0X0103, //场地补给站预约子弹数据，由参赛队发送
	
	JUDGE_WARNING_ID        = 0x0104, //裁判警告数据，警告发生后发送
	DART_STATION_COUNTING_ID= 0x0105, //飞镖发射口倒计时，1Hz 周期发送
	
  ROBOT_STATE_DATA_ID     = 0x0201, //机器人状态数据,10Hz 周期发送
  REAL_POWER_HEAT_DATA_ID = 0x0202, //实时功率热量数据，50Hz 周期发送
	ROBOT_POS_DATA_ID       = 0x0203, //机器人位置数据，10Hz 发送
	ROBOT_GAIN_BUFF_ID      = 0x0204, //机器人增益数据,增益状态改变后发送
	DRONE_ENERGY_DATA_ID    = 0X0205, //空中机器人能量状态数据，10Hz 周期发送，只有空中机器人主控发送
	REAL_BLOOD_DATA_ID      = 0X0206, //伤害状态数据，伤害发生后发送
  REAL_SHOOT_DATA_ID      = 0X0207, //实时射击数据，子弹发射后发送
	NUMBER_BULLETS_LEFT_ID  = 0x0208, //弹丸剩余发射数，仅空中机器人，哨兵机器人以及 ICRA 机器人发送，1Hz 周期发送
	ROBOT_RFID_STATE_ID     = 0x0209, //机器人 RFID 状态，1Hz 周期发送
	DART_CLIENT_CMD_DATA_ID = 0x020A, //飞镖机器人客户端指令数据，10Hz周期发送
	
  ROBOT_INTERACTIVE_ID    = 0x0301, //机器人间交互数据 、客户端自定义数据，上限 10Hz
	STU_CUSTOM_DATA_CMD_ID  = 0xD180, //客户端自定义数据内容ID
} judge_data_id_e;

typedef enum
{
	//Red
  Hero_R1     = 1,
	Engineer_R2 = 2,
	Standard_R3 = 3,
	Standard_R4 = 4,
	Standard_R5 = 5,
	Aerial_R6   = 6,
	Sentry_R7   = 7,
	Dart_R8     = 8,
	Radar_R9    = 9,
	//Blue
	Hero_B1     = 101,
	Engineer_B2 = 102,
	Standard_B3 = 103,
	Standard_B4 = 104,
	Standard_B5 = 105,
	Aerial_B6   = 106,
	Sentry_B7   = 107,
	Dart_B8     = 108,
	Radar_B9    = 109,
	
} robot_id_e;

typedef enum
{
	//Red
  Hero_CLient_R1     = 0x0101,
	Engineer_CLient_R2 = 0x0102,
	Standard_CLient_R3 = 0x0103,
	Standard_CLient_R4 = 0x0104,
	Standard_CLient_R5 = 0x0105,
	Aerial_CLient_R6   = 0x0106,
	
	//Blue
	Hero_CLient_B1     = 0x0165,
	Engineer_CLient_B2 = 0x0166,
	Standard_CLient_B3 = 0x0167,
	Standard_CLient_B4 = 0x0168,
	Standard_CLient_B5 = 0x0169,
	Aerial_CLient_B6   = 0x016A,
} stu_client_id_e;

//0x001 比赛类型、当前比赛阶段、剩余时间
typedef __packed struct
{
	uint8_t game_type : 4;       //1机甲大师赛，2机甲大师单项赛
 uint8_t game_progress : 4;    //0未开始比赛，1准备阶段，2自检阶段，3五秒倒计时，4对战中，5比赛解算中
 uint16_t stage_remain_time;   //当前阶段剩余时间，单位秒
} ext_game_state_t;

//0x002
typedef __packed struct
{
 uint8_t winner;               //0 平局 1 红方胜利 2 蓝方胜利
} ext_game_result_t;

//0x003 敌我方全体机器血量
typedef __packed struct
{
 uint16_t red_1_robot_HP;
 uint16_t red_2_robot_HP; 
 uint16_t red_3_robot_HP; 
 uint16_t red_4_robot_HP; 
 uint16_t red_5_robot_HP; 
 uint16_t red_7_robot_HP; 
 uint16_t red_outpost_HP;
 uint16_t red_base_HP; 
 uint16_t blue_1_robot_HP; 
 uint16_t blue_2_robot_HP; 
 uint16_t blue_3_robot_HP; 
 uint16_t blue_4_robot_HP; 
 uint16_t blue_5_robot_HP; 
 uint16_t blue_7_robot_HP; 
 uint16_t blue_outpost_HP;
 uint16_t blue_base_HP;
} ext_game_robot_HP_t;

//0x0004 飞镖发射状态，在飞镖发射后发送给所有机器人
typedef __packed struct
{
 uint8_t dart_belong;              //1红方飞镖，2蓝方飞镖
 uint16_t stage_remaining_time;    //发射时的剩余比赛时间，单位秒
} ext_dart_status_t;

//0x0005 人工智能挑战赛加成与惩罚区状态，发送范围：所有机器人。
typedef __packed struct
{
 uint8_t F1_zone_status:1;               //激活标志位：0未激活，1已激活
 uint8_t F1_zone_buff_debuff_status:3;   //状态信息：1 为红方回血区；2 为红方弹药补给区；3 为蓝方回血区；
 uint8_t F2_zone_status:1;               //4 为蓝方弹药补给区；5 为禁止射击区；6 为禁止移动区。
 uint8_t F2_zone_buff_debuff_status:3; 
 uint8_t F3_zone_status:1;
 uint8_t F3_zone_buff_debuff_status:3; 
 uint8_t F4_zone_status:1;
 uint8_t F4_zone_buff_debuff_status:3; 
 uint8_t F5_zone_status:1;
 uint8_t F5_zone_buff_debuff_status:3; 
 uint8_t F6_zone_status:1;
 uint8_t F6_zone_buff_debuff_status:3;
} ext_ICRA_buff_debuff_zone_status_t;

//0x0101 场地事件，只发送给己方机器人
typedef __packed struct
{
 uint32_t event_type; 
	/*
	bit 0-1：己方停机坪占领状态
  0 为无机器人占领；
	1 为空中机器人已占领但未停桨；
	2 为空中机器人已占领并停桨
	
	bit 2-3：己方能量机关状态：
bit 2 为小能量机关激活状态，1 为已激活；
bit 3 为大能量机关激活状态，1 为已激活；
	bit 4 己方基地虚拟护盾状态；1 为基地有虚拟护盾血量；0 为基地无虚拟护盾血量；
	
  bit 5 -31: 保留
	
	*/
} ext_event_data_t;


//0x0102 补给站动作标识
typedef __packed struct
{
 uint8_t supply_projectile_id;     //补给站口 ID：1 ，2
 uint8_t supply_robot_id;          //补弹机器人 ID：0 为当前无机器人补弹，1 为红方英雄机器人补弹，2 为红方工程机器人补弹，3/4/5 为红方步兵机器人补弹，蓝方为..
 uint8_t supply_projectile_step;   //出弹口开闭状态：0 为关闭，1 为子弹准备中，2 为子弹下落
 uint8_t supply_projectile_num;    //补弹数量：50 ，100 ，150 ，200
} ext_supply_projectile_action_t;


//0x0103 2020赛季不使用
typedef __packed struct
{
  uint8_t supply_projectile_id;
  uint8_t supply_robot_id; 
  uint8_t supply_num;
} ext_supply_projectile_booking_t;

//0x0104 裁判警告信息，在警告发生后发送
typedef __packed struct
{
 uint8_t level;          //警告等级
	uint8_t foul_robot_id;  //犯规机器人ID ：1 级以及 5 级警告时，机器人 ID 为 0 ，二三四级警告时，机器人 ID 为犯规机器人 ID
} ext_referee_warning_t;

//0x0105 飞镖发射口倒计时
typedef __packed struct
{
 uint8_t dart_remaining_time;
} ext_dart_remaining_time_t;

//0x0201 比赛机器人状态，只发送给自己
typedef __packed struct
{
 uint8_t robot_id;
 uint8_t robot_level;
 uint16_t remain_HP;
 uint16_t max_HP;
 uint16_t shooter_heat0_cooling_rate;
 uint16_t shooter_heat0_cooling_limit;
 uint16_t shooter_heat1_cooling_rate;
 uint16_t shooter_heat1_cooling_limit;
 uint8_t mains_power_gimbal_output : 1;
 uint8_t mains_power_chassis_output : 1;
 uint8_t mains_power_shooter_output : 1;
} ext_game_robot_state_t;

//0x0202 实时功率热量数据
typedef __packed struct
{
 uint16_t chassis_volt;            //单位毫伏
 uint16_t chassis_current;         //单位毫安
 float chassis_power; 						 //单位瓦
 uint16_t chassis_power_buffer;    //单位焦耳，备注：飞坡根据规则增加至250J
 uint16_t shooter_heat0;
 uint16_t shooter_heat1; 
} ext_power_heat_data_t;

//0x0203 机器人位置，只发送给自己
typedef __packed struct
{
 float x;   //单位米
 float y;
 float z;
 float yaw; //枪口位置，单位度
} ext_game_robot_pos_t;

//0x0204 机器人增益，只发送给自己
typedef __packed struct
{ 
 uint8_t power_rune_buff;    
	/* 
	bit 0：机器人血量补血状态
	bit 1：枪口热量冷却加成
	bit 2：机器人防御加成
	bit 3：机器人攻击加成
	*/
}ext_buff_musk_t;

//0x0205 空中机器人能量状态，空中机器人接受自己的信息
typedef __packed struct
{
 uint8_t energy_point;  //积累的能量点
 uint8_t attack_time;   //可攻击时间，单位 s，30s 递减至 0
} aerial_robot_energy_t;

//0x0206 伤害状态，伤害发生后发送
typedef __packed struct
{
 uint8_t armor_id : 4;
 uint8_t hurt_type : 4;
	/*
	bit 0-3：当血量变化类型为装甲伤害，代表装甲 ID，其中数值为 0-4 号代表机器人的五个装甲片，其他血量变化类型，该变量数值为 0。
	bit 4-7：血量变化类型:
	         0x0 装甲伤害扣血；
	         0x1 模块掉线扣血；
	         0x2 超射速扣血；
	         0x3 超枪口热量扣血；
	         0x4 超底盘功率扣血；
	         0x5 装甲撞击扣血
	*/
} ext_robot_hurt_t;

//0x0207 实时射击信息
typedef __packed struct
{
 uint8_t bullet_type;  //子弹类型: 1：17mm 弹丸 2：42mm 弹丸
 uint8_t bullet_freq;  //子弹射频 单位 Hz
 float bullet_speed;   //子弹射速 单位 m/s
} ext_shoot_data_t;

//0x0208 子弹剩余发射数
typedef __packed struct
{
 uint16_t bullet_remaining_num; //空中机器人，哨兵机器人以及 ICRA 机器人主控发送
} ext_bullet_remaining_t;

//0x0209 机器人 RFID 状态
typedef __packed struct
{
 uint32_t rfid_status;
	/*
	bit 0：基地增益点 RFID 状态；
  bit 1：高地增益点 RFID 状态；
  bit 2：能量机关激活点 RFID 状态；
  bit 3：飞坡增益点 RFID 状态；
  bit 4：前哨岗增益点 RFID 状态；
  bit 5：资源岛增益点 RFID 状态；
  bit 6：补血点增益点 RFID 状态；
  bit 7：工程机器人补血卡 RFID 状态；
  bit 8-25：保留
  bit 26-31：人工智能挑战赛 F1-F6 RFID 状态；
  RFID 状态不完全代表对应的增益或处罚状态，例如敌方已占领的高地增益点，不能获取对应的增益效果.
	
	*/
} ext_rfid_status_t;

//0x020A 飞镖机器人客户端指令数据，发送单一机器人
typedef __packed struct
{
 uint8_t dart_launch_opening_status;
 uint8_t dart_attack_target;
 uint16_t target_change_time;
 uint8_t first_dart_speed;
 uint8_t second_dart_speed;
 uint8_t third_dart_speed;
 uint8_t fourth_dart_speed;
 uint16_t last_dart_launch_time;
 uint16_t operate_launch_cmd_time;
} ext_dart_client_cmd_t;

//0x0301
typedef __packed struct
{
 uint16_t data_cmd_id;
 uint16_t send_ID;
 uint16_t receiver_ID;
}ext_student_interactive_header_data_t;

//cmd_id:0x0301。内容ID:0xD180
typedef __packed struct
{
 float data1;
 float data2;
 float data3;
 uint8_t masks;
}client_custom_data_t;



typedef __packed struct
{
 uint16_t data_cmd_id;
 uint16_t send_id;
 uint16_t receiver_id;
	
}ext_student_interative_header_data_t;
/** 
  * @brief  the data structure receive from judgement
  */
typedef struct
{
  ext_game_state_t                game_information;      //0x0001
	ext_game_result_t               game_result_data;      //0x0002
	ext_game_robot_HP_t             robot_hp_data;         //0x0003
	ext_dart_status_t               dart_lauch_data;       //0x0004
	
	ext_event_data_t                field_event_data;      //0x0101
	ext_supply_projectile_action_t  supply_action_data;    //0x0102
	ext_supply_projectile_booking_t supply_booking_data;   //0x0103

  ext_referee_warning_t           referee_warning_data;  //0x0104
	ext_dart_remaining_time_t       dart_remaining_time;   //0x0105
	
	ext_game_robot_state_t          robot_state_data;      //0x0201
	ext_power_heat_data_t           power_heat_data;       //0x0202
	ext_game_robot_pos_t            robot_pos_data;        //0x0203
	ext_buff_musk_t                 get_buff_data;         //0x0204
	aerial_robot_energy_t           drone_energy_data;     //0x0205
	ext_robot_hurt_t                blood_changed_data;    //0x0206
	ext_shoot_data_t                real_shoot_data;       //0x0207     
	ext_bullet_remaining_t          bullet_remaining_data; //0x0208
	ext_rfid_status_t               rfid_status;           //0x0209
	ext_dart_client_cmd_t           dart_client_cmd_data;  //0x020A
	
  ext_student_interactive_header_data_t  stu_interactive_data; //0x0301
	
} receive_judge_t;
/** 
  * @brief  the data structure send to judgement
  */
typedef struct
{
	client_custom_data_t                   show_in_client_data ;
	
  ext_student_interactive_header_data_t  stu_interactive_data;
	ext_client_custom_graphic_delete_t     graphic_delete_data;   //删除图形数据 2 byte
	graphic_data_struct_t                  graphic_data;          //图形数据 15 byte
	
	
} send_judge_t;

/* data send (forward) */
/* data receive */
extern receive_judge_t                       judge_recv_mesg;
extern client_custom_data_t                  client_custom_data;
extern ext_student_interactive_header_data_t student_interactive_header_id;
extern send_judge_t                          judge_send_mesg;
extern uint16_t                              Stu_Client_ID;
void  judgement_data_handler(uint8_t *p_frame);



#endif

#endif
