#ifndef __KNOB_DIMMING_H
#define __KNOB_DIMMING_H
// 旋钮调光头文件

#include "my_config.h"

#define KNOB_DIMMING_LEVELS (160)                                    // 旋钮调光的分级
#define KNOB_DIMMING_ADC_DEAD_ZONE (200)                             // 最大电压值和最小电压值使用到的死区
#define KNOB_DIMMING_MAX_ADC_VAL (4095 - KNOB_DIMMING_ADC_DEAD_ZONE) // 客户在引脚上测得最大的电压值是4.87V，就是VCC的电压，这里最好再加上死区
#define KNOB_DIMMING_MIN_ADC_VAL (479 + KNOB_DIMMING_ADC_DEAD_ZONE)  // 客户引脚上测得最小电压值是0.57V

// 旋钮调光其中一级对应的ad值
#define KNOB_DIMMING_ADC_VAL_PER_LEVEL \
    ((u16)(KNOB_DIMMING_MAX_ADC_VAL - KNOB_DIMMING_MIN_ADC_VAL) / KNOB_DIMMING_LEVELS) // 若干分级中，其中的一级对应的ad值

/*
    旋钮调光的每一级分级之间的死区
    4096 - 479 = 3617
    分成160级，3617 / 160 = 22.60625（取整为 22）
    在 22 取 10% -> 2.2（取整为2）作为死区
    在 22 取 20% -> 4.4（取整为4）作为死区
*/
#define KNOB_DIMMING_ADC_DEAD_ZONE_PER_LEVEL \
    ((u8)(KNOB_DIMMING_ADC_VAL_PER_LEVEL * 2 / 10))

extern volatile u16 limited_max_pwm_duty; // 存放限制的最大占空比

// 根据旋钮，限制当前的最大占空比
void update_max_pwm_duty_coefficient(void);

#endif
