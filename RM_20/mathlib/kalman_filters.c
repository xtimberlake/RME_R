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
  * @brief ���ֽ��㺯��
  * @param input : [1]*p���������˲�����ָ�� [2]��ǰ����ֵ
  *        output: ���Ž��
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
