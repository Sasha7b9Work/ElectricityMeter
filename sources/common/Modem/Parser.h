// 2023/4/30 18:04:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once


namespace Parser
{
    // ���������� num-�� �����. ������������� �������� ':', ' ', ','. ���� ����� � ��������, �� ������� ����������
    // ��������� ���������� � 1
    pchar GetWord(pchar, int num);
    pchar GetWordInQuotes(pchar, int num);

    int NumberSymbols(pchar, char);

    // num ���������� � �������
    int PositionSymbol(pchar, char symbol, int num);
    int PositionSymbol(pchar, char symbol, int num);

    // �������� � out �������, ������������� ����� ��������� start � end
    pchar GetWord(pchar, int start, int end);
}
