//
// Created by sbeam on 09/12/2021.
//

#pragma once

#include <cstdint>
#include <functional>

enum Interrupt_Flag: uint8_t
{
    Interrupt_Flag_VBlank = 1,
    Interrupt_Flag_LCD_Stat = 1 << 1,
    Interrupt_Flag_Timer = 1 << 2,
    Interrupt_Flag_Serial = 1 << 3,
    Interrupt_Flag_Joypad = 1 << 4
};


/*
    $FF00	        Controller
    $FF01	$FF02	DMG	Communication
    0xFF04: Divider register value - Timer
    0xFF06: TMA - Timer Modulo (R/W)
    0xFF07: - TAC - Timer Control
    $FF10	$FF26	DMG	Sound
    $FF30	$FF3F	DMG	Waveform RAM
    $FF40	$FF4B	DMG	LCD
    $FF4F		    CGB	VRAM Bank Select
    $FF50		    DMG	Set to non-zero to disable boot ROM
    $FF51	$FF55	CGB	VRAM DMA
    $FF68	$FF69	CGB	BG / OBJ Palettes
    $FF70		    CGB	WRAM Bank Select
*/

enum class IOAddress : uint16_t
{
    /// Bit 7 - Not used
    /// Bit 6 - Not used
    /// Bit 5 - P15 Select Action buttons    (0=Select)
    /// Bit 4 - P14 Select Direction buttons (0=Select)
    /// Bit 3 - P13 Input: Down  or Start    (0=Pressed) (Read Only)
    /// Bit 2 - P12 Input: Up    or Select   (0=Pressed) (Read Only)
    /// Bit 1 - P11 Input: Left  or B        (0=Pressed) (Read Only)
    /// Bit 0 - P10 Input: Right or A        (0=Pressed) (Read Only)
    Joypad = 0xff00,
    /// DIV
    TimerDivider = 0xFF04,
    /// FF05: TIMA - Timer counter (R/W)
    TimerCounter = 0xFF05,
    /// TMA
    TimerModule = 0xFF06,
    /// Tac
    TimerControl = 0xff07,
    /// FF0F - IF Interrupt Flag (R/W)
    ///
    /// When an interrupt signal changes from low to high, the corresponding bit in the IF register becomes set.
    /// For example, Bit 0 becomes set when the LCD controller enters the VBlank period.
    ///
    /// Any set bits in the IF register are only requesting an interrupt to be executed.
    /// The actual execution happens only if both the IME flag and the corresponding bit in the IE register are set,
    /// otherwise the interrupt “waits” until both IME and IE allow its execution.
    InterruptFlag = 0xff0f,
    /// FFFF - IE - Interrupt Enable (R/W)
    ///
    /// Bit 0: VBlank   Interrupt Enable  (INT $40)  (1=Enable)
    /// Bit 1: LCD STAT Interrupt Enable  (INT $48)  (1=Enable)
    /// Bit 2: Timer    Interrupt Enable  (INT $50)  (1=Enable)
    /// Bit 3: Serial   Interrupt Enable  (INT $58)  (1=Enable)
    /// Bit 4: Joypad   Interrupt Enable  (INT $60)  (1=Enable)
    InterruptEnabled = 0xffff,

    /// FF42 - SCY (Scroll Y) (R/W),
    /// The top coordinate of the visible 160×144 pixel area within the 256×256 pixels BG map. Values in the range 0–255 may be used. wraps around.
    Scroll_Y = 0xff42,

    /// FF43 - SCX (Scroll X) (R/W)
    /// The top coordinate of the visible 160×144 pixel area within the 256×256 pixels BG map. Values in the range 0–255 may be used. wraps around.
    Scroll_X = 0xff43,

    /// Writing to this register launches a DMA transfer from ROM or RAM to OAM (Object Attribute Memory).
    /// The written value specifies the transfer source address divided by $100, that is, source and destination are:
    ///
    /// Source:      $XX00-$XX9F   ;XX = $00 to $DF
    /// Destination: $FE00-$FE9F
    DMATransferStartAddress = 0xff46,

    /// FF47 - BG Palette Data
    ///
    /// Bit 7-6 - Color for index 3
    /// Bit 5-4 - Color for index 2
    /// Bit 3-2 - Color for index 1
    /// Bit 1-0 - Color for index 0
    ///
    /// Value	Color
    /// 0	    White
    /// 1	    Light gray
    /// 2	    Dark gray
    /// 3	    Black
    BG_Palette_Data = 0xff47,

    /// Object palette 0.
    /// Works exactly like BG palette data, except that color 0 is ignored because index 0 is transparent on objects.
    OBJ_Palette_0_Data = 0xff48,

    /// Object palette 1.
    /// Works exactly like BG palette data, except that color 0 is ignored because index 0 is transparent on objects.
    OBJ_Palette_1_Data = 0xff49,

    /// FF4A - Window Y Position
    /// The Window is visible (if enabled) when both coordinates are in the ranges WX=0..166, WY=0..143 respectively.
    /// Values WX=7, WY=0 place the Window at the top left of the screen, completely covering the background.
    Window_Y_Position = 0xff4a,

    /// FF4B - Window X Position (+7)
    Window_X_Position = 0xff4b,

    /// Set to non-zero to disable boot ROM
    Boot_ROM_Disabled = 0xff50

};

class HostMemory {

public:

    HostMemory() noexcept;

    /// Called when a value is written to an memory mapped io address (0xff00 - 0xffff)
    std::function<void(uint16_t,uint8_t)> didWriteToIOAddress = nullptr;

    uint8_t& operator[] (uint16_t index);

    /// Read a byte from the bus
    [[nodiscard]] uint8_t Read(uint16_t address) const;

    /// Read a byte from the bus
    [[nodiscard]] uint8_t Read( IOAddress address ) const
    {
        return memory[static_cast<uint16_t>(address)];
    }

    /// Write a byte to a given address on the bus
    void Write(uint16_t address, uint8_t value);

    void Write(IOAddress address, uint8_t value);

    void SetInterruptFlag( Interrupt_Flag flag , bool enabled )
    {
        if( enabled ){
            memory[static_cast<uint16_t>(IOAddress::InterruptFlag)] |= static_cast<uint8_t>(flag);
        } else {
            memory[static_cast<uint16_t>(IOAddress::InterruptFlag)] &= ~static_cast<uint8_t>(flag);
        }
    }

    uint8_t memory[UINT16_MAX+1]{};


private:


};
