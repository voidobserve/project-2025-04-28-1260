#include "knob_dimming.h" // 旋钮调光头文件

volatile u16 limited_max_pwm_duty = 0; // 存放限制的最大占空比

// 根据旋钮，限制当前的最大占空比
void update_max_pwm_duty_coefficient(void)
{
    volatile u16 adc_val = 0;
    volatile u16 knob_dimming_cur_level = 0;
    adc_sel_pin(ADC_SEL_PIN_P31);
    adc_val = adc_get_val();

    // limited_max_pwm_duty = (u32)adjust_duty * adc_val / 4096; // 会出现指数级下降，因为adjust_duty一直在变化
    // limited_max_pwm_duty = (u32)MAX_PWM_DUTY * adc_val / 4096; // 分级太多，加上抖动和迅速变化，会导致灯光闪烁

#if USE_MY_DEBUG

    printf("adc_val %u\n", adc_val);
    printf("limited_max_pwm_duty %u\n", limited_max_pwm_duty);

    {
        static u8 cnt = 0;
        cnt++;
        if (cnt >= 200)
        {
            cnt = 0;
            // printf("adc_val %u\n", adc_val);
            // printf("limited_max_pwm_duty %u\n", limited_max_pwm_duty);
        }
    }
#endif

#if 1

    knob_dimming_cur_level = (adc_val - KNOB_DIMMING_MIN_ADC_VAL) / KNOB_DIMMING_ADC_VAL_PER_LEVEL;
    if (adc_val >= (KNOB_DIMMING_MAX_ADC_VAL))
    {
        // 大于某个值，直接输出100%
        limited_max_pwm_duty = MAX_PWM_DUTY;
        knob_dimming_cur_level = KNOB_DIMMING_LEVELS;
    }
    else if (adc_val <= (KNOB_DIMMING_MIN_ADC_VAL))
    {
        // 小于某个值，直接关闭
        limited_max_pwm_duty = 0;
        knob_dimming_cur_level = 0;
    }
    else
    {
        // 计算得出当前ad值对应的等级
        // u16 knob_dimming_cur_level = (adc_val - KNOB_DIMMING_MIN_ADC_VAL) / KNOB_DIMMING_ADC_VAL_PER_LEVEL;

        if (adc_val > (knob_dimming_cur_level * KNOB_DIMMING_ADC_VAL_PER_LEVEL + KNOB_DIMMING_ADC_DEAD_ZONE_PER_LEVEL) &&
            adc_val < ((knob_dimming_cur_level + 1) * KNOB_DIMMING_ADC_VAL_PER_LEVEL - KNOB_DIMMING_ADC_DEAD_ZONE_PER_LEVEL))
        {
            // 如果采集的ad值落在这一级
        }
        else if (adc_val < (knob_dimming_cur_level * KNOB_DIMMING_ADC_VAL_PER_LEVEL + KNOB_DIMMING_ADC_DEAD_ZONE_PER_LEVEL))
        {
            // 如果采集的ad值小于这一级对应的ad值+死区值
            if (knob_dimming_cur_level > 0)
            {
                knob_dimming_cur_level--;
            }
        }

        limited_max_pwm_duty = (u32)MAX_PWM_DUTY * knob_dimming_cur_level / KNOB_DIMMING_LEVELS;
    }

#if USE_MY_DEBUG

    printf("cur_level %u\n", knob_dimming_cur_level);

#endif // #if USE_MY_DEBUG

#endif
}
