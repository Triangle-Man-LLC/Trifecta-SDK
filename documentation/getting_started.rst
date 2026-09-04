Getting started
===============

Overview
--------

The Trifecta driver provides a unified C API for working with Trifecta-K,
Trifecta-M, and related IMU/AHRS/INS devices. It covers device discovery,
initialization, streaming, packet parsing, configuration, replay, and data
logging.

Almost every function returns ``0`` on success, and the main handle type is
``fs_device_info_t``.

Supported platforms
-------------------

The driver is portable C and ships with per-platform backend
implementations:

- **Linux** (C/C++) — UDP over Wi-Fi, serial (``/dev/ttyACM*`` / ``/dev/ttyUSB*``)
- **Windows** (C/C++, MSVC or MinGW) — UDP over Wi-Fi, serial (``COM*``)
- **ESP-IDF** (C/C++, ESP32) — UDP, ESP-NOW, UART with interrupt support
- In-progress: **STM32**, **Android (NDK)**, **Python**, **MicroPython**, **C#**

See :doc:`porting` for the steps required to bring the driver to a new
platform.

Installation
------------

The driver is built with CMake. Add the repository to your project as a
subdirectory and link against the ``DriverTrifecta`` target::

    add_subdirectory(path/to/Trifecta-Driver)
    target_link_libraries(my_app PRIVATE DriverTrifecta)

The target exports its include directories, so you only need to include the
user-facing header::

    #include "FS_Trifecta.h"

For ESP-IDF builds, the ``CMakeLists.txt`` detects the IDF environment
automatically.

Basic workflow
--------------

1. (Optional) Discover the device — see :doc:`discovery`.
2. Configure driver parameters with ``fs_set_driver_parameters()`` (optional).
3. Initialize the device, networked or serial — see :doc:`initialization`.
4. Start streaming (``fs_start_stream()``) or poll one-shot readings
   (``fs_read_one_shot()``) — see :doc:`streaming`.
5. Pull packets and extract orientation, acceleration, etc. — see
   :doc:`packets`.
6. Optionally save the data to CSV or replay it later — see :doc:`saving`
   and :doc:`replay`.

Example minimal setup
---------------------

The example below auto-discovers a Trifecta device over Wi-Fi, starts the
data stream, and prints the orientation as Euler angles::

    #include <stdio.h>

    #include "FS_Trifecta.h"

    int main(void) {
        // Statically allocated handle, the preferred usage in C/C++.
        fs_device_info_t dev = {0};

        fs_set_driver_parameters(&dev, NULL);   // default configuration
        fs_initialize_networked(&dev, "\0");    // auto-scan the network

        fs_start_stream(&dev);

        while (1) {
            fs_packet_union_t pkt;
            if (fs_get_raw_packet(&dev, &pkt) == 0) {
                fs_vector3_t euler;
                fs_euler_angles_from_packet(&pkt, &euler);
                printf("Roll: %.2f  Pitch: %.2f  Yaw: %.2f\n",
                       euler.x, euler.y, euler.z);
            }
        }

        fs_stop_stream(&dev);
        fs_closedown(&dev);
        return 0;
    }

For a complete list of every function, see :doc:`api_reference`. For all
data types, see :doc:`types`.
