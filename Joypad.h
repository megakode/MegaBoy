//
// Created by sbeam on 3/11/22.
//

#ifndef MEGABOY_JOYPAD_H
#define MEGABOY_JOYPAD_H

#include <cstdint>
#include "HostMemory.h"

class Joypad {

public:

    enum class Button {
        Left,
        Right,
        Up,
        Down,
        Start,
        Select,
        A,
        B
    };

    HostMemory &memory;

    explicit Joypad( HostMemory &memory ) : memory(memory) {

    };

    Joypad() = delete;

    void SetButtonState( Button button, bool pressed );
    void WriteRegisterData( uint8_t data );
    uint8_t ReadRegisterData();

private:

    uint8_t action_bits    = 0b00101111;
    uint8_t direction_bits = 0b00011111;

    enum class StateMode {
        Action,
        Direction,
        NONE // Does this even exist?
    };

    enum RegisterBits : uint8_t {
        SelectActionButtons = 1 << 5,
        SelectDirectionButtons = 1 << 4,
        Right   = 1 << 0,
        Left    = 1 << 1,
        Up      = 1 << 2,
        Down    = 1 << 3,
        A       = 1 << 0,
        B       = 1 << 1,
        Select  = 1 << 2,
        Start   = 1 << 3
    };

    StateMode mode = StateMode::Action;

};


#endif //MEGABOY_JOYPAD_H
