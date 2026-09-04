API reference
=============

Complete reference of the user-facing API declared in ``FS_Trifecta.h``
(grouped the same way as the header). Unless stated otherwise, functions
return ``0`` on success and a negative value on failure.

Handle allocation and deallocation
----------------------------------

.. code-block:: c

    FS_API fs_device_info_t *fs_export_allocate_device();

.. code-block:: c

    FS_API void fs_export_free_device(fs_device_info_t *device);

- **For external bindings only** (C#, Python, ...). In native C/C++ use a
  statically allocated ``fs_device_info_t`` instead.
- ``fs_export_allocate_device`` returns a default-initialized, dynamically
  allocated handle; ``fs_export_free_device`` frees it. Free it *after*
  ``fs_closedown()``.

Device discovery
----------------

.. code-block:: c

    FS_API ssize_t fs_obtain_network_devices_list(
        char ip_addr_list[FS_MAX_NUMBER_DEVICES][FS_MAX_IP_ADDRESS_LENGTH],
        int timeout_micros);

    FS_API ssize_t fs_obtain_serial_devices_list(
        char device_path_list[FS_MAX_NUMBER_DEVICES][FS_MAX_DEVICE_PATH_LENGTH],
        int timeout_micros);

See :doc:`discovery` for details and usage notes.

Initialization
--------------

.. code-block:: c

    FS_API int fs_set_driver_parameters(fs_device_info_t *device_handle,
                                        fs_driver_config_t *driver_config);

- Sets the connection configuration parameters. Passing ``NULL`` selects
  the platform defaults (``FS_DRIVER_CONFIG_DEFAULT``).

.. code-block:: c

    FS_API int fs_initialize_networked(fs_device_info_t *device_handle,
                                       const char *device_ip_address);

- Starts the device in networked (Wi-Fi) mode. ``device_ip_address`` of
  ``"\0"`` triggers an automatic scan on supported platforms.

.. code-block:: c

    FS_API int fs_initialize_serial(fs_device_info_t *device_handle,
                                    fs_serial_handle_t context,
                                    fs_communication_mode_t serial_mode);

- Starts the device in wired serial mode. ``context`` is the platform
  handle (or port path on Linux/Windows/Android). If
  ``device_descriptor.device_name`` is set, only a device with that name is
  connected.

.. code-block:: c

    FS_API int fs_closedown(fs_device_info_t *device_handle);

- Deallocates all driver resources for the device (the handle memory itself
  is *not* freed; see ``fs_export_free_device``).

Stream control
--------------

.. code-block:: c

    FS_API int fs_start_stream(fs_device_info_t *device_handle);

- Requests asynchronous streaming at the device's native rate (e.g. 200 Hz
  on Trifecta-K). May be too heavy for small hosts, many devices, or slow
  baud rates; use one-shot reads in those cases.

.. code-block:: c

    FS_API int fs_stop_stream(fs_device_info_t *device_handle);

- Stops the asynchronous data stream.

.. code-block:: c

    FS_API int fs_read_one_shot(fs_device_info_t *device_handle);

- Requests a single reading; the updated reading arrives within about 5 ms.

.. code-block:: c

    FS_API int fs_reboot_device(fs_device_info_t *device_handle);

- Triggers a device restart and internally calls ``fs_closedown()``. You
  must re-scan for the device afterwards.

Data export / viewing
---------------------

.. code-block:: c

    FS_API int fs_get_last_timestamp(fs_device_info_t *device_handle,
                                     uint32_t *time);

- Retrieves the internal timestamp (RTOS ticks) of the last received
  transmission.

.. code-block:: c

    FS_API int fs_get_raw_packet(fs_device_info_t *device_handle,
                                 fs_packet_union_t *packet_buffer);

- Retrieves the latest device data packet.

.. code-block:: c

    FS_API int fs_get_raw_packet_queue_size(fs_device_info_t *device_handle);

    FS_API int fs_get_raw_packet_from_queue(fs_device_info_t *device_handle,
                                            fs_packet_union_t *packet_buffer,
                                            int pos);

    FS_API int fs_pop_raw_packet_from_queue(fs_device_info_t *device_handle,
                                            fs_packet_union_t *packet_buffer);

- Queue inspection: current queue size (max ``FS_MAX_PACKET_QUEUE_LENGTH``
  = 16), peek at entry ``pos`` (0 = earliest), and pop the oldest entry,
  respectively.

.. code-block:: c

    FS_API int fs_get_orientation(fs_device_info_t *device_handle,
                                  fs_quaternion_t *orientation_buffer);

    FS_API int fs_get_orientation_euler(fs_device_info_t *device_info,
                                        fs_vector3_t *orientation_buffer,
                                        bool degrees);

- Latest orientation as a quaternion, or as Euler angles (pass
  ``degrees == true`` for degrees, otherwise radians).

.. code-block:: c

    FS_API int fs_get_acceleration(fs_device_info_t *device_handle,
                                   fs_vector3_t *acceleration_buffer);

    FS_API int fs_get_angular_velocity(fs_device_info_t *device_handle,
                                       fs_vector3_t *angular_velocity_buffer);

    FS_API int fs_get_magnetic_field(fs_device_info_t *device_handle,
                                     fs_vector3_t *mag_buffer);

- Latest compensated acceleration (m/s²), angular velocity (deg/s), and
  magnetometer (mG). The magnetometer reads NaN on devices without one.

.. code-block:: c

    FS_API int fs_get_velocity(fs_device_info_t *device_handle,
                               fs_vector3_t *velocity_buffer);

- Latest measured velocity (m/s, ENU frame). Trifecta-M devices only.

.. code-block:: c

    FS_API int fs_get_movement_state(fs_device_info_t *device_handle,
                                     fs_run_status_t *device_state_buffer);

- ``FS_RUN_STATUS_RUNNING`` when the device is in motion,
  ``FS_RUN_STATUS_IDLE`` when stationary.

.. code-block:: c

    FS_API int fs_get_position(fs_device_info_t *device_handle,
                               fs_vector3_d_t *position_buffer);

- Latest position (WGS84: latitude, longitude, height in m). Only
  meaningful for GNSS-stabilized systems.

Packet helpers
--------------

All helpers take a packet (as received by the driver or from a replay file)
and decode one field in the standard units:

.. code-block:: c

    FS_API int fs_euler_angles_from_packet(const fs_packet_union_t *packet,
                                           fs_vector3_t *euler_angles_out);

- Euler angles in **degrees** converted from the packet's quaternion.

.. code-block:: c

    FS_API int fs_lat_long_from_packet(const fs_packet_union_t *packet,
                                       fs_vector3_d_t *lat_long_height);

- ``x`` = latitude (deg), ``y`` = longitude (deg), ``z`` = height (m).

.. code-block:: c

    FS_API int fs_angular_velocity_from_packet(const fs_packet_union_t *packet,
                                               fs_vector3_t *angular_velocity);

    FS_API int fs_angular_velocity_raw_from_packet(const fs_packet_union_t *packet,
                                                   fs_vector3_t *angular_velocity);

- Compensated vs. raw angular velocity (deg/s, body frame). Prefer the
  compensated version for most uses.

.. code-block:: c

    FS_API int fs_acceleration_from_packet(const fs_packet_union_t *packet,
                                           fs_vector3_t *acceleration);

    FS_API int fs_acceleration_raw_from_packet(const fs_packet_union_t *packet,
                                               fs_vector3_t *acceleration);

- Compensated vs. raw acceleration (m/s², body frame). Prefer the
  compensated version for most uses.

.. code-block:: c

    FS_API int fs_magnetic_field_from_packet(const fs_packet_union_t *packet,
                                             fs_vector3_t *mag_values);

- Magnetometer values (mG); all NaN on devices without a magnetometer.

.. code-block:: c

    FS_API int fs_barometric_pressure_from_packet(const fs_packet_union_t *packet,
                                                  float *pres_value);

- Barometric pressure in Pa.

.. code-block:: c

    FS_API int fs_velocity_from_packet(const fs_packet_union_t *packet,
                                       fs_vector3_t *velocity);

- Velocity (m/s, ENU frame). Trifecta-M only.

Device configuration
--------------------

.. code-block:: c

    FS_API int fs_set_device_name(fs_device_info_t *device_handle,
                                  const char name[32]);

.. code-block:: c

    FS_API int fs_set_communication_mode(fs_device_info_t *device_handle,
                                         int modes);

.. code-block:: c

    FS_API int fs_set_network_parameters(fs_device_info_t *device_handle,
                                         const char ssid[32],
                                         const char pw[64],
                                         bool access_point);

.. code-block:: c

    FS_API int fs_set_network_udp_port(fs_device_info_t *device_handle,
                                       int port);

.. code-block:: c

    FS_API int fs_set_serial_uart_baudrate(fs_device_info_t *device_handle,
                                           int baudrate);

.. code-block:: c

    FS_API int fs_set_gnss_baseline(fs_device_info_t *device_handle,
                                    fs_vector3_t baseline);

.. code-block:: c

    FS_API int fs_set_gnss_lever_arm(fs_device_info_t *device_handle,
                                     fs_vector3_t lever_arm);

.. code-block:: c

    FS_API int fs_set_ahrs_heading(fs_device_info_t *device_handle,
                                   float heading_deg);

.. code-block:: c

    FS_API int fs_set_ins_position(fs_device_info_t *device_handle,
                                   fs_vector3_d_t *position);

See :doc:`configuration` for details. Most of these are applied on device
restart; ``fs_set_network_udp_port``, ``fs_set_ahrs_heading`` are
non-volatile.

Device state and descriptors
----------------------------

.. code-block:: c

    FS_API int fs_get_device_operating_state(fs_device_info_t *device_handle,
                                             fs_device_params_t *device_params_info);

- Copies the device's current operating state into ``device_params_info``.

.. code-block:: c

    FS_API int fs_get_device_descriptors(fs_device_info_t *device_handle,
                                         fs_device_descriptor_t *desc);

- Copies the device's identity descriptors (id, name, firmware,
  description, serial number, model) into ``desc``.

Utilities
---------

.. code-block:: c

    FS_API int fs_eulers_from_quaternion(fs_vector3_t *euler_angles,
                                         const fs_quaternion_t *quaternion);

- Quaternion-to-Euler-angle conversion (degrees).

.. code-block:: c

    FS_API int fs_enable_logging(bool do_enable);

    FS_API int fs_enable_logging_at_path(const char *path, bool do_enable);

- Toggles internal logging, optionally redirected to a file. Logging is
  disabled by default (it is a latency penalty); enable it only when
  debugging, e.g. while porting the driver.

.. code-block:: c

    FS_API int fs_factory_reset(fs_device_info_t *device_handle);

- **Danger!** Clears all user configuration on the device. Use only when
  needed.

Replay
------

.. code-block:: c

    FS_API fs_replay_t *fs_export_allocate_replay();

    FS_API void fs_export_free_replay(fs_replay_t *replay);

- Dynamic allocation for external bindings; ``fs_export_free_replay`` must
  be called only *after* ``fs_replay_close``.

.. code-block:: c

    FS_API int fs_replay_open(fs_replay_t *r, const char *path,
                              uint32_t sparse_step);

- Opens a CSV file exported by the saver. ``sparse_step`` is typically
  ``FS_REPLAY_DEFAULT_STEP`` (200).

.. code-block:: c

    FS_API size_t fs_replay_get_size(const fs_replay_t *r);

- Number of packets in the opened replay file.

.. code-block:: c

    FS_API int fs_replay_read_next(fs_replay_t *r, fs_packet_union_t *out);

    FS_API int fs_replay_read_line(fs_replay_t *r, uint32_t line_index,
                                   fs_packet_union_t *out);

- Advance to the next row, or jump to an arbitrary row index; both decode
  the row into ``out``.

.. code-block:: c

    FS_API void fs_replay_close(fs_replay_t *r);

- Closes the file and frees the index. Must be called before
  ``fs_export_free_replay`` (and before the handle goes out of scope for
  static handles).

Saving
------

.. code-block:: c

    FS_API fs_save_device_t *fs_export_allocate_save();

    FS_API void fs_export_free_save(fs_save_device_t *device);

- Dynamic allocation for external bindings.

.. code-block:: c

    FS_API int fs_save_init(fs_save_device_t *device,
                            const fs_save_config_t *cfg);

    FS_API void fs_save_destroy(fs_save_device_t *device);

- Create/initialize the saver context (``NULL`` config = defaults) and
  destroy it, respectively. ``fs_save_destroy`` closes all open files.

.. code-block:: c

    FS_API int fs_save_begin_device(fs_save_device_t *device,
                                    fs_device_info_t *dev);

    FS_API int fs_save_end_device(fs_save_device_t *device,
                                  fs_device_info_t *dev);

- Start/stop saving for a specific device; the filename and metadata are
  derived from the device. ``fs_save_end_device`` is safe to call multiple
  times. See :doc:`saving` for the CSV layout.
