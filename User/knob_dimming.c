#include "knob_dimming.h" // 旋钮调光头文件

volatile u16 limited_max_pwm_duty = 0; // 存放限制的最大占空比
// volatile u16 last_limited_max_pwm_duty = MAX_PWM_DUTY;

// volatile bit flag_is_knob_change = 0; // 标志位，旋钮的状态是否发生变化

// 根据旋钮，限制当前的最大占空比
void update_max_pwm_duty_coefficient(void)
{
    // static u16 last_adc_val = 4095;
    volatile u16 adc_val = 0;
    // volatile u16 knob_dimming_cur_level = 0;
    adc_sel_pin(ADC_SEL_PIN_P31);
    adc_val = adc_get_val();

    // if (last_adc_val != adc_val)
    // {
    //     flag_is_knob_change = 1;
    //     last_adc_val = adc_val;
    // }

    // limited_max_pwm_duty = (u32)adjust_duty * adc_val / 4096; // 会出现指数级下降，因为adjust_duty一直在变化
    // limited_max_pwm_duty = (u32)MAX_PWM_DUTY * adc_val / 4096; // 分级太多，加上抖动和迅速变化，会导致灯光闪烁

#if 1

    // knob_dimming_cur_level = (adc_val - KNOB_DIMMING_MIN_ADC_VAL) / KNOB_DIMMING_ADC_VAL_PER_LEVEL;
    // if (adc_val >= (KNOB_DIMMING_MAX_ADC_VAL))
    // {
    //     // 大于某个值，直接输出100%
    //     limited_max_pwm_duty = MAX_PWM_DUTY;
    //     knob_dimming_cur_level = KNOB_DIMMING_LEVELS;
    // }
    // else if (adc_val <= (KNOB_DIMMING_MIN_ADC_VAL))
    // {
    //     // 小于某个值，直接关闭
    //     limited_max_pwm_duty = 0;
    //     knob_dimming_cur_level = 0;
    // }
    // else
    // {
    //     // 计算得出当前ad值对应的等级
    //     // u16 knob_dimming_cur_level = (adc_val - KNOB_DIMMING_MIN_ADC_VAL) / KNOB_DIMMING_ADC_VAL_PER_LEVEL;

    //     if ((adc_val - KNOB_DIMMING_MIN_ADC_VAL) > (knob_dimming_cur_level * KNOB_DIMMING_ADC_VAL_PER_LEVEL + KNOB_DIMMING_ADC_DEAD_ZONE_PER_LEVEL) &&
    //         (adc_val - KNOB_DIMMING_MIN_ADC_VAL) < ((knob_dimming_cur_level + 1) * KNOB_DIMMING_ADC_VAL_PER_LEVEL - KNOB_DIMMING_ADC_DEAD_ZONE_PER_LEVEL))
    //     {
    //         // 如果采集的ad值落在这一级
    //     }
    //     else if ((adc_val - KNOB_DIMMING_MIN_ADC_VAL) < (knob_dimming_cur_level * KNOB_DIMMING_ADC_VAL_PER_LEVEL + KNOB_DIMMING_ADC_DEAD_ZONE_PER_LEVEL))
    //     {
    //         // 如果采集的ad值小于这一级对应的ad值+死区值
    //         if (knob_dimming_cur_level > 0)
    //         {
    //             knob_dimming_cur_level--;
    //         }
    //     }

    //     limited_max_pwm_duty = (u32)MAX_PWM_DUTY * knob_dimming_cur_level / KNOB_DIMMING_LEVELS;
    // }

    if (adc_val <= KNOB_DIMMING_MIN_ADC_VAL) // MIN 0%  0.57V
    {
        limited_max_pwm_duty = 0;
    }
    else if (adc_val <= 1278) // 20% 1.52V
    {
        // 计算采集到的ad值所占20%对应的ad值得占比，再乘以 20%的占空比
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY * 2 / 10 / 1278;
    }
    else if (adc_val <= 2498) // 40% 2.97V
    {
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY * 4 / 10 / 2498;
    }
    else if (adc_val <= 2641) // 50% 3.14V
    {
        // limited_max_pwm_duty = adc_val * MAX_PWM_DUTY * 5 / 10 / 2641;
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY / 2 / 2641;
    }
    else if (adc_val <= 3482) // 60% 4.14V
    {
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY * 6 / 10 / 3482;
    }
    else if (adc_val <= 4087) // 80% 4.86V
    {
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY * 8 / 10 / 4087;
    }
    else
    {
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY / 4095;
    }

#if USE_MY_DEBUG

    // printf("cur_level %u\n", knob_dimming_cur_level);

#endif // #if USE_MY_DEBUG

#endif
}
