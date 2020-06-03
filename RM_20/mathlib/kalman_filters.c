/**
  * @file kalman_filters.c
  * @version 0.5
  * @date May,31 2020
  * @brief  卡尔曼滤波处理
	* 卡尔曼使用条件：
	* 1运动方程、观测方程是线性的
	* 2过程/测量噪声服从高斯分布
	*
	* reference: 
		<卡尔曼滤波器及其在云台控制中的应用> in zhihu
		link: https://zhuanlan.zhihu.com/p/38745950
	*
  *	@author Haoyun Yan
  */
	
#include "kalman_filters.h"
	
#define __KALMAN_FILTERS_EXT

/**
  * @brief 卡尔曼指针初始化
  * @param input : [1]*p：卡尔曼滤波对象指针 [2]Q:过程噪声 [3]R:测量噪声 
  * 
  * @note  测量噪声大，R需要设置的较大；模型不准确，Q需要增大
	*        Q/R决定了滤波器的稳态频率响应,值越大，会包含更多高频噪声
  */
void kalman_init(kalman_filter_struct_t *p, float Q, float R)
{
	 p->x_last = 0;       
   p->P_last = 0;      
	
	 p->A = 1;  
	 p->H = 1;  

	 p->Q = Q;  
	 p->R = R;  

}


/**
  * @brief 麦轮解算函数
  * @param input : [1]*p：卡尔曼滤波对象指针 [2]当前测量值
  *        output: 最优结果
  * @note  1=FL 2=FR 3=BL 4=BR
  */
float kalman_filter_calc(kalman_filter_struct_t *p, float z_k)
{
	//predict
	p->x_predict = p->x_last;
	p->P_predict = p->P_last + p->Q;
	
	//correct
	p->Kg = p->P_predict / (p->P_predict + p->R);
	p->x_opti = p->x_predict + p->Kg*(z_k - p->x_predict);
	p->P_opti = (1 - p->Kg*p->H)*p->P_predict;

	p->x_last = p->x_opti;
	p->P_last = p->P_opti;
	
	return p->x_opti;
}
