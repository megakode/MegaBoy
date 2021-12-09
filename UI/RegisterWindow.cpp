//
// Created by sbeam on 05/12/2021.
//

#include "RegisterWindow.h"
#include "../imgui/imgui.h"
#include "UIConfig.h"

void RegisterWindow::UpdateUI() {

    // Create window
    ImGui::Begin("Registers");

    ImGui::Text("Main:");

    ImGui::BeginTable("Regs",2,ImGuiTableFlags_None);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("A"); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%02x",cpu.regs.A); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.regs.A);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("F"); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%02x",cpu.regs.F); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.regs.F);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("B"); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%02x",cpu.regs.B); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.regs.B);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("C"); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%02x",cpu.regs.C); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.regs.C);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("D"); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%02x",cpu.regs.D); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.regs.D);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("E"); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%02x",cpu.regs.E); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.regs.E);

    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);
    ImGui::Text("H"); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%02x",cpu.regs.H); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.regs.H);
    ImGui::TableSetColumnIndex(1);
    ImGui::Text("L"); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%02x",cpu.regs.L); ImGui::SameLine(); ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.regs.L);
    ImGui::EndTable();

    ImGui::Text("Special:");

    ImGui::BeginTable("Special Regs",1,ImGuiTableFlags_None);
    ImGui::TableNextRow();
    ImGui::TableSetColumnIndex(0);

    ImGui::Text("PC "); ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%04x",cpu.specialRegs.PC); ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.specialRegs.PC);

    ImGui::Text("SP "); ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%04x",cpu.specialRegs.SP);  ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.specialRegs.SP);

    ImGui::Text("IX "); ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%04x",cpu.specialRegs.IX);  ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.specialRegs.IX);

    ImGui::Text("IY "); ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_HEX, "0x%04x",cpu.specialRegs.IY);  ImGui::SameLine();
    ImGui::TextColored(UIConfig::COLOR_VALUE_DECIMAL, "(%d)",cpu.specialRegs.IY);

    ImGui::Text("Flags:");
    ImGui::BeginTable("Special Regs",8,ImGuiTableFlags_Borders);
    //  [S | Z | . | H | . | P/V | N | C ]
    ImGui::TableNextColumn();
    ImGui::Text("S"); ImGui::TableNextColumn();
    ImGui::Text("Z"); ImGui::TableNextColumn();
    ImGui::Text("."); ImGui::TableNextColumn();
    ImGui::Text("H"); ImGui::TableNextColumn();
    ImGui::Text("."); ImGui::TableNextColumn();
    ImGui::Text("P"); ImGui::TableNextColumn();
    ImGui::Text("N"); ImGui::TableNextColumn();
    ImGui::Text("C"); ImGui::TableNextColumn();
    ImGui::Text( cpu.getFlag(FlagBitmaskSign) ? "1" : "0" ); ImGui::TableNextColumn();
    ImGui::Text( cpu.getFlag(FlagBitmaskZero) ? "1" : "0" ); ImGui::TableNextColumn();
    ImGui::Text( (cpu.regs.F & (1 << 5)) ? "1" : "0" ); ImGui::TableNextColumn();
    ImGui::Text( cpu.getFlag(FlagBitmaskHalfCarry) ? "1" : "0" ); ImGui::TableNextColumn();
    ImGui::Text( (cpu.regs.F & (1 << 3)) ? "1" : "0" ); ImGui::TableNextColumn();
    ImGui::Text( cpu.getFlag(FlagBitmaskPV) ? "1" : "0" ); ImGui::TableNextColumn();
    ImGui::Text( cpu.getFlag(FlagBitmaskN) ? "1" : "0" ); ImGui::TableNextColumn();
    ImGui::Text( cpu.getFlag(FlagBitmaskC) ? "1" : "0" );

    ImGui::EndTable();

/*
    ImGui::Text("I 0x%02x R 0x%02x",cpu.specialRegs.I,cpu.specialRegs.R);
*/
    ImGui::EndTable();

}
