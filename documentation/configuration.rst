Device configuration
====================

These functions reconfigure the device itself (as opposed to the driver).
Unless noted otherwise, configuration changes are applied **on device
restart** — call ``fs_reboot_device()`` afterwards to make them take
effect.

Identity and connectivity
-------------------------

.. code-block:: c

    FS_API int fs_set_device_name(fs_device_info_t *device_handle,
                                  const char name[32]);

.. code-block:: c

    FS_API int fs_set_communication_mode(fs_device_info_t *device_handle,
                                         int modes);

    FS_API int fs_set_network_parameters(fs_device_info_t *device_handle,
                                         const char ssid[32],
                                         const char pw[64],
                                         bool access_point);

- ``fs_set_device_name`` — sets the device name (max 31 characters +
  terminator).
- ``fs_set_communication_mode`` — sets *all* enabled communication
  interfaces of the device; pass an OR of ``fs_communication_mode_t``
  values (see :doc:`initialization`).
- ``fs_set_network_parameters`` — sets the Wi-Fi SSID/password. Pass
  ``access_point = true`` to configure the device's own access point, or
  ``false`` to configure the network it should join as a station.

Networking parameters
---------------------

.. code-block:: c

    FS_API int fs_set_network_udp_port(fs_device_info_t *device_handle,
                                       int port);

.. code-block:: c

    FS_API int fs_set_serial_uart_baudrate(fs_device_info_t *device_handle,
                                           int baudrate);

- ``fs_set_network_udp_port`` — sets the UDP port the device listens on
  (allowed range 1024–65535, default **8888**). Non-volatile. The device
  must be connected in Wi-Fi mode to set this.
- ``fs_set_serial_uart_baudrate`` — sets the UART baud rate. Allowed range
  is **921,600 – 3,000,000** (factory default 2,000,000); values outside
  the range may cause lag or instability.

GNSS configuration (Trifecta-M and other GNSS devices)
------------------------------------------------------

.. code-block:: c

    FS_API int fs_set_gnss_baseline(fs_device_info_t *device_handle,
                                    fs_vector3_t baseline);

.. code-block:: c

    FS_API int fs_set_gnss_lever_arm(fs_device_info_t *device_handle,
                                     fs_vector3_t lever_arm);

- ``fs_set_gnss_baseline`` — the x, y, z baseline between the GNSS_1 and
  GNSS_0 antennas, in meters, following the IMU coordinate conventions.
- ``fs_set_gnss_lever_arm`` — the x, y, z lever arm between the GNSS_0
  antenna and the IMU mounting point, in meters, following the IMU
  coordinate conventions.

Heading and INS position
------------------------

.. code-block:: c

    FS_API int fs_set_ahrs_heading(fs_device_info_t *device_handle,
                                   float heading_deg);

.. code-block:: c

    FS_API int fs_set_ins_position(fs_device_info_t *device_handle,
                                   fs_vector3_d_t *position);

- ``fs_set_ahrs_heading`` — manually sets the AHRS yaw angle (degrees) to a
  known value. Non-volatile. In practice this is usually done automatically
  once a compatible GNSS is connected.
- ``fs_set_ins_position`` — updates the INS position, typically from an
  external GNSS system. Currently this function only resets the position to
  zero.

Querying the device
-------------------

.. code-block:: c

    FS_API int fs_get_device_operating_state(fs_device_info_t *device_handle,
                                             fs_device_params_t *device_params_info);

.. code-block:: c

    FS_API int fs_get_device_descriptors(fs_device_info_t *device_handle,
                                         fs_device_descriptor_t *desc);

- ``fs_get_device_operating_state`` — fills in an ``fs_device_params_t``
  with the device's current operating state (communication mode, status, IP
  address, SSID, ports, baud rate, time since last communication, ...).
- ``fs_get_device_descriptors`` — fills in an ``fs_device_descriptor_t``
  with the device's identity (model, name, firmware version, serial number,
  description).

.. code-block:: c

    typedef struct fs_device_descriptor
    {
        fs_device_id_t device_id;
        char device_name[32];
        char device_fw[32];
        char device_desc[64];
        char device_sn[32];
        char device_model[32];
    } fs_device_descriptor_t;

Factory reset
-------------

.. code-block:: c

    FS_API int fs_factory_reset(fs_device_info_t *device_handle);

.. warning::

   **Danger!** A factory reset clears all user configuration (name, Wi-Fi
   settings, GNSS baselines, ...). Use only when needed; the device reverts
   to factory defaults and must be reconfigured.
