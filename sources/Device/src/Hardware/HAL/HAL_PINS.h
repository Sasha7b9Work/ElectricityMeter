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

