#include "Fetcher.h"
// https://github.com/trekawek/coffee-gb/blob/master/src/main/java/eu/rekawek/coffeegb/gpu/DmgPixelFifo.java
// https://blog.tigris.fr/2019/09/15/writing-an-emulator-the-first-pixel/
// Ultimate Gameboy Talk (PPU part)
// https://youtu.be/HyzD8pNlpwI?t=2691

// FIFO (4MHz)
// - pushes one pixel per clock
// - pauses unless it contains more than 8 pixels

// Fetcher (2MHz)
// 3 clocks to fetch 8 pixels
// pauses in 4th clock unless space in FIFO

Fetcher::Fetcher( HostMemory& mem) : mem(mem) {

    NewLine();

};

void Fetcher::NewLine()
{
    m_tilemap_x_index = 0;
    // uint8_t scroll_x = 0 - mem.memory[(int)IOAddress::Scroll_X];
    // uint8_t tile_pixel_scroll_x = scroll_x & 0b111; // how many pixels to scroll the line
    // uint8_t tile_index_scroll_x = scroll_x >> 3; // How many tile map tiles to scroll
}

void Fetcher::Tick(FIFO& fifo){

    static int delay = 1;

    // Only tick at 2MHz (half the rate of the main CPU clock)
    if(--delay == 0){
        delay = 2;
    }

    switch(m_fetcherState) {
        case FetcherState::ReadTileNumber: {
            // Read tile # (1 tick)
            uint8_t scroll_y = mem.memory[(int)IOAddress::Scroll_Y];
            uint8_t line_buffer[256];
            uint8_t line_x = 0 - mem.memory[(int)IOAddress::Scroll_X];
            //uint8_t tile_pixel_scroll_x = scroll_x & 0b111; // how many pixels to scroll the line
            //uint8_t tile_index_scroll_x = scroll_x >> 3; // How many tile map tiles to scroll

            uint16_t bg_tile_map_addr = IsFlagSet(LCDCBitmask::BG_Tile_Map_Area) ? Tile_Map_Block_1 : Tile_Map_Block_0;
            uint8_t tilemap_y_index = (m_currentScanline + scroll_y) >> 3;
            uint8_t tile_y_line = (m_currentScanline + scroll_y) & 0b111;
            uint16_t map_index = (tilemap_y_index * Tile_Map_Width) + m_tilemap_x_index;
            uint8_t tile_id = mem.memory[bg_tile_map_addr + map_index];
            // Find the tile address, and add offset to the tile line we want to draw
            m_tile_data_addr = GetTileDataAddr(tile_id) + (tile_y_line<<1);
            m_fetcherState = FetcherState::ReadData0;
        } break;


        case FetcherState::ReadData0: {
            // todo: read data 0  (1 tick)
            m_lobits = mem.memory[m_tile_data_addr++];
            m_fetcherState = FetcherState::ReadData1;
        } break;

        case FetcherState::ReadData1: {
            // todo: read data 1 (1 tick)
            m_hibits = mem.memory[m_tile_data_addr];
            m_fetcherState = FetcherState::PushToFIFO;
        } break;

        case FetcherState::PushToFIFO: {

            // ScrollX:
            // Just discard the amount of pixels that are scrolled


            for (int x = 0; x < TILE_WIDTH; x++) {
                // TODO: Unroll this X loop. figure out way to do the planar to indexed thing faster.
                // TODO: maybe just cache these in indexed mode instead? Why transform them every time we draw them?
                uint8_t bit_number = 7-x;
                uint8_t color = ( (m_lobits >> bit_number) & 1 ) |  ( ((m_hibits >> bit_number) & 1)  << 1 );
                // line_buffer[line_x++] = color;
                // *dst_ptr = color;
                // dst_ptr++;
            }

            // 
            // todo: push to FIFO if space (otherwise wait)
            // if(m_fifo->HasSpaceFor8Pixels()){
            //     for(int i = 0 ; i < 8 ; i-- ){
            //         m_fifo->push( ... );
            //     }
            // when we have pushed 32 tiles we have filled an entire LCD screen width, so restart on a new line
                    // if(m_tilemap_x_index == 32){
                    //     m_fetcherState = FetcherState::ReadTileNumber;
                    //     NewLine();
                    // }
            // }

            
        } break;
    }

}  

/// Get the address of a tiles data based on tile id and the current LCD_Control_Register control bits
/// \param tile_id for which to get data
/// \return Address in GB memory
uint16_t Fetcher::GetTileDataAddr(uint8_t tile_id)
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

/*
void Fetcher::DrawBackground()
{
    //uint8_t scroll_x = mem.memory[(int)IOAddress::Scroll_X];
    uint8_t scroll_y = mem.memory[(int)IOAddress::Scroll_Y];
    uint8_t line_buffer[256];
    uint8_t line_x = 0 - mem.memory[(int)IOAddress::Scroll_X];
    //uint8_t tile_pixel_scroll_x = scroll_x & 0b111; // how many pixels to scroll the line
    //uint8_t tile_index_scroll_x = scroll_x >> 3; // How many tile map tiles to scroll

    uint16_t bg_tile_map_addr = IsFlagSet(LCDCBitmask::BG_Tile_Map_Area) ? Tile_Map_Block_1 : Tile_Map_Block_0;
    uint8_t tilemap_y_index = (m_currentScanline + scroll_y) >> 3;
    uint8_t tile_y_line = (m_currentScanline + scroll_y) & 0b111;
    uint8_t *dst_ptr = &renderBuffer[m_currentScanline * BUFFER_WIDTH];

    for( uint8_t tilemap_x = 0 ; tilemap_x < 32 ; tilemap_x++)
    {
        uint16_t index = (tilemap_y_index * Tile_Map_Width) + tilemap_x;
        uint8_t tile_id = mem.memory[bg_tile_map_addr + index];
        // Find the tile address, and add offset to the tile line we want to draw
        uint16_t tile_data_addr = GetTileDataAddr(tile_id) + (tile_y_line<<1);

        uint8_t lobits = mem.memory[tile_data_addr++];
        uint8_t hibits = mem.memory[tile_data_addr];
        for (int x = 0; x < TILE_WIDTH; x++) {
            // TODO: Unroll this X loop. figure out way to do the planar to indexed thing faster.
            // TODO: maybe just cache these in indexed mode instead? Why transform them every time we draw them?
            uint8_t bit_number = 7-x;
            uint8_t color = ( (lobits >> bit_number) & 1 ) |  ( ((hibits >> bit_number) & 1)  << 1 );
            line_buffer[line_x++] = color;
            *dst_ptr = color;
            dst_ptr++;
        }

    }

    memcpy(dst_ptr,line_buffer,BUFFER_WIDTH);



}

*/