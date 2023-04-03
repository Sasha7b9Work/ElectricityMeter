#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include <gd32f30x.h>


#define I2C0_SPEED              100000
#define I2C0_SLAVE_ADDRESS7     (0x3c << 1)
#define I2C_PAGE_SIZE           8


void HAL_I2C0_Init(void)
{
    /* enable GPIOB clock */
    rcu_periph_clock_enable(RCU_GPIOB);
    /* enable I2C0 clock */
    rcu_periph_clock_enable(RCU_I2C0);

    /* connect PB6 to I2C0_SCL */
    /* connect PB7 to I2C0_SDA */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_6 | GPIO_PIN_7);

    /* configure I2C clock */
    i2c_clock_config(I2C0,I2C0_SPEED,I2C_DTCY_2);
    /* configure I2C address */
    i2c_mode_addr_config(I2C0,I2C_I2CMODE_ENABLE,I2C_ADDFORMAT_7BITS,I2C0_SLAVE_ADDRESS7);
    /* enable I2C0 */
    i2c_enable(I2C0);
    /* enable acknowledge */
    i2c_ack_config(I2C0,I2C_ACK_ENABLE);
}

void HAL_I2C0_Write8(uint8 data)
{
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);

    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_TRANSMITTER);

    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));

    /* N=1,reset ACKEN bit before clearing ADDRSEND bit */
    i2c_ack_config(I2C0, I2C_ACK_DISABLE);

    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    i2c_data_transmit(I2C0, data);
    
    /* wait until the TBE bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);

    /* wait until stop condition generate */
    while(I2C_CTL0(I2C0) & 0x0200);

    /* Enable Acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}


void HAL_I2C0_Write(uint8 *data, int size)
{
    /* wait until I2C bus is idle */
    while(i2c_flag_get(I2C0, I2C_FLAG_I2CBSY));

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C0);

    /* wait until SBSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_SBSEND));

    /* send slave address to I2C bus */
    i2c_master_addressing(I2C0, I2C0_SLAVE_ADDRESS7, I2C_TRANSMITTER);

    /* wait until ADDSEND bit is set */
    while(!i2c_flag_get(I2C0, I2C_FLAG_ADDSEND));

    /* N=1,reset ACKEN bit before clearing ADDRSEND bit */
    i2c_ack_config(I2C0, I2C_ACK_DISABLE);

    /* clear ADDSEND bit */
    i2c_flag_clear(I2C0, I2C_FLAG_ADDSEND);

    for(int i = 0; i < size; i++)
    {
        i2c_data_transmit(I2C0, *data++);
    
        /* wait until the TBE bit is set */
        while(!i2c_flag_get(I2C0, I2C_FLAG_TBE));
    }

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C0);

    /* wait until stop condition generate */
    while(I2C_CTL0(I2C0) & 0x0200);

    /* Enable Acknowledge */
    i2c_ack_config(I2C0, I2C_ACK_ENABLE);
}
