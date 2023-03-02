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

enum class OAM_Sprite_Attributes : uint8_t {
    BG_And_Window_Over_OBJ = 1 << 7, // Bit7: BG and Window over OBJ (0=No, 1=BG and Window colors 1-3 over the OBJ)
    Y_Flip = 1 << 6,                 // Bit6: Y flip          (0=Normal, 1=Vertically mirrored)
    X_Flip = 1 << 5,                 // Bit5: X flip          (0=Normal, 1=Horizontally mirrored)
    Palette_Number = 1 << 4,         // Bit4: Palette number  **Non CGB Mode Only** (0=OBP0, 1=OBP1)
    CGB_Tile_VRAM_Bank = 1 << 3,     // Bit3: Tile VRAM-Bank  **CGB Mode Only**     (0=Bank 0, 1=Bank 1)
    CGB_PAlette_Number = 0b111       // Bit2-0 Palette number  **CGB Mode Only**     (OBP0-7)
};


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
