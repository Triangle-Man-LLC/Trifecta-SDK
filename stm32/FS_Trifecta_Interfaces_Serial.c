/// Driver for the Trifecta series of IMU/AHRS/INS devices
/// Copyright 2026 4rge.ai and/or Triangle Man LLC
/// Usage and redistribution of this code is permitted
/// but this notice must be retained in all copies of the code.

/// THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
/// AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "FS_Trifecta_Interfaces.h"

#include <string.h>
#include <stdio.h>

#include "cmsis_os2.h"

#if defined(STM32F3)
#include "stm32f3xx_hal.h"
#elif defined(STM32F4)
#include "stm32f4xx_hal.h"
#elif defined(STM32F7)
#include "stm32f7xx_hal.h"
#elif defined(STM32H7)
#include "stm32h7xx_hal.h"
#elif defined(STM32G4)
#include "stm32g4xx_hal.h"
#elif defined(STM32G0)
#include "stm32g0xx_hal.h"
#elif defined(STM32L4)
#include "stm32l4xx_hal.h"
#else
#error "Unsupported STM32 family"
#endif

int fs_init_serial_driver(fs_device_info_t *device_handle)
{
    if (!device_handle)
        return -1;

    if (device_handle->device_params.serial_port < 0)
    {
        fs_log_output("[Trifecta] Serial port number cannot be less than zero!");
        return -1;
    }

    switch (device_handle->device_params.communication_mode)
    {
    case FS_COMMUNICATION_MODE_UART:
        return 0;

    case FS_COMMUNICATION_MODE_USB_CDC:
        fs_log_output("[Trifecta] USB CDC not yet implemented on STM32.");
        return -1;

    case FS_COMMUNICATION_MODE_I2C:
        return 0;

    case FS_COMMUNICATION_MODE_SPI:
        return 0;

    default:
        fs_log_critical("[Trifecta] Error: Unsupported communication mode!");
        return -1;
    }
}

int fs_platform_supported_serial_interrupts()
{
    return (FS_COMMUNICATION_MODE_UART | FS_COMMUNICATION_MODE_I2C);
}

static void fs_serial_data_ready_isr(void *args)
{
    fs_device_info_t *device_handle = (fs_device_info_t *)args;
    if (!device_handle)
        return;

    GPIO_PinState level = HAL_GPIO_ReadPin(device_handle->driver_config.serial_data_ready_port,
                                           device_handle->driver_config.serial_data_ready_pin);

    if (level == GPIO_PIN_SET)
    {
        device_handle->device_params.hp_timestamp = osKernelGetTickCount();
    }
    else
    {
        osThreadId_t tid = device_handle->background_task_handle.handle;
        if (tid)
            osThreadFlagsSet(tid, 0x01);
    }
}

int fs_init_serial_interrupts(fs_device_info_t *device_handle)
{
    if (!device_handle)
        return -1;

    HAL_NVIC_SetPriority(device_handle->driver_config.serial_data_ready_irq, 5, 0);
    HAL_NVIC_EnableIRQ(device_handle->driver_config.serial_data_ready_irq);

    device_handle->driver_config.serial_isr_callback = fs_serial_data_ready_isr;

    return 0;
}

int fs_wait_until_next_serial_interrupt(fs_device_info_t *device_handle)
{
    if (!device_handle || device_handle->background_task_handle.handle == NULL)
        return -1;

    uint32_t timeout_ms = device_handle->driver_config.task_wait_ms;
    uint32_t flags = osThreadFlagsWait(0x01, osFlagsWaitAny, timeout_ms);

    return (flags > 0) ? 0 : -1;
}

ssize_t fs_transmit_serial(fs_device_info_t *device_handle,
                           void *tx_buffer,
                           size_t length_bytes,
                           int timeout_micros)
{
    if (!device_handle || !tx_buffer)
        return -1;

    switch (device_handle->device_params.communication_mode)
    {
    case FS_COMMUNICATION_MODE_UART:
    {
        UART_HandleTypeDef *huart = (UART_HandleTypeDef *)device_handle->driver_config.uart_handle;
        if (!huart)
        {
            fs_log_critical("[Trifecta] UART handle is NULL!");
            return -1;
        }

        HAL_StatusTypeDef st = HAL_UART_Transmit(huart,
                                                 (uint8_t *)tx_buffer,
                                                 length_bytes,
                                                 timeout_micros / 1000);

        if (st != HAL_OK)
        {
            fs_log_critical("[Trifecta] UART transmit failed!");
            return -1;
        }

        return (ssize_t)length_bytes;
    }

    case FS_COMMUNICATION_MODE_USB_CDC:
        return -1;

    case FS_COMMUNICATION_MODE_I2C:
        return -1;

    case FS_COMMUNICATION_MODE_SPI:
        return -1;

    default:
        return -1;
    }
}

ssize_t fs_receive_serial(fs_device_info_t *device_handle,
                          void *rx_buffer,
                          size_t length_bytes,
                          int timeout_micros)
{
    if (!device_handle || !rx_buffer)
        return -1;

    memset(rx_buffer, 0, length_bytes);

    switch (device_handle->device_params.communication_mode)
    {
    case FS_COMMUNICATION_MODE_UART:
    {
        UART_HandleTypeDef *huart = (UART_HandleTypeDef *)device_handle->driver_config.uart_handle;
        if (!huart)
        {
            fs_log_critical("[Trifecta] UART handle is NULL!");
            return -1;
        }

        HAL_StatusTypeDef st = HAL_UART_Receive(huart,
                                                (uint8_t *)rx_buffer,
                                                length_bytes,
                                                timeout_micros / 1000);

        if (st == HAL_OK)
            return (ssize_t)length_bytes;

        if (st == HAL_TIMEOUT)
            return 0;

        fs_log_critical("[Trifecta] UART receive failed!");
        return -1;
    }

    case FS_COMMUNICATION_MODE_USB_CDC:
        return -1;

    case FS_COMMUNICATION_MODE_I2C:
        return -1;

    case FS_COMMUNICATION_MODE_SPI:
        return -1;

    default:
        return -1;
    }
}

int fs_shutdown_serial_driver(fs_device_info_t *device_handle)
{
    if (!device_handle)
        return -1;

    device_handle->device_params.serial_port = -1;
    return 0;
}

int fs_attempt_reconnect_serial(fs_device_info_t *device_handle)
{
    return 0;
}
