/// Driver for the Trifecta series of IMU/AHRS/INS devices
/// Copyright 2026 4rge.ai and/or Triangle Man LLC
/// Usage and redistribution of this code is permitted
/// but this notice must be retained in all copies of the code.

/// THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
/// AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#ifndef TRIFECTA_DEFS_RINGBUFFER_H
#define TRIFECTA_DEFS_RINGBUFFER_H

#include <stdatomic.h>

/// @brief Ringbuffer generator.
#define FS_RINGBUFFER_DECLARE(type, name, size) \
    typedef struct                              \
    {                                           \
        type buffer[size];                      \
        _Atomic uint16_t head;                  \
        _Atomic uint16_t tail;                  \
        _Atomic uint16_t count;                 \
    } name

/// @section Packet ring buffer functions (implemented using MACROS to allow use with all fs_ringbuffers)
#define FS_RINGBUFFER_INIT(rb) \
    do                         \
    {                          \
        (rb)->head = 0;        \
        (rb)->tail = 0;        \
        (rb)->count = 0;       \
    } while (0)

/// @brief Ringbuffer push
/// @param rb Ringbuffer handle
/// @param buffer_size Ringbuffer size
/// @param value_ptr Pointer to the thing to enqueue
/// @return TRUE on success, FALSE if failed (out of space)
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

/// @brief Ringbuffer push, but overwrite oldest element if full
/// @param rb Ringbuffer handle
/// @param buffer_size Ringbuffer size
/// @param value_ptr Pointer to the thing to enqueue
/// @return TRUE on success (always)
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

/// @brief Ringbuffer pop
/// @param rb Ringbuffer handle
/// @param buffer_size Ringbuffer size
/// @param value_ptr Pointer to the thing to dequeue
/// @return TRUE on success, FALSE if failed (no items)
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

/// @brief Ringbuffer peek
/// @param rb Ringbuffer handle
/// @param out_ptr Pointer to the thing to enqueue
/// @return TRUE on success, FALSE if failed (no items)
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

/// @brief Ringbuffer peek, but at an indicated index
/// @param rb Ringbuffer handle
/// @param out_ptr Pointer to the thing to enqueue
/// @return TRUE on success, FALSE if failed (no items)
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

#endif