// 2023/03/16 16:31:47 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#pragma once
#include "Blocks/Calculator/Calculator.h"


namespace SIM800C
{
    void Init();

    bool IsConnected();

    void SendData(OutputData &);
}