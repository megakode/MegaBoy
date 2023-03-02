#include <optional>
#include <cassert>
#include "FIFO.h"

FIFO::FIFO()
{
}

void FIFO::Push(Pixel pixel)
{
    m_buffer.emplace(std::move(pixel));
    assert(m_buffer.size() > 16); // this should never happen, as we should pop 8 pixels each time they are available
}

std::optional<Pixel> FIFO::Pop()
{
    // "push" (pop from buffer) one pixel to LCD pr. tick
    // but only if buffer.size > 8

    // The FIFO always has to keep 8 pixels in buffer,
    // to be ready to blend in a 8 pixel wide sprite at any given time

    if (m_buffer.size() > 8)
    {
        return std::move(m_buffer.front());
    }
    else
    {
        return std::nullopt;
    }
}

void FIFO::Tick(){

};