/// Driver for the Trifecta series of IMU/AHRS/INS devices
/// Copyright 2026 4rge.ai and/or Triangle Man LLC
/// Usage and redistribution of this code is permitted
/// but this notice must be retained in all copies of the code.
///
/// THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
/// AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "FS_Trifecta_Interfaces.h"

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "cmsis_os2.h"

int fs_logging_level = 0; // 0 = OFF, 1 = ON

/// @brief Platform-specific start thread given a function handle.
/// @param thread_func Pointer to the thread function handle.
/// @param params Parameters to pass to the thread function.
/// @param thread_running_flag Pointer to the flag used to indicate thread status.
/// @param thread_handle Pointer to the task handle.
/// @param stack_size Size of the stack allocated for the thread (in words or bytes depending on config).
/// @param priority Priority level of the thread.
/// @param core_affinity -1 for indifference, else preferred core number (ignored on STM32/CMSIS-RTOS2).
/// @return 0 on success, -1 on failure.
int fs_thread_start(void (*thread_func)(void *),
                    void *params,
                    fs_run_status_t *thread_running_flag,
                    fs_thread_t *thread_handle,
                    size_t stack_size,
                    int priority,
                    int core_affinity)
{
    (void)core_affinity; // CMSIS-RTOS2 on STM32 has no core affinity

    if (thread_func == NULL || thread_running_flag == NULL)
    {
        fs_log_critical("[Trifecta] Error: Invalid thread function or running flag!\n");
        return -1;
    }

    if (thread_handle == NULL)
    {
        fs_log_output("[Trifecta] Warning: No task handle provided! This could interrupt certain functions.\n");
    }

    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.name = "TrifectaThread";
    attr.priority = (osPriority_t)priority;
    attr.stack_size = stack_size;

    osThreadId_t tid = osThreadNew((osThreadFunc_t)thread_func, params, &attr);
    if (tid == NULL)
    {
        fs_log_output("[Trifecta] Error: Thread creation failed!\n");
        *thread_running_flag = FS_RUN_STATUS_ERROR;
        return -1;
    }

    if (thread_handle)
    {
        thread_handle->handle = tid;
    }

    *thread_running_flag = FS_RUN_STATUS_RUNNING;
    return 0;
}

/// @brief Properly exit/delete a thread.
/// @param thread_handle Pointer to the thread handle (optional).
/// @return 0 on success.
int fs_thread_exit(void *thread_handle)
{
    if (thread_handle == NULL)
    {
        osThreadExit();
        return 0;
    }

    osThreadId_t tid = *(osThreadId_t *)thread_handle;
    if (tid != NULL)
    {
        osThreadTerminate(tid);
    }
    return 0;
}

/// @brief Logs output with formatting.
/// @param format Format string.
/// @param ... Additional arguments.
/// @return Number of characters printed.
int fs_log_output(const char *format, ...)
{
    int chars_printed = 0;

    if (fs_logging_level <= 0)
    {
        return 0;
    }

    va_list args;
    va_start(args, format);

    chars_printed = vprintf(format, args);
    fflush(stdout);

    if (chars_printed > 0)
    {
        size_t len = strlen(format);
        if (len == 0 || format[len - 1] != '\n')
        {
            putchar('\n');
            chars_printed++;
        }
    }

    va_end(args);
    return chars_printed;
}

/// @brief Logs output regardless of logging level.
/// @param format Format string.
/// @param ... Additional arguments.
/// @return Number of characters printed.
int fs_log_critical(const char *format, ...)
{
    int chars_printed = 0;

    va_list args;
    va_start(args, format);

    chars_printed = vprintf(format, args);
    fflush(stdout);

    if (chars_printed > 0)
    {
        size_t len = strlen(format);
        if (len == 0 || format[len - 1] != '\n')
        {
            putchar('\n');
            chars_printed++;
        }
    }

    va_end(args);
    return chars_printed;
}

/// @brief Toggle logging.
/// @param do_log TRUE to turn log on, FALSE to turn log off.
/// @return 1 if logging turned on, 0 if logging turned off.
int fs_toggle_logging(bool do_log)
{
    fs_logging_level = do_log ? 1 : 0;
    return fs_logging_level;
}

/// @brief Delay by at least this amount of time.
/// @param millis Number of milliseconds to delay.
/// @return The number of ticks the delay lasted.
int fs_delay(int millis)
{
    uint32_t freq = osKernelGetTickFreq();    // ticks per second
    uint32_t ticks = (millis * freq) / 1000U; // ms → ticks

    osDelay(ticks);
    return (int)ticks;
}

/// @brief Real-time delay relative to a stored time.
/// @param current_time Pointer to the current time (tick count).
/// @param millis The exact amount of time to delay.
/// @return The number of ticks the delay lasted.
int fs_delay_for(uint64_t *current_time, int millis)
{
    if (!current_time)
        return -1;

    uint32_t freq = osKernelGetTickFreq();          // ticks per second
    uint32_t delay_ticks = (millis * freq) / 1000U; // ms → ticks

    if (*current_time == 0)
    {
        *current_time = (uint64_t)osKernelGetTickCount();
    }

    uint64_t initial_time = *current_time;
    uint32_t start_ticks = (uint32_t)*current_time;

    osDelayUntil(start_ticks + delay_ticks);

    *current_time = (uint64_t)osKernelGetTickCount();
    uint64_t elapsed_ticks = *current_time - initial_time;
    return (int)elapsed_ticks;
}

/// @brief Get the current system time (tick count).
/// @param current_time Pointer to the current time.
/// @return 0 on success.
int fs_get_current_time(uint64_t *current_time)
{
    if (!current_time)
        return -1;

    *current_time = (uint64_t)osKernelGetTickCount();
    return 0;
}

/// @brief Get local wall-clock time (if RTC/time is available).
/// @param out Pointer to fs_tm_t to fill.
/// @return 0 on success, -1 on failure.
int fs_get_local_time(fs_tm_t *out)
{
    if (!out)
        return -1;

    time_t t = time(NULL);
    if (t == (time_t)-1)
        return -1;

    struct tm tmv;
#if defined(_POSIX_THREAD_SAFE_FUNCTIONS) || defined(__USE_MISC)
    localtime_r(&t, &tmv);
#else
    const struct tm *ptm = localtime(&t);
    if (!ptm)
        return -1;
    tmv = *ptm;
#endif

    out->year = tmv.tm_year + 1900;
    out->month = tmv.tm_mon + 1;
    out->day = tmv.tm_mday;
    out->hour = tmv.tm_hour;
    out->min = tmv.tm_min;
    out->sec = tmv.tm_sec;

    return 0;
}
