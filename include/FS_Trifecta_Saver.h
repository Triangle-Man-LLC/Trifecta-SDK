/// Driver for the Trifecta series of IMU/AHRS/INS devices
/// Copyright 2026 4rge.ai and/or Triangle Man LLC
/// Usage and redistribution of this code is permitted
/// but this notice must be retained in all copies of the code.
///
/// THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
/// AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#pragma once

#include "FS_Trifecta_Defs.h"
#include "FS_Trifecta_Interfaces.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /// @brief Configuration for saver
    typedef struct fs_save_config_t
    {
        /// Directory where CSV files will be written (must exist or be creatable)
        const char *output_directory;

        /// Optional filename prefix (e.g. "Trifecta_"); may be NULL
        const char *filename_prefix;

        /// If non-zero, include timestamp (yyyyMMddHHmmss) in filename
        int include_timestamp_in_filename;

        /// If non-zero, write CSV header line at file creation
        int write_header;
    } fs_save_config_t;

    /// @brief Opaque saver context - user should not call these fields directly
    typedef struct fs_save_device_t
    {
        const fs_device_info_t *dev; // pointer only, not owned
        fs_save_config_t config;
        fs_packet_ringbuffer_t packets_to_save; //
        fs_run_status_t running;                //
        fs_thread_t thread_handle;              // Handle for the saver thread
        FILE *file;
    } fs_save_device_t;

    /// @brief Write a single packet for a given device to its CSV file.
    /// This is the function the driver calls from its packet callback.
    /// Returns 0 on success, non-zero on error.
    int fs_save_on_packet(fs_save_device_t *save_context,
                          const fs_device_info_t *dev,
                          const fs_packet_union_t *packet);

#ifdef __cplusplus
}
#endif
