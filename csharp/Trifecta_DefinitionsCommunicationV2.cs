using System.Collections;
using System.Collections.Generic;
using System.Runtime.InteropServices;

public static class FsConstants
{
    public const int FS_TRIFECTA_PORT = 8888;
    public const int FS_TRIFECTA_SERIAL_BAUDRATE = 2000000;
    public const int FS_MAX_DATA_LENGTH = 512;

    public const int FS_MAX_CMD_QUEUE_LENGTH = 16;
    public const int FS_MAX_CMD_LENGTH = 72;

    public const char FS_SERIAL_PACKET_HEADER_B64 = ':';
    public const char FS_SERIAL_PACKET_HEADER_BIN = '?';
    public const char FS_SERIAL_PACKET_FOOTER = '!';
    public const char FS_SERIAL_COMMAND_TERMINATOR = ';';
}

public enum FsCommand : byte
{
    CMD_RESTART = (byte)'R',
    CMD_CLEAR_CONFIG = (byte)'C',
    CMD_SET_SSID = (byte)'S',
    CMD_SET_PASSWORD = (byte)'P',
    CMD_SET_SSID_AP = (byte)'q',
    CMD_SET_PASSWORD_AP = (byte)'w',
    CMD_SET_FILT_BETA = (byte)'B',
    CMD_SET_DEV_NAME = (byte)'N',
    CMD_SETUP_FINISH = (byte)'F',

    CMD_IDENTIFY = (byte)'I',
    CMD_IDENTIFY_PARAM_DEV_SN = (byte)'p',
    CMD_IDENTIFY_PARAM_DEVMODEL = (byte)'m',
    CMD_IDENTIFY_PARAM_DEVFWVERSION = (byte)'f',
    CMD_IDENTIFY_PARAM_DEVDESC = (byte)'d',
    CMD_IDENTIFY_PARAM_ACCELRANGE = (byte)'a',
    CMD_IDENTIFY_PARAM_GYRORANGE = (byte)'g',
    CMD_IDENTIFY_PARAM_REFRESHRATE = (byte)'r',
    CMD_IDENTIFY_PARAM_UART_BAUD_RATE = (byte)'b',
    CMD_IDENTIFY_PARAM_SSID = (byte)'s',
    CMD_IDENTIFY_PARAM_SSID_AP = (byte)'1',
    CMD_IDENTIFY_PARAM_PASSWORD_AP = (byte)'3',
    CMD_IDENTIFY_PARAM_TRANSMIT = (byte)'t',

    CMD_REZERO_IMUS = (byte)'Z',
    CMD_TOGGLE_REZERO_AT_START = (byte)'K',

    CMD_REZERO_INS = (byte)'0',
    CMD_SET_YAW_DEG = (byte)'y',

    CMD_STREAM = (byte)'A',
    CMD_SET_LISTENING_PORT = (byte)'l'
}

public enum FsCommunicationMode : int
{
    Uninitialized = -1,
    UsbCdc = 0,
    Uart = 1,
    TcpUdp = 2,
    TcpUdpAp = 4,
    Can = 8,
    I2c = 16,
    EspNow = 32,
    Spi = 64,
    Ble = 128
}

[StructLayout(LayoutKind.Sequential, Pack = 1)]
public struct FsCommandInfo
{
    public FsCommunicationMode Source;

    [MarshalAs(UnmanagedType.ByValArray, SizeConst = FsConstants.FS_MAX_CMD_LENGTH)]
    public byte[] Payload;
}
