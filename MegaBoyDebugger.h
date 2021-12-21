//
// Created by sbeam on 04/12/2021.
//
#include <memory>
#include "imgui/imgui.h"
#include "CPU/cpu.h"
#include "UI/RegisterWindow.h"
#include "Gameboy.h"

#pragma once

class MegaBoyDebugger {

private:

    std::unique_ptr<Gameboy> gb;

    /// Scroll to the lates entry in the disassembly
    bool scroll_to_bottom = false;

public:

    MegaBoyDebugger();

    void UpdateUI();

    void LoadTestRom();

    void Step();
};
