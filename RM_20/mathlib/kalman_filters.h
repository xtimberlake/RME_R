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

#include "arm_math.h"

#define mat         arm_matrix_instance_f32 
#define mat_init    arm_mat_init_f32
#define mat_add     arm_mat_add_f32
#define mat_sub     arm_mat_sub_f32
#define mat_mult    arm_mat_mult_f32
#define mat_trans   arm_mat_trans_f32
#define mat_inv     arm_mat_inverse_f32

//�����壬���ڼ���
typedef struct
{
  float raw_value;              //��ǰ����ֵ
  float filtered_value[2];      //��ά�˲�����󷵻�ֵ,i.e. ����xhat
  mat xhat, xhatminus, z, A, H, AT, HT, Q, R, P, Pminus, K; 
} kalman_filter_t;

//���鶨�壬���ھ����ʼ��
typedef struct
{
  float raw_value;
  float filtered_value[2];
  float xhat_data[2], xhatminus_data[2], z_data[2],Pminus_data[4], K_data[4];
  float P_data[4];
  float AT_data[4], HT_data[4];
  float A_data[4];
  float H_data[4];
  float Q_data[4];
  float R_data[4];
} kalman_filter_init_t;






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
__KALMAN_FILTERS_EXT void kalman_filter_init(kalman_filter_t *F, kalman_filter_init_t *I);
__KALMAN_FILTERS_EXT float *kalman_filter_calc(kalman_filter_t *F, float signal1, float signal2);


#endif
