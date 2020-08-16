#include "bsp_can.h"
#include "stm32f1xx_hal_def.h"

CAN_TxHeaderTypeDef Tx1Message;//�������ݾ��
CAN_RxHeaderTypeDef Rx1Message;//�������ݾ��
uint8_t CAN_Tx_data[8];
uint8_t CAN_Rx_data[8];

float M1_realecd=0;
moto_measure_t moto_rotate[2];

HAL_StatusTypeDef HAL_RetVal;

/**
  * @brief   can filter initialization     ����CAN�˲�����
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
	can_filter.SlaveStartFilterBank = 14;

  HAL_CAN_ConfigFilter(&hcan, &can_filter);//����ָ���Ĳ�������CAN����ɸѡ��
  while (HAL_CAN_ConfigFilter(&hcan, &can_filter) != HAL_OK);
	
}


/**
  * @brief  init the can transmit and receive
  * @param  None
  */
void can_device_init(void)
{
  my_can_filter_init_recv_all();				//����CAN�˲�����
	HAL_Delay(100);
	HAL_CAN_Start(&hcan);									//����CANģ��
 	HAL_CAN_ActivateNotification(&hcan,CAN_IT_RX_FIFO0_MSG_PENDING);		//ʹ��CAN�����ж�
}


/**
  * @brief  send calculated current to motor
						������õĵ������͵����
  * @param  3508 motor current
  */
uint8_t tempCanTxdata2[50];
void send_cur(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4)
{ 
	
	Tx1Message.StdId = 0x200;//����id
	Tx1Message.IDE 	 = CAN_ID_STD;//idģʽ
	Tx1Message.RTR   = CAN_RTR_DATA;//�������Ϣ��֡����
  Tx1Message.DLC   = 0x08;//֡���ȣ�λ��8��0֮�䣩
	
	CAN_Tx_data[0] = iq1 >> 8;
	CAN_Tx_data[1] = iq1;
	CAN_Tx_data[2] = iq2 >> 8;
	CAN_Tx_data[3] = iq2;
	CAN_Tx_data[4] = iq3 >> 8;
	CAN_Tx_data[5] = iq3;
	CAN_Tx_data[6] = iq4 >> 8;
	CAN_Tx_data[7] = iq4;
	
//	//��ѯ���������Ƿ�Ϊ��
//	FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);  
//	while(FreeTxNum == 0) 
//	{  
//    FreeTxNum = HAL_CAN_GetTxMailboxesFreeLevel(&hcan);  
//  }
//	HAL_CAN_AddTxMessage(&hcan, &Tx1Message,CAN_Tx_data,(uint32_t*)CAN_TX_MAILBOX1);
		uint32_t count_overtime=0;
		while(HAL_CAN_GetTxMailboxesFreeLevel(&hcan)==0)	//HAL_CAN_GetTxMailboxesFreeLevel(hcan);
		{
			count_overtime++;
			if(count_overtime>5000)	break;
		}
		HAL_RetVal=HAL_CAN_AddTxMessage(&hcan,&Tx1Message,CAN_Tx_data,(uint32_t*)tempCanTxdata2); 

		if(HAL_RetVal!=HAL_OK)	{return ;}
}


/**
  * @brief     CAN�����жϻص�����
  * @param     CAN_Rx_data ��CAN�ڵ㷴��������֡
  * @attention 
  */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
	__HAL_CAN_DISABLE_IT(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	HAL_CAN_GetRxMessage(hcan,CAN_RX_FIFO0,&Rx1Message, CAN_Rx_data);//��hcan��FIFO0�����ȡһ��Rx1Message(CAN֡)�������������CAN_Rx_data��
	switch (Rx1Message.StdId)
	{
		case CAN_3508_M1_ID:	
		{
			moto_rotate[0].msg_cnt++ <= 50 ? 
			get_moto_offset(&moto_rotate[0], hcan,CAN_Rx_data) : encoder_data_handler(&moto_rotate[0], hcan);
		}break;
		case CAN_3508_M2_ID:	
		{
			moto_rotate[1].msg_cnt++ <= 50 ? 
			get_moto_offset(&moto_rotate[0], hcan,CAN_Rx_data) : encoder_data_handler(&moto_rotate[0], hcan);
		}break;
	}
	__HAL_CAN_ENABLE_IT(hcan, CAN_IT_RX_FIFO0_MSG_PENDING);
	
}

/**
  * @brief     get motor rpm and calculate motor round_count/total_encoder/total_angle    ��ȡ���ת�٣�������ת��
  * @param     ptr: Pointer to a moto_measure_t structure
  * @attention this function should be called after get_moto_offset() function
  */
void encoder_data_handler(moto_measure_t* ptr, CAN_HandleTypeDef* hcan)
{
		//ת��ת��
	ptr->speed_rpm     = (int16_t)(CAN_Rx_data[2] << 8 | CAN_Rx_data[3]);
	
	//��е�Ƕ�
  ptr->last_ecd = ptr->ecd;
  ptr->ecd      = (uint16_t)(CAN_Rx_data[0] << 8 | CAN_Rx_data[1]);	//��������λ���ݺϳ�16λ


  if (ptr->ecd - ptr->last_ecd > 4096)
  {
    ptr->round_cnt--;
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd - 8192;
  }
  else if (ptr->ecd - ptr->last_ecd < -4096)
  {
    ptr->round_cnt++;
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd + 8192;
  }
  else
  {
    ptr->ecd_raw_rate = ptr->ecd - ptr->last_ecd;
  }//������ת�٣��Դ���������,round_cnt�ǵ��ת��Ȧ��

  ptr->total_angle = ptr->round_cnt * 8192 + ptr->ecd - ptr->offset_ecd;//ת���ܱ���ֵ
	
	M1_realecd += ((circle_error(ptr->ecd,ptr->last_ecd,8192))/22.75f/19.0f);
  
}

/**
  * @brief     get motor initialize offset value	��ȡ�����ʼλ��
  * @param     ptr: Pointer to a moto_measure_t structure
  * @retval    None
  * @attention this function should be called after system can init
  */
void get_moto_offset(moto_measure_t* ptr, CAN_HandleTypeDef* hcan,uint8_t CAN_Rx_data[8])
{
  ptr->ecd        = (uint16_t)(CAN_Rx_data[0] << 8 | CAN_Rx_data[1]);
  ptr->offset_ecd = ptr->ecd;
}


/**
	*@func   float Circle_error(float set ,float get ,float circle_para)
	*@bref		�������ݼ����ֵ
	*@param[in] set �趨ֵ get����ֵ circle_para һȦ��ֵ
	*@note	���������£������ֵ
*/
int16_t circle_error(int16_t ref ,int16_t lastref ,int16_t circle_para)
{
	int16_t error;
	if((ref-lastref)>(circle_para/2))
	{
			error = ref - lastref - circle_para;
	}
	else if((ref - lastref)<(-circle_para/2))
	{
			error = ref - lastref +circle_para;
	}
	else	error = ref - lastref;

	return error;
}

