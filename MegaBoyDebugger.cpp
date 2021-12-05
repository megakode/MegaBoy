//
// Created by sbeam on 04/12/2021.
//
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <filesystem>
#include "MegaBoyDebugger.h"
#include "imgui.h"
#include "UI/UIConfig.h"

MegaBoyDebugger::MegaBoyDebugger() : registerWindow(cpu) {
    cpu.reset();

}

void MegaBoyDebugger::LoadTestRom()
{
    //std::filesystem::path filename = "zexdoc.com";
    std::filesystem::path filename = "zexdoc_sdsc.sms";

    auto path = std::filesystem::absolute(filename);
    auto size = std::filesystem::file_size(path);

    std::cout << "path: " << path << std::endl;
    if(exists(path)){
        std::cout << "file exists! :)";
    } else {
        std::cout << "file DOES NOT exist!";
    }
    std::ifstream z80file (path, std::ios::in | std::ios::binary);
    if (!z80file) {
        std::cout << "could not read zexdoc.com";
    } else
    {
        std::cout << "Reading zexdoc.com";
        z80file.read ((char*)&cpu.mem[0], size );
        z80file.close();
    }
}

void MegaBoyDebugger::UpdateUI() {


    registerWindow.UpdateUI();

    ImGui::Begin("Disassembly");

    ImGui::BeginListBox("");

    for( auto entry : cpu.debug_log_entries )
    {
        ImGui::TextColored(UIConfig::COLOR_VALUE_HEX,"0x%04x ",entry.PC);

        for (unsigned char opcode : entry.opcodes) {
            if (opcode != 0) {
                ImGui::SameLine();
                ImGui::Text("%02x", opcode);
            } else {
                ImGui::SameLine();
                ImGui::Text("  ");
            }
        }
        ImGui::SameLine();
        ImGui::Text("%s", entry.text.c_str());
    }

    ImGui::EndListBox();

    ImGui::End();



    if (ImGui::Button("Step"))
    {
        cpu.step();
    }

    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


    ImGui::End();

}
