#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
    #include <direct.h>     // _mkdir()
#else
    #include <sys/stat.h>   // mkdir()
    #include <sys/types.h>
#endif

#include "FS_Trifecta.h"
#include "FS_Trifecta_Saver.h"

static fs_save_config_t default_config = {
    .output_directory = "logs",
    .filename_prefix = "Trifecta_",
    .include_timestamp_in_filename = 1,
    .write_header = 1,
};

static void ensure_dir(const char *path)
{
#if defined(_WIN32)
    _mkdir(path); // returns 0 or -1; ignore "already exists"
#else
    mkdir(path, 0755);
#endif
}

int main()
{
    ensure_dir("logs");

    fs_toggle_logging(1);

    fs_save_device_t save = {0};
    fs_save_init(&save, &default_config);

    fs_device_info_t device = {0};
    strncpy(device.device_descriptor.device_name, "Test_Device", sizeof(device.device_descriptor.device_name));

    fs_save_begin_device(&save, &device);

    fs_packet_union_t packet = {0};
    packet.composite.type = C_PACKET_TYPE_INS;

    for (int i = 0; i < 200; i++)
    {
        packet.composite.time = i * 5;
        int status = fs_save_on_packet(&save, &device, &packet);
        if (status != 0)
        {
            fprintf(stderr, "Error saving packet for device %s - returned %d on iteration %i\n",
                    device.device_descriptor.device_name, status, i);
            fs_save_destroy(&save);
            return -1;
        }
        fs_delay(1); // Simulate some delay between packets
    }

    printf("Test packets saved successfully for device %s\n", device.device_descriptor.device_name);
    fs_delay(20); // Allow some time for the saver thread to flush and close the file
    
    fs_save_end_device(&save, &device);

    fs_save_destroy(&save);
    return 0;
}