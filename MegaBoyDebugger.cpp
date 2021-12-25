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
#include <thread>

MegaBoyDebugger::MegaBoyDebugger() {

    gb = std::make_unique<Gameboy>();
    gb->cpu.reset();

}

MegaBoyDebugger::~MegaBoyDebugger()
{
    is_running = false;
    if(gb_thread.joinable()) {
        gb_thread.join();
    }
}

void MegaBoyDebugger::LoadTestRom()
{
    //std::filesystem::path filename = "../tests/zexdoc.com";
    //std::filesystem::path filename = "../tests/cpu_instrs.gb";
    std::filesystem::path filename = "../tests/TETRIS.GB";

    auto path = std::filesystem::absolute(filename);
    auto size = std::filesystem::file_size(path);

    if( size > 0xffff ){
        std::cout << "Warning: ROM file size is larger than 64k! will only read first 64k" << std::endl;
        size = 0xffff;
    }

    std::cout << "path: " << path << std::endl;
    if(exists(path)){
        std::cout << "file exists! :)";
    } else {
        std::cout << "file DOES NOT exist!";
    }
    std::ifstream z80file (path, std::ios::in | std::ios::binary);
    if (!z80file) {
        std::cout << "could not read " << path;
    } else
    {
        std::cout << "Reading " << path;
        z80file.read ((char*)&gb->cpu.mem[0], size );
        z80file.close();
    }

    gb->cpu.regs.PC = 0x100;
}

void MegaBoyDebugger::UpdateUI() 
{
    RegisterWindow::UpdateUI(gb->cpu);

    ImGui::Begin("Disassembly");

   static int item_current_idx = 0;

    if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 20 * ImGui::GetTextLineHeightWithSpacing()))) {

        //for( int index = 0 ; index < cpu.debug_log_entries.size() ; index++ )
        {
            ImGuiListClipper clipper;
            clipper.Begin(gb->cpu.debug_log_entries.size());
            while (clipper.Step())
                for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; i++){
                    auto& entry = gb->cpu.debug_log_entries[i];
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
        gb->cpu.debug_log_entries.clear();
        //Step();
    }

   if (ImGui::Button("Step"))
   {
       is_running = false;
       Step();
   }

   if(is_running){
       if(ImGui::Button("Stop")){
           is_running = false;
           if(gb_thread.joinable()) {
               gb_thread.join();
           }

       }
   } else {
       if (ImGui::Button("Run"))
       {
           is_running = true;
           gb_thread = std::thread(&MegaBoyDebugger::Run,this);
       }
   }

    //ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);


    ImGui::End();

}

void MegaBoyDebugger::Run()
{
    while(is_running){
        gb->Step();
        UpdateLCDBuffer();
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }
}

void MegaBoyDebugger::UpdateLCDBuffer()
{
    for(int i = 0 ; i < GB_SCREEN_WIDTH*GB_SCREEN_HEIGHT; i++)
    {
        // TODO: palette
        static uint8_t color_values[] = {0,64,128,255};
        uint8_t color_index = gb->lcd.renderBuffer[i];
        screenData[i*3] = color_values[color_index]; // Red
        screenData[i*3+1] = color_values[color_index]; // Green
        screenData[i*3+2] = color_values[color_index]; // Blue
    }
}

void MegaBoyDebugger::Step()
{
    gb->Step();
    UpdateLCDBuffer();
    scroll_to_bottom = true;
}