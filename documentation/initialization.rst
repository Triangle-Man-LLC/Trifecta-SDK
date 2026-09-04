Initializing the device
=======================

All driver state lives in an ``fs_device_info_t`` handle (see :doc:`types`).
The driver is designed to be *statically allocated* in native C/C++ code:

.. code-block:: c

    fs_device_info_t dev = {0};

Dynamically allocated handles exist for external language bindings (C#,
Python, ...) via ``fs_export_allocate_device()`` /
``fs_export_free_device()`` and are not recommended for direct C/C++ usage.

Driver parameters
-----------------

Before connecting you can tune the driver with ``fs_set_driver_parameters()``.
If you never call it, the platform-appropriate ``FS_DRIVER_CONFIG_DEFAULT``
values are used.

.. code-block:: c

    FS_API int fs_set_driver_parameters(fs_device_info_t *device_handle,
                                        fs_driver_config_t *driver_config);

The configuration struct:

.. code-block:: c

    typedef struct fs_driver_config
    {
        bool use_serial_interrupt_mode;    // use interrupt-driven serial if the platform supports it
        int serial_data_ready_gpio;        // DRDY GPIO, when interrupt mode is enabled
        int background_task_priority;      // background task priority (-1 = default)
        int background_task_core_affinity; // background task core (-1 = any)
        int read_timeout_micros;           // read timeout, in microseconds
        int task_wait_ms;                  // wait between background task updates, ms
        int task_stack_size_bytes;         // background task stack size, bytes
    } fs_driver_config_t;

Networked (Wi-Fi) initialization
--------------------------------

.. code-block:: c

    FS_API int fs_initialize_networked(fs_device_info_t *device_handle,
                                       const char *device_ip_address);

**Parameters**

- ``device_handle`` — the device handle.
- ``device_ip_address`` — the device's IP address as a string. Pass
  ``"\0"`` to auto-scan (on supported platforms) for a device over UDP
  broadcasts.

**Returns** ``0`` on success.

By default the device listens for UDP on port **8888**
(``FS_TRIFECTA_PORT``); device identification uses port **6868**.

Serial initialization
---------------------

.. code-block:: c

    FS_API int fs_initialize_serial(fs_device_info_t *device_handle,
                                    fs_serial_handle_t context,
                                    fs_communication_mode_t serial_mode);

**Parameters**

- ``device_handle`` — the device handle.
- ``context`` — the platform's UART/I2C/SPI/CAN/USB handle on embedded
  platforms; the port path (``"COM5"``, ``"/dev/ttyACM0"``) on Linux,
  Windows, and Android.
- ``serial_mode`` — one of the ``FS_COMMUNICATION_MODE_*`` values, e.g.
  ``FS_COMMUNICATION_MODE_UART``.

**Returns** ``0`` on success.

If a name has been set on ``device_handle->device_descriptor.device_name``
  beforehand, the driver will only connect to a device with that name.

Communication modes
-------------------

The ``fs_communication_mode_t`` enum defines the transport between host and
device:

- ``FS_COMMUNICATION_MODE_UNINITIALIZED`` (``-1``) — not initialized.
- ``FS_COMMUNICATION_MODE_USB_CDC`` (``0``) — USB-CDC (always on).
- ``FS_COMMUNICATION_MODE_UART`` (``1``) — UART.
- ``FS_COMMUNICATION_MODE_TCP_UDP`` (``2``) — UDP streaming, device in
  station (STA) mode.
- ``FS_COMMUNICATION_MODE_TCP_UDP_AP`` (``3``) — UDP streaming, device in
  access-point (AP) mode.
- ``FS_COMMUNICATION_MODE_CAN`` (``4``) — CAN bus (reserved).
- ``FS_COMMUNICATION_MODE_I2C`` (``5``) — I2C bus (reserved).
- ``FS_COMMUNICATION_MODE_ESPN`` (``6``) — ESP-NOW, 2.4 GHz, ESP32 only
  (reserved).
- ``FS_COMMUNICATION_MODE_SPI`` (``7``) — SPI (reserved).
- ``FS_COMMUNICATION_MODE_BLE`` (``8``) — Bluetooth Low Energy (reserved).

Shutdown
--------

.. code-block:: c

    FS_API int fs_closedown(fs_device_info_t *device_handle);

Releases all driver resources (socket, serial port, background task) for the
device. If the handle was obtained with
``fs_export_allocate_device()``, you must still call
``fs_export_free_device()`` afterwards to free the memory.

.. note::

   ``fs_reboot_device()`` (see :doc:`streaming`) also calls
   ``fs_closedown()`` internally; after a reboot you must re-scan for the
   device before reconnecting.
