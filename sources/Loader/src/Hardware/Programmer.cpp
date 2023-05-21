﻿// 2023/05/21 10:22:02 (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by
#include "defines.h"
#include "Hardware/Programmer.h"
#include "Modem/ReaderFTP.h"
#include "HAL/HAL.h"
#include <cstring>


namespace Programmer
{
    static int written_bytes = 0;
    static uint address = 0;            // Текущий адрес записи

    static uint Hash(uint hash, char byte)
    {
        return (uint8)byte + (hash << 6) + (hash << 16) - hash;
    }

    static int num_bytes = 0;
    static char bytes[ReaderFTP::SIZE_DATA_BUFFER + 4];

    static void Write(void *data, int size)
    {
        HAL_ROM::WriteData(address, (uint8 *)data, size);
        address += size;
        written_bytes += size;
    }
}


void Programmer::Prepare(uint start_address)
{
    const int num_pages = HAL_ROM::MAX_SIZE_STORAGE / HAL_ROM::SIZE_PAGE;

    const int page = (int)((start_address - HAL_ROM::ADDR_FLASH) / HAL_ROM::SIZE_PAGE);

    for (int i = 0; i < num_pages; i++)
    {
        if (!HAL_ROM::PageIsEmpty(page + i))
        {
            HAL_ROM::ErasePage(page + i);
        }
    }

    address = start_address;
    written_bytes = 0;
    num_bytes = 0;
}


void Programmer::WriteBytes(char data[ReaderFTP::SIZE_DATA_BUFFER], int size)
{
    if (size == 0)
    {
        return;
    }

    std::memcpy(&bytes[num_bytes], data, (uint)size);

    num_bytes += size;

    if ((num_bytes % 4) == 0)
    {
        Write(bytes, num_bytes);
        num_bytes = 0;
    }
    else
    {
        if (num_bytes > 3)
        {
            int need_bytes = num_bytes;         // Столько байт нужно записать. Остальные оствим на потом
            while (need_bytes % 4)
            {
                need_bytes--;
            }

            int delta = num_bytes - need_bytes;     // Столько байт оствим на потом

            Write(bytes, need_bytes);

            std::memmove(bytes, &bytes[need_bytes], (uint)delta);

            num_bytes = delta;
        }
    }
}


void Programmer::CloseSession()
{
    if (num_bytes)
    {
        while (num_bytes % 4)
        {
            bytes[num_bytes++] = (uint8)0xFF;
        }

        Write(bytes, num_bytes);
    }
}


int Programmer::WrittenBytes()
{
    return written_bytes;
}


uint Programmer::CalculateCRC(uint addr, int size)
{
    uint crc = 0;

    uint8 *data = (uint8 *)addr;

    for (int i = 0; i < size; i++)
    {
        crc = Hash(crc, (char)*data++);
    }

    return crc;
}