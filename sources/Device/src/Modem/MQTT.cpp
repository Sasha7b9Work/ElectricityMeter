// 2023/5/2 13:43:57 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Modem/Modem.h"
#include <cstring>


namespace Modem
{
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
    Modem::Transmit("AT+CIPSEND");
    Modem::TransmitUINT8(0x10);                                                              // ������ ������ �� ��������� ����������
    Modem::TransmitUINT(std::strlen(MQTT_type) + std::strlen(MQTT_CID) + std::strlen(MQTT_user) + std::strlen(MQTT_pass) + 12);

    // ��� ���������
    Modem::TransmitUINT8(0);
    Modem::TransmitUINT(std::strlen(MQTT_type));
    Modem::Transmit(MQTT_type);

    // ������ ��� �����
    Modem::TransmitUINT8(0x03);
    Modem::TransmitUINT8(0xC2);
    Modem::TransmitUINT8(0);
    Modem::TransmitUINT8(0x3C);

    // MQTT  ������������� ����������
    Modem::TransmitUINT8(0);
    Modem::TransmitUINT(std::strlen(MQTT_CID));
    Modem::Transmit(MQTT_CID);

    // MQTT �����
    Modem::TransmitUINT8(0);
    Modem::TransmitUINT(std::strlen(MQTT_user));
    Modem::Transmit(MQTT_user);

    // MQTT ������
    Modem::TransmitUINT8(0);
    Modem::TransmitUINT(std::strlen(MQTT_pass));
    Modem::Transmit(MQTT_pass);

    // ����� ����������
    PublishPacket("C5/status", "����������");

    // ����� �������� �� ����������� �������
    SubscribePacket("C5/comand");

    // ����� �������� �� ����������� �������� �������
    SubscribePacket("C5/settimer");

    // ������ ���������� ������
    Modem::TransmitUINT8(0x1A);
}


void  MQTT::PublishPacket(const char MQTT_topic[15], const char MQTT_messege[15])
{
    Modem::TransmitUINT8(0x30);
    Modem::TransmitUINT(std::strlen(MQTT_topic) + std::strlen(MQTT_messege) + 2);
    Modem::TransmitUINT8(0);
    Modem::TransmitUINT(std::strlen(MQTT_topic));
    Modem::Transmit(MQTT_topic); // �����
    Modem::Transmit(MQTT_messege);
}


void MQTT::SubscribePacket(const char MQTT_topic[15])
{
    // ����� ������
    Modem::TransmitUINT8(0x82);
    Modem::TransmitUINT(std::strlen(MQTT_topic) + 5);

    // ������ ��� �����
    Modem::TransmitUINT8(0);
    Modem::TransmitUINT8(0x01);
    Modem::TransmitUINT8(0);

    // �����
    Modem::TransmitUINT(std::strlen(MQTT_topic));
    Modem::Transmit(MQTT_topic);

    Modem::TransmitUINT8(0);
}
