#pragma once

#include <cstring>
#include "../HostMemory.h"
#include "Fetcher.h"
#include "FIFO.h"
#include "lcd_regs.h"
class LCD
{
private:

    static constexpr uint8_t TILE_WIDTH = 8;
    static constexpr uint8_t TILE_HEIGHT = 8;

    static constexpr uint16_t OAM_Address = 0xfe00;

    struct RGB {
        union {
            struct {
                uint8_t R = 0;
                uint8_t G = 0;
                uint8_t B = 0;
                uint8_t A = 0xff;
            };
            uint32_t RGBA;
        };
    };

    struct OAM_Sprite {
        uint8_t y_position;
        uint8_t x_position;
        uint8_t tile_index;
        uint8_t attributes;
    } __attribute__((packed));

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

    /// A index-color based render buffer. Every time `Step` is called all tiles and sprites are rendered to this buffer based on OAM, tile maps and tile data.
    /// Each byte represents an index color between 0-3.
    /// It is up to the implementer to actually draw this to screen and map it to actual RGB values.
    uint8_t renderBuffer[BUFFER_WIDTH * BUFFER_HEIGHT] = {};
    RGB rgbBuffer[BUFFER_WIDTH * BUFFER_HEIGHT] = {};

    uint8_t& current_scanline;
    bool draw_scanline = true;
    uint8_t current_mode_index = 0;
    uint16_t accumulated_cycles = 0;

    static constexpr uint16_t CyclesPerScanline = 456;
    uint16_t current_scanline_cycles = 0;

    FIFO fifo;
    Fetcher fetcher;

    explicit LCD( HostMemory& mem ) : mem(mem), current_scanline(mem[LCD_Y_Register]), fetcher(mem)
    {
    };

    LCD() = delete;

    void Step( uint16_t delta_cycles );

    void RenderRGBBuffer( uint8_t line_number );

    inline bool IsFlagSet(LCDCBitmask flag)
    {
        return mem.Read(LCD_Control_Register) & static_cast<uint8_t>(flag);
    }

    inline void SetFlag(LCDCBitmask flag, bool value)
    {
        if(value)
           mem.Write(LCD_Control_Register, mem.Read(LCD_Control_Register) | static_cast<uint8_t>(flag) );
        else
            mem.Write(LCD_Control_Register, mem.Read(LCD_Control_Register) & ~static_cast<uint8_t>(flag) );
    }

    /// Get the address of a tiles data based on tile id and the current LCD_Control_Register control bits
    /// \param tile_id for which to get data
    /// \return Address in GB memory
    uint16_t GetTileDataAddr(uint8_t tile_id);

    /// Render a tile from GB memory to an internal renderBuffer, and translate the GB 2BPP format to an index color.
    /// \param tile_data_addr Address in GB memory where tile data for this tile is located
    /// \param dst_x Destination X on screen (0-255)
    /// \param dst_y Destination Y on screen (0-255)
    void DrawTile( uint16_t tile_data_addr, uint8_t dst_x, uint8_t dst_y );

private:

    uint8_t window_internal_line_counter = 0;

    void DrawWindow();

    void DrawBackground();

    void DrawScanline();

    void DrawSprites();
};