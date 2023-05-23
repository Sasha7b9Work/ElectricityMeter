// 2023/04/09 14:55:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Measurer/Contactors.h"
#include "Hardware/HAL/HAL.h"
#include "Hardware/HAL/systick.h"
#include "Hardware/Timer.h"
#include "Utils/Math.h"
#include "Modem/Modem.h"
#include <gd32f30x.h>
#include <cstring>
#include <cstdlib>


    /*
    if (new_state == current[phase])
    {
        return;
    }

    current[phase] = new_state;

    Enable(2, phase);
    Enable(3, phase);                       // TRANSIT_EN_1
    Disable(1, phase);                      // TRANSIT_EN_2
    Disable(2, phase);                      // TRANSIT_EN_3
    TimeMeterMS().Wait(TIME_WAIT_BIG);      // TRANSIT_EN_4
    Disable(3, phase);                      // TRANSIT_EN_5

    if (new_state == TRANSIT)               // TRANSIT_EN_6
    {
//        Disable(4, phase);
        Disable(5, phase);                  // TRANSIT_DIS_1
        Disable(6, phase);                  // TRANSIT_DIS_2
        Disable(7, phase);                  // TRANSIT_DIS_3
        Disable(8, phase);                  // TRANSIT_DIS_4

        return;                             // TRANSIT_DIS_5
    }

    if (new_state == -4 || new_state == 4)
    {
//        Enable(4, phase);
        Enable(5, phase);   6                // STAGE_4_1
        Enable(6, phase);   7               // STAGE_4_2
        Enable(7, phase);   8               // STAGE_4_3
    }
    else if (new_state == -3 || new_state == 3)
    {
//        Disable(4, phase);
        Enable(5, phase);   6                // STAGE_3_1
        Enable(6, phase);   7               // STAGE_3_2
        Enable(7, phase);   8               // STAGE_3_3
    }
    else if (new_state == -2 || new_state == 2)
    {
//        Disable(4, phase);
        Disable(5, phase);     6             // STAGE_2_1
        Enable(6, phase);      7             // STAGE_2_2
        Enable(7, phase);      8             // STAGE_2_3
    }
    else if (new_state == -1 || new_state == 1)
    {
//        Disable(4, phase);
        Disable(5, phase);    6              // STAGE_1_1
        Disable(6, phase);    7             // STAGE_1_2
        Disable(7, phase);    8             // STAGE_1_3
    }

    new_state > 0 ? Disable(8, phase) : Enable(8, phase);       // POLARITY_LEVEL

    Enable(2, phase);                   // TRANSIT_EXIT_1
    Enable(3, phase);                   // TRANSIT_EXIT_2
    Enable(1, phase);                   // TRANSIT_EXIT_3
    Disable(2, phase);                  // TRANSIT_EXIT_4
    TimeMeterMS().Wait(TIME_WAIT_BIG);  // TRANSIT_EXIT_5
    Disable(3, phase)                   // TRANSIT_EXIT_6
    return;                             // TRANSIT_EXIT_7
    */


namespace Contactors
{
    static const uint TIME_WAIT_BIG = 5000;
    static const uint TIME_WAIT_SMALL = 100;

    struct State
    {
        static Phase::E phase;
        static PinOUT *pin;

        enum E
        {
            IDLE,
            TRANSIT_EN_1,
            TRANSIT_EN_2,
            TRANSIT_EN_3,
            TRANSIT_EN_4,
            TRANSIT_EN_5,
            TRANSIT_EN_6,
            TRANSIT_DIS_1,
            TRANSIT_DIS_2,
            TRANSIT_DIS_3,
            TRANSIT_DIS_4,
#ifdef FIVE_STEPS_VERSION
            STAGE_5_1,
            STAGE_5_2,
#endif
            STAGE_4_1,
            STAGE_4_2,
            STAGE_3_1,
            STAGE_3_2,
            STAGE_2_1,
            STAGE_2_2,
            STAGE_1_1,
            STAGE_1_2,
            POLARITY_LEVEL,
            TRANSIT_EXIT_1,
            TRANSIT_EXIT_2,
            TRANSIT_EXIT_3,
            TRANSIT_EXIT_4,
            TRANSIT_EXIT_5,
            TRANSIT_EXIT_6,
            END
        };

        static E current[3];
    };

    State::E State::current[3] = { State::IDLE, State::IDLE, State::IDLE };

    struct Contactor
    {
        PinOUT *pin;
        bool    enabled;
        void Init();
        void Enable();
        void Disable();
    };

    static Contactor _contactors[3][10] =
    {
        {{ &pinKMA1 }, { &pinKMA1 }, { &pinKMA2 }, { &pinKMA3 }, { &pinKMA4 }, { &pinKMA5 }, { &pinKMA6 }, { &pinKMA7 }, { &pinKMA8 }, { &pinKMA9 } },
        {{ &pinKMB1 }, { &pinKMB1 }, { &pinKMB2 }, { &pinKMB3 }, { &pinKMB4 }, { &pinKMB5 }, { &pinKMB6 }, { &pinKMB7 }, { &pinKMB8 }, { &pinKMB9 } },
        {{ &pinKMC1 }, { &pinKMC1 }, { &pinKMC2 }, { &pinKMC3 }, { &pinKMC4 }, { &pinKMC5 }, { &pinKMC6 }, { &pinKMC7 }, { &pinKMC8 }, { &pinKMC9 } },
    };

    // Состояние контакторов
    namespace Level
    {
#ifdef FOUR_STEPS_VERSION
        static const int LESS_180 = -4;
        static const int ABOVE_280 = 4;
        static const int MIN = LESS_180;
        static const int MAX = ABOVE_280;
#endif

#ifdef FIVE_STEPS_VERSION
        static const int LESS_170 = -5;
        static const int ABOVE_290 = 5;
        static const int MIN = LESS_170;
        static const int MAX = ABOVE_290;
#endif

        static const int TRANSIT = 0;

        static int current[3] = { 0, 0, 0 };
    }

    static void Enable(int contactor, Phase::E, State::E next, TimeMeterMS &);

    static void Disable(int contactor, Phase::E, State::E next, TimeMeterMS &);

    static void UpdatePhase(Phase::E, const PhaseMeasure &, bool good);

    // Возвращаемое значение true означает, что фаза находится в режиме перелючения. Измерения по ней производить нельзя
    bool IsBusy(Phase::E phase);

    // Устанавливает адрес на линиях MX
    static void SetAddressMX(uint);

    // Возвращает состояние реле, выбранного установленным ранее адресом по SetAddressMX()
    static bool StateRele();

    // Возвращает true, если реле по адресу address находится в состоянии переключения (нельзя замерять)
    static bool ReleIsBusy(uint address);

    // Сюда накапливаются состояния всех реле, чтобы потом одной строкой отослать неисправные
    static bool state_contactor[NUM_PINS_MX] =
    {
        false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false,
        false, false, false, false, false, false, false, false, false,
        false
    };
}


bool Contactors::IsBusy(Phase::E phase)
{
    return State::current[phase] != State::IDLE;
}


void Contactors::Init()
{
    for (int phase = 0; phase < 3; phase++)
    {
        for (int i = 1; i < 10; i++)
        {
            _contactors[phase][i].Init();
        }
    }

    pinMX0.Init();
    pinMX1.Init();
    pinMX2.Init();
    pinMX3.Init();
    pinMX4.Init();

    pinP1.Init(GPIO_MODE_IPU);
    pinP2.Init(GPIO_MODE_IPU);
}


void Contactors::Update(const FullMeasure &measure)
{
    for (int i = 0; i < 3; i++)
    {
        UpdatePhase((Phase::E)i, measure.measures[i], measure.is_good[i]);
    }
}


void Contactors::UpdatePhase(Phase::E phase, const PhaseMeasure &measure, bool is_good)
{
#define ENABLE_RELE(num, state)  { Enable(num, phase, state, meter[phase]); }
#define DISABLE_RELE(num, state) { Disable(num, phase, state, meter[phase]); }

#define WAIT_ENABLE_RELE(num, state)  if(meter[phase].IsWorked()) { ENABLE_RELE(num, state); }
#define WAIT_DISABLE_RELE(num, state) if(meter[phase].IsWorked()) { DISABLE_RELE(num, state); }

    static TimeMeterMS meter[3];

    switch (State::current[phase])
    {
    case State::IDLE:
        {
            if (!is_good)
            {
                break;
            }
            
            float inU = measure.voltage + (float)Level::current[phase] * 10.0f;
            int new_level = 0;
            
            if (inU < 160.5f)
            {
                new_level = 0;
            }
            else
            {
                float delta = 0.0f;
                int num_steps = 0;
                if (measure.voltage < 220.0f)
                {
                    delta = 220.0f - measure.voltage;

                    num_steps = -(int)(delta / 10.0f + 1.0f);
                }
                else if (measure.voltage > 240.0f)
                {
                    delta = measure.voltage - 240.0f;

                    num_steps = (int)(delta / 10.0f + 1.0f);
                }
                else
                {
                    break;
                }

                new_level = Math::Limitation(Level::current[phase] + num_steps, Level::MIN, Level::MAX);
            }
    
            if (new_level == Level::current[phase])
            {
                break;
            }
    
            Level::current[phase] = new_level;

            ENABLE_RELE(2, State::TRANSIT_EN_1);
        }

        break;

    case State::TRANSIT_EN_1:   WAIT_ENABLE_RELE(3, State::TRANSIT_EN_2);       break;
    case State::TRANSIT_EN_2:   WAIT_DISABLE_RELE(1, State::TRANSIT_EN_3);      break;
    case State::TRANSIT_EN_3:   WAIT_DISABLE_RELE(2, State::TRANSIT_EN_4);      break;
    case State::TRANSIT_EN_4:       
        if (meter[phase].IsWorked()) { meter[phase].SetResponseTime(TIME_WAIT_BIG); State::current[phase] = State::TRANSIT_EN_5; } break;

    case State::TRANSIT_EN_5:
        if (meter[phase].IsWorked()) { DISABLE_RELE(3, State::TRANSIT_EN_6); } break;

    case State::TRANSIT_EN_6:
        if (meter[phase].IsWorked())
        {
            if (Level::current[phase] == Level::TRANSIT)                        { DISABLE_RELE(5, State::TRANSIT_DIS_1); }
            else if (Level::current[phase] == -4 || Level::current[phase] == 4) { ENABLE_RELE(6, State::STAGE_4_1); }
            else if (Level::current[phase] == -3 || Level::current[phase] == 3) { ENABLE_RELE(6, State::STAGE_3_1); }
            else if (Level::current[phase] == -2 || Level::current[phase] == 2) { DISABLE_RELE(6, State::STAGE_2_1); }
            else if (Level::current[phase] == -1 || Level::current[phase] == 1) { DISABLE_RELE(6, State::STAGE_1_1); }
            else
            {
                State::current[phase] = State::POLARITY_LEVEL;
            }
        }
        break;

    case State::TRANSIT_DIS_1:  WAIT_DISABLE_RELE(6, State::TRANSIT_DIS_2);
    case State::TRANSIT_DIS_2:  WAIT_DISABLE_RELE(7, State::TRANSIT_DIS_3);
    case State::TRANSIT_DIS_3:  WAIT_DISABLE_RELE(8, State::TRANSIT_DIS_4);
    case State::TRANSIT_DIS_4:
        if (meter[phase].IsWorked())  State::current[phase] = State::IDLE;  break;

    case State::STAGE_4_1:      WAIT_ENABLE_RELE(7, State::STAGE_4_2);
    case State::STAGE_4_2:      WAIT_ENABLE_RELE(8, State::POLARITY_LEVEL);

    case State::STAGE_3_1:      WAIT_ENABLE_RELE(7, State::STAGE_3_2);
    case State::STAGE_3_2:      WAIT_ENABLE_RELE(8, State::POLARITY_LEVEL);

    case State::STAGE_2_1:      WAIT_ENABLE_RELE(7, State::STAGE_2_2);
    case State::STAGE_2_2:      WAIT_ENABLE_RELE(8, State::POLARITY_LEVEL);

    case State::STAGE_1_1:      WAIT_DISABLE_RELE(7, State::STAGE_1_2);
    case State::STAGE_1_2:      WAIT_DISABLE_RELE(8, State::POLARITY_LEVEL);

    case State::POLARITY_LEVEL:
        if (meter[phase].IsWorked())
        {
            if (Level::current[phase] > 0) {  ENABLE_RELE(9, State::TRANSIT_EXIT_1) } else { DISABLE_RELE(9, State::TRANSIT_EXIT_1); }
        }
        break;
    case State::TRANSIT_EXIT_1:     WAIT_ENABLE_RELE(2, State::TRANSIT_EXIT_2);
    case State::TRANSIT_EXIT_2:     WAIT_ENABLE_RELE(3, State::TRANSIT_EXIT_3);
    case State::TRANSIT_EXIT_3:     WAIT_ENABLE_RELE(1, State::TRANSIT_EXIT_4);
    case State::TRANSIT_EXIT_4:     WAIT_DISABLE_RELE(2, State::TRANSIT_EXIT_5);
    case State::TRANSIT_EXIT_5:
        if (meter[phase].IsWorked())
        {
            meter[phase].SetResponseTime(TIME_WAIT_BIG);
            State::current[phase] = State::TRANSIT_EXIT_6;
        }
        break;
    case State::TRANSIT_EXIT_6:     WAIT_DISABLE_RELE(3, State::END);

    case State::END:
        if (meter[phase].IsWorked())
        {
            State::current[phase] = State::IDLE;
        }
        break;
    }

    Modem::Send::LevelContactors(Level::current);
}


void Contactors::Enable(int num, Phase::E phase, State::E next, TimeMeterMS &meter)
{
    _contactors[phase][num].Enable();

    State::current[phase] = next;

    meter.SetResponseTime(TIME_WAIT_SMALL);
}


void Contactors::Disable(int num, Phase::E phase, State::E next, TimeMeterMS &meter)
{
    _contactors[phase][num].Disable();

    State::current[phase] = next;

    meter.SetResponseTime(TIME_WAIT_SMALL);
}


void Contactors::Contactor::Init()
{
    pin->Init();

    enabled = false;

    pin->Reset();
}


void Contactors::Contactor::Enable()
{
    if (!enabled)
    {
        enabled = true;

        pin->Set();
    }
}


void Contactors::Contactor::Disable()
{
    if (enabled)
    {
        enabled = false;

        pin->Reset();
    }
}


void Contactors::VerifyCondition()
{
    static TimeMeterMS meter;

    if (meter.ElapsedTime() < 1)
    {
        return;
    }

    meter.Reset();

    static bool first = true;

    static uint address = 0;

    if (first)
    {
        SetAddressMX(address);
        first = false;
    }
    else
    {
        if (!ReleIsBusy(address))
        {
            bool state = StateRele();

            state_contactor[address] = state;
        }

        if (address == 27)                          // Был выставлен адрес P2 = 31
        {
            state_contactor[address] = !pinP2.IsHi();
        }

        address = Math::CircularIncrease(address, 0U, (uint)NUM_PINS_MX);

        if (address == 27)                          // 28-й элемент массива - адрес пина P2 для контроля напряжения 100В
        {
            address = 31;
        };

        SetAddressMX(address);

        if (address == 31)
        {
            address = 27;
        }

        if (address == 0)   // Опросили все реле, будем посылать результат
        {
            Modem::Send::StateContactors(state_contactor);
        }
    }
}


void Contactors::SetAddressMX(uint address)
{
    pinMX0.SetState((address & 1) != 0);
    pinMX1.SetState((address & 2) != 0);
    pinMX2.SetState((address & 4) != 0);
    pinMX3.SetState((address & 8) != 0);
    pinMX4.SetState((address & 16) == 0);
}


bool Contactors::StateRele()
{
    bool p1 = pinP1.IsHi();
    bool p2 = pinP2.IsHi();

    return ((p1 && !p2) || (!p1 && p2));
}


bool Contactors::ReleIsBusy(uint address)
{
    if (address < 9)
    {
        return Contactors::IsBusy(Phase::A);
    }
    else if (address < 18)
    {
        return Contactors::IsBusy(Phase::B);
    }

    return Contactors::IsBusy(Phase::C);
}