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

    std::thread gb_thread;
    std::unique_ptr<Gameboy> gb;

    /// Scroll to the lates entry in the disassembly
    bool scroll_to_bottom = false;

    bool is_running = false;

public:

    MegaBoyDebugger();
    ~MegaBoyDebugger();

    void UpdateUI();

    void LoadTestRom();

    void Step();

    static constexpr uint8_t GB_SCREEN_WIDTH = 160;
    static constexpr uint8_t GB_SCREEN_HEIGHT = 144;
    /// RGB screen data
    uint8_t screenData [GB_SCREEN_HEIGHT*GB_SCREEN_WIDTH*3] = {};

    void UpdateLCDBuffer();

    void Run();
};
