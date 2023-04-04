// 2023/04/03 19:52:48 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>
#include <systick.h>


/*
    ADC012_IN0      23  PA0
    ADC012_IN1      24  PA1
    ADC012_IN2      25  PA2
    ADC012_IN3      26  PA3
    ADC012_IN4      29  PA4
    ADC012_IN5      30  PA5
    ADC012_IN6      31  PA6
    ADC012_IN7      32  PA7
    ADC012_IN8      35  PB0
    ADC012_IN9      36  PB1
    ADC012_IN10     15  PC0
    ADC012_IN11     16  PC1
    ADC012_IN12     17  PC2
    ADC012_IN13     18  PC3
    ADC012_IN14     33  PC4
    ADC012_IN15     34  PC5
*/


static uint16 adc_value[6] = { 0, 0, 0, 0, 0, 0 };


void HAL_ADC_Update()
{
    for (int i = 0; i < 6; i++)
    {
        volatile uint16 value = adc_value[i];

        if (value != 0)
        {
            int i = 0;
        }
    }
}


void HAL_ADC_Init()
{
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_ADC0);
    rcu_periph_clock_enable(RCU_DMA0);
    rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV6);

    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_0);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_1);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_2);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_3);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_4);
    gpio_init(GPIOA, GPIO_MODE_AIN, GPIO_OSPEED_10MHZ, GPIO_PIN_5);

    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;

    /* ADC DMA_channel configuration */
    dma_deinit(DMA0, DMA_CH0);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr = (uint32_t)(&ADC_RDATA(ADC0));
    dma_data_parameter.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr = (uint32_t)(&adc_value);
    dma_data_parameter.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
    dma_data_parameter.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number = 6;
    dma_data_parameter.priority = DMA_PRIORITY_HIGH;
    dma_init(DMA0, DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA0, DMA_CH0);

    /* enable DMA channel */
    dma_channel_enable(DMA0, DMA_CH0);


    /* ADC mode config */
    adc_mode_config(ADC_MODE_FREE);

    /* ADC continuous function enable */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);

    /* ADC scan function enable */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, ENABLE);

    /* ADC data alignment config */
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);

    /* ADC channel length config */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 6);

    /* ADC regular channel config */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 1, ADC_CHANNEL_1, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 2, ADC_CHANNEL_2, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 3, ADC_CHANNEL_3, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 4, ADC_CHANNEL_4, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(ADC0, 5, ADC_CHANNEL_5, ADC_SAMPLETIME_55POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);
    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);

    /* ADC DMA function enable */
    adc_dma_mode_enable(ADC0);

    /* enable ADC interface */
    adc_enable(ADC0);
    delay_1ms(1);

    /* ADC calibration and reset calibration */
    adc_calibration_enable(ADC0);

    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);
}
