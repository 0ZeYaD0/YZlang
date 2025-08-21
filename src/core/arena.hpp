#pragma once
#include <cstddef>
#include <cstdlib>
#include <iostream>

class ArenaAlloc
{
public:
    inline explicit ArenaAlloc(size_t bytes)
        : m_size(bytes)
    {
        m_buff = static_cast<std::byte *>(malloc(m_size));
        m_offset = m_buff;
    }

    template <typename T>
    inline T *alloc()
    {
        if (m_offset + sizeof(T) > m_buff + m_size)
        {
            std::cerr << "Arena allocator out of memory!" << std::endl;
            exit(EXIT_FAILURE);
        }
        void *offset = m_offset;
        m_offset += sizeof(T);
        return static_cast<T *>(offset);
    }

    inline ArenaAlloc(const ArenaAlloc &other) = delete;

    inline ArenaAlloc operator=(const ArenaAlloc &other) = delete;

    inline ~ArenaAlloc()
    {
        free(m_buff);
    }

private:
    size_t m_size;
    std::byte *m_buff;
    std::byte *m_offset;
};