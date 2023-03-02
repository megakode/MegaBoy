#pragma once
#include <stdint.h>
#include "FIFO.h"
#include "../HostMemory.h"
#include "lcd_regs.h"

 enum class FetcherState {
    ReadTileNumber,
    ReadData0,
    ReadData1,
    PushToFIFO
};

class Fetcher {

    public:
    
        Fetcher(HostMemory& mem);
        Fetcher(Fetcher &&) = delete;

        void Tick(FIFO& fifo);

    private:

        static constexpr uint16_t Tile_Map_Block_0 = 0x9800;
        static constexpr uint16_t Tile_Map_Block_1 = 0x9c00;
        static constexpr uint16_t Tile_Data_Block_0 = 0x8000;
        static constexpr uint16_t Tile_Data_Block_1 = 0x8800;
        static constexpr uint16_t Tile_Data_Block_2 = 0x9000;

        static constexpr uint8_t TILE_WIDTH = 8;
        static constexpr uint8_t TILE_HEIGHT = 8;


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

        void DrawBackground();
        void NewLine();
        uint16_t GetTileDataAddr(uint8_t tile_id);

        FetcherState m_fetcherState;

        HostMemory& mem;

        // Authority on the current scanline being rendered
        int m_currentScanline;

        // The id of the current tile, read from the tile map in the ReadTileNumber state, 
        // and used as a basis for reading tile data in the ReadData0 and ReadData1 states.
        uint8_t tile_id;
  
        // The current X index of the background map tile being fetched (0..31)
        uint8_t m_tilemap_x_index; 

        // The current bg tiles data address
        uint16_t m_tile_data_addr;

        // Data for the bg tile currently being rendered
        uint8_t m_lobits;
        // Data for the bg tile currently being rendered
        uint8_t m_hibits;

};