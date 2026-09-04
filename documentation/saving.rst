Saving data
===========

The driver can log every received packet to a CSV file, suitable for later
offline analysis and for :doc:`replay`.

Saver lifecycle
---------------

.. code-block:: c

    FS_API fs_save_device_t *fs_export_allocate_save();

    FS_API int fs_save_init(fs_save_device_t *device,
                            const fs_save_config_t *cfg);

    FS_API int fs_save_begin_device(fs_save_device_t *device,
                                    fs_device_info_t *dev);

    FS_API int fs_save_end_device(fs_save_device_t *device,
                                  fs_device_info_t *dev);

    FS_API void fs_save_destroy(fs_save_device_t *device);

    FS_API void fs_export_free_save(fs_save_device_t *device);

- ``fs_save_init`` — creates/initializes a saver context. Pass ``NULL`` for
  ``cfg`` to use defaults. Returns ``0`` on success.
- ``fs_save_begin_device`` — starts saving for a specific device; the CSV
  filename and metadata are derived from the device's name and description.
  The driver hooks the saver into its packet callback from this point on.
- ``fs_save_end_device`` — stops saving for that device and closes the file.
  Safe to call multiple times.
- ``fs_save_destroy`` — destroys the context and closes any open file.
  Call it before ``fs_export_free_save()`` when using a dynamically
  allocated handle.

As with the device and replay handles, the *preferred* usage in native
C/C++ is a statically allocated ``fs_save_device_t``;
``fs_export_allocate_save()`` / ``fs_export_free_save()`` exist for external
language bindings.

Configuration
-------------

.. code-block:: c

    typedef struct fs_save_config_t
    {
        const char output_directory[256];        // directory to write CSVs into
        const char filename_prefix[128];         // optional filename prefix, e.g. "Trifecta_"
        int include_timestamp_in_filename;       // non-zero: append yyyyMMddHHmmss
        int write_header;                        // non-zero: write a CSV header line
    } fs_save_config_t;

The ``output_directory`` must exist or be creatable. If
``include_timestamp_in_filename`` is non-zero, a ``yyyyMMddHHmmss`` suffix
is appended so concurrent runs never collide.

CSV format
----------

Each saved file contains one row per packet with the following columns (a
header line is written when ``write_header`` is set):

.. code-block:: text

    PacketType,Time,Ax0,Ay0,Az0,Gx0,Gy0,Gz0,Ax1,Ay1,Az1,Gx1,Gy1,Gz1,Ax2,Ay2,Az2,Gx2,Gy2,Gz2,Q0,Q1,Q2,Q3,MagX,MagY,MagZ,OmegaX0,OmegaY0,OmegaZ0,AccX,AccY,AccZ,Vx,Vy,Vz,Rx,Ry,Rz,Temp_0,Temp_1,Temp_2,DeviceMotionStatus

Columns that a given packet type does not carry (e.g. ``Vx``/``Vy``/``Vz``
and ``Rx``/``Ry``/``Rz`` on Trifecta-K devices) are left empty. Fields are
in the units of the corresponding packet fields (see :doc:`packets`);
``Time`` is the device timestamp in ticks (milliseconds), and
``DeviceMotionStatus`` is 0 = no status, 1 = stationary, 2 = in motion.

A typical session writes files like
``Trifecta__My_Device_20260718123349.csv`` into the configured directory.
The files are plain CSV and can be loaded in any spreadsheet tool or in
Python (e.g. ``pandas``) for post-processing.
