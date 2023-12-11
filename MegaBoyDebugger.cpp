//
// Created by sbeam on 04/12/2021.
//
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include "MegaBoyDebugger.h"
#include "imgui.h"
#include "UI/UIConfig.h"
#include "UI/DisassemblyWindow.h"
#include "cartridge/Cartridge.h"

MegaBoyDebugger::MegaBoyDebugger()
{

    gb = std::make_unique<Gameboy>();
    screenData = reinterpret_cast<uint8_t *>(&gb->lcd.rgbBuffer);
    gb->cpu.reset();
}

MegaBoyDebugger::~MegaBoyDebugger()
{
    is_running = false;
    if (gb_thread.joinable())
    {
        gb_thread.join();
    }
}

void MegaBoyDebugger::LoadBIOSRom()
{
    std::filesystem::path filename = "../DMG_ROM.bin";
    auto path = std::filesystem::absolute(filename);
    auto size = std::filesystem::file_size(path);

    if (exists(path))
    {
        std::cout << "file exists! :)";
    }
    else
    {
        std::cout << "file DOES NOT exist!";
    }
    std::ifstream z80file(path, std::ios::in | std::ios::binary);
    if (!z80file)
    {
        std::cout << "could not read " << path;
    }
    else
    {
        std::cout << "Reading " << path;
        z80file.read((char *)&gb->cpu.mem[0], size);
        z80file.close();
    }

    gb->cpu.regs.PC = 0;
}

void MegaBoyDebugger::LoadTestRom()
{

    // std::filesystem::path filename = "../tests/game_roms/Dr. Mario (World).gb";
    // std::filesystem::path filename = "../tests/game_roms/Pac-Man (USA).gb";

    // std::filesystem::path filename = "../tests/game_roms/Asteroids (USA, Europe).gb";
    // std::filesystem::path filename = "../tests/game_roms/Alleyway (World).gb";
    // std::filesystem::path filename = "../tests/game_roms/Bubble Ghost (USA, Europe).gb";
    // std::filesystem::path filename = "../tests/game_roms/Motocross Maniacs (USA).gb"; // Invalid opcode - investigate this!
    // std::filesystem::path filename = "../tests/game_roms/Space Invaders (Japan).gb";
    // std::filesystem::path filename = "../tests/game_roms/Pipe Dream (USA).gb";
    // std::filesystem::path filename = "../tests/game_roms/Heiankyo Alien (USA).gb";
    // std::filesystem::path filename = "../tests/game_roms/Missile Command (USA, Europe).gb";

    // std::filesystem::path filename = "../tests/game_roms/BOBBLE.GB";
    // std::filesystem::path filename = "../tests/game_roms/Adventures of Lolo (Europe).gb";
    // std::filesystem::path filename = "../tests/game_roms/After Burst (Japan).gb";
    // std::filesystem::path filename = "../tests/game_roms/Alien 3 (USA, Europe).gb";
    // std::filesystem::path filename = "../tests/game_roms/Arcade Classic No. 1 - Asteroids & Missile Command (USA, Europe).gb";
    // std::filesystem::path filename = "../tests/game_roms/Bust-A-Move 2 - Arcade Edition (USA, Europe).gb";

    //std::filesystem::path filename = "../tests/test_roms/cpu_instrs.gb";
    //std::filesystem::path filename = "../tests/test_roms/01-special.gb"; // PASSED
    //std::filesystem::path filename = "../tests/test_roms/02-interrupts.gb"; // PASSED
    //std::filesystem::path filename = "../tests/test_roms/03-op sp,hl.gb"; // PASSED
    //std::filesystem::path filename = "../tests/test_roms/04-op r,imm.gb"; // PASSED
    //std::filesystem::path filename = "../tests/test_roms/05-op rp.gb"; // PASSED
    //std::filesystem::path filename = "../tests/test_roms/06-ld r,r.gb"; PASSED
    //std::filesystem::path filename = "../tests/test_roms/07-jr,jp,call,ret,rst.gb"; PASSED
    //std::filesystem::path filename = "../tests/test_roms/08-misc instrs.gb"; // PASSED
    //std::filesystem::path filename = "../tests/test_roms/09-op r,r.gb"; // PASSED
    //std::filesystem::path filename = "../tests/test_roms/10-bit ops.gb"; // PASSED
    //std::filesystem::path filename = "../tests/test_roms/11-op a,(hl).gb"; // PASSED
    //std::filesystem::path filename = "../tests/test_roms/halt_bug.gb"; // PASSED
    // std::filesystem::path filename = "../tests/test_roms/dmg-acid2.gb"; //
    
    // Mooneyes
    // https://github.com/Gekkio/mooneye-test-suite/tree/8d742b9d55055f6878a2f3017e0ccf2234cd692c/acceptance
    
    // std::filesystem::path filename = "../tests/test_roms/mts/acceptance/bits/mem_oam.gb"; // OK
    // std::filesystem::path filename = "../tests/test_roms/mts/acceptance/bits/reg_f.gb"; // OK
    
    // std::filesystem::path filename = "../tests/test_roms/mts/acceptance/instr/daa.gb"; // OK
    
    // std::filesystem::path filename = "../tests/test_roms/mts/acceptance/interrupts/ie_push.gb"; // failed
    
    // std::filesystem::path filename = "../tests/test_roms/mts/acceptance/oam_dma/basic.gb"; // OK
    // std::filesystem::path filename = "../tests/test_roms/mts/acceptance/oam_dma/reg_read.gb"; // OK
    // std::filesystem::path filename = "../tests/test_roms/mts/acceptance/oam_dma/sources-GS.gb"; // OK
    std::filesystem::path filename = "../tests/test_roms/mts/acceptance/ppu/stat_irq_blocking.gb";

    // std::filesystem::path filename = "../tests/game-boy-test-roms-v5.1/mooneye-test-suite/acceptance/oam_dma/basic.gb";
    // std::filesystem::path filename = "../tests/game-boy-test-roms-v5.1/mooneye-test-suite/acceptance/oam_dma/reg_read.gb";
    //std::filesystem::path filename = "../tests/game-boy-test-roms-v5.1/mooneye-test-suite/acceptance/oam_dma/sources-GS.gb";

    auto path = std::filesystem::absolute(filename);

    std::cout << "path: " << path << std::endl;
    if (exists(path))
    {
        std::cout << "file exists! :)";
    }
    else
    {
        std::cout << "file DOES NOT exist!";
    }
    std::ifstream z80file(path, std::ios::in | std::ios::binary);
    if (!z80file)
    {
        std::cout << "could not read " << path;
    }
    else
    {
        auto size = std::filesystem::file_size(path);
        std::cout << "Reading " << path << "size=" << size << std::endl;
        auto *buffer = new uint8_t[size];
        z80file.read((char *)buffer, size);

        // TODO:
        //  - Create instance of Cartridge on HostMemory.
        //  - load buffer into Cartridge
        gb->cartridge.load(buffer, size);
        // memcpy(&gb->mem[0],buffer, size);
        delete[] buffer;
        z80file.close();
    }
}

void MegaBoyDebugger::UpdateUI()
{

    //if (!is_running)
    {
        DisassemblyWindow::UpdateUI(gb->cpu);

        if(ImGui::Begin("Debugger"))
        {

            // ImGui::Begin("Debugging controls");

            DrawDebuggingControls();
    
            // ImGui::End();

            if (ImGui::CollapsingHeader("CPU",ImGuiTreeNodeFlags_DefaultOpen))
            {
                RegisterWindow::UpdateUI(gb->cpu);
            }

            if (ImGui::CollapsingHeader("PPU"))
            {
                DrawPPURegisters();
            }

        }
            ImGui::End();
    }

    if (is_running)
    {
        Run();
    }
}

void MegaBoyDebugger::DrawPPURegisters()
{
    ImGui::Text("FF42 - SCY:");
    ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%04x", gb->mem[0xFF42]);
    ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)", gb->mem[0xFF42]);

    ImGui::Text("FF43 - SCX:");
    ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%04x", gb->mem[0xFF43]);
    ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)", gb->mem[0xFF43]);

    ImGui::Text("FF44 - LY :");
    ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%04x", gb->mem[0xff44]);
    ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)", gb->mem[0xff44]);

    ImGui::Text("FF45 - LYC:");
    ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%04x", gb->mem[0xff45]);
    ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)", gb->mem[0xff45]);
}

void MegaBoyDebugger::DrawDebuggingControls()
{
    if (ImGui::Button("Step"))
    {
        is_running = false;
        Step();
    }

    ImGui::SameLine();

    if (is_running)
    {
        if (ImGui::Button("Stop"))
        {
            is_running = false;
        }
    }
    else
    {
        if (ImGui::Button("Run"))
        {
            is_running = true;
        }
    }

    ImGui::SameLine();

    if (ImGui::Button("Reset"))
    {
        is_running = false;
        gb->cpu.debug_log_entries.clear();
        gb->cpu.reset();
        // Step();
    }

    if (ImGui::Button("Dump debug log"))
    {
        gb->cpu.DumpDebugLog();
    }

    static char addr_input[5] = {"100"};
    ImGui::InputText("hexadecimal", addr_input, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
    ImGui::SameLine();
    if (ImGui::Button("Run to"))
    {
        std::string addr_string = addr_input;
        unsigned int addr_int = std::stoul(addr_string, nullptr, 16);
        run_to = addr_int;
        is_running = true;
    };

}

void MegaBoyDebugger::SetKeyState(Joypad::Button button, bool pressed)
{
    gb->joypad.SetButtonState(button, pressed);
}

void MegaBoyDebugger::Run()
{
    int cycles = 0;

    // std::this_thread::sleep_for(std::chrono::nanoseconds (1/4194304));
    //  The CPU clock rate is fixed at 2MHz, so a single clock cycle is 1/2000000s = 0.5µs. The frametime is the inverse of the refresh rate; so if the refresh rate is 60Hz, the frame time is 1/60s or ~16ms. The ratio is 16ms/0.5µs ~~ 33000.
    //  67108 cycles pr frame
    constexpr int CYCLES_PR_FRAME = 67108;
    while (cycles < CYCLES_PR_FRAME)
    {
        cycles += gb->Step();

        if (gb->cpu.regs.PC == run_to)
        {
            is_running = false;
            break;
        }
    }
}

void MegaBoyDebugger::Step()
{
    gb->Step();
    // UpdateLCDBuffer();
    scroll_to_bottom = true;
}