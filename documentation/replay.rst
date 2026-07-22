Replaying saved data
====================

``fs_replay_t`` reads CSV files that were produced by :doc:`saving` and
yields them back as decoded ``fs_packet_union_t`` packets, so saved data can
be processed offline exactly like live data.

Replay lifecycle
----------------

.. code-block:: c

    FS_API fs_replay_t *fs_export_allocate_replay();

    FS_API int fs_replay_open(fs_replay_t *r, const char *path,
                              uint32_t sparse_step);

    FS_API size_t fs_replay_get_size(const fs_replay_t *r);

    FS_API int fs_replay_read_next(fs_replay_t *r, fs_packet_union_t *out);

    FS_API int fs_replay_read_line(fs_replay_t *r, uint32_t line_index,
                                   fs_packet_union_t *out);

    FS_API void fs_replay_close(fs_replay_t *r);

    FS_API void fs_export_free_replay(fs_replay_t *replay);

- ``fs_replay_open`` — opens the CSV file at ``path`` (must be a file
  exported by this library). ``sparse_step`` controls the density of the
  index that is built over the file; use
  ``FS_REPLAY_DEFAULT_STEP`` (200) in most cases. A larger step means a
  smaller index (and faster open) at the cost of coarser
  ``fs_replay_read_line()`` seeking.
- ``fs_replay_get_size`` — total number of packets in the file. The file
  must already be opened.
- ``fs_replay_read_next`` — advances to the next row and decodes it into
  ``out``.
- ``fs_replay_read_line`` — jumps to an arbitrary row index and decodes it.
- ``fs_replay_close`` — closes the file and frees the index. **Always call
  this**; ``fs_replay_t`` holds dynamically allocated memory.
- ``fs_export_free_replay`` — frees a *dynamically allocated* handle; call
  it only **after** ``fs_replay_close()``.

Example: iterate a whole capture
--------------------------------

.. code-block:: c

    #include <stdio.h>

    #include "FS_Trifecta.h"

    int main(void) {
        fs_replay_t replay = {0};

        if (fs_replay_open(&replay, "logs/capture.csv",
                           FS_REPLAY_DEFAULT_STEP) != 0) {
            return 1;
        }

        size_t n = fs_replay_get_size(&replay);
        printf("Replay file contains %zu packets\n", n);

        for (uint32_t i = 0; i < n; i++) {
            fs_packet_union_t pkt;
            if (fs_replay_read_next(&replay, &pkt) != 0) {
                break;
            }

            fs_vector3_t euler;
            fs_euler_angles_from_packet(&pkt, &euler);
            printf("%u: roll %.2f, pitch %.2f, yaw %.2f\n",
                   i, euler.x, euler.y, euler.z);
        }

        fs_replay_close(&replay);
        return 0;
    }

The same packet helpers used for live data (see :doc:`streaming`) work on
replayed packets, since both produce ``fs_packet_union_t`` values.
