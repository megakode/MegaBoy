//
// Created by sbeam on 04/12/2021.
//

#include "imgui/imgui.h"
#include "cpu.h"
#include "UI/RegisterWindow.h"

#pragma once

class MegaBoyDebugger {

private:

    CPU cpu;
    RegisterWindow registerWindow;

    /// Scroll to the lates entry in the disassembly
    bool scroll_to_bottom = false;

public:

    MegaBoyDebugger();

    void UpdateUI();

    void LoadTestRom();

    void Step();
};
