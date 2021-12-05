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

public:

    MegaBoyDebugger();

    void UpdateUI();

    void LoadTestRom();
};
