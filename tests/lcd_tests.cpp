//
// Created by sbeam on 12/25/21.
//

#include <catch2/catch.hpp>
#include <fstream>

#include "../CPU/cpu.h"
#include "../LCD/lcd.h"


TEST_CASE("LCD_Control_Register flags")
{
    HostMemory mem;
    LCD lcd(mem);
    mem.Write(LCD::LCD_Control_Register, 0);

    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::LCD_enabled));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::Window_Tile_Map_Area));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::BG_Tile_Map_Area));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::BG_And_Window_Enable));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::BG_And_Window_Tile_Data_Area));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::OBJ_Enable));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::OBJ_Size));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::Window_Enabled));

    lcd.SetFlag(LCDCBitmask::LCD_enabled, true);
    REQUIRE(lcd.IsFlagSet(LCDCBitmask::LCD_enabled));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::Window_Tile_Map_Area));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::BG_Tile_Map_Area));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::BG_And_Window_Enable));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::BG_And_Window_Tile_Data_Area));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::OBJ_Enable));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::OBJ_Size));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::Window_Enabled));

    mem.Write(LCD::LCD_Control_Register, 0);
    lcd.SetFlag(LCDCBitmask::Window_Enabled, true);
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::LCD_enabled));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::Window_Tile_Map_Area));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::BG_Tile_Map_Area));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::BG_And_Window_Enable));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::BG_And_Window_Tile_Data_Area));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::OBJ_Enable));
    REQUIRE(!lcd.IsFlagSet(LCDCBitmask::OBJ_Size));
    REQUIRE(lcd.IsFlagSet(LCDCBitmask::Window_Enabled));
}

TEST_CASE("GetTileDataAddr")
{
    HostMemory mem;
    LCD lcd(mem);

    lcd.SetFlag(LCDCBitmask::BG_And_Window_Tile_Data_Area,true); // 0=8800-97FF (signed), 1=8000-8FFF (unsigned)
    REQUIRE( lcd.GetTileDataAddr(0) == 0x8000 );
    REQUIRE( lcd.GetTileDataAddr(1) == 0x8000+16 );

    lcd.SetFlag(LCDCBitmask::BG_And_Window_Tile_Data_Area,false); // 0=8800-97FF (signed), 1=8000-8FFF (unsigned)
    REQUIRE( lcd.GetTileDataAddr(0) == 0x9000 );
    REQUIRE( lcd.GetTileDataAddr(1) == 0x9000+16 );
    REQUIRE( lcd.GetTileDataAddr(static_cast<uint8_t>(-1)) == 0x9000-16 );
}

TEST_CASE("DrawTile")
{
    HostMemory mem;
    LCD lcd(mem);

    lcd.SetFlag(LCDCBitmask::BG_And_Window_Tile_Data_Area,true); // 0=8800-97FF (signed), 1=8000-8FFF (unsigned)

    // Mock some tile data with 4 lines in each of the available 2 bit colors...

    // Line 0 - color 0
    mem.Write(0x8000,0x00); // LSB
    mem.Write(0x8001,0x00); // MSB
    // Line 1- color 1
    mem.Write(0x8002,0xff); // LSB
    mem.Write(0x8003,0x00); // MSB
    // Line 2- color 2
    mem.Write(0x8004,0x00); // LSB
    mem.Write(0x8005,0xff); // MSB
    // Line 3- color 3
    mem.Write(0x8006,0xff); // LSB
    mem.Write(0x8007,0xff); // MSB

    // ...Draw the tile in the render buffer...

    lcd.DrawTile(0x8000,4,0);

    // ...And check that it is actually draw and in the right location

    REQUIRE(lcd.renderBuffer[LCD::BUFFER_WIDTH*0 + 4] == 0);
    REQUIRE(lcd.renderBuffer[LCD::BUFFER_WIDTH*1 + 4] == 1);
    REQUIRE(lcd.renderBuffer[LCD::BUFFER_WIDTH*2 + 4] == 2);
    REQUIRE(lcd.renderBuffer[LCD::BUFFER_WIDTH*3 + 4] == 3);

}