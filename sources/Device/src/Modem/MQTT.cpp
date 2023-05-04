// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include "Hardware/Timer.h"
#include <cstring>


namespace SIM800
{
    uint Transmit(pchar);
    void TransmitUINT8(uint8);
    void TransmitUINT(uint);
}


namespace MQTT
{
    struct State
    {
        enum E
        {
            IDLE,
            WAIT_RESPONSE_CIPSEND   // ���� ����������� ">"
        };
    };

    static State::E state = State::IDLE;

    void Update(const String &);

//    static const char MQTT_type[15] = "MQIsdp";             // ��� ��������� �� �������!
    static const char MQTT_type[15] = "MQTT";
    static const char MQTT_CID[17] = "mqtt-pks3-r0rk8m";    // ���������� ��� ���������� � ���� MQTT
    static const char MQTT_user[10] = "";                   // api.cloudmqtt.com > Details > User
    static const char MQTT_pass[15] = "";                   // api.cloudmqtt.com > Details > Password

    // ����� �� ����������
    static void PublishPacket(const char MQTT_topic[15], const char MQTT_messege[15]);

    // ����� �������� �� �����
    static void SubscribePacket(const char MQTT_topic[15]);

    static void Reset();
}


void MQTT::Update(const String &answer)
{
    const uint DEFAULT_TIME = 10000;

    TimeMeterMS meter;

    switch (state)
    {
    case State::IDLE:
        SIM800::Transmit("AT+CIPSEND");
        meter.Reset();
        state = State::WAIT_RESPONSE_CIPSEND;
        break;

    case State::WAIT_RESPONSE_CIPSEND:
        if (meter.ElapsedTime() > DEFAULT_TIME)
        {
            Reset();
        }
        else if (answer == ">")
        {
            SIM800::TransmitUINT8(0x10);                                                              // ������ ������ �� ��������� ����������
            SIM800::TransmitUINT(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + std::strlen(MQTT_user) + std::strlen(MQTT_pass) + 12);

            // ��� ���������
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_type));
            SIM800::Transmit(MQTT_type);

            // ������ ��� �����
            SIM800::TransmitUINT8(0x03);
            SIM800::TransmitUINT8(0xC2);
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8(0x3c);

            // MQTT  ������������� ����������
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_CID));
            SIM800::Transmit(MQTT_CID);

            // MQTT �����
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_user));
//            SIM800::Transmit(MQTT_user);

            // MQTT ������
            SIM800::TransmitUINT8(0x00);
            SIM800::TransmitUINT8((uint8)std::strlen(MQTT_pass));
//            SIM800::Transmit(MQTT_pass);

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
        }
        break;
    }
}


void MQTT::Reset()
{

}


void  MQTT::PublishPacket(const char MQTT_topic[15], const char MQTT_messege[15])
{
    SIM800::TransmitUINT8(0x30);
    SIM800::TransmitUINT(std::strlen(MQTT_topic) + std::strlen(MQTT_messege) + 2);
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT(std::strlen(MQTT_topic));
    SIM800::Transmit(MQTT_topic); // �����
    SIM800::Transmit(MQTT_messege);
}


void MQTT::SubscribePacket(const char MQTT_topic[15])
{
    // ����� ������
    SIM800::TransmitUINT8(0x82);
    SIM800::TransmitUINT(std::strlen(MQTT_topic) + 5);

    // ������ ��� �����
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT8(0x01);
    SIM800::TransmitUINT8(0);

    // �����
    SIM800::TransmitUINT(std::strlen(MQTT_topic));
    SIM800::Transmit(MQTT_topic);

    SIM800::TransmitUINT8(0);
}
