// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Modem
{
    void Init();

    void Update();

    // ���� ����������
    bool ExistUpdate();

    void Transmit(pchar);

    // ���������� ��������� ���������� �����. ����� ������� ������ Transmit() ����� ���������
    pchar LastAnswer();

    void CallbackOnReceive(char);
}
