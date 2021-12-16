//
// Created by sbeam on 04/12/2021.
//

#include "imgui/imgui.h"
#include "CPU/cpu.h"
#include "UI/RegisterWindow.h"
#include "Gameboy.h"

#pragma once

class MegaBoyDebugger {

private:

    std::unique_ptr<Gameboy> gb = std::make_unique<Gameboy>();
    RegisterWindow registerWindow;

    /// Scroll to the lates entry in the disassembly
    bool scroll_to_bottom = false;

public:

    MegaBoyDebugger();

    void UpdateUI();

    void LoadTestRom();

    void Step();
};
