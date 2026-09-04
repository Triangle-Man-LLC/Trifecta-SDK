Porting to a new platform
=========================

The Trifecta driver is split into a **common core** (in ``common/``) and
**per-platform backends** (one directory per platform: ``linux/``,
``windows/``, ``android/``, ``esp-idf/``, ``stm32/``). To port the driver to
a new platform, implement the methods declared in
``include/FS_Trifecta_Interfaces.h`` and place your implementation in
``<Trifecta-Driver>/<PLATFORM_NAME>/FS_Interfaces.c``.

Any method that is not relevant on your platform (e.g. wireless networking
on a target without Wi-Fi) may simply return ``-1`` to indicate "not
supported".

Network backend
---------------

.. code-block:: c

    int fs_init_network_tcp_driver(fs_device_info_t *device_handle);
    int fs_init_network_udp_driver(fs_device_info_t *device_handle);

    ssize_t fs_transmit_networked_tcp(fs_device_info_t *device_handle, void *tx_buffer,
                                      size_t length_bytes, int timeout_micros);
    ssize_t fs_transmit_networked_udp(fs_device_info_t *device_handle, void *tx_buffer,
                                      size_t length_bytes, int timeout_micros);
    ssize_t fs_receive_networked_tcp(fs_device_info_t *device_handle, void *rx_buffer,
                                     size_t length_bytes, int timeout_micros);
    ssize_t fs_receive_networked_udp(fs_device_info_t *device_handle, void *rx_buffer,
                                     size_t length_bytes, int timeout_micros);

    int fs_shutdown_network_tcp_driver(fs_device_info_t *device_handle);
    int fs_shutdown_network_udp_driver(fs_device_info_t *device_handle);

    int fs_attempt_reconnect_network_tcp(fs_device_info_t *device_handle);
    int fs_attempt_reconnect_network_udp(fs_device_info_t *device_handle);

- The ``fs_init_*`` drivers create the socket(s) and return ``0`` on
  success or a negative error code.
- Transmit/receive functions return the number of bytes transferred, or a
  negative error code on failure. Timeouts are in microseconds.
- The ``fs_attempt_reconnect_*`` functions are called by the core when a
  connection has gone stale.

Serial backend
--------------

.. code-block:: c

    int fs_init_serial_driver(fs_device_info_t *device_handle);

    ssize_t fs_transmit_serial(fs_device_info_t *device_handle, void *tx_buffer,
                               size_t length_bytes, int timeout_micros);
    ssize_t fs_receive_serial(fs_device_info_t *device_handle, void *rx_buffer,
                              size_t length_bytes, int timeout_micros);

    int fs_shutdown_serial_driver(fs_device_info_t *device_handle);

    int fs_attempt_reconnect_serial(fs_device_info_t *device_handle);

    int fs_platform_supported_serial_interrupts(void);
    int fs_init_serial_interrupts(fs_device_info_t *device_handle);
    int fs_wait_until_next_serial_interrupt(fs_device_info_t *device_handle);

- ``fs_platform_supported_serial_interrupts`` returns an OR-flag of the
  serial interfaces that support interrupt-driven reads (or
  ``FS_COMMUNICATION_MODE_UNINITIALIZED`` if none do).
- When interrupt mode is active, ``fs_init_serial_interrupts`` wires up the
  DRDY GPIO and ``fs_wait_until_next_serial_interrupt`` yields the task
  until the next interrupt. This gives much more precise timestamping of
  incoming packets (see ``fs_device_params_t::hp_timestamp``).
- On POSIX-like platforms the "handle" is typically a file descriptor; on
  Windows it is a ``HANDLE``; on RTOS platforms it is the UART/I2C/SPI
  driver handle.

Threads and time
----------------

.. code-block:: c

    int fs_thread_start(fs_thread_func_t (*thread_func)(void *), void *params,
                        fs_run_status_t *thread_running_flag,
                        fs_thread_t *thread_handle, size_t stack_size,
                        int priority, int core_affinity);

    int fs_thread_exit(void *thread_handle);

    int fs_delay(int millis);
    int fs_delay_for(uint64_t *current_time, int millis);
    int fs_get_current_time(uint64_t *current_time);
    int fs_get_local_time(fs_tm_t *out);

- ``fs_thread_start`` creates a background task (FreeRTOS task on ESP32/
  STM32, pthread/Win32 thread elsewhere) with the given stack size,
  priority, and core affinity (``-1`` for "no preference").
- ``fs_get_current_time`` returns milliseconds; ``fs_get_local_time``
  breaks a millisecond time value down into an ``fs_tm_t``.

Discovery support
-----------------

.. code-block:: c

    ssize_t fs_listen_for_udp_broadcasts(char ip_addr_list[FS_MAX_NUMBER_DEVICES][64],
                                         int timeout_micros);

Used by ``fs_obtain_network_devices_list()``; listen on the device
identification port (6868) for the broadcast packets described in
:doc:`packets` and collect the reported IPs.

Logging
-------

.. code-block:: c

    int fs_log_output(const char *format, ...);
    int fs_log_critical(const char *format, ...);
    int fs_toggle_logging(bool do_log);
    int fs_set_log_location(const char *directory);

``fs_log_output`` respects the current logging toggle (off by default, as
logging is a latency penalty); ``fs_log_critical`` always prints.
``fs_set_log_location`` is only needed on platforms with a filesystem.

Filesystem / saving
-------------------

The saver (:doc:`saving`) and replay (:doc:`replay`) modules use standard
``FILE*`` APIs, so any platform with a C stdio-compatible filesystem layer
works out of the box.

Build integration
-----------------

Add the new platform sources to ``CMakeLists.txt`` behind the appropriate
``CMAKE_SYSTEM_NAME``/platform check (see how ``linux/``, ``windows/``, and
``android/`` are wired up in ``cmake/``), or, for ESP-IDF-style builds, add
a component manifest. The ``FS_API`` export macro in
``FS_Trifecta_Defs_Platform_Types.h`` already handles Windows DLL export
and GCC visibility; most other platforms leave it empty.
