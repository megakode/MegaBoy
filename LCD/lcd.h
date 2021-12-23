#pragma once
#include "../HostMemory.h"

enum class LCDCBitmask: uint8_t {
    LCD_enabled = 1 << 7,                  // 7	LCD and PPU enable	0=Off, 1=On
    Window_Tile_Map_Area = 1 << 6,         // 6	Window tile map area	0=9800-9BFF, 1=9C00-9FFF
    Window_Enabled = 1 << 5,               // 5	Window enable	0=Off, 1=On
    BG_And_Window_Tile_Data_Area = 1 << 4, // 4	BG and Window tile data area	0=8800-97FF, 1=8000-8FFF
    BG_Tile_Map_Area = 1 << 3,             // 3	BG tile map area	0=9800-9BFF, 1=9C00-9FFF
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

    HostMemory& mem;

public:
    /// LCD Control register address
    static constexpr uint16_t LCDC = 0xff40;

    static constexpr int WIDTH = 160;
    static constexpr int HEIGHT = 144;
    static constexpr int Tile_Map_Width = 32;

    /// A index-color based render buffer. Every time `Step` is called all tiles and sprites are rendered to this buffer based on OAM, tile maps and tile data.
    /// Each byte represents an index color between 0-3.
    /// It is up to the implementer to actually draw this to screen and map it to actual RGB values.
    uint8_t renderBuffer[WIDTH*HEIGHT] = {};

    explicit LCD( HostMemory& mem ) : mem(mem)
    {

    };

    LCD() = delete;

    void Step()
    {
        // Draw BG tiles

        uint16_t bg_tile_map_addr = IsFlagSet(LCDCBitmask::BG_Tile_Map_Area) ? Tile_Map_Block_1 : Tile_Map_Block_0;

        for(uint8_t y = 0; y < Tile_Map_Width; y++){
            for(uint8_t x = 0; x < Tile_Map_Width; x++) {
                uint8_t current_tile_id = mem.Read(bg_tile_map_addr + (y*Tile_Map_Width)+x);
                uint16_t current_tile_data_addr = GetTileDataAddr(current_tile_id);
                // TODO: Factor in BG scroll registers
                // https://gbdev.io/pandocs/Scrolling.html#ff4a---wy-window-y-position-rw-ff4b---wx-window-x-position--7-rw
                DrawTile(current_tile_data_addr, x, y);
            }
        }

        // Draw window

        uint16_t window_tile_map_addr = IsFlagSet(LCDCBitmask::Window_Tile_Map_Area) ? Tile_Map_Block_1 : Tile_Map_Block_0;

        for(uint8_t y = 0; y < Tile_Map_Width; y++){
            for(uint8_t x = 0; x < Tile_Map_Width; x++) {
                uint8_t current_tile_id = mem.Read(window_tile_map_addr + (y*Tile_Map_Width)+x);
                uint16_t current_tile_data_addr = GetTileDataAddr(current_tile_id);
                // TODO: Factor in WX and WY registers.
                // https://gbdev.io/pandocs/Scrolling.html#ff4a---wy-window-y-position-rw-ff4b---wx-window-x-position--7-rw
                DrawTile(current_tile_data_addr, x, y);
            }
        }


    }

    bool IsFlagSet(LCDCBitmask flag)
    {
        return mem.Read(LCDC) & static_cast<uint8_t>(flag);
    }

    /// Get the address of a tiles data based on tile id and the current LCDC control bits
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
            // If LCDC.4 == 1 then tile ids are interpreted as signed with 0x9000 as base address.
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
                renderBuffer[dst_y*WIDTH + dst_x] = color;
            }
        }
    }

};