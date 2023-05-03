﻿// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Commands.h"
#include <cstring>


namespace Modem
{
    void CallbackOnErrorSIM800();
}


namespace MQTT
{
    void Connect();
}


namespace SIM800
{
    struct State
    {
        enum E
        {
            START,
            WAIT_REGISTRATION,

            RUNNING
        };
    };

    static State::E state = State::START;

    static String last_answer;

    // Возращает время до получения ответа
    void Transmit(pchar);
    void TransmitUINT8(uint8);
    void TransmitUINT(uint);

    // Передаёт сообщение и возвращает true, если принят ответ answer
    bool TransmitAndWaitAnswer(pchar message, pchar answer, uint timeout = TIME_WAIT_ANSWER);

    void Update();

    // Ожидает ответа. Возвращает true, если ответ получен
    static bool WaitAnswer(pchar, uint timeout = TIME_WAIT_ANSWER);

    static void Reset();

    void HandleNewAnswer(pchar);

    String LastAnswer();
}


void SIM800::Update()
{
    static TimeMeterMS meter;

    switch (state)
    {
    case State::START:
        SIM800::Transmit("ATE0");
        if (SIM800::TransmitAndWaitAnswer("AT+GSMBUSY=1", "OK"))
        {
            state = State::WAIT_REGISTRATION;
            meter.Reset();
        }
        else
        {
            Reset();
        }
        break;

    case State::WAIT_REGISTRATION:
        if (Command::RegistrationIsOk())
        {
            if (!SIM800::TransmitAndWaitAnswer("AT+CSTT=\"internet\",\"\",\"\"", "OK"))
            {
                Reset();
            }

            TimeMeterMS().Wait(1000);

            if (!SIM800::TransmitAndWaitAnswer("AT+CIICR", "OK"))
            {
                Reset();
            }

            TimeMeterMS().Wait(1000);

            SIM800::Transmit("AT+CIFSR");

            TimeMeterMS().Wait(1000);

            if (!Command::ConnectToTCP())
            {
                Reset();
            }
            else
            {
                MQTT::Connect();

                state = State::RUNNING;
            }
        }
        else if (meter.ElapsedTime() > 30000)
        {
            Reset();
        }
        break;

    case State::RUNNING:
        break;
    }
}


void SIM800::Transmit(pchar message)
{
    last_answer.Set("");

    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);
}


void SIM800::TransmitUINT8(uint8 byte)
{
    HAL_USART_GPRS::Transmit(&byte, 1);
}


void SIM800::TransmitUINT(uint value)
{
    HAL_USART_GPRS::Transmit(&value, 4);
}


bool SIM800::TransmitAndWaitAnswer(pchar message, pchar answer, uint timeout)
{
    Transmit(message);

    return WaitAnswer(answer, timeout);
}


void SIM800::Reset()
{
    state = State::START;
    Modem::CallbackOnErrorSIM800();
}


bool SIM800::WaitAnswer(pchar answer, uint timeout)
{
    TimeMeterMS meter;

    while (meter.ElapsedTime() < timeout)
    {
        if (last_answer == answer)
        {
            return true;
        }
    }

    return false;
}


void SIM800::HandleNewAnswer(pchar answer)
{
    last_answer.Set(answer);
}


String SIM800::LastAnswer()
{
    return last_answer;
}
