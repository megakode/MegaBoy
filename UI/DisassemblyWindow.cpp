//
// Created by sbeam on 1/18/22.
//

#include "DisassemblyWindow.h"
#include "imgui.h"
#include "UIConfig.h"

void DisassemblyWindow::UpdateUI(CPU &cpu)
{
    ImGui::Begin("Disassembly");

    static int item_current_idx = 0;

    if (ImGui::BeginListBox("##listbox 2", ImVec2(-FLT_MIN, 20 * ImGui::GetTextLineHeightWithSpacing()))) {

        int number_of_lines = 10;
        //int first_index = std::max(0,gb->cpu.debug_log_entries.size() - number_of_lines);

        int lines = std::min(10,(int)cpu.debug_log_entries.size());
        int max_line_index = (int)cpu.debug_log_entries.size();
        for( int index = max_line_index-lines ; index < max_line_index ; index++ )
            //for(auto it = gb->cpu.debug_log_entries.end() - lines; it != std::end(gb->cpu.debug_log_entries); ++it)
        {
            // Intentionally copy the entry here, as otherwise a sigsegv might occur when we print it further down
            //auto entry = *it; // gb->cpu.debug_log_entries[i];
            auto entry = cpu.debug_log_entries[index];
            if(index == max_line_index-1){
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
}
