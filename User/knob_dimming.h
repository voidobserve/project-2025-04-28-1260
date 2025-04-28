#ifndef __KNOB_DIMMING_H
#define __KNOB_DIMMING_H
// 旋钮调光头文件

#include "my_config.h"
 
extern volatile u16 limited_max_pwm_duty ; // 存放限制的最大占空比

// 根据旋钮，限制当前的最大占空比
void update_max_pwm_duty_coefficient(void);

#endif
