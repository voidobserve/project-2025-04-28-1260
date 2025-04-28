#include "knob_dimming.h" // 旋钮调光头文件

volatile u16 limited_max_pwm_duty = 0; // 存放限制的最大占空比

// 根据旋钮，限制当前的最大占空比
void update_max_pwm_duty_coefficient(void)
{
    volatile u16 adc_val = 0;
    adc_sel_pin(ADC_SEL_PIN_P31);
    adc_val = adc_get_val();

    limited_max_pwm_duty = (u32)adjust_duty * adc_val / 4096;

    // adjust_duty = limited_max_pwm_duty;
}
