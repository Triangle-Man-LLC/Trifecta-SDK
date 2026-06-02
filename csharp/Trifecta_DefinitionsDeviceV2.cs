using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public enum FsDeviceId
{
    Unknown = 0,
    TK = 1,   // Trifecta-K (IMU)
    TM = 2,   // Trifecta-M (GNSS/INS)
    STV = 2   // Super Trifecta
}

public enum FsRunStatus
{
    Error = -1,
    Idle = 0,
    Running = 1
}

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct FsDriverConfig
{
    [MarshalAs(UnmanagedType.I1)]
    public bool UseSerialInterruptMode;

    public int SerialDataReadyGpio;

    public int BackgroundTaskPriority;
    public int BackgroundTaskCoreAffinity;
    public int ReadTimeoutMicros;
    public int TaskWaitMs;
    public int TaskStackSizeBytes;
}

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct FsDeviceDescriptor
{
    public FsDeviceId DeviceId; // Matches fs_device_id_t

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string DeviceName;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string DeviceFw;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string DeviceDesc;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string DeviceSn;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string DeviceModel;
}

[StructLayout(LayoutKind.Sequential, CharSet = CharSet.Ansi)]
public struct FsDeviceParams
{
    public int CommunicationMode;      // fs_communication_mode_t
    public int Status;                 // fs_run_status_t
    public int AllEnabledInterfaces;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 39)]
    public string IpAddr;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string Ssid;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 32)]
    public string SsidAp;

    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 64)]
    public string PwAp;

    public int TcpPort;
    public int UdpPort;

#if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
    public IntPtr TcpSock;             // fs_sock_t
    public IntPtr UdpSock;             // fs_sock_t
#else
    public IntPtr TcpSock;
    public IntPtr UdpSock;
#endif

    // NEW FIELD: serial_path[128]
    [MarshalAs(UnmanagedType.ByValTStr, SizeConst = 128)]
    public string SerialPath;

#if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
    public IntPtr SerialPort;          // fs_serial_handle_t (opaque)
#else
    public IntPtr SerialPort;
#endif

    public int Baudrate;               // int32_t
    public int Ping;                   // int32_t

    public ulong HpTimestamp;          // uint64_t
}
