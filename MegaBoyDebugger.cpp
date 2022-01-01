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
    screenData = reinterpret_cast<uint8_t *>(&gb->lcd.rgbBuffer);
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
    //std::filesystem::path filename = "../tests/cpu_instrs.gb";
    //std::filesystem::path filename = "../tests/BOBBLE.GB";
    //std::filesystem::path filename = "../tests/DRMARIO.GB";
    //std::filesystem::path filename = "../tests/TETRIS.GB";
    //std::filesystem::path filename = "../tests/01-special.gb";
    //std::filesystem::path filename = "../tests/02-interrupts.gb";
    //std::filesystem::path filename = "../tests/03-op sp,hl.gb"; (fail)
    std::filesystem::path filename = "../tests/04-op r,imm.gb";
    //std::filesystem::path filename = "../tests/05-op rp.gb";
    //std::filesystem::path filename = "../tests/06-ld r,r.gb"; (PASSED!)
    //std::filesystem::path filename = "../tests/07-jr,jp,call,ret,rst.gb";
    //std::filesystem::path filename = "../tests/08-misc instrs.gb"; // (failed)
    //std::filesystem::path filename = "../tests/09-op r,r.gb";
    //std::filesystem::path filename = "../tests/";



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

        int number_of_lines = 10;
        //int first_index = std::max(0,gb->cpu.debug_log_entries.size() - number_of_lines);

        int lines = std::min(10,(int)gb->cpu.debug_log_entries.size());
        //for( int index = 0 ; index < gb->cpu.debug_log_entries.size() ; index++ )
        for(auto it = gb->cpu.debug_log_entries.end() - lines; it != std::end(gb->cpu.debug_log_entries); ++it)
        {
                    auto& entry = *it; // gb->cpu.debug_log_entries[i];
                    if(it==gb->cpu.debug_log_entries.end()-1){
                        ImGui::TextColored(UIConfig::COLOR_VALUE_HEX,">0x%04x ",entry.PC);
                    } else {
                        ImGui::TextColored(UIConfig::COLOR_VALUE_HEX," 0x%04x ",entry.PC);
                    }


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

        //for( int index = 0 ; index < cpu.debug_log_entries.size() ; index++ )
        /*
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
*/
    }
    ImGui::EndListBox();

    ImGui::End();




   if (ImGui::Button("Step"))
   {
       is_running = false;
       Step();
   }


    ImGui::SameLine();


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


    ImGui::SameLine();


    if (ImGui::Button("Reset") )
    {
        is_running = false;
        gb->cpu.debug_log_entries.clear();
        gb->cpu.reset();
        //Step();
    }

    static char addr_input[5] = {"100"}; ImGui::InputText("hexadecimal", addr_input, 5, ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsUppercase);
    ImGui::SameLine();
    if(ImGui::Button("Run to")){
        std::string addr_string = addr_input;
        unsigned int addr_int = std::stoul(addr_string, nullptr, 16);
        run_to = addr_int;
        is_running = true;
        if(gb_thread.joinable()) {
            gb_thread.join();
        }
        gb_thread = std::thread(&MegaBoyDebugger::Run,this);
    };

    ImGui::End();

}

void MegaBoyDebugger::Run()
{
    int steps = 0;
    while(is_running && gb->cpu.regs.PC != run_to){

        gb->Step();
        steps++;
        // std::this_thread::sleep_for(std::chrono::seconds(2));
    }
    is_running = false;
    std::cout << "steps = " << steps << std::endl;
}

void MegaBoyDebugger::Step()
{
    gb->Step();
    //UpdateLCDBuffer();
    scroll_to_bottom = true;
}