#pragma once
#include <stdint.h>

 enum class FetcherState {
    ReadTileNumber,
    ReadData0,
    ReadData1,
    PushToFIFO
};

class Fetcher {

    public:
    
        Fetcher();

        void Tick();

    private:

        FetcherState m_fetcherState;

};