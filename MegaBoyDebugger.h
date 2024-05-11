//
// Created by sbeam on 04/12/2021.
//
#include <memory>
#include <filesystem>
#include "imgui/imgui.h"
#include "CPU/cpu.h"
#include "UI/RegisterWindow.h"
#include "UI/DisassemblyWindow.h"
#include "Gameboy.h"

#pragma once

class MegaBoyDebugger
{

private:
    std::thread gb_thread;

    /// Scroll to the lates entry in the disassembly
    bool scroll_to_bottom = false;

    bool is_running = false;
    uint16_t run_to = 0xffff;

    std::unique_ptr<Gameboy> gb;

    DisassemblyWindow disassemblyWindow;

public:
    MegaBoyDebugger();
    ~MegaBoyDebugger();

    void UpdateUI();

    void DrawMainMenu();

    void DrawDebuggingControls();

    void DrawCartridgeHeader();
    void DrawTimerRegisters();
    void DrawPPURegisters();

    void LoadRom(std::filesystem::path filename);

    void Step();

    void Run();

    void LoadBIOSRom();

    void SetKeyState(Joypad::Button button, bool pressed);

    // Callback function when "Open File" is pressed in the cross-platform ImGUI UI.
    // The native code should set a callback on this property and spawn a native file picker UI when called.
    std::function<void()> onOpenFile = nullptr;

    static constexpr uint16_t GB_SCREEN_WIDTH = 256;
    static constexpr uint16_t GB_SCREEN_HEIGHT = 256;
    /// RGB screen data
    uint8_t *screenData;
};
