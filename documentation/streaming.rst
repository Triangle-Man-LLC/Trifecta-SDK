Streaming data
==============

Once a device is initialized, data can either be streamed continuously or
polled on demand. In both cases the driver maintains an internal queue of
received packets which you can peek into or drain, as well as "latest value"
accessors for the most common measurements.

Starting and stopping the stream
--------------------------------

.. code-block:: c

    FS_API int fs_start_stream(fs_device_info_t *device_handle);
    FS_API int fs_stop_stream(fs_device_info_t *device_handle);

``fs_start_stream()`` requests that the device transmit data asynchronously
at its native rate (e.g. 200 Hz on Trifecta-K). Because of the high data
rate this may not work well on small host platforms, with many devices at
once, or at slow baud rates. In resource-constrained situations, prefer
periodic one-shot reads instead.

One-shot readings
-----------------

.. code-block:: c

    FS_API int fs_read_one_shot(fs_device_info_t *device_handle);

Requests a single reading from the device; the updated reading arrives
within about 5 ms. Handy for low-power or low-bandwidth applications: call it
from a timer at the rate you need data.

Rebooting the device
--------------------

.. code-block:: c

    FS_API int fs_reboot_device(fs_device_info_t *device_handle);

Triggers a device restart. The function internally calls
``fs_closedown()`` to shut the handle down; afterwards you must re-scan for
the device before reconnecting.

The packet queue
----------------

Received packets are buffered in a ring buffer of up to
``FS_MAX_PACKET_QUEUE_LENGTH`` (16) packets. The queue can be inspected in
several ways:

.. code-block:: c

    FS_API int fs_get_raw_packet(fs_device_info_t *device_handle,
                                 fs_packet_union_t *packet_buffer);

    FS_API int fs_get_raw_packet_queue_size(fs_device_info_t *device_handle);

    FS_API int fs_get_raw_packet_from_queue(fs_device_info_t *device_handle,
                                            fs_packet_union_t *packet_buffer,
                                            int pos);

    FS_API int fs_pop_raw_packet_from_queue(fs_device_info_t *device_handle,
                                            fs_packet_union_t *packet_buffer);

- ``fs_get_raw_packet`` — peek at the *most recent* packet.
- ``fs_get_raw_packet_queue_size`` — number of packets currently queued.
- ``fs_get_raw_packet_from_queue`` — peek at a specific entry, ``pos``
  counted from the earliest packet (0-based).
- ``fs_pop_raw_packet_from_queue`` — pop the *oldest* packet. Use this in a
  loop to drain every packet that arrived since the last drain, rather than
  peeking.

All return ``0`` on success; ``fs_get_raw_packet_queue_size`` additionally
returns the queue size through the function result on some backends, so
prefer the dedicated getters above for clarity.

Latest-value accessors
----------------------

Convenience getters return the newest reading without touching the packet
queue directly:

.. code-block:: c

    FS_API int fs_get_last_timestamp(fs_device_info_t *device_handle, uint32_t *time);
    FS_API int fs_get_orientation(fs_device_info_t *device_handle, fs_quaternion_t *orientation_buffer);
    FS_API int fs_get_orientation_euler(fs_device_info_t *device_handle, fs_vector3_t *orientation_buffer, bool degrees);
    FS_API int fs_get_acceleration(fs_device_info_t *device_handle, fs_vector3_t *acceleration_buffer);
    FS_API int fs_get_angular_velocity(fs_device_info_t *device_handle, fs_vector3_t *angular_velocity_buffer);
    FS_API int fs_get_magnetic_field(fs_device_info_t *device_handle, fs_vector3_t *mag_buffer);
    FS_API int fs_get_velocity(fs_device_info_t *device_handle, fs_vector3_t *velocity_buffer);
    FS_API int fs_get_movement_state(fs_device_info_t *device_handle, fs_run_status_t *device_state_buffer);
    FS_API int fs_get_position(fs_device_info_t *device_handle, fs_vector3_d_t *position_buffer);

Notes:

- ``fs_get_orientation_euler()`` — pass ``true`` for degrees, ``false`` for
  radians.
- ``fs_get_magnetic_field()`` — reports NaN for devices without a
  magnetometer.
- ``fs_get_movement_state()`` — ``FS_RUN_STATUS_RUNNING`` when moving,
  ``FS_RUN_STATUS_IDLE`` when stationary.
- ``fs_get_position()`` — only meaningful for GNSS-stabilized systems
  (e.g. Trifecta-M); for a plain IMU/AHRS it has little meaning.

Extracting values from packets
------------------------------

When working from a raw ``fs_packet_union_t`` (e.g. from the queue or from a
replay file), use the helper functions to decode fields in the correct units:

- ``fs_euler_angles_from_packet()`` — Euler angles (degrees) from the
  packet's quaternion.
- ``fs_lat_long_from_packet()`` — WGS84 latitude/longitude/height.
- ``fs_angular_velocity_from_packet()`` / ``fs_angular_velocity_raw_from_packet()``
  — compensated vs. raw angular velocity (deg/s).
- ``fs_acceleration_from_packet()`` / ``fs_acceleration_raw_from_packet()``
  — compensated vs. raw acceleration (m/s²).
- ``fs_magnetic_field_from_packet()`` — magnetometer (mG); NaN when the
  device has no magnetometer.
- ``fs_barometric_pressure_from_packet()`` — barometric pressure (Pa).
- ``fs_velocity_from_packet()`` — velocity (m/s), Trifecta-M only.

See :doc:`api_reference` for exact signatures and :doc:`packets` for the
fields each packet family carries.
