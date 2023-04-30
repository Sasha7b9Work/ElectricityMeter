// 2023/03/16 16:31:54 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Modules/SIM800C/SIM800C.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/Timer.h"
#include <gd32f30x.h>
#include <cstring>


/*
        5. РАБОТА С GSM МОДЕМОМ
    Устройство имеет GSM/GPRS модем Simcom SIM800C, служащий для передачи данных на сервер по GPRS и удаленного управления контроллером.
    Модем имеет собственный источник питания +4В, управляемый портом PD2 (GSM_PWR), 0 – питание включено, 1 – питание выключено.
    Контроль питания модема осуществляется портом PE2 (GSM_PG). Данный порт является толерантным к 5В, поэтому подключен к цепи +4В без делителя.
    Порядок включения модема.
    1)  Выход GSM_PWR перевести в 1, тем самым обесточив модем.
    2)  Порт GSM_PG перевести в состояние выход 0. Необходимо убедиться, что конденсаторы фильтра питания модема разряжены и на цепи его питания
        нет остаточного напряжения, могущего привести его в неопределенное состояние или зависание. Резистор 1k в цепи станет нагрузкой для разряда конденсаторов.
    3)  Выждать 1000мс для полного разряда емкостей.
    4)  Порт GSM_PG перевести в состояние вход с подтяжкой к земле (pull-down).
    5)  Выход GSM_PWR перевести в 0, тем самым включив модем.
    6)  Ожидать появления 1 на входе GSM_PG, либо выход по таймауту 100мс, при наступлении таймаута считать, что питание модема неисправно и работать оффлайн.
    7)  Ждать не менее 0,5с
    8)  Выход GSM_PWRKEY (PD0) перевести в 0 (аналог удержания кнопки включения) на 1,25с.
    9)  Если в течение 5с на порту PD1 (GSM_STATUS) лог.0 и не появилась единица, то модем по какой-то причине не включился, повторить всю процедуру сначала.
    10)  С момента появления лог.1 на GSM_STATUS готов к работе UART модема, ожидать регистрации в сети не менее 30с, если за 30с регистрации в сети нет, то
        начать всю процедуру сначала.

        Исходные состояния

        GSM_PWR = 1
        GSM_PWRKEY = 1
*/


/*
    Последовательность инициализации

    ATE0 – отключаем режим ЭХО.
    AT+GSMBUSY=0 – запрещаем входящие звонки.
    AT+CPAS – проверяем готовностью и текущее состояние модуля.
    AT+CREG? – проверка регистрации в сети.
    AT+CSQ – проверка уровня сигнала, уровень нужно будет потом вывести на экран
    GPRS test
    AT+SAPBR=3,1,"APN","internet"
    AT+SAPBR=3,1,"USER",""
    AT+SAPBR=3,1,"PWD",""
    AT+SAPBR=1,1
    AT+HTTPINIT
    AT+HTTPPARA="CID",1
*/


namespace Modem
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_DISCHARGE_CAPS,    // Ожидание разряда конденсатора после обесточивания модема
            WAIT_HI_GSM_PG,         // Ожидаине появления 1 на входе GSM_PG после включения модема
            WAIT_500_MS,            // Ожидать полсекунды после появления 1 на входе GSM_PG
            WAIT_1250_MS,           // Ожидать 1.25 сек после перевода GSM_PWRKEY в 0
            WAIT_5000_MS,           // Ожидать 5 секунд единицу на GSM_STATUS
            WAIT_REGISTRATION,      // Ожидание регистрации
            NORMAL                  // Подключились, зарегистрировались
        };
    };

    static State::E state = State::IDLE;

    static const int SIZE_BUFFER = 128;
    static char answer[SIZE_BUFFER] = { '\0' };
    static int pointer = 0;

    namespace GSM_PG
    {
        static void ToOutLow();

        static void ToInPullDown();

        static bool ReadInput();
    }

    // Возвращает true, если произошла регистрация в сети
    static bool RegistrationIsOk();

    static bool ExistAnswer();
}


void Modem::Update()
{
    static TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        pinGSM_PWR.Set();
        GSM_PG::ToOutLow();
        state = State::WAIT_DISCHARGE_CAPS;
        meter.Reset();
        break;

    case State::WAIT_DISCHARGE_CAPS:
        if (meter.ElapsedTime() > 1000)
        {
            GSM_PG::ToInPullDown();
            pinGSM_PWR.Reset();
            state = State::WAIT_HI_GSM_PG;
            meter.Reset();
        }
        break;

    case State::WAIT_HI_GSM_PG:
#ifdef OLD_VERSION
        meter.Reset();
        state = State::WAIT_500_MS;
#endif
        if (GSM_PG::ReadInput())
        {
            meter.Reset();
            state = State::WAIT_500_MS;
        }
        if (meter.ElapsedTime() > 100)
        {
            state = State::IDLE;
        }
        break;

    case State::WAIT_500_MS:
        if (meter.ElapsedTime() > 500)
        {
            pinGSM_PWRKEY.Reset();
            meter.Reset();
            state = State::WAIT_1250_MS;
        }
        break;

    case State::WAIT_1250_MS:
        if (meter.ElapsedTime() > 1250)
        {
            pinGSM_PWRKEY.Set();
            meter.Reset();
            state = State::WAIT_5000_MS;
        }
        break;

    case State::WAIT_5000_MS:
        if (pinGSM_STATUS.IsHi())
        {
            state = State::WAIT_REGISTRATION;
            meter.Reset();
        }
        if (meter.ElapsedTime() > 5000)
        {
            state = State::IDLE;
        }
        break;

    case State::WAIT_REGISTRATION:
        if (RegistrationIsOk())
        {
            state = State::NORMAL;
        }
        if (meter.ElapsedTime() > 30 * 1000)
        {
            state = State::IDLE;
        }
        break;

    case State::NORMAL:
        break;
    }
}


void Modem::Init()
{
    pinGSM_PWR.Init();
    pinGSM_PWRKEY.Init();
    pinGSM_STATUS.Init();

    pinGSM_PWR.Set();
    pinGSM_PWRKEY.Set();

    SIM800C::Init();

    Modem::Transmit("ATE0");
    Modem::Transmit("ATV0");
}


bool Modem::ExistUpdate()
{
    return false;
}


void Modem::Transmit(pchar message)
{
    answer[0] = '\0';
    pointer = 0;

    HAL_USART_GPRS::Transmit(message);

    static const char end_message[2] = { 0x0d, 0 };

    HAL_USART_GPRS::Transmit(end_message);
}


pchar Modem::LastAnswer()
{
    if (ExistAnswer())
    {
        return answer;
    }
    
    static const pchar null_answer = "";

    return null_answer;
}


bool Modem::ExistAnswer()
{
    for (int i = 0; i < pointer; i++)
    {
        if (answer[i] == 0x0d)
        {
            return true;
        }
    }

    return false;
}


void Modem::CallbackOnReceive(char symbol)
{
    // Если в приёмном буфере уже есть ответ - очищаем его
    if (pointer != 0 && answer[pointer - 1] == 0x0d)
    {
        pointer = 0;
    }

    if (pointer == SIZE_BUFFER - 1)
    {
        pointer = 0;
    }

    answer[pointer++] = symbol;
    answer[pointer] = '\0';
}


void Modem::GSM_PG::ToOutLow()
{
    gpio_init(GPIOE, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    GPIO_BC(GPIOE) = GPIO_PIN_2;
}


void Modem::GSM_PG::ToInPullDown()
{
    gpio_init(GPIOE, GPIO_MODE_IPD, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
}


bool Modem::GSM_PG::ReadInput()
{
    return gpio_input_bit_get(GPIOE, GPIO_PIN_2) == SET;
}


bool Modem::RegistrationIsOk()
{
    // AT+CREG?

    return false;
}
