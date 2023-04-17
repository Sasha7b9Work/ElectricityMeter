#include "defines.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>


#define I2C_SPEED              400000
#define I2C_SLAVE_ADDRESS7     (0x3c << 1)
#define I2C_PAGE_SIZE           8


namespace HAL_I2C
{
    static void WaitFlagYes(i2c_flag_enum);
    static void WaitFlagNo(i2c_flag_enum);
}


void HAL_I2C::WaitFlagYes(i2c_flag_enum flag)
{
    TimeMeterMS meter;

    while (i2c_flag_get(I2C_ADDR, flag))
    {
        if (meter.ElapsedTime() > 100)
        {
            break;
        }
    }
}


void HAL_I2C::WaitFlagNo(i2c_flag_enum flag)
{
    TimeMeterMS meter;

    while (!i2c_flag_get(I2C_ADDR, flag))
    {
        if (meter.ElapsedTime() > 100)
        {
            break;
        }
    }
}


void HAL_I2C::Init()
{
    pinI2C_SCL.Init();
    pinI2C_SDA.Init();

    /* configure I2C clock */
    i2c_clock_config(I2C_ADDR, I2C_SPEED, I2C_DTCY_2);

    /* configure I2C address */
    i2c_mode_addr_config(I2C_ADDR, I2C_I2CMODE_ENABLE, I2C_ADDFORMAT_7BITS, I2C_SLAVE_ADDRESS7);

    /* enable I2C_ADDR */
    i2c_enable(I2C_ADDR);

    /* enable acknowledge */
    i2c_ack_config(I2C_ADDR, I2C_ACK_ENABLE);
}


void HAL_I2C::Write(uint8 command, uint8 *data, int size)
{
    TimeMeterMS meter;

    /* wait until I2C bus is idle */
    WaitFlagYes(I2C_FLAG_I2CBSY);

    /* send a start condition to I2C bus */
    i2c_start_on_bus(I2C_ADDR);

    /* wait until SBSEND bit is set */
    WaitFlagNo(I2C_FLAG_SBSEND);

    /* send slave address to I2C bus */
    i2c_master_addressing(I2C_ADDR, I2C_SLAVE_ADDRESS7, I2C_TRANSMITTER);

    /* wait until ADDSEND bit is set */
    WaitFlagNo(I2C_FLAG_ADDSEND);

    /* N=1,reset ACKEN bit before clearing ADDRSEND bit */
    i2c_ack_config(I2C_ADDR, I2C_ACK_DISABLE);

    /* clear ADDSEND bit */
    i2c_flag_clear(I2C_ADDR, I2C_FLAG_ADDSEND);

    /* send command */
    i2c_data_transmit(I2C_ADDR, command);

    /* wait until the TBE bit is set */
    WaitFlagNo(I2C_FLAG_TBE);

    /* send array of data */
    for (int i = 0; i < size; i++)
    {
        i2c_data_transmit(I2C_ADDR, *data++);

        /* wait until the TBE bit is set */
        WaitFlagNo(I2C_FLAG_TBE);
    }

    /* send a stop condition to I2C bus */
    i2c_stop_on_bus(I2C_ADDR);

    /* wait until stop condition generate */
    while (I2C_CTL0(I2C_ADDR) & 0x0200)
    {
        if (meter.ElapsedTime() > 100)
        {
            break;
        }
    }

    /* Enable Acknowledge */
    i2c_ack_config(I2C_ADDR, I2C_ACK_ENABLE);
}
