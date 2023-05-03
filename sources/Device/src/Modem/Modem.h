﻿// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Utils/String.h"


namespace Modem
{
    static const int MAX_LENGTH_ANSWERR = 128;
    static const uint TIME_WAIT_ANSWER_DEFAULT = 1500;

    void Init();

    void Update();

    // Возращает время до получения ответа
    uint Transmit(pchar, uint timeout = TIME_WAIT_ANSWER_DEFAULT);

    // Есть обновление
    bool ExistUpdate();

    bool LastAnswer(char [MAX_LENGTH_ANSWERR]);

    String LastAnswer();

    void CallbackOnReceive(char);
}
