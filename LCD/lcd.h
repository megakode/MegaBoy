#pragma once
#include "../HostMemory.h"

enum class LCDCBitmask: uint8_t {
    LCD_enabled = 1 << 7,                  // 7	LCD and PPU enable	0=Off, 1=On
    Window_Tile_Map_Area = 1 << 6,         // 6	Window tile map area	0=9800-9BFF, 1=9C00-9FFF
    Window_Enabled = 1 << 5,               // 5	Window enable	0=Off, 1=On
    BG_And_Window_Tile_Data_Area = 1 << 4, // 4	BG and Window tile data area	0=8800-97FF (signed), 1=8000-8FFF (unsigned)
    BG_Tile_Map_Area = 1 << 3,             // 3	BG tile map area	0=9800-9BFF (Tile_Map_Block_0) , 1=9C00-9FFF (Tile_Map_Block_1)
    OBJ_Size = 1 << 2,                     // 2	OBJ size	0=8x8, 1=8x16
    OBJ_Enable = 1 << 1,                   // 1	OBJ enable	0=Off, 1=On
    BG_And_Window_Enable = 1               // 0	BG and Window enable/priority	0=Off, 1=On
};

class LCD
{
private:

    static constexpr uint8_t TILE_WIDTH = 8;
    static constexpr uint8_t TILE_HEIGHT = 8;
    static constexpr uint16_t Tile_Map_Block_0 = 0x9800;
    static constexpr uint16_t Tile_Map_Block_1 = 0x9c00;
    static constexpr uint16_t Tile_Data_Block_0 = 0x8000;
    static constexpr uint16_t Tile_Data_Block_1 = 0x8800;
    static constexpr uint16_t Tile_Data_Block_2 = 0x9000;

    struct RGB {
        uint8_t R = 0;
        uint8_t G = 0;
        uint8_t B = 0;
        uint8_t A = 0xff;
    };

    HostMemory& mem;

    static constexpr uint16_t Number_Of_Scanlines = 154;

    enum LCD_Modes {
        LCD_Mode_HBlank = 0,
        LCD_Mode_VBlank = 1,
        LCD_Mode_Searching_OAM = 2,
        LCD_Mode_Reading_OAM = 3
    };

    static constexpr uint16_t LCD_Mode_Cycles[] = { 208, 4560, 80,  168 };
    static constexpr uint8_t LCD_Mode_Order[] = { LCD_Mode_Searching_OAM,LCD_Mode_Reading_OAM,LCD_Mode_HBlank,LCD_Mode_VBlank };

    // On scanlines 0 through 143, the PPU cycles through modes 2, 3, and 0 once every 456 dots. Scanlines 144 through 153 are mode 1.
    // Mode 2  2_____2_____2_____2_____2_____2___________________2____
    // Mode 3  _33____33____33____33____33____33__________________3___
    // Mode 0  ___000___000___000___000___000___000________________000
    // Mode 1  ____________________________________11111111111111_____

    // Mode 2 - Searching OAM for OBJs whose Y coordinate overlap this line - 80 dots (19 µs)
    // Mode 3 - Reading OAM and VRAM to generate the picture - 168 to 291 dots (40 to 60 µs) depending on sprite count
    // Mode 0 - Nothing (HBlank) - 85 to 208 dots (20 to 49 µs) depending on previous mode 3 duration
    // Mode 1 - Nothing (VBlank)	4560 dots (1087 µs, 10 scanlines)




public:

    enum LCD_Stat_Bitmask {
        LCD_Stat_IRQ_From_LYC = 1 << 6,
        LCD_Stat_IRQ_From_OAM = 1 << 5,
        LCD_Stat_IRQ_From_VBlank = 1 << 4,
        LCD_Stat_IRQ_From_HBlank = 1 << 3,
        LCD_Stat_LY_EQ_LYC = 1 << 2,
    };

    /// LCD Control register address
    static constexpr uint16_t LCD_Control_Register = 0xff40;

    /// LCD Stat Register (0xFF41)
    /// Bit 6 - LYC=LY STAT Interrupt source         (1=Enable) (Read/Write)
    /// Bit 5 - Mode 2 OAM STAT Interrupt source     (1=Enable) (Read/Write)
    /// Bit 4 - Mode 1 VBlank STAT Interrupt source  (1=Enable) (Read/Write)
    /// Bit 3 - Mode 0 HBlank STAT Interrupt source  (1=Enable) (Read/Write)
    /// Bit 2 - LYC=LY Flag                          (0=Different, 1=Equal) (Read Only)
    /// Bit 1-0 - Mode Flag                          (Mode 0-3, see below) (Read Only)
    /// 0: HBlank
    /// 1: VBlank
    /// 2: Searching OAM
    /// 3: Transferring Data to LCD Controller
    static constexpr uint16_t LCD_Stat_Register = 0xff41;
    /// LCD Y Register - indicates current Y line (R)
    static constexpr uint16_t LCD_Y_Register = 0xff44;
    /// LCD Y Compare register - triggers a stat irq if stat bit 2 is set and Y == YC
    static constexpr uint16_t LCD_Y_Compare_Register = 0xff45;

    static constexpr int BUFFER_WIDTH = 256;
    static constexpr int BUFFER_HEIGHT = 256;
    static constexpr int Tile_Map_Width = 32;

    /// A index-color based render buffer. Every time `Step` is called all tiles and sprites are rendered to this buffer based on OAM, tile maps and tile data.
    /// Each byte represents an index color between 0-3.
    /// It is up to the implementer to actually draw this to screen and map it to actual RGB values.
    uint8_t renderBuffer[BUFFER_WIDTH * BUFFER_HEIGHT] = {};
    RGB rgbBuffer[BUFFER_WIDTH * BUFFER_HEIGHT] = {};

    uint8_t& current_scanline;
    bool redraw = true;
    uint8_t current_mode_index = 0;
    uint16_t accumulated_cycles = 0;

    static constexpr uint16_t CyclesPerScanline = 456;
    uint16_t current_scanline_cycles = 0;

    explicit LCD( HostMemory& mem ) : mem(mem), current_scanline(mem[LCD_Y_Register])
    {
    };

    LCD() = delete;

    void Step( uint16_t delta_cycles )
    {
        static bool is_hblank_irq_already_triggered_on_this_line = false;
        static bool is_scanline_irq_flag_just_set = false;
        // Scanline cycles
        // Make current_scanline iterate between 0..153 each lasting 456 cycles

        current_scanline_cycles += delta_cycles;
        if(current_scanline_cycles >= CyclesPerScanline ){
            current_scanline_cycles -= CyclesPerScanline;
            current_scanline++;

            // Trigger VBlank IRQ on line 144

            if(current_scanline == 144){
                if(!is_scanline_irq_flag_just_set) {
                    mem.SetInterruptFlag(Interrupt_Flag_VBlank, true);
                    is_scanline_irq_flag_just_set = true;
                }
            } else {
                is_scanline_irq_flag_just_set = false;
            }

            if(current_scanline == 154){
                current_scanline = 0;
            }
        }

        mem[LCD_Y_Register] = current_scanline;

        accumulated_cycles += delta_cycles;

        if( accumulated_cycles >= LCD_Mode_Cycles[current_mode_index] )
        {
            accumulated_cycles -= LCD_Mode_Cycles[current_mode_index];
            current_mode_index++;

            if( current_scanline < 144 && LCD_Mode_Order[current_mode_index] == LCD_Mode_VBlank){
                current_mode_index = 0;
            }

            if(current_mode_index == 4) { // VBlank is done, restart mode order
                current_mode_index = 0;
            }

            // TODO: does this trigger on OAM search or OAM draw?
            if(LCD_Mode_Order[current_mode_index] == LCD_Mode_Searching_OAM){
                if(mem[LCD_Stat_Register] & LCD_Stat_IRQ_From_OAM){
                    mem.SetInterruptFlag(Interrupt_Flag_LCD_Stat,true);
                }
            }

            if(LCD_Mode_Order[current_mode_index] == LCD_Mode_HBlank){
                if(mem[LCD_Stat_Register] & LCD_Stat_IRQ_From_HBlank){
                    if(!is_hblank_irq_already_triggered_on_this_line) {
                        mem.SetInterruptFlag(Interrupt_Flag_LCD_Stat, true);
                        is_hblank_irq_already_triggered_on_this_line = true;
                    }
                }
            } else {
                is_hblank_irq_already_triggered_on_this_line = false;
            }

            if(LCD_Mode_Order[current_mode_index] == LCD_Mode_VBlank){
                if(mem[LCD_Stat_Register] & LCD_Stat_IRQ_From_VBlank){
                    mem.SetInterruptFlag(Interrupt_Flag_LCD_Stat,true);
                }
            }
        }

        mem[LCD_Stat_Register] &= 0b11111100;                         // Clear mode bits
        mem[LCD_Stat_Register] |= LCD_Mode_Order[current_mode_index]; // Set the current mode bits

        // LY Compare

        static bool lyc_just_triggered = false;

        if( mem[LCD_Y_Register] == mem[LCD_Y_Compare_Register])
        {
            // Trigger Stat IRQ from LYC (if enabled in the stat register)
            if((mem[LCD_Stat_Register] & LCD_Stat_IRQ_From_LYC) && !lyc_just_triggered ){
                mem.SetInterruptFlag(Interrupt_Flag_LCD_Stat,true);
                lyc_just_triggered = true;
            }
            mem[LCD_Stat_Register] |= LCD_Stat_LY_EQ_LYC;
        } else {
            mem[LCD_Stat_Register] &= ~LCD_Stat_LY_EQ_LYC;
            lyc_just_triggered = false;
        }

        // Draw BG tiles

        // TODO: Hack to only redraw display on line 0
        if(current_scanline != 0 || !redraw){
            redraw = true;
            return;
        }

        redraw = false;

        uint16_t bg_tile_map_addr = IsFlagSet(LCDCBitmask::BG_Tile_Map_Area) ? Tile_Map_Block_1 : Tile_Map_Block_0;
    // 38912 = 0x9800
        for(uint8_t y = 0; y < Tile_Map_Width; y++){
            for(uint8_t x = 0; x < Tile_Map_Width; x++) {
                uint16_t index = (y*Tile_Map_Width)+x;
                uint8_t current_tile_id = mem.Read(bg_tile_map_addr + index);
                uint16_t current_tile_data_addr = GetTileDataAddr(current_tile_id);
                //uint16_t current_tile_data_addr = GetTileDataAddr(index); // Test which draws the tiles in order
                // TODO: Factor in BG scroll registers
                // https://gbdev.io/pandocs/Scrolling.html#ff4a---wy-window-y-position-rw-ff4b---wx-window-x-position--7-rw
                DrawTile(current_tile_data_addr, x*8, y*8);
            }
        }

        // Draw window

        uint16_t window_tile_map_addr = IsFlagSet(LCDCBitmask::Window_Tile_Map_Area) ? Tile_Map_Block_1 : Tile_Map_Block_0;
/*
        if(IsFlagSet(LCDCBitmask::Window_Enabled)){
            for (uint8_t y = 0; y < Tile_Map_Width; y++) {
                for (uint8_t x = 0; x < Tile_Map_Width; x++) {
                    uint8_t current_tile_id = mem.Read(window_tile_map_addr + (y * Tile_Map_Width) + x);
                    uint16_t current_tile_data_addr = GetTileDataAddr(current_tile_id);
                    // TODO: Factor in WX and WY registers.
                    // https://gbdev.io/pandocs/Scrolling.html#ff4a---wy-window-y-position-rw-ff4b---wx-window-x-position--7-rw
                    DrawTile(current_tile_data_addr, x, y);
                }
            }
        }
*/
        RenderRGBBuffer();
    }

    void RenderRGBBuffer()
    {
        constexpr RGB rgb[4] = { {20,50,00} , {20,128,20}, {50,192,50}, {100,255,100} };
        for(int i = 0 ; i < BUFFER_HEIGHT*BUFFER_WIDTH; i++) {
            uint8_t colorIndex = renderBuffer[i];
            rgbBuffer[i] = rgb[colorIndex];
        }
    }

    bool IsFlagSet(LCDCBitmask flag)
    {
        return mem.Read(LCD_Control_Register) & static_cast<uint8_t>(flag);
    }

    void SetFlag(LCDCBitmask flag, bool value)
    {
        if(value)
           mem.Write(LCD_Control_Register, mem.Read(LCD_Control_Register) | static_cast<uint8_t>(flag) );
        else
            mem.Write(LCD_Control_Register, mem.Read(LCD_Control_Register) & ~static_cast<uint8_t>(flag) );
    }

    /// Get the address of a tiles data based on tile id and the current LCD_Control_Register control bits
    /// \param tile_id for which to get data
    /// \return Address in GB memory
    uint16_t GetTileDataAddr(uint8_t tile_id)
    {
        if(IsFlagSet(LCDCBitmask::BG_And_Window_Tile_Data_Area))
        {
            return Tile_Data_Block_0 + tile_id*16;
        }
        else
        {
            // If LCD_Control_Register.4 == 1 then tile ids are interpreted as signed with 0x9000 as base address.
            // See table here: https://gbdev.io/pandocs/Tile_Data.html
            return Tile_Data_Block_2 + static_cast<int8_t>(tile_id)*16;
        }
    }

    /// Render a tile from GB memory to an internal renderBuffer, and translate the GB 2BPP format to an index color.
    /// \param tile_data_addr Address in GB memory where tile data for this tile is located
    /// \param dst_x Destination X on screen (0-255)
    /// \param dst_y Destination Y on screen (0-255)
    void DrawTile( uint16_t tile_data_addr, uint8_t dst_x, uint8_t dst_y ) {
        for (int y = 0; y < TILE_HEIGHT; y++) {
            uint8_t lobits = mem.Read(tile_data_addr++);
            uint8_t hibits = mem.Read(tile_data_addr++);
            for (int x = 0; x < TILE_WIDTH; x++) {
                uint8_t bit_number = 7-x;
                uint8_t color = ( (lobits >> bit_number) & 1 ) |  ( ((hibits >> bit_number) & 1)  << 1 );
                renderBuffer[(dst_y+y) * BUFFER_WIDTH + (dst_x+x)] = color;
            }
        }
    }

};