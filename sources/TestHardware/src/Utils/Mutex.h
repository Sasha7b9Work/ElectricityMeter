// 2022/08/29 15:56:08 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once

// ����� ��� ������ ������ �� ����������
class Mutex
{
public:

    Mutex() : locked(false) {}

    // ���������� � �������� ����� ����� �������� � �������, �������������� � �����������
    void Lock()
    {
        locked = true;
    };

    // ���������� � �������� ����� ����� �������� � �������, �������������� � ������������
    void Unlock()
    {
        locked = false;
    };

    // ����� ���������� ����� ���������, ����� �� �������
    bool Locked() const
    {
        return locked;
    };

private:

    bool locked;
};
