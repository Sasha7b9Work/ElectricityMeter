// (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include <gd32f30x.h>


/*
*   ��������������� �������
*   ADC0     - ��������� ������� ����������
*   DMA0_CH0 - ���
*   TIM0_CH0 - ���
*   TIM1     - ������ ��
*/


void HAL::Init()
{
    systick_config();
    
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    
    HAL_I2C0::Init();

    HAL_ADC::Init();
}


void HAL::ErrorHandler()
{
    while (1)
    {
    }
}
