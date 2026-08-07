// AbstractRingBuffer.h
#pragma once

#include <vector>
#include <atomic>
#include <cstddef>
#include <cstdint>

class AbstractRingBuffer
{
public:
    explicit AbstractRingBuffer(size_t capacity)
        : m_capacity(capacity)
        , m_readIndex(0)
        , m_writeIndex(0)
        , m_buffer(std::make_unique<uint8_t[]>(capacity))
    {
    }

    ~AbstractRingBuffer() = default;

    // Single-Producer Single-Consumer write
    bool push(const uint8_t* data, size_t size)
    {
        size_t currentWrite = m_writeIndex.load(std::memory_order_acquire);
        size_t nextWrite = (currentWrite + size) % m_capacity;

        if (nextWrite == m_readIndex.load(std::memory_order_acquire))
            return false; // Full

        std::memcpy(&m_buffer[currentWrite], data, size);
        m_writeIndex.store(nextWrite, std::memory_order_release);
        return true;
    }

    // Single-Consumer read
    bool pop(uint8_t* data, size_t size, size_t& outSize)
    {
        size_t currentRead = m_readIndex.load(std::memory_order_acquire);
        size_t available = (m_writeIndex.load(std::memory_order_acquire) - currentRead + m_capacity) % m_capacity;

        if (available < size)
            return false; // Not enough data

        std::memcpy(data, &m_buffer[currentRead], size);
        m_readIndex.store((currentRead + size) % m_capacity, std::memory_order_release);
        outSize = size;
        return true;
    }

    size_t capacity() const { return m_capacity; }

private:
    size_t m_capacity;
    std::atomic<size_t> m_readIndex;
    std::atomic<size_t> m_writeIndex;
    std::unique_ptr<uint8_t[]> m_buffer;
};