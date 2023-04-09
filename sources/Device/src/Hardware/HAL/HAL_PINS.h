// 2023/04/05 23:31:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


struct PinADC
{
    PinADC(uint _port, uint _pin, uint8 _channel) : port(_port), pin(_pin), channel(_channel) { }
    void Init();
    uint8 Channel() const { return channel; }
private:
    uint port;
    uint pin;
    uint8 channel;
};


struct PinI2C
{
    PinI2C(uint _port, uint _pin) : port(_port), pin(_pin) { }
    void Init();
private:
    uint port;
    uint pin;
};


struct PinUSART_TX
{
    PinUSART_TX(uint _port, uint _pin) : port(_port), pin(_pin) { }
    void Init();
private:
    uint port;
    uint pin;
};


struct PinUSART_RX
{
    PinUSART_RX(uint _port, uint _pin) : port(_port), pin(_pin) { }
    void Init();
private:
    uint port;
    uint pin;
};


struct PinOUT
{
    PinOUT(uint _port, uint _pin) : port(_port), pin(_pin) { }
    void Init();
    void Set();
    void Reset();
private:
    uint port;
    uint pin;
};


extern PinADC pinVolt1;     // ACVOLTOUT1
extern PinADC pinVolt2;     // ACVOLTOUT2
extern PinADC pinVolt3;     // ACVOLTOUT3

extern PinADC pinCur1L;     // ACCURROUTL1
extern PinADC pinCur2L;     // ACCURROUTL2
extern PinADC pinCur3L;     // ACCURROUTL3

extern PinADC pinCur1H;     // ACCURROUTH1
extern PinADC pinCur2H;     // ACCURROUTH2
extern PinADC pinCur3H;     // ACCURROUTH3

extern const uint I2C_ADDR;
extern PinI2C pinI2C_SCL;
extern PinI2C pinI2C_SDA;

extern const uint USART_GPRS_ADDR;
extern PinUSART_TX pinUSART_GPRS_TX;
extern PinUSART_RX pinUSART_GPRS_RX;

extern const uint USART_LOG_ADDR;
extern PinUSART_TX pinUSART_LOG_TX;
extern PinUSART_RX pinUSART_LOG_RX;

extern const uint SPI_ADDR;

extern PinOUT pinOutMX0;
extern PinOUT pinOutMX1;
extern PinOUT pinOutMX2;
extern PinOUT pinOutMX3;
extern PinOUT pinOutMX4;
extern PinOUT pinOutKMA1;
extern PinOUT pinOutKMA2;
extern PinOUT pinOutKMA3;
extern PinOUT pinOutKMA4;
extern PinOUT pinOutKMA5;
extern PinOUT pinOutKMA6;
extern PinOUT pinOutKMA7;
extern PinOUT pinOutKMA8;
extern PinOUT pinOutKMA9;
extern PinOUT pinOutKMB1;
extern PinOUT pinOutKMB2;
extern PinOUT pinOutKMB3;
extern PinOUT pinOutKMB4;
extern PinOUT pinOutKMB5;
extern PinOUT pinOutKMB6;
extern PinOUT pinOutKMB7;
extern PinOUT pinOutKMB8;
extern PinOUT pinOutKMB9;
extern PinOUT pinOutKMC1;
extern PinOUT pinOutKMC2;
extern PinOUT pinOutKMC3;
extern PinOUT pinOutKMC4;
extern PinOUT pinOutKMC5;
extern PinOUT pinOutKMC6;
extern PinOUT pinOutKMC7;
extern PinOUT pinOutKMC8;
extern PinOUT pinOutKMC9;
