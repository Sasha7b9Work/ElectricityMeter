// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include <cstring>


namespace SIM800
{
    uint Transmit(pchar, uint timeout = TIME_WAIT_ANSWER);
    void TransmitUINT8(uint8);
    void TransmitUINT(uint);
}


namespace MQTT
{
    void Connect();

    static const char MQTT_type[15] = "MQIsdp";         // ��� ��������� �� �������!
    static const char MQTT_CID[15] = "CITROEN";         // ���������� ��� ���������� � ���� MQTT
    static const char MQTT_user[10] = "drive2ru";       // api.cloudmqtt.com > Details > User
    static const char MQTT_pass[15] = "martinhool221";  // api.cloudmqtt.com > Details > Password

    // ����� �� ����������
    static void PublishPacket(const char MQTT_topic[15], const char MQTT_messege[15]);

    // ����� �������� �� �����
    static void SubscribePacket(const char MQTT_topic[15]);
}


void MQTT::Connect()
{
    SIM800::Transmit("AT+CIPSEND");
    SIM800::TransmitUINT8(0x10);                                                              // ������ ������ �� ��������� ����������
    SIM800::TransmitUINT(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + std::strlen(MQTT_user) + std::strlen(MQTT_pass) + 12);

    // ��� ���������
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT(std::strlen(MQTT_type));
    SIM800::Transmit(MQTT_type);

    // ������ ��� �����
    SIM800::TransmitUINT8(0x03);
    SIM800::TransmitUINT8(0xC2);
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT8(0x3C);

    // MQTT  ������������� ����������
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT(std::strlen(MQTT_CID));
    SIM800::Transmit(MQTT_CID);

    // MQTT �����
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT(std::strlen(MQTT_user));
    SIM800::Transmit(MQTT_user);

    // MQTT ������
    SIM800::TransmitUINT8(0);
    SIM800::TransmitUINT(std::strlen(MQTT_pass));
    SIM800::Transmit(MQTT_pass);

    // ����� ����������
    PublishPacket("C5/status", "����������");

    // ����� �������� �� ����������� �������
    SubscribePacket("C5/comand");

    // ����� �������� �� ����������� �������� �������
    SubscribePacket("C5/settimer");

    // ������ ���������� ������
    SIM800::TransmitUINT8(0x1A);
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
