#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#include "can.h"

/* can receive motor parameter structure */
#define FILTER_BUF 5

typedef struct
{
  uint16_t ecd;
  uint16_t last_ecd;
  
  int16_t  speed_rpm;
  int16_t  given_current;

  int32_t  round_cnt;
  int32_t  total_ecd;
  int32_t  total_angle;
  
  uint16_t offset_ecd;
  uint32_t msg_cnt;
  
  int32_t  ecd_raw_rate;
  int32_t  rate_buf[FILTER_BUF];
  uint8_t  buf_cut;
  int32_t  filter_rate;
} moto_measure_t;

typedef enum
{
  CAN_3508_M1_ID       = 0x201,
  CAN_3508_M2_ID       = 0x202,
} can_msg_id_e;

void my_can_filter_init_recv_all(void);
void can_device_init(void);
void send_cur(int16_t iq1, int16_t iq2, int16_t iq3, int16_t iq4);
int16_t circle_error(int16_t ref ,int16_t lastref ,int16_t circle_para);
void encoder_data_handler(moto_measure_t* ptr, CAN_HandleTypeDef* hcan);
void get_moto_offset(moto_measure_t* ptr, CAN_HandleTypeDef* hcan,uint8_t CAN_Rx_data[8]);

extern moto_measure_t moto_rotate[2];
#endif

