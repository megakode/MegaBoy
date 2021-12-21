#pragma once
#include "../HostMemory.h"

enum class LCDCBitmask {
    LCD_enabled = 1 << 7
};

class LCD
{
    private:

    HostMemory& mem;

    public:

    LCD( HostMemory& mem ) : mem(mem)
    {

    };

    void Step()
    {

    }

};