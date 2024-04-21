//
// Created by sbeam on 1/18/22.
//

#ifndef MEGABOY_DISASSEMBLYWINDOW_H
#define MEGABOY_DISASSEMBLYWINDOW_H

#include "../CPU/cpu.h"
#include "../disassembler/disassembler.h"

class DisassemblyWindow
{

public:
    void UpdateUI(CPU &cpu);

private:
    Disassembler disassembler;
};

#endif // MEGABOY_DISASSEMBLYWINDOW_H
