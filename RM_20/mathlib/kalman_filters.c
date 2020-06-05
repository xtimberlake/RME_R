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
  * @brief 一阶卡尔曼滤波计算
  * @param input : [1]*p：卡尔曼滤波对象指针 [2]当前测量值
  *        output: 最优结果
  * 
  */
float kalman_filter_compute(kalman_filter_struct_t *p, float z_k)
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

/**
  * @brief 二阶初始化
  * @param input : [1]*F：卡尔曼滤波对象指针 [2]*I：初始化指针数据
  * 
  * 
  */
void kalman_filter_init(kalman_filter_t *F, kalman_filter_init_t *I)
{
  mat_init(&F->xhat,2,1,(float *)I->xhat_data); //矩阵初始化,中间两个参数为矩阵行列m*n
  mat_init(&F->xhatminus,2,1,(float *)I->xhatminus_data);
	mat_init(&F->z,2,1,(float *)I->z_data);
	mat_init(&F->A,2,2,(float *)I->A_data);
	mat_init(&F->H,2,2,(float *)I->H_data);
  mat_init(&F->Q,2,2,(float *)I->Q_data);
  mat_init(&F->R,2,2,(float *)I->R_data);
  mat_init(&F->P,2,2,(float *)I->P_data);
	mat_init(&F->Pminus,2,2,(float *)I->Pminus_data);
  mat_init(&F->K,2,2,(float *)I->K_data);
  mat_init(&F->AT,2,2,(float *)I->AT_data);
  mat_init(&F->HT,2,2,(float *)I->HT_data);
	mat_trans(&F->A, &F->AT);
  mat_trans(&F->H, &F->HT);
}

float *kalman_filter_calc(kalman_filter_t *F, float signal1, float signal2)
{
  float TEMP_data[4] = {0, 0, 0, 0};
  float TEMP_data21[2] = {0, 0};
  mat TEMP,TEMP21;

  mat_init(&TEMP,2,2,(float *)TEMP_data);
  mat_init(&TEMP21,2,1,(float *)TEMP_data21);

  F->z.pData[0] = signal1;
  F->z.pData[1] = signal2;

  //1. xhat'(k)= A xhat(k-1)
  mat_mult(&F->A, &F->xhat, &F->xhatminus);

  //2. P'(k) = A P(k-1) AT + Q
  mat_mult(&F->A, &F->P, &F->Pminus);
  mat_mult(&F->Pminus, &F->AT, &TEMP);
  mat_add(&TEMP, &F->Q, &F->Pminus);

  //3. K(k) = P'(k) HT / (H P'(k) HT + R)
  mat_mult(&F->H, &F->Pminus, &F->K);
  mat_mult(&F->K, &F->HT, &TEMP);
  mat_add(&TEMP, &F->R, &F->K);

  mat_inv(&F->K, &F->P);
  mat_mult(&F->Pminus, &F->HT, &TEMP);
  mat_mult(&TEMP, &F->P, &F->K);

  //4. xhat(k) = xhat'(k) + K(k) (z(k) - H xhat'(k))
  mat_mult(&F->H, &F->xhatminus, &TEMP21);
  mat_sub(&F->z, &TEMP21, &F->xhat);
  mat_mult(&F->K, &F->xhat, &TEMP21);
  mat_add(&F->xhatminus, &TEMP21, &F->xhat);

  //5. P(k) = (1-K(k)H)P'(k)
  mat_mult(&F->K, &F->H, &F->P);
  mat_sub(&F->Q, &F->P, &TEMP);
  mat_mult(&TEMP, &F->Pminus, &F->P);

  F->filtered_value[0] = F->xhat.pData[0];
  F->filtered_value[1] = F->xhat.pData[1];

  return F->filtered_value;
}
