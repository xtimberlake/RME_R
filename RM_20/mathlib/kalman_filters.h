/**
  * @file kalman_filters.h
  * @version 0.5
  * @date May,31 2020
  * @brief  卡尔曼滤波处理头文件
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

//矩阵定义，用于计算
typedef struct
{
  float raw_value;              //当前测量值
  float filtered_value[2];      //二维滤波计算后返回值,i.e. 等于xhat
  mat xhat, xhatminus, z, A, H, AT, HT, Q, R, P, Pminus, K; 
} kalman_filter_t;

//数组定义，用于矩阵初始化
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






//一阶卡尔曼滤波结构体定义,都是标量
typedef struct
{
	float x_last;       //上次后验状态估计(最优估计)
	float x_predict;    //先验状态估计(由x_last估计)
	float x_opti;       //当前最优估计,函数返回值
	
	float P_last;       //上次误差协方差矩阵
	float P_predict;    //预测值和真实值之间的误差协方差矩阵
  float P_opti;       //估计值和真实值之间误差协方差矩阵
	
	float A;  //预测矩阵，在一阶卡尔曼滤波常取1
	float H;  //状态值到测量值的变换矩阵
	float Kg; //卡尔曼增益
	float Q;  //过程噪声
	float R;  //测量噪声
	
} kalman_filter_struct_t;







__KALMAN_FILTERS_EXT void kalman_init(kalman_filter_struct_t *p, float Q, float R);
__KALMAN_FILTERS_EXT void kalman_filter_init(kalman_filter_t *F, kalman_filter_init_t *I);
__KALMAN_FILTERS_EXT float *kalman_filter_calc(kalman_filter_t *F, float signal1, float signal2);


#endif
