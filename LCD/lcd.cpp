
#include <cstring>
#include "../HostMemory.h"
#include "lcd.h"

void LCD::DrawScanline()
{

    if(IsFlagSet(LCDCBitmask::BG_And_Window_Enable) )
    {
        DrawBackground();

        if(IsFlagSet(LCDCBitmask::Window_Enabled))
        {
            DrawWindow();
        }

    } else
    {
        uint8_t *dst_ptr = &renderBuffer[current_scanline * BUFFER_WIDTH];
        memset(dst_ptr,0,BUFFER_WIDTH);
    }

    // Render objects

    // Sprite attributes reside in the Sprite Attribute Table (OAM - Object Attribute Memory) at $FE00-FE9F. Each of the 40 entries consists of four bytes with the following meanings:

    // TODO: finish sprite drawing

    if(IsFlagSet(LCDCBitmask::OBJ_Enable))
    {
        OAM_Sprite *spr = reinterpret_cast<OAM_Sprite *>(&mem.memory[OAM_Address]);
        uint8_t sprites_drawn_on_scanline = 0;

        for(int sprIndex = 0 ; sprIndex < 40; sprIndex++) {
            if(spr->x_position == 0 || spr->y_position == 0 ) {
                continue;
            }

            // Only draw 10 sprites per scanline
            if(sprites_drawn_on_scanline == 10){
                break;
            }

            if(IsFlagSet(LCDCBitmask::OBJ_Size))
            {
                // 8x16 sprites
                sprites_drawn_on_scanline++;
            } else
            {
                // 8x8 sprites
                bool sprite_is_visible_on_current_scanline = spr->y_position <= current_scanline && spr->y_position+8 > current_scanline;

                if(sprite_is_visible_on_current_scanline){

                }
                uint8_t *spr_tile_data = reinterpret_cast<uint8_t *>(mem.memory[Tile_Data_Block_0]);
                spr_tile_data += spr->tile_index << 4; // 16 = size of sprite struct
                sprites_drawn_on_scanline++;
            }

        }
    }

    RenderRGBBuffer(current_scanline);
}

void LCD::DrawBackground()
{
    //uint8_t scroll_x = mem.memory[(int)IOAddress::Scroll_X];
    uint8_t scroll_y = mem.memory[(int)IOAddress::Scroll_Y];
    uint8_t line_buffer[256];
    uint8_t line_x = mem.memory[(int)IOAddress::Scroll_X];
    //uint8_t tile_pixel_scroll_x = scroll_x & 0b111; // how many pixels to scroll the line
    //uint8_t tile_index_scroll_x = scroll_x >> 3; // How many tile map tiles to scroll

    uint16_t bg_tile_map_addr = IsFlagSet(LCDCBitmask::BG_Tile_Map_Area) ? Tile_Map_Block_1 : Tile_Map_Block_0;
    uint8_t tilemap_y_index = (current_scanline + scroll_y) >> 3;
    uint8_t tile_y_line = (current_scanline + scroll_y) & 0b111;
    uint8_t *dst_ptr = &renderBuffer[current_scanline * BUFFER_WIDTH];

    for( uint8_t tilemap_x = 0 ; tilemap_x < 32 ; tilemap_x++)
    {
        uint16_t index = (tilemap_y_index * Tile_Map_Width) + tilemap_x;
        uint8_t tile_id = mem.memory[bg_tile_map_addr + index];
        // Find the tile address, and add offset to the tile line we want to draw
        uint16_t tile_data_addr = GetTileDataAddr(tile_id) + (tile_y_line<<1);

        uint8_t lobits = mem.memory[tile_data_addr++];
        uint8_t hibits = mem.memory[tile_data_addr++];
        for (int x = 0; x < TILE_WIDTH; x++) {
            // TODO: Unroll this X loop. figure out way to do the planar to indexed thing faster.
            // TODO: maybe just cache these in indexed mode instead? Why transform them every time we draw them?
            uint8_t bit_number = 7-x;
            uint8_t color = ( (lobits >> bit_number) & 1 ) |  ( ((hibits >> bit_number) & 1)  << 1 );
            line_buffer[line_x++] = color;
            //*dst_ptr = color;
            //dst_ptr++;
        }

    }

    memcpy(dst_ptr,line_buffer,BUFFER_WIDTH);

}
void LCD::DrawWindow()
{
        uint8_t wx = mem.Read(IOAddress::Window_X_Position);
        uint8_t wy = mem.Read(IOAddress::Window_Y_Position);

        if(current_scanline >= wy ){

        }
        // The Window is visible (if enabled) when both coordinates are in the ranges WX=0..166, WY=0..143
        bool window_is_visible = wx < 167 && wy < 144;

        if(current_scanline >= wy && window_is_visible ){

            wx-=7;

            if(wx<7){
                // TODO: offset 7-wx into the window sprite
            }

            uint16_t window_tile_map_addr = IsFlagSet(LCDCBitmask::Window_Tile_Map_Area) ? Tile_Map_Block_1 : Tile_Map_Block_0;
            uint8_t *window_dst_ptr = &renderBuffer[current_scanline * BUFFER_WIDTH + wx];
            uint8_t window_line_to_draw = window_internal_line_counter;
            uint8_t line_in_window_tiles_to_draw = window_line_to_draw & 0b111;
            uint8_t window_tilemap_line_to_draw = (window_line_to_draw >> 3);
            // Offset into the window tilemap to find the line we want to draw
            window_tile_map_addr += window_tilemap_line_to_draw*Tile_Map_Width;

            for(int window_x_tile_index = 0 ; wx < 160 ; window_x_tile_index++ ){

                uint8_t tile_id = mem.memory[window_tile_map_addr + window_x_tile_index];
                // Find the tile address, and add offset to the tile line we want to draw
                uint16_t tile_data_addr = GetTileDataAddr(tile_id) + line_in_window_tiles_to_draw*2; // *2 because each line is 2 bytes

                uint8_t lobits = mem.memory[tile_data_addr++];
                uint8_t hibits = mem.memory[tile_data_addr++];
                for (int x = 0; x < TILE_WIDTH; x++) {
                    // TODO: Unroll this X loop. figure out way to do the planar to indexed thing faster.
                    // TODO: maybe just cache these in indexed mode instead? Why transform them every time we draw them?
                    uint8_t bit_number = 7-x;
                    uint8_t color = ( (lobits >> bit_number) & 1 ) |  ( ((hibits >> bit_number) & 1)  << 1 );
                    *window_dst_ptr = color;
                    window_dst_ptr++;

                }
                wx+=8;


            }

            window_internal_line_counter++;

        }
}

void LCD::Step( uint16_t delta_cycles )
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
            window_internal_line_counter = 0;
        }

        draw_scanline = true;
    }

    mem[LCD_Y_Register] = current_scanline;

    accumulated_cycles += delta_cycles;

    // If enough cycles has passed, switch to next mode..

    if( accumulated_cycles >= LCD_Mode_Cycles[LCD_Mode_Order[current_mode_index]] )
    {
        accumulated_cycles -= LCD_Mode_Cycles[LCD_Mode_Order[current_mode_index]];
        current_mode_index++;

        if( current_scanline < 144 && LCD_Mode_Order[current_mode_index] == LCD_Mode_VBlank){
            current_mode_index = 0;
        }

        if(current_mode_index == 4) { // VBlank is done, restart mode order
            current_mode_index = 0;
        }
        
        if(LCD_Mode_Order[current_mode_index] == LCD_Mode_Searching_OAM){
            if(mem[LCD_Stat_Register] & LCD_Stat_IRQ_From_OAM){
                mem.SetInterruptFlag(Interrupt_Flag_LCD_Stat,true);
            }
        }

        if(LCD_Mode_Order[current_mode_index] == LCD_Mode_Reading_OAM){
            DrawScanline();
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

}

void LCD::RenderRGBBuffer( uint8_t line_number )
{
    constexpr RGB paletteColors[4] = { {20,50,00} , {20,128,20}, {50,192,50}, {100,255,100} };
    RGB rgb[4] = {};

    uint8_t paletteMapping = mem.memory[static_cast<uint16_t>(IOAddress::BG_Palette_Data)];

    // Map colors according to the BG palette register
    rgb[0] = paletteColors[paletteMapping & 0b11];
    rgb[1] = paletteColors[(paletteMapping>>2) & 0b11];
    rgb[2] = paletteColors[(paletteMapping>>4) & 0b11];
    rgb[3] = paletteColors[(paletteMapping>>6) & 0b11];

    uint16_t offset = line_number * BUFFER_WIDTH;
    uint32_t *dst_ptr = reinterpret_cast<uint32_t *>(&rgbBuffer[offset]);
    for(int i = offset ; i < offset+BUFFER_WIDTH; i++) {
        uint8_t colorIndex = renderBuffer[i];
        *dst_ptr++ = rgb[colorIndex].RGBA;
    }
}

/// Get the address of a tiles data based on tile id and the current LCD_Control_Register control bits
/// \param tile_id for which to get data
/// \return Address in GB memory
uint16_t LCD::GetTileDataAddr(uint8_t tile_id)
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
void LCD::DrawTile( uint16_t tile_data_addr, uint8_t dst_x, uint8_t dst_y ) {

    uint8_t *dst_ptr = &renderBuffer[(dst_y) * BUFFER_WIDTH + (dst_x)];

    for (int y = 0; y < TILE_HEIGHT; y++) {
        uint8_t lobits = mem.memory[tile_data_addr++]; //mem.Read(tile_data_addr++);
        uint8_t hibits = mem.memory[tile_data_addr++];
        for (int x = 0; x < TILE_WIDTH; x++) {
            // TODO: Unroll this X loop. figure out way to do the planar to indexed thing faster.
            // TODO: maybe just cache these in indexed mode instead? Why transform them every time we draw them?
            uint8_t bit_number = 7-x;
            uint8_t color = ( (lobits >> bit_number) & 1 ) |  ( ((hibits >> bit_number) & 1)  << 1 );
            *dst_ptr = color;
            dst_ptr++;
            //renderBuffer[(dst_y+y) * BUFFER_WIDTH + (dst_x+x)] = color;
        }

        dst_ptr += BUFFER_WIDTH-TILE_WIDTH;

    }
}