// 2023/4/30 18:04:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Parser
{
    int NumberSymbols(pchar, char);

    // num ���������� � �������
    int PositionSymbol(pchar, char symbol, int num);

    // �������� � out �������, ������������� ����� ��������� start � end
    pchar GetWord(pchar, char out[32], int start, int end);
}
