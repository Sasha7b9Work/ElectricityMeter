// 2023/5/8 19:15:14 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


void HAL_TIM1::Init()
{
    /* -----------------------------------------------------------------------
TIMER2CLK is 2KHz
TIMER5 channel0 duty cycle = (2000 / 4000) * 100% = 50%
----------------------------------------------------------------------- */
    timer_parameter_struct timer_initpara;

    timer_deinit(TIMER5);

    /* TIMER configuration */
    timer_initpara.prescaler = 5999;
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_DOWN;
    timer_initpara.period = 399;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;

    timer_init(TIMER5, &timer_initpara);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER5);

    timer_interrupt_flag_clear(TIMER5, TIMER_INT_FLAG_UP);

    timer_update_event_enable(TIMER5);

    timer_interrupt_enable(TIMER5, TIMER_INT_UP);

    /* auto-reload preload enable */
    timer_enable(TIMER5);
}
