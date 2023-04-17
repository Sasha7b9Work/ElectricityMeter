// 2023/04/05 23:37:15 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/HAL/HAL_PINS.h"
#include <gd32f30x.h>


PinADC pinVolt1(GPIOA, GPIO_PIN_0, ADC_CHANNEL_0);
PinADC pinVolt2(GPIOA, GPIO_PIN_1, ADC_CHANNEL_1);
PinADC pinVolt3(GPIOA, GPIO_PIN_2, ADC_CHANNEL_2);

PinADC pinCur1L(GPIOA, GPIO_PIN_3, ADC_CHANNEL_3);
PinADC pinCur2L(GPIOA, GPIO_PIN_5, ADC_CHANNEL_5);
PinADC pinCur3L(GPIOA, GPIO_PIN_7, ADC_CHANNEL_7);

PinADC pinCur1H(GPIOA, GPIO_PIN_4, ADC_CHANNEL_4);
PinADC pinCur2H(GPIOA, GPIO_PIN_6, ADC_CHANNEL_6);
PinADC pinCur3H(GPIOC, GPIO_PIN_4, ADC_CHANNEL_14);

const uint I2C_ADDR = I2C0;
PinI2C pinI2C_SCL(GPIOB, GPIO_PIN_6);
PinI2C pinI2C_SDA(GPIOB, GPIO_PIN_7);

const uint USART_GPRS_ADDR = UART3;
PinUSART_TX pinUSART_GPRS_TX(GPIOC, GPIO_PIN_10);
PinUSART_RX pinUSART_GPRS_RX(GPIOC, GPIO_PIN_11);

const uint USART_LOG_ADDR = USART1;
PinUSART_TX pinUSART_LOG_TX(GPIOD, GPIO_PIN_5);
PinUSART_RX pinUSART_LOG_RX(GPIOD, GPIO_PIN_6);

const uint SPI_ADDR = SPI2;

PinOUT pinOutMX0(GPIOA, GPIO_PIN_8);
PinOUT pinOutMX1(GPIOA, GPIO_PIN_9);
PinOUT pinOutMX2(GPIOA, GPIO_PIN_10);
PinOUT pinOutMX3(GPIOA, GPIO_PIN_11);
PinOUT pinOutMX4(GPIOA, GPIO_PIN_12);
PinOUT pinOutKMA1(GPIOE, GPIO_PIN_9);
PinOUT pinOutKMA2(GPIOE, GPIO_PIN_8);
PinOUT pinOutKMA3(GPIOE, GPIO_PIN_7);
PinOUT pinOutKMA4(GPIOB, GPIO_PIN_2);
PinOUT pinOutKMA5(GPIOB, GPIO_PIN_1);
PinOUT pinOutKMA6(GPIOB, GPIO_PIN_0);
PinOUT pinOutKMA7(GPIOC, GPIO_PIN_5);
PinOUT pinOutKMA8(GPIOE, GPIO_PIN_10);
PinOUT pinOutKMA9(GPIOE, GPIO_PIN_11);
PinOUT pinOutKMB1(GPIOE, GPIO_PIN_12);
PinOUT pinOutKMB2(GPIOE, GPIO_PIN_13);
PinOUT pinOutKMB3(GPIOE, GPIO_PIN_14);
PinOUT pinOutKMB4(GPIOE, GPIO_PIN_15);
PinOUT pinOutKMB5(GPIOB, GPIO_PIN_10);
PinOUT pinOutKMB6(GPIOB, GPIO_PIN_11);
PinOUT pinOutKMB7(GPIOB, GPIO_PIN_12);
PinOUT pinOutKMB8(GPIOB, GPIO_PIN_13);
PinOUT pinOutKMB9(GPIOB, GPIO_PIN_14);
PinOUT pinOutKMC1(GPIOB, GPIO_PIN_15);
PinOUT pinOutKMC2(GPIOD, GPIO_PIN_8);
PinOUT pinOutKMC3(GPIOD, GPIO_PIN_9);
PinOUT pinOutKMC4(GPIOD, GPIO_PIN_10);
PinOUT pinOutKMC5(GPIOD, GPIO_PIN_11);
PinOUT pinOutKMC6(GPIOD, GPIO_PIN_12);
PinOUT pinOutKMC7(GPIOD, GPIO_PIN_13);
PinOUT pinOutKMC8(GPIOD, GPIO_PIN_14);
PinOUT pinOutKMC9(GPIOD, GPIO_PIN_15);


PinIN pinInKMA1;
PinIN pinInKMA2;
PinIN pinInKMA3;
PinIN pinInKMA4;
PinIN pinInKMA5;
PinIN pinInKMA6;
PinIN pinInKMA7;
PinIN pinInKMA8;
PinIN pinInKMA9;
PinIN pinInKMB1;
PinIN pinInKMB2;
PinIN pinInKMB3;
PinIN pinInKMB4;
PinIN pinInKMB5;
PinIN pinInKMB6;
PinIN pinInKMB7;
PinIN pinInKMB8;
PinIN pinInKMB9;
PinIN pinInKMC1;
PinIN pinInKMC2;
PinIN pinInKMC3;
PinIN pinInKMC4;
PinIN pinInKMC5;
PinIN pinInKMC6;
PinIN pinInKMC7;
PinIN pinInKMC8;
PinIN pinInKMC9;


void PinADC::Init()
{
    gpio_init(port, GPIO_MODE_AIN, GPIO_OSPEED_MAX, pin);
}


void PinI2C::Init()
{
    gpio_init(port, GPIO_MODE_AF_OD, GPIO_OSPEED_50MHZ, pin);
}


void PinUSART_TX::Init()
{
    gpio_init(port, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, pin);
}


void PinUSART_RX::Init()
{
    gpio_init(port, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, pin);
}


void PinOUT::Init()
{
    gpio_init(port, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, pin);
}


void PinOUT::Set()
{
    GPIO_BOP(port) = pin;
}


void PinOUT::Reset()
{
    GPIO_BC(port) = pin;
}


void PinOUT::SetState(bool state)
{
    state ? Set() : Reset();
}


bool PinIN::IsLow()
{
    return gpio_input_bit_get(port, pin) == RESET;
}
