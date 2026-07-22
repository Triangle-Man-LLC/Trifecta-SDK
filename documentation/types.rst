Data types
==========

The types shared by the public API, defined in ``FS_Trifecta_Defs.h`` and
the headers it includes.

Basic math types
----------------

.. code-block:: c

    typedef struct fs_quaternion
    {
        float w;  // scalar term
        float x;  // vector terms
        float y;
        float z;
    } fs_quaternion_t;

    typedef struct fs_vector3
    {
        float x, y, z;
    } fs_vector3_t;

    typedef struct fs_vector3_d
    {
        double x, y, z;
    } fs_vector3_d_t;

``fs_vector3_t`` is used for most measurements (acceleration in m/s²,
angular velocity in deg/s, magnetometer in mG, velocity in m/s, Euler
angles in degrees). ``fs_vector3_d_t`` (double precision) is used for WGS84
position, where ``x`` = latitude (deg), ``y`` = longitude (deg), ``z`` =
height (m).

Device identifiers and status
-----------------------------

.. code-block:: c

    typedef enum fs_device_id
    {
        FS_DEVICE_ID_UNKNOWN = 0,
        FS_DEVICE_ID_TK = 1,    // Trifecta-K (IMU - Unspecified)
        FS_DEVICE_ID_TK0 = 10,  // Trifecta-K0 (IMU - Compact version)
        FS_DEVICE_ID_TK1 = 11,  // Trifecta-K1 (IMU - Standard performance)
        FS_DEVICE_ID_TK2 = 12,  // Trifecta-K2 (IMU - High performance)
        FS_DEVICE_ID_TM = 2,    // Trifecta-M (GNSS/INS - Unspecified)
        FS_DEVICE_ID_TM0 = 20,  // Trifecta-M0 (RTK GNSS/INS - Single antenna)
        FS_DEVICE_ID_TM1 = 21,  // Trifecta-M1 (RTK GNSS/INS - Dual antenna)
        FS_DEVICE_ID_TM2 = 22,  // Trifecta-M2 (RTK GNSS/INS - Dual antenna)
        FS_DEVICE_ID_STV = 3,   // Super Trifecta (Unspecified)
        FS_DEVICE_ID_STV1 = 31, // Super Trifecta 1
        FS_DEVICE_ID_STV2 = 32, // Super Trifecta 2
    } fs_device_id_t;

.. code-block:: c

    typedef enum fs_run_status
    {
        FS_RUN_STATUS_ERROR = -1,
        FS_RUN_STATUS_IDLE = 0,
        FS_RUN_STATUS_RUNNING = 1,
    } fs_run_status_t;

The device handle
-----------------

.. code-block:: c

    typedef struct fs_device_info
    {
        fs_device_descriptor_t device_descriptor; // device name, etc.
        fs_device_params_t device_params;         // managed in the backend, do not modify
        fs_driver_config_t driver_config;         // driver configuration
        /* ... backend-managed state: lock, background task, ... */
        fs_packet_union_t last_received_packet;   // most recent packet (read-only)
        /* ... backend-managed buffers (data buffer, packet queue, command queue) ... */
        void *save_context;                       // set while saving is active
    } fs_device_info_t;

- ``device_descriptor`` — identity information (see
  :doc:`configuration`). You may pre-set ``device_name`` before
  ``fs_initialize_serial()`` to select a specific device.
- ``device_params`` — connection state: communication mode, status, IP
  address, SSID, ports, serial path, baud rate, ping (time since last
  communication), ... **managed by the driver — do not modify**; use
  ``fs_get_device_operating_state()`` to read it.
- ``driver_config`` — the driver configuration set with
  ``fs_set_driver_parameters()`` (see :doc:`initialization`).
- Everything else is backend-managed (mutex, background task handle,
  buffers, packet queue).

``fs_device_info_t`` is a fairly large structure, so it is intended to be
*statically allocated*, not placed on the stack of a tight loop:

.. code-block:: c

    fs_device_info_t dev = {0};

The related parameter struct:

.. code-block:: c

    typedef struct fs_device_params
    {
        fs_communication_mode_t communication_mode;
        fs_run_status_t status;          // 0 = stopped, 1 = running, -1 = error
        int all_enabled_interfaces;
        char ip_addr[39];
        char ssid[32];
        char ssid_ap[32];
        char pw_ap[64];
        int tcp_port;
        int udp_port;
        char serial_path[128];
        fs_serial_handle_t serial_port;
        int32_t baudrate;
        int32_t ping;                    // ms since last communication
        uint64_t hp_timestamp;           // accurate timestamp in interrupt mode
    } fs_device_params_t;

Packets
-------

All packet structs, the ``fs_packet_union_t`` union, packet type enum, and
diagnostic flags are documented in :doc:`packets`.

Communication modes
-------------------

``fs_communication_mode_t`` (``FS_COMMUNICATION_MODE_USB_CDC``,
``_UART``, ``_TCP_UDP``, ``_TCP_UDP_AP``, ``_CAN``, ``_I2C``, ``_ESPN``,
``_SPI``, ``_BLE``) — values and meanings are listed in
:doc:`initialization`.

Platform types
--------------

A few types in ``FS_Trifecta_Defs_Platform_Types.h`` appear in the public
signatures:

.. code-block:: c

    typedef intptr_t fs_sock_t;          // socket handle (platform-specific)
    typedef intptr_t fs_serial_handle_t; // serial port handle / path

    typedef struct fs_tm
    {
        int year;   // 4-digit
        int month;  // 1-12
        int day;    // 1-31
        int hour;   // 0-23
        int min;    // 0-59
        int sec;    // 0-59
        int usec;   // 0-999999
    } fs_tm_t;

Relevant constants:

+--------------------------------------------+----------------------------------------------+
| Constant                                   | Value / meaning                              |
+============================================+==============================================+
| ``FS_TRIFECTA_PORT``                       | ``8888`` — default UDP data port             |
+--------------------------------------------+----------------------------------------------+
| ``FS_TRIFECTA_DEVICE_IDENTIFY_PORT``       | ``6868`` — device identification port        |
+--------------------------------------------+----------------------------------------------+
| ``FS_TRIFECTA_SERIAL_BAUDRATE``            | ``2000000`` — default UART baud rate         |
+--------------------------------------------+----------------------------------------------+
| ``FS_MAX_NUMBER_DEVICES``                  | ``16`` — max devices in discovery lists      |
+--------------------------------------------+----------------------------------------------+
| ``FS_MAX_IP_ADDRESS_LENGTH``               | ``64`` — max IP address string length        |
+--------------------------------------------+----------------------------------------------+
| ``FS_MAX_DEVICE_PATH_LENGTH``              | ``256`` — max serial path string length      |
+--------------------------------------------+----------------------------------------------+
| ``FS_MAX_PACKET_QUEUE_LENGTH``             | ``16`` — packet queue depth                  |
+--------------------------------------------+----------------------------------------------+
| ``FS_MAX_DATA_LENGTH``                     | ``512`` — internal byte buffer size          |
+--------------------------------------------+----------------------------------------------+
| ``FS_MAX_CMD_LENGTH``                      | ``256`` — max inbound command length         |
+--------------------------------------------+----------------------------------------------+
| ``FS_MAX_CMD_QUEUE_LENGTH``                | ``16`` — command queue depth                 |
+--------------------------------------------+----------------------------------------------+
| ``FS_REPLAY_DEFAULT_STEP``                 | ``200`` — recommended replay index step      |
+--------------------------------------------+----------------------------------------------+
