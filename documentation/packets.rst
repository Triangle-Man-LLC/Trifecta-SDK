Packet Formats
==============

The Trifecta devices use a family of packet formats designed to cover everything
from raw IMU telemetry to full GNSS/INS navigation solutions. Although the
driver exposes a unified ``fs_packet_union_t`` type, each packet format has its
own structure, size, and intended use.

This page documents all packet types defined in the driver, how they differ, and
how to interpret their fields.

Packet Type Enumeration
-----------------------

Packet types are defined in ``fs_packet_type_t``:

.. code-block:: c

   typedef enum fs_packet_type {
       C_PACKET_TYPE_IMU = 0,
       C_PACKET_TYPE_AHRS = 1,
       C_PACKET_TYPE_INS = 2,
       C_PACKET_TYPE_RESERVED = 3,

       S_PACKET_TYPE_IMU = 4,
       S_PACKET_TYPE_AHRS = 5,
       S_PACKET_TYPE_INS = 6,
       S_PACKET_TYPE_RESERVED = 7,

       C2_PACKET_TYPE_IMU = 8,
       C2_PACKET_TYPE_AHRS = 9,
       C2_PACKET_TYPE_INS = 10,
       C2_PACKET_TYPE_RESERVED = 11,

       HR_PACKET_TYPE_IMU = 21,
       HR_PACKET_TYPE_RESERVED_1 = 22,
       HR_PACKET_TYPE_RESERVED_2 = 23,
       HR_PACKET_TYPE_RESERVED_3 = 24,

       C64_PACKET_TYPE_IMU = 100,
       C64_PACKET_TYPE_AHRS = 101,
       C64_PACKET_TYPE_INS = 102,
       C64_PACKET_TYPE_RESERVED = 103,

       S64_PACKET_TYPE_IMU = 104,
       S64_PACKET_TYPE_AHRS = 105,
       S64_PACKET_TYPE_INS = 106,
       S64_PACKET_TYPE_RESERVED = 107,

       C642_PACKET_TYPE_IMU = 108,
       C642_PACKET_TYPE_AHRS = 109,
       C642_PACKET_TYPE_INS = 110,
       C642_PACKET_TYPE_RESERVED = 111,

       HR64_PACKET_TYPE_IMU = 121,
       HR64_PACKET_TYPE_RESERVED_1 = 122,
       HR64_PACKET_TYPE_RESERVED_2 = 123,
       HR64_PACKET_TYPE_RESERVED_3 = 124,
   } fs_packet_type_t;

Packet Families
---------------

The packet types fall into several families. Which family a device sends
depends on the model: Trifecta-K devices use the C-series, Trifecta-M
devices use the C2-series, and devices with a GNSS-synchronized clock use
the 64-bit timestamp variants.

**C-series (Composite)**  
Standard packets used by Trifecta-K0/K1/K2. Include raw IMU data, orientation,
and optional GNSS/INS fields.

**S-series (Simplified)**  
Reserved for future use. Structurally smaller, intended for low-bandwidth
applications.

**C2-series (Composite v2)**  
Extended packets used by Trifecta-M. Include velocity and full WGS84 position.

**HR-series (High Rate)**  
Raw IMU-only packets intended for high-rate control loops.

**64-bit variants**  
Same families as above, but with a 64-bit UTC timestamp instead of a 32-bit RTOS
tick counter.

Composite Packet (C-series)
---------------------------

Size: **145 bytes**

.. code-block:: c

   struct fs_imu_composite_packet {
       uint8_t type;
       uint32_t time;

       float ax0, ay0, az0;
       float gx0, gy0, gz0;

       float ax1, ay1, az1;
       float gx1, gy1, gz1;

       float ax2, ay2, az2;
       float gx2, gy2, gz2;

       float q0, q1, q2, q3;

       float mag_x, mag_y, mag_z;

       float acc_x, acc_y, acc_z;

       float omega_x0, omega_y0, omega_z0;

       int16_t temperature[3];

       int8_t device_motion_status;
       uint8_t diagnostic_flag;

       int8_t reserved[3];
       int8_t c;
       float barometric_pressure;
   };

This packet contains:

- Raw IMU data from three sensors (ax0/1/2, gx0/1/2)
- Orientation quaternion
- Magnetometer
- Compensated acceleration
- Compensated angular velocity
- Temperature array
- Motion status (stationary/moving)
- Diagnostic flags (GNSS/IMU status)
- Barometric pressure

Regular Packet (C-series)
-------------------------

Size: **85 bytes**

A smaller packet containing:

- Angular velocity
- Orientation quaternion
- Magnetometer
- Compensated acceleration
- Temperature
- Motion status
- Diagnostics
- Barometric pressure

Composite Packet v2 (C2-series)
-------------------------------

Size: **181 bytes**

Adds:

- Velocity (ENU frame)
- WGS84 latitude/longitude/height

This is the main packet for Trifecta-M devices.

64-bit Timestamp Variants
-------------------------

Packets ``*_64`` and ``*_64_2`` are identical to their 32-bit counterparts except
for:

.. code-block:: c

   uint64_t time;

This allows devices synchronized to GNSS UTC to report absolute timestamps.

Packet Union
------------

All packet formats are wrapped in a single union:

.. code-block:: c

   typedef union fs_packet_union {
       fs_imu_composite_packet_t composite;
       fs_imu_regular_packet_t regular;
       fs_imu_composite_packet_2_t composite2;
       fs_imu_composite_packet_64_t composite64;
       fs_imu_regular_packet_64_t regular64;
       fs_imu_composite_packet_64_2_t composite64_2;
   } fs_packet_union_t;

The union size is **185 bytes**, matching the largest packet type
(``fs_imu_composite_packet_64_2_t``). The ``_64`` variants are selected by
the device when it has a GNSS-synchronized clock.

The HR (high-rate) packet family exists in the type enum but is not part of
the union, as it is not currently decoded by the driver.

Diagnostic Flags
----------------

Diagnostic flags are reported in ``diagnostic_flag``:

.. code-block:: c

   typedef enum fs_device_diagnostic_flags {
       FS_DEVICE_DIAG_UTC_SYNCED = 0x01,
       FS_DEVICE_DIAG_GNSS_AVAILABLE = 0x02,
       FS_DEVICE_DIAG_IMU_AVAILABLE = 0x04,
       FS_DEVICE_DIAG_GNSS_DGPS = 0x08,
       FS_DEVICE_DIAG_GNSS_RTK_FLOAT = 0x10,
       FS_DEVICE_DIAG_GNSS_RTK_FIX = 0x20,
       FS_DEVICE_DIAG_GNSS_RTK_MB = 0x40,
       FS_DEVICE_DIAG_GNSS_RTK_EXT = 0x80,
   } fs_device_diagnostic_flags_t;

Broadcast Packet
----------------

Used during network discovery:

.. code-block:: c

   struct fs_device_broadcast_information_packet {
       uint32_t magic_number; // == 0x88886868
       char device_name[32];
       char device_ip[39];
   };

``magic_number`` is always ``0x88886868``.
``device_ip`` supports IPv6. However, the Trifecta devices will typically only use IPv4.

