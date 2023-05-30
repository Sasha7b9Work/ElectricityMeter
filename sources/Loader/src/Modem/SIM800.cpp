// 2023/5/3 11:29:56 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include "Hardware/HAL/HAL.h"
#include "Modem/Parser.h"
#include "Modem/SIM800.h"
#include "Application.h"
#include <cstring>
#include <cstdio>


using namespace Parser;
using namespace std;


namespace Updater
{
    void Update(pchar);
}


namespace SIM800
{
    struct State
    {
        enum E
        {
            START,
            WAIT_ATE0,
            WAIT_BAUDRADE,
            WAIT_GSMBUSY,
            WAIT_CREG_INIT,
            WAIT_REGISTRATION,
            WAIT_IP_INITIAL,
            RUNNING_UPDATER
        };

        static TimeMeterMS meter;

        static void Set(E);
    };

    TimeMeterMS State::meter;

    static State::E state = State::START;

    void State::Set(E new_state)
    {
        state = new_state;
        meter.Reset();
    }

    void Reset()
    {
        state = State::START;
        Modem::Reset();
    }

    static bool MeterIsRunning(uint time)
    {
        if (State::meter.ElapsedTime() <= time)
        {
            return true;
        }

        Reset();

        return false;
    }


    // Передать без завершающего 0x0d
    void TransmitRAW(pchar);
    void TransmitUINT8(uint8);

    static bool ProcessUnsolicited(pchar);

    static TimeMeterMS meterCSQ;

    static char levelSignal[16] = { '0', '\0' };
}


bool SIM800::ProcessUnsolicited(pchar answer)
{
    char buffer[32];

    pchar first_word = Parser::GetWord(answer, 1, buffer);

    if (strcmp(answer, "CLOSED") == 0)
    {
        Reset();
        return true;
    }
    else if (strcmp(first_word, "+CSQ") == 0)
    {
        strcpy(levelSignal, Parser::GetWord(answer, 2, buffer));
        return true;
    }
    else if (strcmp(answer, "SEND FAIL") == 0)
    {
        return true;
    }
    else if (strcmp(answer, "SEND OK") == 0)
    {
        return true;
    }
    else if (strcmp(first_word, "+IPD") == 0)
    {
        return false;
    }

    return false;
}


void SIM800::Update(pchar answer)
{
    if (ProcessUnsolicited(answer))
    {
        return;
    }

    const uint DEFAULT_TIME = 10000;

    char buffer[32];

    switch (state)
    {
    case State::START:
        SIM800::Transmit::With0D("ATE0");
        State::Set(State::WAIT_ATE0);
        strcpy(levelSignal, "0");
        break;

    case State::WAIT_ATE0:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                State::Set(State::WAIT_BAUDRADE);
                SIM800::Transmit::With0D("AT+IPR=115200");
            }
        }
        break;

    case State::WAIT_BAUDRADE:

        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "RDY") == 0)
            {
                State::Set(State::WAIT_GSMBUSY);
                SIM800::Transmit::With0D("AT+GSMBUSY=1");
            }
        }
        break;

    case State::WAIT_GSMBUSY:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                State::Set(State::WAIT_CREG_INIT);
                SIM800::Transmit::With0D("AT+CREG=1");
            }
        }
        break;

    case State::WAIT_CREG_INIT:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(answer, "OK") == 0)
            {
                State::Set(State::WAIT_REGISTRATION);
            }
        }
        break;

    case State::WAIT_REGISTRATION:
        if (MeterIsRunning(60000))
        {
            if (strcmp(GetWord(answer, 1, buffer), "+CREG") == 0)
            {
                int stat = GetWord(answer, 2, buffer)[0] & 0x0f;

                if (stat == 1 ||    // Registered, home network
                    stat == 5)      // Registered, roaming
                {
                    State::Set(State::WAIT_IP_INITIAL);
                    SIM800::Transmit::With0D("AT+CIPSTATUS");
                }
            }
        }
        break;

    case State::WAIT_IP_INITIAL:
        if (MeterIsRunning(DEFAULT_TIME))
        {
            if (strcmp(GetWord(answer, 3, buffer), "INITIAL") == 0)
            {
                state = State::RUNNING_UPDATER;
            }
        }
        break;

    case State::RUNNING_UPDATER:
        Updater::Update(answer);
        break;
    }
}


pchar SIM800::LevelSignal()
{
    return levelSignal;
}


void SIM800::Transmit::With0D(pchar message)
{
    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);
}


void SIM800::Transmit::Format(pchar format, pchar param)
{
    char buffer[64];
    std::sprintf(buffer, (char *)format, param);
    SIM800::Transmit::With0D(buffer);
}


void SIM800::Transmit::Format(pchar format, int param)
{
    char buffer[64];
    std::sprintf(buffer, (char *)format, param);
    SIM800::Transmit::With0D(buffer);
}


void SIM800::TransmitRAW(pchar message)
{
    HAL_USART_GPRS::Transmit(message);
}


void SIM800::Transmit::UINT8(uint8 byte)
{
    HAL_USART_GPRS::Transmit(&byte, 1);
}
