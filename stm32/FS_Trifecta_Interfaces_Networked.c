#include "FS_Trifecta_Interfaces.h"

#include <string.h>
#include <stdio.h>

ssize_t fs_listen_for_udp_broadcasts(char ip_addr_list[FS_MAX_NUMBER_DEVICES][64],
                                     int timeout_micros)
{
    (void)ip_addr_list;
    (void)timeout_micros;
    return -1;
}

int fs_init_network_tcp_driver(fs_device_info_t *device_handle)
{
    (void)device_handle;
    return -1;
}

int fs_init_network_udp_driver(fs_device_info_t *device_handle)
{
    (void)device_handle;
    return -1;
}

ssize_t fs_transmit_networked_tcp(fs_device_info_t *device_handle,
                                  void *tx_buffer,
                                  size_t length_bytes,
                                  int timeout_micros)
{
    (void)device_handle;
    (void)tx_buffer;
    (void)length_bytes;
    (void)timeout_micros;
    return -1;
}

ssize_t fs_transmit_networked_udp(fs_device_info_t *device_handle,
                                  void *tx_buffer,
                                  size_t length_bytes,
                                  int timeout_micros)
{
    (void)device_handle;
    (void)tx_buffer;
    (void)length_bytes;
    (void)timeout_micros;
    return -1;
}

ssize_t fs_receive_networked_tcp(fs_device_info_t *device_handle,
                                 void *rx_buffer,
                                 size_t length_bytes,
                                 int timeout_micros)
{
    (void)device_handle;
    (void)rx_buffer;
    (void)length_bytes;
    (void)timeout_micros;
    return -1;
}

ssize_t fs_receive_networked_udp(fs_device_info_t *device_handle,
                                 void *rx_buffer,
                                 size_t length_bytes,
                                 int timeout_micros)
{
    (void)device_handle;
    (void)rx_buffer;
    (void)length_bytes;
    (void)timeout_micros;
    return -1;
}

int fs_shutdown_network_tcp_driver(fs_device_info_t *device_handle)
{
    (void)device_handle;
    return -1;
}

int fs_shutdown_network_udp_driver(fs_device_info_t *device_handle)
{
    (void)device_handle;
    return -1;
}

int fs_attempt_reconnect_network_tcp(fs_device_info_t *device_handle)
{
    (void)device_handle;
    return -1;
}

int fs_attempt_reconnect_network_udp(fs_device_info_t *device_handle)
{
    (void)device_handle;
    return -1;
}
