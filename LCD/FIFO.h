#pragma once
#include <stdint.h>
#include <queue>

// The palette that the FIFO pixel uses
enum PaletteType
{
    Sprite0,
    Sprite1,
    Background
};

struct Pixel
{
    PaletteType paletteType;
    uint8_t value;
};

class FIFO
{

public:
    FIFO();

    void Push(Pixel pixel);

    std::optional<Pixel> Pop();

    void HasSpaceFor8Pixels();

    void Tick();

    // Indicates whether the FIFO has more than 8 pixels in it buffer, so we can take the 8 of them out.
    // bool HasPixelsAvailable();

private:
    std::queue<Pixel> m_buffer;
};