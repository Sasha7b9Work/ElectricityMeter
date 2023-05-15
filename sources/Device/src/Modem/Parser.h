// 2023/4/30 18:04:29 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


namespace Parser
{
    // ���������� num-�� �����. ������������� �������� ':', ' ', ','. ���� ����� � ��������, �� ������� ����������
    // ��������� ���������� � 1
    String GetWord(const String &, int num);
    String GetWordInQuotes(const String &, int num);

    int NumberSymbols(pchar, char);
    int NumberSymbols(const String &, char);

    // num ���������� � �������
    int PositionSymbol(pchar, char symbol, int num);
    int PositionSymbol(const String &, char symbol, int num);

    // �������� � out �������, ������������� ����� ��������� start � end
    pchar GetWord(pchar, char out[32], int start, int end);
    String GetWord(const String &, int start, int end);
}
