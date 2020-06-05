/**
  * @file kalman_filters.c
  * @version 0.5
  * @date May,31 2020
  * @brief  �������˲�����
	* ������ʹ��������
	* 1�˶����̡��۲ⷽ�������Ե�
	* 2����/�����������Ӹ�˹�ֲ�
	*
	* reference: 
		<�������˲�����������̨�����е�Ӧ��> in zhihu
		link: https://zhuanlan.zhihu.com/p/38745950
	*
  *	@author Haoyun Yan
  */
	
#include "kalman_filters.h"
	
#define __KALMAN_FILTERS_EXT

/**
  * @brief ������ָ���ʼ��
  * @param input : [1]*p���������˲�����ָ�� [2]Q:�������� [3]R:�������� 
  * 
  * @note  ����������R��Ҫ���õĽϴ�ģ�Ͳ�׼ȷ��Q��Ҫ����
	*        Q/R�������˲�������̬Ƶ����Ӧ,ֵԽ�󣬻���������Ƶ����
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
  * @brief һ�׿������˲�����
  * @param input : [1]*p���������˲�����ָ�� [2]��ǰ����ֵ
  *        output: ���Ž��
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
  * @brief ���׳�ʼ��
  * @param input : [1]*F���������˲�����ָ�� [2]*I����ʼ��ָ������
  * 
  * 
  */
void kalman_filter_init(kalman_filter_t *F, kalman_filter_init_t *I)
{
  mat_init(&F->xhat,2,1,(float *)I->xhat_data); //�����ʼ��,�м���������Ϊ��������m*n
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
