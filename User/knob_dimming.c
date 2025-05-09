#include "knob_dimming.h" // 旋钮调光头文件

volatile u16 limited_max_pwm_duty = 0; // 存放限制的最大占空比
volatile u16 limited_adjust_pwm_duty;  // 存放旋钮限制之后的，待调整的占空比值

// 根据旋钮，限制当前的最大占空比
void update_max_pwm_duty_coefficient(void)
{
    volatile u16 adc_val = 0;
    adc_sel_pin(ADC_SEL_PIN_P31);
    adc_val = adc_get_val();

    // limited_max_pwm_duty = (u32)adjust_duty * adc_val / 4096; // 会出现指数级下降，因为adjust_duty一直在变化
    // limited_max_pwm_duty = (u32)MAX_PWM_DUTY * adc_val / 4096; // 分级太多，加上抖动和迅速变化，会导致灯光闪烁

#if 1

    // ad值是假设参考电压VCC为4.87V，计算得出
    if (adc_val <= KNOB_DIMMING_MIN_ADC_VAL) // MIN 0%  0.57V
    {
        limited_max_pwm_duty = 0;
    }
    // else if (adc_val <= 1278) // 20% 1.52V
    else if (adc_val <= 1245) // 20% 1.52V /* 这里用5V作为参考电压 */
    {
        // 计算采集到的ad值所占20%对应的ad值得占比，再乘以 20%的占空比
        /*
            计算验证，这里的ad值如果只相差1，占空比的值也只相差1
        */
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY * 2 / 10 / 1278;
    }
    // else if (adc_val <= 2498) // 40% 2.97V
    else if (adc_val <= 2286) // 40% 2.79V /* 这里用5V作为参考电压 */
    {
        /*
            计算验证，这里的ad值如果只相差1，占空比的值也只相差1
        */
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY * 4 / 10 / 2498;
    }
    // else if (adc_val <= 2641) // 50% 3.14V
    else if (adc_val <= 2834) // 50% 3.46V /* 这里用5V作为参考电压 */
    {
        // limited_max_pwm_duty = adc_val * MAX_PWM_DUTY * 5 / 10 / 2641;
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY / 2 / 2641;
    }
    // else if (adc_val <= 3482) // 60% 4.14V
    else if (adc_val <= 3260) // 60% 3.98V /* 这里用5V作为参考电压 */
    {
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY * 6 / 10 / 3482;
    }
    // else if (adc_val <= 4087) // 80% 4.86V
    else if (adc_val <= KNOB_DIMMING_MAX_ADC_VAL) // 
    {
        limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY * 8 / 10 / 4087;
    }
    else
    {
        // limited_max_pwm_duty = (u32)adc_val * MAX_PWM_DUTY / 4095;
        limited_max_pwm_duty = MAX_PWM_DUTY;
    }

    limited_adjust_pwm_duty = (u32)adjust_duty * limited_max_pwm_duty / MAX_PWM_DUTY; // adjust_duty * 旋钮限制的占空比系数

    if (limited_adjust_pwm_duty >= 5950) // 大于该值，直接输出最大功率，防止从MIN扭到MAX时，输出不了最大功率
    {
        limited_adjust_pwm_duty = adjust_duty;
    }

#if USE_MY_DEBUG

    // printf("cur_level %u\n", knob_dimming_cur_level);

#endif // #if USE_MY_DEBUG

#endif
}
