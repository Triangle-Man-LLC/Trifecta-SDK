/// Driver for the Trifecta series of IMU/AHRS/INS devices
/// Copyright 2026 4rge.ai and/or Triangle Man LLC
/// Usage and redistribution of this code is permitted
/// but this notice must be retained in all copies of the code.

/// THIS SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
/// INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
/// AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
/// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "FS_Trifecta.h"

#define LINEBUF_SIZE 1024

static void write_header(FILE *f)
{
    fputs(
        "PacketType,Time,"
        "Ax0,Ay0,Az0,Gx0,Gy0,Gz0,"
        "Ax1,Ay1,Az1,Gx1,Gy1,Gz1,"
        "Ax2,Ay2,Az2,Gx2,Gy2,Gz2,"
        "Q0,Q1,Q2,Q3,"
        "MagX,MagY,MagZ,"
        "OmegaX0,OmegaY0,OmegaZ0,"
        "AccX,AccY,AccZ,"
        "Vx,Vy,Vz,"
        "Rx,Ry,Rz,"
        "Temp_0,Temp_1,Temp_2,"
        "DeviceMotionStatus\n",
        f);
}

fs_save_device_t *fs_export_allocate_save()
{
    return (fs_save_device_t *)calloc(1, sizeof(fs_save_device_t));
}

void fs_export_free_save(fs_save_device_t *save)
{
    if (save)
        free(save);
    save = NULL;
}

int fs_save_init(fs_save_device_t *saver, const fs_save_config_t *config)
{
    if (!saver)
        return -1;

    fs_save_destroy(saver);

    if (config)
        saver->config = *config;
    else
    {
        memset(&saver->config, 0, sizeof(saver->config));
        saver->config.write_header = 1;
        saver->config.include_timestamp_in_filename = 1;
    }

    return 0;
}

void fs_save_destroy(fs_save_device_t *saver)
{
    if (!saver)
        return;

    if (saver->file)
    {
        fflush(saver->file);

        // int fd = fileno(saver->file);
        // if (fd >= 0)
        //     fsync(fd);

        fclose(saver->file);
        saver->file = NULL;
    }
}

static int format_packet(char *buf, size_t buf_size, const fs_packet_union_t *p)
{
    const uint8_t type = p->composite.type;

    // Local scratch fields (empty by default)
    const char *empty = "";

    // All fields as strings
    const char *ax0 = empty, *ay0 = empty, *az0 = empty;
    const char *gx0 = empty, *gy0 = empty, *gz0 = empty;

    const char *ax1 = empty, *ay1 = empty, *az1 = empty;
    const char *gx1 = empty, *gy1 = empty, *gz1 = empty;

    const char *ax2 = empty, *ay2 = empty, *az2 = empty;
    const char *gx2 = empty, *gy2 = empty, *gz2 = empty;

    const char *q0 = empty, *q1 = empty, *q2 = empty, *q3 = empty;

    const char *magX = empty, *magY = empty, *magZ = empty;

    const char *omegaX0 = empty, *omegaY0 = empty, *omegaZ0 = empty;

    const char *accX = empty, *accY = empty, *accZ = empty;

    const char *vx = empty, *vy = empty, *vz = empty;

    const char *rx = empty, *ry = empty, *rz = empty;

    const char *t0 = empty, *t1 = empty, *t2 = empty;

    const char *motion = empty;

    static char fbuf[64][32]; // 64 fields × 32 chars each

    int fi = 0;
#define FMT(dst, fmt, val)                              \
    do                                                  \
    {                                                   \
        snprintf(fbuf[fi], sizeof(fbuf[fi]), fmt, val); \
        dst = fbuf[fi++];                               \
    } while (0)

    switch (type)
    {
    case C_PACKET_TYPE_AHRS:
    case C_PACKET_TYPE_IMU:
    case C_PACKET_TYPE_INS:
    case C_PACKET_TYPE_RESERVED:
        FMT(ax0, "%f", (float)p->composite.ax0);
        FMT(ay0, "%f", (float)p->composite.ay0);
        FMT(az0, "%f", (float)p->composite.az0);
        FMT(gx0, "%f", (float)p->composite.gx0);
        FMT(gy0, "%f", (float)p->composite.gy0);
        FMT(gz0, "%f", (float)p->composite.gz0);
        FMT(ax1, "%f", (float)p->composite.ax1);
        FMT(ay1, "%f", (float)p->composite.ay1);
        FMT(az1, "%f", (float)p->composite.az1);
        FMT(gx1, "%f", (float)p->composite.gx1);
        FMT(gy1, "%f", (float)p->composite.gy1);
        FMT(gz1, "%f", (float)p->composite.gz1);
        FMT(ax2, "%f", (float)p->composite.ax2);
        FMT(ay2, "%f", (float)p->composite.ay2);
        FMT(az2, "%f", (float)p->composite.az2);
        FMT(gx2, "%f", (float)p->composite.gx2);
        FMT(gy2, "%f", (float)p->composite.gy2);
        FMT(gz2, "%f", (float)p->composite.gz2);
        FMT(q0, "%f", (float)p->composite.q0);
        FMT(q1, "%f", (float)p->composite.q1);
        FMT(q2, "%f", (float)p->composite.q2);
        FMT(q3, "%f", (float)p->composite.q3);
        FMT(magX, "%f", (float)p->composite.mag_x);
        FMT(magY, "%f", (float)p->composite.mag_y);
        FMT(magZ, "%f", (float)p->composite.mag_z);
        FMT(omegaX0, "%f", (float)p->composite.omega_x0);
        FMT(omegaY0, "%f", (float)p->composite.omega_y0);
        FMT(omegaZ0, "%f", (float)p->composite.omega_z0);
        FMT(accX, "%f", (float)p->composite.acc_x);
        FMT(accY, "%f", (float)p->composite.acc_y);
        FMT(accZ, "%f", (float)p->composite.acc_z);
        FMT(t0, "%f", (float)p->composite.temperature[0]);
        FMT(t1, "%f", (float)p->composite.temperature[1]);
        FMT(t2, "%f", (float)p->composite.temperature[2]);
        FMT(motion, "%u", p->composite.device_motion_status);
        break;

    case S_PACKET_TYPE_AHRS:
    case S_PACKET_TYPE_IMU:
    case S_PACKET_TYPE_INS:
    case S_PACKET_TYPE_RESERVED:
        FMT(omegaX0, "%f", (float)p->regular.omega_x0);
        FMT(omegaY0, "%f", (float)p->regular.omega_y0);
        FMT(omegaZ0, "%f", (float)p->regular.omega_z0);
        FMT(q0, "%f", (float)p->regular.q0);
        FMT(q1, "%f", (float)p->regular.q1);
        FMT(q2, "%f", (float)p->regular.q2);
        FMT(q3, "%f", (float)p->regular.q3);
        FMT(magX, "%f", (float)p->regular.mag_x);
        FMT(magY, "%f", (float)p->regular.mag_y);
        FMT(magZ, "%f", (float)p->regular.mag_z);
        FMT(accX, "%f", (float)p->regular.acc_x);
        FMT(accY, "%f", (float)p->regular.acc_y);
        FMT(accZ, "%f", (float)p->regular.acc_z);
        FMT(t0, "%f", (float)p->composite.temperature[0]);
        FMT(t1, "%f", (float)p->composite.temperature[1]);
        FMT(t2, "%f", (float)p->composite.temperature[2]);
        FMT(motion, "%u", p->regular.device_motion_status);
        break;

    case C2_PACKET_TYPE_AHRS:
    case C2_PACKET_TYPE_IMU:
    case C2_PACKET_TYPE_INS:
    case C2_PACKET_TYPE_RESERVED:
        FMT(ax0, "%f", (float)p->composite2.ax0);
        FMT(ay0, "%f", (float)p->composite2.ay0);
        FMT(az0, "%f", (float)p->composite2.az0);
        FMT(gx0, "%f", (float)p->composite2.gx0);
        FMT(gy0, "%f", (float)p->composite2.gy0);
        FMT(gz0, "%f", (float)p->composite2.gz0);
        FMT(ax1, "%f", (float)p->composite2.ax1);
        FMT(ay1, "%f", (float)p->composite2.ay1);
        FMT(az1, "%f", (float)p->composite2.az1);
        FMT(gx1, "%f", (float)p->composite2.gx1);
        FMT(gy1, "%f", (float)p->composite2.gy1);
        FMT(gz1, "%f", (float)p->composite2.gz1);
        FMT(ax2, "%f", (float)p->composite2.ax2);
        FMT(ay2, "%f", (float)p->composite2.ay2);
        FMT(az2, "%f", (float)p->composite2.az2);
        FMT(gx2, "%f", (float)p->composite2.gx2);
        FMT(gy2, "%f", (float)p->composite2.gy2);
        FMT(gz2, "%f", (float)p->composite2.gz2);
        FMT(q0, "%f", (float)p->composite2.q0);
        FMT(q1, "%f", (float)p->composite2.q1);
        FMT(q2, "%f", (float)p->composite2.q2);
        FMT(q3, "%f", (float)p->composite2.q3);
        FMT(magX, "%f", (float)p->composite2.mag_x);
        FMT(magY, "%f", (float)p->composite2.mag_y);
        FMT(magZ, "%f", (float)p->composite2.mag_z);
        FMT(omegaX0, "%f", (float)p->composite2.omega_x0);
        FMT(omegaY0, "%f", (float)p->composite2.omega_y0);
        FMT(omegaZ0, "%f", (float)p->composite2.omega_z0);
        FMT(accX, "%f", (float)p->composite2.acc_x);
        FMT(accY, "%f", (float)p->composite2.acc_y);
        FMT(accZ, "%f", (float)p->composite2.acc_z);
        FMT(vx, "%f", (float)p->composite2.vx);
        FMT(vy, "%f", (float)p->composite2.vy);
        FMT(vz, "%f", (float)p->composite2.vz);
        FMT(rx, "%f", (double)p->composite2.rx);
        FMT(ry, "%f", (double)p->composite2.ry);
        FMT(rz, "%f", (double)p->composite2.rz);
        FMT(t0, "%f", (float)p->composite.temperature[0]);
        FMT(t1, "%f", (float)p->composite.temperature[1]);
        FMT(t2, "%f", (float)p->composite.temperature[2]);
        FMT(motion, "%u", p->composite2.device_motion_status);
        break;
    }
#undef FMT

    return snprintf(
        buf, buf_size,
        "%u,%lu,"
        "%s,%s,%s,%s,%s,%s,"
        "%s,%s,%s,%s,%s,%s,"
        "%s,%s,%s,%s,%s,%s,"
        "%s,%s,%s,%s,"
        "%s,%s,%s,"
        "%s,%s,%s,"
        "%s,%s,%s,"
        "%s,%s,%s,"
        "%s,%s,%s,"
        "%s,%s,%s,"
        "%s\n",
        type, (uint64_t)p->composite.time,
        ax0, ay0, az0, gx0, gy0, gz0,
        ax1, ay1, az1, gx1, gy1, gz1,
        ax2, ay2, az2, gx2, gy2, gz2,
        q0, q1, q2, q3,
        magX, magY, magZ,
        omegaX0, omegaY0, omegaZ0,
        accX, accY, accZ,
        vx, vy, vz,
        rx, ry, rz,
        t0, t1, t2,
        motion);
}

static fs_thread_func_t save_thread_func(void *arg)
{
    fs_save_device_t *d = (fs_save_device_t *)arg;
    if (!d || !d->file)
        return FS_THREAD_RETVAL;

    fprintf(stderr, "THREAD START: d=%p file=%p running=%d\n",
            (void *)d, (void *)d->file, d->running);

    // Enable large buffered writes
    // setvbuf(d->file, NULL, _IOFBF, 64 * 1024);

    // write_header(d->file);

    fs_packet_union_t packet = {0};
    char linebuf[1024] = {0};
    int flush_counter = 0;

    while (d->running == FS_RUN_STATUS_RUNNING)
    {
        while (FS_RINGBUFFER_POP(&d->packets_to_save, FS_MAX_PACKET_QUEUE_LENGTH, &packet))
        {
            if (format_packet(linebuf, sizeof(linebuf), &packet) > 0)
            {
                fprintf(d->file, "%s", linebuf);

                if (++flush_counter >= 200)
                {
                    fflush(d->file);
                    flush_counter = 0;
                }
            }
        }

        fs_delay(1);
    }
    fflush(d->file);
    fclose(d->file);
    d->file = NULL;
    return FS_THREAD_RETVAL;
}
int fs_save_begin_device(fs_save_device_t *saver, fs_device_info_t *dev)
{
    if (!saver || !dev)
        return -1;

    // If already saving for this device, do nothing
    if (dev->save_context && ((fs_save_device_t *)(dev->save_context))->file)
        return 0;

    char filename[256] = {0};
    char fullpath[512] = {0};

    // Build filename
    if (saver->config.include_timestamp_in_filename)
    {
        fs_tm_t tmv;
        fs_get_local_time(&tmv);

        snprintf(filename, sizeof(filename),
                 "%s%s_%04d%02d%02d%02d%02d%02d.csv",
                 saver->config.filename_prefix ? saver->config.filename_prefix : "",
                 dev->device_descriptor.device_name,
                 tmv.year, tmv.month, tmv.day,
                 tmv.hour, tmv.min, tmv.sec);
    }
    else
    {
        snprintf(filename, sizeof(filename),
                 "%s%s.csv",
                 saver->config.filename_prefix ? saver->config.filename_prefix : "",
                 dev->device_descriptor.device_name);
    }

    snprintf(fullpath, sizeof(fullpath), "%s/%s",
             saver->config.output_directory ? saver->config.output_directory : ".",
             filename);

    // Open file
    saver->file = fopen(fullpath, "w");
    if (!saver->file)
        return -3;

    // Write header if requested
    if (saver->config.write_header)
        write_header(saver->file);

    // Attach saver to device so packet callback can use it
    dev->save_context = saver;

    return 0;
}

int fs_save_end_device(fs_save_device_t *saver, fs_device_info_t *dev)
{
    if (!saver || !dev)
        return -1;

    // Detach saver from device
    dev->save_context = NULL;

    if (saver->file)
    {
        fflush(saver->file);

        // int fd = fileno(saver->file);
        // fsync(fd);

        fclose(saver->file);
        saver->file = NULL;
    }

    return 0;
}

int fs_save_on_packet(fs_save_device_t *saver,
                      const fs_device_info_t *dev,
                      const fs_packet_union_t *packet)
{
    if (!saver || !dev || !packet)
        return -1;

    if (!saver->file)
        return -2;

    char linebuf[1024];

    int n = format_packet(linebuf, sizeof(linebuf), packet);
    if (n <= 0)
        return -3;

    if (fputs(linebuf, saver->file) < 0)
        return -4;

    return 0;
}
