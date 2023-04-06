// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Measurer/Calculator.h"


namespace Modem
{
    void Init();

    // ���� ����������
    bool ExistUpdate();

    void Transmit(pchar);

    // ���������� ��������� ���������� �����. ����� ������� ������ Transmit() ����� ���������
    pchar LastAnswer();

    void CallbackOnReceive(char);
}
