#include "math_calcu.h"
#include "remote_msg.h"

ramp_function_source_t chassis_x_ramp;
ramp_function_source_t chassis_y_ramp;
ramp_function_source_t chassis_w_ramp;
ramp_function_source_t chassis_mouse_w_ramp;

extern float chassis_speed;
/**
  * @brief          斜波函数计算，根据输入的值进行叠加， 输入单位为 /s 即一秒后增加输入的值
  * @author         RM
  * @param[in]      斜波函数结构体
  * @param[in]      输入值
  * @param[in]      滤波参数
  * @retval         返回空
  */
void ramp_calc(ramp_function_source_t *ramp_source_type, float frame_period, float input, float max, float min)
{
	ramp_source_type->max_value = max;
	ramp_source_type->min_value = min;
	  ramp_source_type->frame_period = frame_period;

    ramp_source_type->input = input;

    ramp_source_type->out += ramp_source_type->input * ramp_source_type->frame_period;

    if (ramp_source_type->out > ramp_source_type->max_value)
    {
        ramp_source_type->out = ramp_source_type->max_value;
    }
    else if (ramp_source_type->out < ramp_source_type->min_value)
    {
        ramp_source_type->out = ramp_source_type->min_value;
    }
}

void chassis_ramp()
{
  if(rc.kb.bit.W)
	{
	  ramp_calc(&chassis_x_ramp,1.0f,100.0f, chassis_speed, 0.0f);			
	}
	else if(rc.kb.bit.S)
	{
		ramp_calc(&chassis_x_ramp,1.0f,-100.0f, 0.0f, -chassis_speed);			
	}
	else
	{
		if(chassis_x_ramp.out > 0)
		{
			ramp_calc(&chassis_x_ramp,1.0f,-100.0f, chassis_speed, 0.0f);			
		}
		else if(chassis_x_ramp.out < 0)
		{
			ramp_calc(&chassis_x_ramp,1.0f,100.0f, 0.0f, -chassis_speed);	
		}
	}
	if(rc.kb.bit.E)
	{
	  ramp_calc(&chassis_y_ramp,1.0f,100.0f, chassis_speed, 0.0f);			
	}
	else if(rc.kb.bit.Q)
	{
	  ramp_calc(&chassis_y_ramp,1.0f,-100.0f, 0.0f, -chassis_speed);			
	}
	else
	{
		if(chassis_y_ramp.out > 0)
		{
			ramp_calc(&chassis_y_ramp,1.0f,-100.0f, chassis_speed, 0.0f);			
		}
		else if(chassis_y_ramp.out < 0)
		{
			ramp_calc(&chassis_y_ramp,1.0f,100.0f, 0.0f, -chassis_speed);	
		}
	}
	if(rc.kb.bit.A)
	{
	  ramp_calc(&chassis_w_ramp,1.0f,180.0f, chassis_speed, 0.0f);			
	}
	else if(rc.kb.bit.D)
	{
	  ramp_calc(&chassis_w_ramp,1.0f,-180.0f, 0.0f, -chassis_speed);			
	}
	else
	{
		if(chassis_w_ramp.out > 0)
		{
			ramp_calc(&chassis_w_ramp,1.0f,-150.0f, chassis_speed, 0.0f);			
		}
		else if(chassis_w_ramp.out < 0)
		{
			ramp_calc(&chassis_w_ramp,1.0f,150.0f, 0.0f, -chassis_speed);	
		}
	}
	
//		if(rc.mouse.x>2)
//	{
//	  ramp_calc(&chassis_mouse_w_ramp,0.2f,100.0f, chassis_speed, 0.0f);			
//	}
//	else if(rc.mouse.x<-2)
//	{
//	  ramp_calc(&chassis_mouse_w_ramp,0.2f,-100.0f, 0.0f, -chassis_speed);			
//	}
//	else
//	{
//		if(chassis_mouse_w_ramp.out > 0)
//		{
//			ramp_calc(&chassis_mouse_w_ramp,0.5f,-10.0f, chassis_speed, 0.0f);			
//		}
//		else if(chassis_mouse_w_ramp.out < 0)
//		{
//			ramp_calc(&chassis_mouse_w_ramp,0.5f,10.0f, 0.0f, -chassis_speed);	
//		}
//	}	
}
