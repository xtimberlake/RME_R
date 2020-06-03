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
__KALMAN_FILTERS_EXT float kalman_filter_calc(kalman_filter_struct_t *p, float z_k);


#endif
