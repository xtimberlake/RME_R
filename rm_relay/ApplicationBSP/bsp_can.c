#include "bsp_can.h"


CAN_TxHeaderTypeDef Tx1Message;//发送数据句柄
CAN_RxHeaderTypeDef Rx1Message;//接收数据句柄
uint8_t CAN_Tx_data[8];
uint8_t CAN_Rx_data[8];




/**
  * @brief   can filter initialization     设置CAN滤波器组
  * @param   CAN_HandleTypeDef
  * @retval  None
  */
void my_can_filter_init_recv_all(void)
{
  //can1 filter config
  CAN_FilterTypeDef  can_filter;

	can_filter.FilterBank           = 0;
  can_filter.FilterMode           = CAN_FILTERMODE_IDMASK;
  can_filter.FilterScale          = CAN_FILTERSCALE_32BIT;
  can_filter.FilterIdHigh         = 0x0000;
  can_filter.FilterIdLow          = 0x0000;
  can_filter.FilterMaskIdHigh     = 0x0000;
  can_filter.FilterMaskIdLow      = 0x0000;
  can_filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
	can_filter.SlaveStartFilterBank = 0;  
  can_filter.FilterActivation     = ENABLE;

  HAL_CAN_ConfigFilter(&hcan, &can_filter);//根据指定的参数配置CAN接收筛选器
  while (HAL_CAN_ConfigFilter(&hcan, &can_filter) != HAL_OK);
	
}


/**
  * @brief  init the can transmit and receive
  * @param  None
  */
void can_device_init(void)
{
  my_can_filter_init_recv_all();				//设置CAN滤波器组
	HAL_Delay(100);
	HAL_CAN_Start(&hcan);									//启动CAN模块
 	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);		//使能CAN接收中断
}


/**
  * @brief  send calculated current to motor
						将计算好的电流发送到电机
  * @param  3508 motor current
  */
void send_chassis_cur(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{
	uint8_t FreeTxNum = 0;  
	
	Tx1Message.StdId = 0x200;//基本id
	Tx1Message.IDE 	 = CAN_ID_STD;//id模式
	Tx1Message.RTR   = CAN_RTR_DATA;//传输的消息的帧类型
  Tx1Message.DLC   = 0x08;//帧长度（位于8与0之间）
	
	CAN_Tx_data[0] = iq1 >> 8;
	CAN_Tx_data[1] = iq1;
	CAN_Tx_data[2] = iq2 >> 8;
	CAN_Tx_data[3] = iq2;
	CAN_Tx_data[4] = iq3 >> 8;
	CAN_Tx_data[5] = iq3;
	CAN_Tx_data[6] = iq4 >> 8;
	CAN_Tx_data[7] = iq4;
	
	//查询发送邮箱是否为空
	FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);  
	while(FreeTxNum == 0) 
	{  
    FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);  
  }
	
	HAL_CAN_AddTxMessage(&hcan, &Tx1Message,CAN_Tx_data,(uint32_t*)CAN_TX_MAILBOX1);
}


/**
  * @brief     CAN接受中断回调函数
  * @param     CAN_Rx_data ：CAN节点反馈的数据帧
  * @attention 
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&Rx1Message, CAN_Rx_data);//从hcan的FIFO0区域获取一个Rx1Message(CAN帧)并将其放入数组CAN_Rx_data中
//	switch (Rx1Message.StdId)
//	{
//		case CAN_3508_M1_ID:	encoder_data_handler(&M1, hcan);
//	}
	__HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	
}

