#include "timer2.h"
#define TIMER2_PEROID_VAL (SYSCLK / 128 / 10000 - 1) // 周期值=系统时钟/分频/频率 - 1

extern volatile bit flag_is_in_power_on; // 是否处于开机缓启动

static volatile u16 pwm_duty_add_cnt; // 用于控制pwm增加的时间计数
static volatile u16 pwm_duty_sub_cnt; // 用于控制pwm递减的时间计数

volatile bit flag_is_pwm_add_time_comes = 0; // 标志位，pwm占空比递增时间到来
volatile bit flag_is_pwm_sub_time_comes = 0; // 标志位，pwm占空比递减时间到来

static volatile u16 pwm_duty_change_cnt = 0;    // 用于控制pwm变化的时间计数（用在旋钮调节的PWM占空比中）
// volatile bit flag_is_pwm_change_time_comes = 0; // 标志位，pwm变化时间到来（用在旋钮调节的PWM占空比中）

void timer2_config(void)
{
    __EnableIRQ(TMR2_IRQn); // 使能timer2中断
    IE_EA = 1;              // 使能总中断

    // 设置timer2的计数功能，配置一个频率为1kHz的中断
    TMR_ALLCON = TMR2_CNT_CLR(0x1);                               // 清除计数值
    TMR2_PRH = TMR_PERIOD_VAL_H((TIMER2_PEROID_VAL >> 8) & 0xFF); // 周期值
    TMR2_PRL = TMR_PERIOD_VAL_L((TIMER2_PEROID_VAL >> 0) & 0xFF);
    TMR2_CONH = TMR_PRD_PND(0x1) | TMR_PRD_IRQ_EN(0x1);                          // 计数等于周期时允许发生中断
    TMR2_CONL = TMR_SOURCE_SEL(0x7) | TMR_PRESCALE_SEL(0x7) | TMR_MODE_SEL(0x1); // 选择系统时钟，128分频，计数模式
}

// 定时器 中断服务函数
void TIMR2_IRQHandler(void) interrupt TMR2_IRQn
{
    // 进入中断设置IP，不可删除
    __IRQnIPnPush(TMR2_IRQn);

    // ---------------- 用户函数处理 -------------------

    // 周期中断
    if (TMR2_CONH & TMR_PRD_PND(0x1)) // 约100us触发一次中断
    {
        TMR2_CONH |= TMR_PRD_PND(0x1); // 清除pending

        // tmr2_cnt++;
        // P13 = ~P13; // 测试中断触发周期

        pwm_duty_add_cnt++;
        pwm_duty_sub_cnt++;
        pwm_duty_change_cnt++;

        if (pwm_duty_sub_cnt >= 13) // 1300us，1.3ms
        // if (pwm_duty_sub_cnt >= 50)
        {
            pwm_duty_sub_cnt = 0;
            flag_is_pwm_sub_time_comes = 1;
        }

        // if (pwm_duty_add_cnt >= 133) // 13300us, 13.3ms
        if (pwm_duty_add_cnt >= 13) //
        {
            pwm_duty_add_cnt = 0;
            flag_is_pwm_add_time_comes = 1;
        }

#if 1
        // if (pwm_duty_change_cnt >= 10) // 1000us,1ms
        if (pwm_duty_change_cnt >= 5) // 500us
        {

            pwm_duty_change_cnt = 0;
            // flag_is_pwm_change_time_comes = 1;

#if 1
            if (0 == flag_is_in_power_on) // 不处于开机缓启动，才使能PWM占空比调节
            {
                if (limited_adjust_pwm_duty > c_duty)
                {
                    c_duty++;


                    // // if ((limited_adjust_pwm_duty - c_duty) < 20) // 在旋钮挡位20%时，灯光闪烁最明显
                    // if ((limited_adjust_pwm_duty - c_duty) < 50) // 旋钮挡位在10%附近时，灯光会有微微闪烁
                    // // if ((limited_adjust_pwm_duty - c_duty) < 80) //  
                    // {
                    //     if (pwm_duty_change_cnt >= 10000) // 1s
                    //     {
                    //         pwm_duty_change_cnt = 0;
                    //         c_duty++;
                    //     }
                    // }
                    // else
                    // {
                    //     if (pwm_duty_change_cnt >= 5) // 500us
                    //     {
                    //         pwm_duty_change_cnt = 0;
                    //         c_duty++;
                    //     }
                    // }
                }
                else if (limited_adjust_pwm_duty < c_duty)
                {
                    c_duty--;

                    // // if ((c_duty - limited_adjust_pwm_duty) < 20) // 在旋钮挡位20%时，灯光闪烁最明显
                    // if ((c_duty - limited_adjust_pwm_duty) < 50) // 旋钮挡位在10%附近时，灯光会有微微闪烁
                    // // if ((c_duty - limited_adjust_pwm_duty) < 80) // 
                    // {
                    //     if (pwm_duty_change_cnt >= 10000) // 1s
                    //     {
                    //         pwm_duty_change_cnt = 0;
                    //         c_duty--;
                    //     }
                    // }
                    // else
                    // {
                    //     if (pwm_duty_change_cnt >= 5) // 500us
                    //     {
                    //         pwm_duty_change_cnt = 0;
                    //         c_duty--;
                    //     }
                    // }
                }
                else // 如果相等
                {
                    // last_limited_max_pwm_duty = limited_max_pwm_duty;
                    // flag_is_knob_change = 0;
                    // pwm_duty_change_cnt = 0;
                }

                set_pwm_duty(); // 函数内部会将 c_duty 的值代入相关寄存器中

                // if (c_duty <= KNOB_DIMMING_MIN_ADC_VAL) // 小于某个值，直接输出0%占空比，关闭PWM输出，引脚配置为输出模式(尽量小于等于2%的占空比再灭灯)
                if (c_duty <= 0) // 小于某个值，直接输出0%占空比，关闭PWM输出，引脚配置为输出模式(尽量小于等于2%的占空比再灭灯)
                {
                    // 直接输出0%的占空比，可能会有些跳动，需要将对应的引脚配置回输出模式，输出低电平
                    STMR_PWMEN &= ~0x01;          // 不使能PWM0的输出
                    FOUT_S16 = GPIO_FOUT_AF_FUNC; //
                    P16 = 1;                      // 高电平为关灯
                }
                // else if (c_duty >= KNOB_DIMMING_MIN_ADC_VAL) // 大于某个值，再打开PWM，引脚配置回PWM
                else if (c_duty >= 0) // 大于某个值，再打开PWM，引脚配置回PWM
                {
                    FOUT_S16 = GPIO_FOUT_STMR0_PWMOUT; // stmr0_pwmout
                    STMR_PWMEN |= 0x01;                // 使能PWM0的输出
                }
            } // if (0 == flag_is_in_power_on) // 不处于开机缓启动，才使能PWM占空比调节
#endif

#if 0
            // printf("c_duty %u\n", c_duty);
            // printf(",c=%u\n", c_duty);
#endif
        }

#endif
    }

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(TMR2_IRQn);
}