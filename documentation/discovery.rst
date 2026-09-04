Device discovery
================

Trifecta devices can be found before connecting to them, either over Wi-Fi
(UDP broadcast) or over a serial interface (port scan). Both discovery
functions block for the amount of time you request and fill in a
pre-allocated list of addresses/paths.

Network (Wi-Fi) discovery
-------------------------

.. code-block:: c

    FS_API ssize_t fs_obtain_network_devices_list(
        char ip_addr_list[FS_MAX_NUMBER_DEVICES][FS_MAX_IP_ADDRESS_LENGTH],
        int timeout_micros);

**Parameters**

- ``ip_addr_list`` — pre-allocated list of strings that will receive the
  discovered IP addresses. Allocate at least ``FS_MAX_NUMBER_DEVICES``
  (16) entries, each at least ``FS_MAX_IP_ADDRESS_LENGTH`` (64) characters.
- ``timeout_micros`` — how long to listen for UDP broadcasts, in
  microseconds (e.g. ``500000`` = half a second).

**Returns** the number of devices discovered.

Every call listens for UDP broadcasts that the devices emit continuously, so
you do not need to know the IP addresses in advance. Keep the following in
mind:

- Call this from **at most one thread at a time** — concurrent callers all
  receive the same broadcasts and race on the shared device list.
- Repeatedly calling the function accumulates *newly* discovered devices in
  the list until it is full, so a single generous timeout is usually enough.

.. code-block:: c

    char ips[FS_MAX_NUMBER_DEVICES][FS_MAX_IP_ADDRESS_LENGTH];
    int found = fs_obtain_network_devices_list(ips, 500000);

    if (found > 0) {
        printf("Discovered %d device(s), first one at %s\n", found, ips[0]);
    }

Each broadcast also carries the device name and its IP in a
``fs_device_broadcast_information_packet_t`` (magic number
``0x88886868``); see :doc:`packets` for the layout.

Serial discovery
----------------

.. code-block:: c

    FS_API ssize_t fs_obtain_serial_devices_list(
        char device_path_list[FS_MAX_NUMBER_DEVICES][FS_MAX_DEVICE_PATH_LENGTH],
        int timeout_micros);

**Parameters**

- ``device_path_list`` — pre-allocated list of strings that will receive
  the discovered serial port paths (at least ``FS_MAX_NUMBER_DEVICES``
  entries, each at least ``FS_MAX_DEVICE_PATH_LENGTH`` = 256 characters).
- ``timeout_micros`` — how long to scan, in microseconds. Serial scanning
  can be slow on some platforms, so a longer timeout (e.g. ``1000000`` =
  1 second) is recommended.

**Returns** the number of serial devices discovered.

Platform behavior:

- **Linux** — scans for devices whose paths match ``/dev/ttyACM*`` and
  ``/dev/ttyUSB*``.
- **Windows** — scans for devices whose names match ``COM*``.
- **Embedded platforms** — depends on what the platform backend supports; it
  may not be available at all.

The underlying UDP broadcast listener used for network discovery is exposed
directly as ``fs_listen_for_udp_broadcasts()`` (see
:doc:`api_reference`); use it when you need finer control.
