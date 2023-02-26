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

Fetcher::Fetcher() {

};

void Fetcher::Tick(){

    static int delay = 1;

    // Only tick at 2MHz (half the rate of the main CPU clock)
    if(--delay == 0){
        delay = 2;
    }

    switch(m_fetcherState) {
        case FetcherState::ReadTileNumber:
            // todo: read tile # (1 tick)
            m_fetcherState = FetcherState::ReadData0;
            break;

        case FetcherState::ReadData0:
            // todo: read data 0  (1 tick)
            m_fetcherState = FetcherState::ReadData1;
            break;

        case FetcherState::ReadData1:
            // todo: read data 1 (1 tick)
            m_fetcherState = FetcherState::PushToFIFO;
            break;

        case FetcherState::PushToFIFO:
            // todo: push to FIFO if space (otherwise wait)
            // if(m_fifo->HasSpaceFor8Pixels()){
            //     for(int i = 0 ; i < 8 ; i-- ){
            //         m_fifo->push( ... );
            //     }
            //     m_fetcherState = FetcherState::ReadTileNumber;
            // }
            break;
    }

}