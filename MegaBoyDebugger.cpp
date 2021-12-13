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

MegaBoyDebugger::MegaBoyDebugger() : registerWindow(gb.cpu)  {
    gb.cpu.reset();
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
        z80file.read ((char*)&gb.cpu.mem[0], size );
        z80file.close();
    }
}

void MegaBoyDebugger::UpdateUI() {


    static bool is_running = false;

    registerWindow.UpdateUI();

    ImGui::Begin("Disassembly");

   static int item_current_idx = 0;

    if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 20 * ImGui::GetTextLineHeightWithSpacing()))) {

        //for( int index = 0 ; index < cpu.debug_log_entries.size() ; index++ )
        {
            ImGuiListClipper clipper;
            clipper.Begin(gb.cpu.debug_log_entries.size());
            while (clipper.Step())
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++){
                    auto& entry = gb.cpu.debug_log_entries[i];
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

                    if( i == clipper.DisplayEnd-1){
                        if(scroll_to_bottom){
                            ImGui::SetScrollHereY();
                            scroll_to_bottom = false;
                        }
                    }
                }
        }

    }
    ImGui::EndListBox();

    ImGui::End();




    if (ImGui::Button("Reset"))
    {
        is_running = false;
        gb.cpu.debug_log_entries.clear();
        //Step();
    }

   if (ImGui::Button("Step"))
   {
       is_running = false;
       Step();
   }

    if (ImGui::Button("Run"))
    {
        is_running = true;
    }

    if(is_running){
        Step();
    }


    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


    ImGui::End();

}

void MegaBoyDebugger::Step()
{
    gb.Step();
    scroll_to_bottom = true;
}