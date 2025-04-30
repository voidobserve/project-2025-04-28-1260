#include "knob_dimming.h" // 旋钮调光头文件

volatile u16 limited_max_pwm_duty = 0; // 存放限制的最大占空比

// 根据旋钮，限制当前的最大占空比
void update_max_pwm_duty_coefficient(void)
{
    volatile u16 adc_val = 0;
    adc_sel_pin(ADC_SEL_PIN_P31);
    adc_val = adc_get_val();

    // limited_max_pwm_duty = (u32)adjust_duty * adc_val / 4096; // 会出现指数级下降
    // limited_max_pwm_duty = (u32)adjust_duty * 2048 / 4096;
    limited_max_pwm_duty = (u32)MAX_PWM_DUTY * adc_val / 4096;

    // printf("adc_val %u\n", adc_val);
    // printf("limited_max_pwm_duty %u\n", limited_max_pwm_duty);

#if USE_MY_DEBUG
    {
        static u8 cnt = 0;
        cnt++;
        if (cnt >= 200)
        {
            cnt = 0;
            printf("adc_val %u\n", adc_val);
            printf("limited_max_pwm_duty %u\n", limited_max_pwm_duty);
        }
    }
#endif
}
