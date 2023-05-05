// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include <cstring>


namespace SIM800
{
    uint Transmit(pchar);
    void TransmitUINT8(uint8);
    void TransmitRAW(pchar);
}


namespace MQTT
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_RESPONSE_CIPSEND,      // ���� ����������� ">"
            RUNNING
        };
    };

    static State::E state = State::IDLE;

    static TimeMeterMS meterPing;

    // ���� nned_ping == true, �� �������� ������� �����
    static bool need_ping = false;

    static FullMeasure measure;

    // ���� true - ���� ���������� ���������
    static bool need_measure = false;

    void Update(const String &);

//    static const char MQTT_type[15] = "MQIsdp";             // ��� ��������� �� �������!
    static const char *MQTT_type = "MQTT";
    static const char *MQTT_CID = "mqtt-pks3-r0rk8m";    // ���������� ��� ���������� � ���� MQTT
    static const char *MQTT_user = "";                   // api.cloudmqtt.com > Details > User
    static const char *MQTT_pass = "";                   // api.cloudmqtt.com > Details > Password

    // ����� �� ����������
    static void PublishPacket(const char *MQTT_topic, const char *MQTT_messege);

    // ����� �������� �� �����
    static void SubscribePacket(const char MQTT_topic[15]);

    static void Reset();

    void SendMeasure(const FullMeasure &);
}


void MQTT::Update(const String &answer)
{
    TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        SIM800::Transmit("AT+CIPSEND");
        meter.Reset();
        state = State::WAIT_RESPONSE_CIPSEND;
        break;

    case State::WAIT_RESPONSE_CIPSEND:
        if (answer == ">")
        {
            SIM800::TransmitUINT8(0x10);   // ������ ������ �� ��������� ����������
            SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + 14));
//            SIM800::TransmitUINT8(0x22);

            // ��� ���������
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_type));
            SIM800::TransmitRAW(MQTT_type);

            // ������ ��� �����
            SIM800::TransmitUINT8(0x05);    // ������ ���������
            SIM800::TransmitUINT8(0x00);    // connect flag
            SIM800::TransmitUINT8(0x00);    // \ keep alive 
            SIM800::TransmitUINT8(0x3c);    // /

            SIM800::TransmitUINT8(0x05);    // property lenth
            SIM800::TransmitUINT8(0x11);    // 
            SIM800::TransmitUINT8(0x00);    // \ 
            SIM800::TransmitUINT8(0x00);    // | sexxion expiry interval
            SIM800::TransmitUINT8(0x00);    // |
            SIM800::TransmitUINT8(0x00);    // /

            // MQTT  ������������� ����������
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_CID));
            SIM800::TransmitRAW(MQTT_CID);

            // MQTT �����
//            SIM800::TransmitUINT8(0x00);
//            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_user));
//            SIM800::TransmitRAW(MQTT_user);
//
//            // MQTT ������
//            SIM800::TransmitUINT8(0x00);
//            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_pass));
//            SIM800::TransmitRAW(MQTT_pass);

            //    // ����� ����������
            //    PublishPacket("C5/status", "����������");
            //
            //    // ����� �������� �� ����������� �������
            //    SubscribePacket("C5/comand");
            //
            //    // ����� �������� �� ����������� �������� �������
            //    SubscribePacket("C5/settimer");

                // ������ ���������� ������
            SIM800::TransmitUINT8(0x1A);

            state = State::RUNNING;

            meterPing.Reset();
        }
        break;

    case State::RUNNING:

        if (meterPing.ElapsedTime() > 20000)
        {
            meterPing.Reset();
            SIM800::Transmit("AT+CIPSEND");
            need_ping = true;
        }

        if (answer == ">")
        {
            if (need_measure)
            {
//                PublishPacket("base/state/voltage_b", "12345");
//                PublishPacket("base/state/voltage_c", "25235");
                need_measure = false;
            }
            else if(need_ping)
            {
                SIM800::TransmitUINT8(0xC0);
                SIM800::TransmitUINT8(0x00);
                SIM800::TransmitUINT8(0x1A);
                need_ping = false;
            }
        }

        break;
    }
}


void MQTT::SendMeasure(const FullMeasure &meas)
{
    measure = meas;

    need_measure = true;

    SIM800::Transmit("AT+CIPSEND");
}


void MQTT::Reset()
{

}


void  MQTT::PublishPacket(const char *MQTT_topic, const char *MQTT_messege)
{
    SIM800::TransmitUINT8(0x30);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic) + std::strlen(MQTT_messege) + 2));
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic)));
    SIM800::Transmit(MQTT_topic); // �����
    SIM800::Transmit(MQTT_messege);
}


void MQTT::SubscribePacket(const char MQTT_topic[15])
{
    // ����� ������
    SIM800::TransmitUINT8(0x82);
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic) + 5));

    // ������ ��� �����
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT8(0x01);
    SIM800::TransmitUINT8(0);

    // �����
    SIM800::TransmitUINT8((uint8)(std::strlen(MQTT_topic)));
    SIM800::Transmit(MQTT_topic);

    SIM800::TransmitUINT8(0);
}
