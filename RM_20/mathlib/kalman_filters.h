/**
  * @file kalman_filters.h
  * @version 0.5
  * @date May,31 2020
  * @brief  �������˲�����ͷ�ļ�
  *	@author Haoyun Yan
  */
#ifndef __KALMAN_FILTERS_H__
#define __KALMAN_FILTERS_H__

#ifndef  __KALMAN_FILTERS_H_GLOBALS
#define __KALMAN_FILTERS_EXT
#else
#define __KALMAN_FILTERS_EXT extern
#endif

//һ�׿������˲��ṹ�嶨��,���Ǳ���
typedef struct
{
	float x_last;       //�ϴκ���״̬����(���Ź���)
	float x_predict;    //����״̬����(��x_last����)
	float x_opti;       //��ǰ���Ź���,��������ֵ
	
	float P_last;       //�ϴ����Э�������
	float P_predict;    //Ԥ��ֵ����ʵֵ֮������Э�������
  float P_opti;       //����ֵ����ʵֵ֮�����Э�������
	
	float A;  //Ԥ�������һ�׿������˲���ȡ1
	float H;  //״ֵ̬������ֵ�ı任����
	float Kg; //����������
	float Q;  //��������
	float R;  //��������
	
} kalman_filter_struct_t;







__KALMAN_FILTERS_EXT void kalman_init(kalman_filter_struct_t *p, float Q, float R);
__KALMAN_FILTERS_EXT float kalman_filter_calc(kalman_filter_struct_t *p, float z_k);


#endif
