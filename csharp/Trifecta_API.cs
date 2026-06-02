using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

using FsDeviceInfo = System.IntPtr;
public static class Trifecta_API
{
#if UNITY_STANDALONE_WIN || UNITY_EDITOR_WIN
    private const string NativeLib = "DriverTrifecta.dll";

#elif UNITY_STANDALONE_LINUX || UNITY_EDITOR_LINUX
    private const string NativeLib = "libDriverTrifecta.so";

#elif UNITY_STANDALONE_OSX || UNITY_EDITOR_OSX
    private const string NativeLib = "libDriverTrifecta.dylib";

#else
    // Non‑Unity: runtime OS detection
    private static readonly string NativeLib =
        RuntimeInformation.IsOSPlatform(OSPlatform.Windows) ? "DriverTrifecta.dll" :
        RuntimeInformation.IsOSPlatform(OSPlatform.Linux)   ? "libDriverTrifecta.so" :
        RuntimeInformation.IsOSPlatform(OSPlatform.OSX)     ? "libDriverTrifecta.dylib" :
        "DriverTrifecta";
#endif

    // Device allocation and lifecycle
    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern FsDeviceInfo fs_export_allocate_device();

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern void fs_export_free_device(FsDeviceInfo device);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_set_driver_parameters(FsDeviceInfo device, ref FsDriverConfig config);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_initialize_networked(FsDeviceInfo device, [MarshalAs(UnmanagedType.LPStr)] string ipAddress);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Ansi)]
    public static extern int fs_initialize_serial(FsDeviceInfo device, string port, FsCommunicationMode mode);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_closedown(FsDeviceInfo device);

    // Stream control
    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_start_stream(FsDeviceInfo device);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_stop_stream(FsDeviceInfo device);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_read_one_shot(FsDeviceInfo device);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_reboot_device(FsDeviceInfo device);

    // Data export/viewing
    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_last_timestamp(FsDeviceInfo device, out uint time);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_raw_packet(FsDeviceInfo device, ref FsPacketUnion packetBuffer);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_raw_packet_queue_size(FsDeviceInfo device);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_raw_packet_from_queue(FsDeviceInfo device, ref FsPacketUnion packetBuffer, int pos);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_orientation(FsDeviceInfo device, out FsQuaternion orientationBuffer);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_orientation_euler(FsDeviceInfo device, out FsVector3 orientationBuffer, bool degrees);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_acceleration(FsDeviceInfo device, out FsVector3 accelerationBuffer);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_angular_velocity(FsDeviceInfo device, out FsVector3 angularVelocityBuffer);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_velocity(FsDeviceInfo device, out FsVector3 velocityBuffer);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_movement_state(FsDeviceInfo device, out FsRunStatus deviceStateBuffer);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_position(FsDeviceInfo device, out FsVector3 positionBuffer);
    
    // Device configuration methods
    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_set_ahrs_heading(FsDeviceInfo device, float headingDeg);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_set_ins_position(FsDeviceInfo device, ref FsVector3 position);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_set_device_name(FsDeviceInfo device, [MarshalAs(UnmanagedType.LPStr)] string name);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_set_communication_mode(FsDeviceInfo device, int modes);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_set_network_parameters(
        FsDeviceInfo device,
        [MarshalAs(UnmanagedType.LPStr)] string ssid,
        [MarshalAs(UnmanagedType.LPStr)] string pw,
        bool accessPoint);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_set_network_udp_port(FsDeviceInfo device, int port);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_set_serial_uart_baudrate(FsDeviceInfo device, int baudrate);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_device_operating_state(FsDeviceInfo device, ref FsDeviceParams deviceParamsInfo);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_get_device_descriptors(FsDeviceInfo device, ref FsDeviceDescriptor desc);

    // Debug utilities
    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_enable_logging(bool doEnable);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_enable_logging_at_path([MarshalAs(UnmanagedType.LPStr)] string path, bool doEnable);

    [DllImport(NativeLib, CallingConvention = CallingConvention.Cdecl)]
    public static extern int fs_factory_reset(FsDeviceInfo device);
}