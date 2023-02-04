#pragma once

#include <cstring>
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

enum class OAM_Sprite_Attributes : uint8_t {
    BG_And_Window_Over_OBJ = 1 << 7, // Bit7: BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
    Y_Flip = 1 << 6,                 // Bit6: Y flip          (0=Normal, 1=Vertically mirrored)
    X_Flip = 1 << 5,                 // Bit5: X flip          (0=Normal, 1=Horizontally mirrored)
    Palette_Number = 1 << 4,         // Bit4: Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
    CGB_Tile_VRAM_Bank = 1 << 3,     // Bit3: Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
    CGB_PAlette_Number = 0b111       // Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
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
    static constexpr int NUMBER_OF_SPRITES = 40;
    static constexpr int Tile_Map_Width = 32;

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

    explicit LCD( HostMemory& mem ) : mem(mem), current_scanline(mem[LCD_Y_Register])
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