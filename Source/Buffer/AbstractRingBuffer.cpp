// AbstractRingBuffer.cpp
#include "AbstractRingBuffer.h"
#include <cstring>

size_t AbstractRingBuffer::size() const
{
    size_t write = m_writeIndex.load(std::memory_order_acquire);
    size_t read = m_readIndex.load(std::memory_order_acquire);
    return (write >= read) ? (write - read) : (m_capacity - read + write);
}

bool AbstractRingBuffer::isEmpty() const
{
    return m_writeIndex.load(std::memory_order_acquire) == m_readIndex.load(std::memory_order_acquire);
}

bool AbstractRingBuffer::isFull() const
{
    size_t write = m_writeIndex.load(std::memory_order_acquire);
    size_t read = m_readIndex.load(std::memory_order_acquire);
    return (write + 1) % m_capacity == read;
}