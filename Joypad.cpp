//
// Created by sbeam on 3/11/22.
//

#include "Joypad.h"


//    Bit 7 - Not used
//    Bit 6 - Not used
//    Bit 5 - P15 Select Action buttons    (0=Select)
//    Bit 4 - P14 Select Direction buttons (0=Select)
//    Bit 3 - P13 Input: Down  or Start    (0=Pressed) (Read Only)
//    Bit 2 - P12 Input: Up    or Select   (0=Pressed) (Read Only)
//    Bit 1 - P11 Input: Left  or B        (0=Pressed) (Read Only)
//    Bit 0 - P10 Input: Right or A        (0=Pressed) (Read Only)

void Joypad::WriteRegisterData(uint8_t data) {
    if(!(data & RegisterBits::SelectActionButtons)){
        mode = StateMode::Action;
    } else if(!(data & RegisterBits::SelectDirectionButtons)){
        mode = StateMode::Direction;
    } else {
        mode = StateMode::NONE; // Not sure this exists?
    }
}

uint8_t Joypad::ReadRegisterData() {
    if( mode == StateMode::Direction){
        return direction_bits;
    } else {
        return action_bits;
    }
}

void Joypad::SetButtonState(Joypad::Button button, bool pressed ) {

    RegisterBits bit;
    uint8_t *reg;

    memory.SetInterruptFlag(Interrupt_Flag_Joypad,true);

    switch (button) {
        case Button::Left: bit = RegisterBits::Left; reg = &direction_bits; break;
        case Button::Right: bit = RegisterBits::Right; reg = &direction_bits; break;
        case Button::Up: bit = RegisterBits::Up; reg = &direction_bits; break;
        case Button::Down: bit = RegisterBits::Down; reg = &direction_bits; break;
        case Button::Start: bit = RegisterBits::Start; reg = &action_bits; break;
        case Button::Select: bit = RegisterBits::Select; reg = &action_bits; break;
        case Button::A: bit = RegisterBits::A; reg = &action_bits; break;
        case Button::B: bit = RegisterBits::B; reg = &action_bits; break;
    }

    if(pressed){
        // Clear bit
        *reg &= ~bit;
    } else {
        // Set bit
        *reg |= bit;
    }
}
