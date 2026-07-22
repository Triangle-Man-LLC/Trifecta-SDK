using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

public enum FsPacketType : int
{
    // Standard packet format, used by Trifecta-K0, K1, and K2.
    C_PACKET_TYPE_IMU = 0,
    C_PACKET_TYPE_AHRS = 1,
    C_PACKET_TYPE_INS = 2,
    C_PACKET_TYPE_RESERVED = 3,

    // Simplified packet format, currently reserved.
    S_PACKET_TYPE_IMU = 4,
    S_PACKET_TYPE_AHRS = 5,
    S_PACKET_TYPE_INS = 6,
    S_PACKET_TYPE_RESERVED = 7,

    // Long packet formats, used by Trifecta-M.
    C2_PACKET_TYPE_IMU = 8,
    C2_PACKET_TYPE_AHRS = 9,
    C2_PACKET_TYPE_INS = 10,
    C2_PACKET_TYPE_RESERVED = 11,

    // High-rate packet, comprised of raw data suitable for high-rate controls.
    HR_PACKET_TYPE_IMU = 21,
    HR_PACKET_TYPE_RESERVED_1 = 22,
    HR_PACKET_TYPE_RESERVED_2 = 23,
    HR_PACKET_TYPE_RESERVED_3 = 24,

    // Standard packet format, 64-bit UTC timestamp.
    C64_PACKET_TYPE_IMU = 100,
    C64_PACKET_TYPE_AHRS = 101,
    C64_PACKET_TYPE_INS = 102,
    C64_PACKET_TYPE_RESERVED = 103,

    // Simplified packet format, 64-bit UTC timestamp.
    S64_PACKET_TYPE_IMU = 104,
    S64_PACKET_TYPE_AHRS = 105,
    S64_PACKET_TYPE_INS = 106,
    S64_PACKET_TYPE_RESERVED = 107,

    // Long packet formats, Trifecta-M, 64-bit UTC timestamp.
    C642_PACKET_TYPE_IMU = 108,
    C642_PACKET_TYPE_AHRS = 109,
    C642_PACKET_TYPE_INS = 110,
    C642_PACKET_TYPE_RESERVED = 111,

    // High-rate packet, 64-bit UTC timestamp.
    HR64_PACKET_TYPE_IMU = 121,
    HR64_PACKET_TYPE_RESERVED_1 = 122,
    HR64_PACKET_TYPE_RESERVED_2 = 123,
    HR64_PACKET_TYPE_RESERVED_3 = 124,
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct FsQuaternion
{
    public float W; // Scalar term
    public float X; // Vector term
    public float Y;
    public float Z;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct FsVector3
{
    public float X;
    public float Y;
    public float Z;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct FsVector3D
{
    public double X;
    public double Y;
    public double Z;
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct FsVector3I32
{
    public int X;
    public int Y;
    public int Z;
}

public enum FsGnssPositionFormat : int
{
    FS_GNSS_PF_UNKNOWN = 0,
    FS_GNSS_PF_ECEF = 1,
    FS_GNSS_PF_WGS84 = 2,
    FS_GNSS_PF_FIXED = 3,
}

public enum FsDeviceDiagnosticFlags : byte
{
    FS_DEVICE_DIAG_UTC_SYNCED = 0x01,
    FS_DEVICE_DIAG_GNSS_AVAILABLE = 0x02,
    FS_DEVICE_DIAG_PPS_LOCKED = 0x04,
    FS_DEVICE_DIAG_IMU_HEALTHY = 0x08,
    FS_DEVICE_DIAG_RESERVED_4 = 0x10,
    FS_DEVICE_DIAG_RESERVED_5 = 0x20,
    FS_DEVICE_DIAG_RESERVED_6 = 0x40,
    FS_DEVICE_DIAG_RESERVED_7 = 0x80,
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct FsImuCompositePacket
{
    public byte type;   // uint8_t
    public uint time;   // uint32_t

    public float ax0, ay0, az0, gx0, gy0, gz0;
    public float ax1, ay1, az1, gx1, gy1, gz1;
    public float ax2, ay2, az2, gx2, gy2, gz2;

    public float q0, q1, q2, q3;

    public float mag_x, mag_y, mag_z;

    public float acc_x, acc_y, acc_z;

    public float omega_x0, omega_y0, omega_z0;

    // int16_t temperature[3];
    public fixed short temperature[3];

    // int8_t device_motion_status;
    public sbyte device_motion_status;

    // uint8_t diagnostic_flag;
    public byte diagnostic_flag;

    // int8_t reserved[3];
    public fixed sbyte reserved[3];

    // int8_t c;
    public sbyte c;

    // float barometric_pressure;
    public float barometric_pressure;
    // Size: 145 bytes (matches C)
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct FsImuRegularPacket
{
    public byte type;   // uint8_t
    public uint time;   // uint32_t

    public float omega_x0, omega_y0, omega_z0;

    public float q0, q1, q2, q3;

    public float mag_x, mag_y, mag_z;

    public float acc_x, acc_y, acc_z;

    public float reserved_0_1, reserved_0_2, reserved_0_3;

    // int16_t temperature[3];
    public fixed short temperature[3];

    // int8_t device_motion_status;
    public sbyte device_motion_status;

    // uint8_t diagnostic_flag;
    public byte diagnostic_flag;

    // int8_t reserved[3];
    public fixed sbyte reserved[3];

    // int8_t c;
    public sbyte c;

    // float barometric_pressure;
    public float barometric_pressure;
    // Size: 85 bytes (matches C)
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct FsImuCompositePacket2
{
    public byte type;   // uint8_t
    public uint time;   // uint32_t

    public float ax0, ay0, az0, gx0, gy0, gz0;
    public float ax1, ay1, az1, gx1, gy1, gz1;
    public float ax2, ay2, az2, gx2, gy2, gz2;

    public float q0, q1, q2, q3;

    public float mag_x, mag_y, mag_z;

    public float omega_x0, omega_y0, omega_z0;

    public float acc_x, acc_y, acc_z;

    public float vx, vy, vz;

    public double rx, ry, rz;

    // int16_t temperature[3];
    public fixed short temperature[3];

    // int8_t device_motion_status;
    public sbyte device_motion_status;

    // uint8_t diagnostic_flag;
    public byte diagnostic_flag;

    // int8_t reserved[3];
    public fixed sbyte reserved[3];

    // int8_t c;
    public sbyte c;

    // float barometric_pressure;
    public float barometric_pressure;
    // Size: 181 bytes (matches C)
}

//
// Packet structs (64-bit time)
//

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct FsImuCompositePacket64
{
    public byte type;    // uint8_t
    public ulong time;   // uint64_t

    public float ax0, ay0, az0, gx0, gy0, gz0;
    public float ax1, ay1, az1, gx1, gy1, gz1;
    public float ax2, ay2, az2, gx2, gy2, gz2;

    public float q0, q1, q2, q3;

    public float mag_x, mag_y, mag_z;

    public float acc_x, acc_y, acc_z;

    public float omega_x0, omega_y0, omega_z0;

    // int16_t temperature[3];
    public fixed short temperature[3];

    // int8_t device_motion_status;
    public sbyte device_motion_status;

    // uint8_t diagnostic_flag;
    public byte diagnostic_flag;

    // int8_t reserved[3];
    public fixed sbyte reserved[3];

    // int8_t c;
    public sbyte c;

    // float barometric_pressure;
    public float barometric_pressure;
    // Size: 149 bytes (matches C)
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct FsImuRegularPacket64
{
    public byte type;    // uint8_t
    public ulong time;   // uint64_t

    public float omega_x0, omega_y0, omega_z0;

    public float q0, q1, q2, q3;

    public float mag_x, mag_y, mag_z;

    public float acc_x, acc_y, acc_z;

    public float reserved_0_1, reserved_0_2, reserved_0_3;

    // int16_t temperature[3];
    public fixed short temperature[3];

    // int8_t device_motion_status;
    public sbyte device_motion_status;

    // uint8_t diagnostic_flag;
    public byte diagnostic_flag;

    // int8_t reserved[3];
    public fixed sbyte reserved[3];

    // int8_t c;
    public sbyte c;

    // float barometric_pressure;
    public float barometric_pressure;
    // Size: 89 bytes (matches C)
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public unsafe struct FsImuCompositePacket64_2
{
    public byte type;    // uint8_t
    public ulong time;   // uint64_t

    public float ax0, ay0, az0, gx0, gy0, gz0;
    public float ax1, ay1, az1, gx1, gy1, gz1;
    public float ax2, ay2, az2, gx2, gy2, gz2;

    public float q0, q1, q2, q3;

    public float mag_x, mag_y, mag_z;

    public float omega_x0, omega_y0, omega_z0;

    public float acc_x, acc_y, acc_z;

    public float vx, vy, vz;

    public double rx, ry, rz;

    // int16_t temperature[3];
    public fixed short temperature[3];

    // int8_t device_motion_status;
    public sbyte device_motion_status;

    // uint8_t diagnostic_flag;
    public byte diagnostic_flag;

    // int8_t reserved[3];
    public fixed sbyte reserved[3];

    // int8_t c;
    public sbyte c;

    // float barometric_pressure;
    public float barometric_pressure;
    // Size: 185 bytes (matches C)
}

[StructLayout(LayoutKind.Explicit, Size = 185)]
public struct FsPacketUnion
{
    [FieldOffset(0)]
    public FsImuCompositePacket Composite;

    [FieldOffset(0)]
    public FsImuRegularPacket Regular;

    [FieldOffset(0)]
    public FsImuCompositePacket2 Composite2;

    [FieldOffset(0)]
    public FsImuCompositePacket64 Composite64;

    [FieldOffset(0)]
    public FsImuRegularPacket64 Regular64;

    [FieldOffset(0)]
    public FsImuCompositePacket64_2 Composite64_2;
}
