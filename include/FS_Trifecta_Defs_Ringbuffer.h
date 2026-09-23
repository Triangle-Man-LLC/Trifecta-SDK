#ifndef TRIFECTA_DEFS_RINGBUFFER_H
#define TRIFECTA_DEFS_RINGBUFFER_H

#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
#include <atomic>
typedef std::atomic<uint16_t> trifecta_atomic_u16;
#else
#include <stdatomic.h>
typedef _Atomic uint16_t trifecta_atomic_u16;
#endif

/// @brief Ringbuffer generator.
#define FS_RINGBUFFER_DECLARE(type, name, size) \
    typedef struct                              \
    {                                           \
        type buffer[size];                      \
        trifecta_atomic_u16 head;               \
        trifecta_atomic_u16 tail;               \
        trifecta_atomic_u16 count;              \
    } name

/// @section Packet ring buffer functions

#if defined(__cplusplus)

// -------------------------
// C++ IMPLEMENTATION
// -------------------------

#define FS_RINGBUFFER_INIT(rb) \
    do                         \
    {                          \
        (rb)->head.store(0);   \
        (rb)->tail.store(0);   \
        (rb)->count.store(0);  \
    } while (0)

#define FS_RINGBUFFER_PUSH(rb, buffer_size, value_ptr)                               \
    ({                                                                               \
        bool success = false;                                                        \
        uint16_t old_count = (rb)->count.load(std::memory_order_acquire);            \
        if (old_count < (buffer_size))                                               \
        {                                                                            \
            uint16_t head = (rb)->head.load(std::memory_order_relaxed);              \
            (rb)->buffer[head] = *(value_ptr);                                       \
            (rb)->head.store((head + 1) % (buffer_size), std::memory_order_release); \
            (rb)->count.fetch_add(1, std::memory_order_release);                     \
            success = true;                                                          \
        }                                                                            \
        success;                                                                     \
    })

#define FS_RINGBUFFER_PUSH_FORCE(rb, buffer_size, value_ptr)                         \
    ({                                                                               \
        uint16_t head = (rb)->head.load(std::memory_order_relaxed);                  \
        (rb)->buffer[head] = *(value_ptr);                                           \
        (rb)->head.store((head + 1) % (buffer_size), std::memory_order_release);     \
                                                                                     \
        uint16_t old_count = (rb)->count.load(std::memory_order_acquire);            \
        if (old_count < (buffer_size))                                               \
        {                                                                            \
            (rb)->count.fetch_add(1, std::memory_order_release);                     \
        }                                                                            \
        else                                                                         \
        {                                                                            \
            uint16_t tail = (rb)->tail.load(std::memory_order_relaxed);              \
            (rb)->tail.store((tail + 1) % (buffer_size), std::memory_order_release); \
        }                                                                            \
        true;                                                                        \
    })

#define FS_RINGBUFFER_POP(rb, buffer_size, out_ptr)                                  \
    ({                                                                               \
        bool success = false;                                                        \
        uint16_t old_count = (rb)->count.load(std::memory_order_acquire);            \
        if (old_count > 0)                                                           \
        {                                                                            \
            uint16_t tail = (rb)->tail.load(std::memory_order_relaxed);              \
            *(out_ptr) = (rb)->buffer[tail];                                         \
            (rb)->tail.store((tail + 1) % (buffer_size), std::memory_order_release); \
            (rb)->count.fetch_sub(1, std::memory_order_release);                     \
            success = true;                                                          \
        }                                                                            \
        success;                                                                     \
    })

#define FS_RINGBUFFER_PEEK(rb, out_ptr)                                   \
    ({                                                                    \
        bool success = false;                                             \
        uint16_t old_count = (rb)->count.load(std::memory_order_acquire); \
        if (old_count > 0)                                                \
        {                                                                 \
            uint16_t tail = (rb)->tail.load(std::memory_order_relaxed);   \
            *(out_ptr) = (rb)->buffer[tail];                              \
            success = true;                                               \
        }                                                                 \
        success;                                                          \
    })

#define FS_RINGBUFFER_PEEK_AT(rb, buffer_size, index, out_ptr)            \
    ({                                                                    \
        bool success = false;                                             \
        uint16_t old_count = (rb)->count.load(std::memory_order_acquire); \
        if ((index) < old_count)                                          \
        {                                                                 \
            uint16_t tail = (rb)->tail.load(std::memory_order_relaxed);   \
            uint16_t pos = (tail + (index)) % (buffer_size);              \
            *(out_ptr) = (rb)->buffer[pos];                               \
            success = true;                                               \
        }                                                                 \
        success;                                                          \
    })

#else

// -------------------------
// PURE C IMPLEMENTATION
// -------------------------

#define FS_RINGBUFFER_INIT(rb)         \
    do                                 \
    {                                  \
        atomic_store(&(rb)->head, 0);  \
        atomic_store(&(rb)->tail, 0);  \
        atomic_store(&(rb)->count, 0); \
    } while (0)

#define FS_RINGBUFFER_PUSH(rb, buffer_size, value_ptr)             \
    ({                                                             \
        bool success = false;                                      \
        uint16_t old_count = atomic_load(&(rb)->count);            \
        if (old_count < (buffer_size))                             \
        {                                                          \
            uint16_t head = atomic_load(&(rb)->head);              \
            (rb)->buffer[head] = *(value_ptr);                     \
            atomic_store(&(rb)->head, (head + 1) % (buffer_size)); \
            atomic_fetch_add(&(rb)->count, 1);                     \
            success = true;                                        \
        }                                                          \
        success;                                                   \
    })

#define FS_RINGBUFFER_PUSH_FORCE(rb, buffer_size, value_ptr)       \
    ({                                                             \
        uint16_t head = atomic_load(&(rb)->head);                  \
        (rb)->buffer[head] = *(value_ptr);                         \
        atomic_store(&(rb)->head, (head + 1) % (buffer_size));     \
                                                                   \
        uint16_t old_count = atomic_load(&(rb)->count);            \
        if (old_count < (buffer_size))                             \
        {                                                          \
            atomic_fetch_add(&(rb)->count, 1);                     \
        }                                                          \
        else                                                       \
        {                                                          \
            uint16_t tail = atomic_load(&(rb)->tail);              \
            atomic_store(&(rb)->tail, (tail + 1) % (buffer_size)); \
        }                                                          \
        true;                                                      \
    })

#define FS_RINGBUFFER_POP(rb, buffer_size, out_ptr)                \
    ({                                                             \
        bool success = false;                                      \
        uint16_t old_count = atomic_load(&(rb)->count);            \
        if (old_count > 0)                                         \
        {                                                          \
            uint16_t tail = atomic_load(&(rb)->tail);              \
            *(out_ptr) = (rb)->buffer[tail];                       \
            atomic_store(&(rb)->tail, (tail + 1) % (buffer_size)); \
            atomic_fetch_sub(&(rb)->count, 1);                     \
            success = true;                                        \
        }                                                          \
        success;                                                   \
    })

#define FS_RINGBUFFER_PEEK(rb, out_ptr)                 \
    ({                                                  \
        bool success = false;                           \
        uint16_t old_count = atomic_load(&(rb)->count); \
        if (old_count > 0)                              \
        {                                               \
            uint16_t tail = atomic_load(&(rb)->tail);   \
            *(out_ptr) = (rb)->buffer[tail];            \
            success = true;                             \
        }                                               \
        success;                                        \
    })

#define FS_RINGBUFFER_PEEK_AT(rb, buffer_size, index, out_ptr) \
    ({                                                         \
        bool success = false;                                  \
        uint16_t old_count = atomic_load(&(rb)->count);        \
        if ((index) < old_count)                               \
        {                                                      \
            uint16_t tail = atomic_load(&(rb)->tail);          \
            uint16_t pos = (tail + (index)) % (buffer_size);   \
            *(out_ptr) = (rb)->buffer[pos];                    \
            success = true;                                    \
        }                                                      \
        success;                                               \
    })

#endif // C++ / C mode

#endif // TRIFECTA_DEFS_RINGBUFFER_H
