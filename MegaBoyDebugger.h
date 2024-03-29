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

    /// Scroll to the lates entry in the disassembly
    bool scroll_to_bottom = false;

    bool is_running = false;
    uint16_t run_to = 0xffff;

    std::unique_ptr<Gameboy> gb;

public:

    MegaBoyDebugger();
    ~MegaBoyDebugger();

    void UpdateUI();

    void DrawDebuggingControls();

    void DrawPPURegisters();

    void LoadTestRom();

    void Step();

    static constexpr uint16_t GB_SCREEN_WIDTH = 256;
    static constexpr uint16_t GB_SCREEN_HEIGHT = 256;
    /// RGB screen data
    uint8_t *screenData;



    void Run();

    void LoadBIOSRom();

    void SetKeyState(Joypad::Button button, bool pressed);
};
