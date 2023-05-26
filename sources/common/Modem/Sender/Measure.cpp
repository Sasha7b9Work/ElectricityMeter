// 2023/05/26 11:43:55 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Sender/Measure.h"
#include "Hardware/Timer.h"
#include <cstdio>
#include <cstring>


namespace Sender
{
    namespace Measure
    {
        static FullMeasure value;
        static bool need = false;

        void Send(const FullMeasure &meas)
        {
            if (!MQTT::InStateRunning())
            {
                return;
            }

            static TimeMeterMS meter;

            static bool first = true;

            if (meter.ElapsedTime() < 60000 && !first)
            {
                return;
            }

            first = false;

            meter.Reset();

            value = meas;

            need = true;

            MQTT::Send::SendRequest();
        }

        static void SendMeasure(pchar name, float voltage)
        {
            char buffer[32];
            std::sprintf(buffer, "%d", (int)(voltage + 0.5f));
            for (uint i = 0; i < std::strlen(buffer); i++)
            {
                if (buffer[i] == ',')
                {
                    buffer[i] = '.';
                }
            }
            MQTT::Packet::Publish(name, buffer);
        }

        void OnEventSend()
        {
            if (need)
            {
                if (value.is_good[0])
                {
                    SendMeasure("/voltage/a", value.measures[0].voltage);
                    SendMeasure("/current/a", value.measures[0].current * 1000.0f);
                }

                if (value.is_good[1])
                {
                    SendMeasure("/voltage/b", value.measures[1].voltage);
                    SendMeasure("/current/b", value.measures[1].current * 1000.0f);
                }

                if (value.is_good[2])
                {
                    SendMeasure("/voltage/c", value.measures[2].voltage);
                    SendMeasure("/current/c", value.measures[2].current * 1000.0f);
                }

                need = false;
            }
        }
    }
}