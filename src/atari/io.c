#ifdef BUILD_ATARI
/**
 * #FujiNet SIO calls
 */

#include <atari.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <conio.h> // Used for interacting with the standard Atari 'console'
#include <unistd.h> // For sleep
#include "io.h"
#include "globals.h"
#include "screen.h"
#include "bar.h"

static NetConfig nc;
static AdapterConfig adapterConfig;
static SSIDInfo ssidInfo;
NewDisk newDisk;
unsigned char wifiEnabled=true;

// variable to hold various responses that we just need to return a char*.
char response[256];

void set_sio_defaults(void)
{
  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dtimlo = 0x0F; // 15 second timeout
}

bool io_error(void)
{
  return (OS.dcb.dstats > 128);
}

unsigned char io_status(void)
{
}

void io_init(void)
{
  OS.noclik = 0xFF;
  OS.shflok = 0;
  OS.color0 = 0x9f;
  OS.color1 = 0x0f;
  OS.color2 = 0x90;
  OS.color4 = 0x90;
  OS.coldst = 1;
  OS.sdmctl = 0; // Turn off screen
}

bool io_get_wifi_enabled(void)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xEA; // Return wifi status
  OS.dcb.dstats = 0x40; // Peripheral->Computer
  OS.dcb.dbuf = &wifiEnabled;
  OS.dcb.dbyt = 1;
  OS.dcb.daux1 = 0;
  siov();

  if (wifiEnabled == 1)
  {
    return true;
  }
  else
  {
    bar_set_color(COLOR_SETTING_FAILED);
    return false;
  }
}

unsigned char io_get_wifi_status(void)
{
  unsigned char status;

  sleep(1); // give the esp32 a moment to connect

  set_sio_defaults();
  OS.dcb.dcomnd = 0xFA; // Return wifi status
  OS.dcb.dstats = 0x40; // Peripheral->Computer
  OS.dcb.dbuf = &status;
  OS.dcb.dbyt = 1;
  OS.dcb.daux1 = 0;
  siov();

  // Shouldn't do this here, but for now its temporary
  if (status != 3)
  {
    bar_set_color(COLOR_SETTING_FAILED);
  }
  else
  {
    bar_set_color(COLOR_SETTING_SUCCESSFUL);
  }

  return status;
}

NetConfig *io_get_ssid(void)
{

  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dcomnd = 0xFE; // get_ssid
  OS.dcb.dstats = 0x40;
  OS.dcb.dbuf = &nc;
  OS.dcb.dtimlo = 0x0F; // 15 second timeout
  OS.dcb.dbyt = sizeof(nc);
  OS.dcb.daux = 0;
  siov();

  return &nc;
}

unsigned char io_scan_for_networks(void)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xFD; // do scan
  OS.dcb.dstats = 0x40; // Peripheral->Computer
  OS.dcb.dbuf = &response;
  OS.dcb.dbyt = 4; // 4 byte response
  OS.dcb.daux = 0;
  siov();

  return response[0];
}

SSIDInfo *io_get_scan_result(unsigned char n)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xFC; // Return scan result
  OS.dcb.dstats = 0x40; // Peripheral->Computer
  OS.dcb.dbuf = &ssidInfo;
  OS.dcb.dbyt = sizeof(ssidInfo);
  OS.dcb.daux1 = n; // get entry #n
  siov();

  return &ssidInfo;
}

AdapterConfig *io_get_adapter_config(void)
{
  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dcomnd = 0xE8;
  OS.dcb.dstats = 0x40;
  OS.dcb.dbuf = &adapterConfig;
  OS.dcb.dtimlo = 0x0f;
  OS.dcb.dbyt = sizeof(adapterConfig);
  OS.dcb.daux = 0;
  siov();
  return &adapterConfig;
}

void io_set_ssid(NetConfig *nc)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xFB; // Set SSID
  OS.dcb.dstats = 0x80; // Computer->Peripheral
  OS.dcb.dbuf = nc;
  OS.dcb.dbyt = 97; // sizeof(nc);
  OS.dcb.daux = 1;
  siov();
}

void io_get_device_slots(DeviceSlot *d)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xF2;
  OS.dcb.dstats = 0x40;
  OS.dcb.dbuf = d;
  OS.dcb.dbyt = sizeof(deviceSlots);
  OS.dcb.daux = 0;
  siov();
}

void io_get_host_slots(HostSlot *h)
{
  // Query for host slots
  set_sio_defaults();
  OS.dcb.dcomnd = 0xF4; // Get host slots
  OS.dcb.dstats = 0x40;
  OS.dcb.dbuf = h;
  OS.dcb.dbyt = sizeof(hostSlots);
  OS.dcb.daux = 0;
  siov();
}

void io_put_host_slots(HostSlot *h)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xF3;
  OS.dcb.dstats = 0x80;
  OS.dcb.dbuf = &hostSlots;
  OS.dcb.dbyt = 256;
  OS.dcb.daux = 0;
  siov();
}

void io_put_device_slots(DeviceSlot *d)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xF1;
  OS.dcb.dstats = 0x80;
  OS.dcb.dbuf = &deviceSlots;
  OS.dcb.dbyt = sizeof(deviceSlots);
  OS.dcb.daux = 0;
  siov();
}

void io_mount_host_slot(unsigned char hs)
{
  if (hostSlots[hs][0] != 0x00)
  {
    set_sio_defaults();
    OS.dcb.dcomnd = 0xF9;
    OS.dcb.dstats = 0x00;
    OS.dcb.dbuf = NULL;
    OS.dcb.dbyt = 0;
    OS.dcb.daux = hs;
    siov();
  }
}

void io_open_directory(unsigned char hs, char *p, char *f)
{
  set_sio_defaults();

  if (f[0] != 0x00)
  {
    // We have a filter, create a directory+filter string
    memset(response, 0, 256);
    strcpy(response, p);
    strcpy(&response[strlen(response) + 1], f);
    OS.dcb.dbuf = &response;
  }
  else
  {
    OS.dcb.dbuf = p;
  }

  OS.dcb.dcomnd = 0xF7;
  OS.dcb.dstats = 0x80;
  OS.dcb.dbyt = 256;
  OS.dcb.daux = hs;
  siov();
}

char *io_read_directory(unsigned char maxlen, unsigned char a)
{
  memset(response, 0, maxlen);
  response[0] = 0x7f;
  OS.dcb.dcomnd = 0xF6;
  OS.dcb.dstats = 0x40;
  OS.dcb.dbuf = &response;
  OS.dcb.dbyt = maxlen;
  OS.dcb.daux1 = maxlen;
  OS.dcb.daux2 = a;
  siov();

  return &response;
}

void io_close_directory(void)
{
  OS.dcb.dcomnd = 0xF5;
  OS.dcb.dstats = 0x00;
  OS.dcb.dbuf = NULL;
  OS.dcb.dtimlo = 0x0f;
  OS.dcb.dbyt = 0;
  OS.dcb.daux = selected_host_slot;
  siov();
}

void io_set_directory_position(DirectoryPosition pos)
{
  OS.dcb.dcomnd = 0xE4;
  OS.dcb.dstats = 0x00;
  OS.dcb.dbuf = NULL;
  OS.dcb.dbyt = 0;
  OS.dcb.daux = pos;
  siov();
}

void io_set_device_filename(unsigned char ds, char *e)
{
  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dcomnd = 0xE2;
  OS.dcb.dstats = 0x80;
  OS.dcb.dbuf = e;
  OS.dcb.dtimlo = 0x0F;
  OS.dcb.dbyt = 256;
  OS.dcb.daux1 = ds;
  OS.dcb.daux2 = 0;
  siov();
}

char *io_get_device_filename(unsigned char slot)
{
  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dcomnd = 0xDA;
  OS.dcb.dstats = 0x40;
  OS.dcb.dbuf = &response;
  OS.dcb.dtimlo = 0x0F;
  OS.dcb.dbyt = 256;
  OS.dcb.daux1 = slot;
  OS.dcb.daux2 = 0;
  siov();

  return &response;
}

void io_set_boot_config(unsigned char toggle)
{
  OS.dcb.dcomnd = 0xD9;
  OS.dcb.dstats = 0x00;
  OS.dcb.dbuf = NULL;
  OS.dcb.dtimlo = 0x0f;
  OS.dcb.dbyt = 0;
  OS.dcb.daux1 = toggle;
  siov();
}

void io_set_boot_mode(unsigned char mode)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xD6;
  OS.dcb.dstats = 0x00;
  OS.dcb.dbuf = NULL;
  OS.dcb.dtimlo = 0x0f;
  OS.dcb.dbyt = 0;
  OS.dcb.daux1 = mode;
  siov();
}


void io_mount_disk_image(unsigned char ds, unsigned char mode)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xF8;
  OS.dcb.dstats = 0x00;
  OS.dcb.dbuf = NULL;
  OS.dcb.dbyt = 0;
  OS.dcb.dtimlo = 0xFE; // Due to ATX support.
  OS.dcb.daux1 = ds;
  OS.dcb.daux2 = mode;
  siov();
}

void io_umount_disk_image(unsigned char ds)
{
  set_sio_defaults();
  OS.dcb.dcomnd = 0xE9;
  OS.dcb.dstats = 0x00;
  OS.dcb.dbuf = NULL;
  OS.dcb.dbyt = 0;
  OS.dcb.daux = ds;
  siov();
}

void io_boot(void)
{
}

void io_create_new(unsigned char selected_host_slot, unsigned char selected_device_slot, unsigned long selected_size, char *path)
{
  if (selected_size == 999)
  {
    newDisk.numSectors = custom_numSectors;
    newDisk.sectorSize = custom_sectorSize;
  }
  else
  {
    switch (selected_size)
    {
    case 90:
      newDisk.numSectors = 720;
      newDisk.sectorSize = 128;
      break;
    case 130:
      newDisk.numSectors = 1040;
      newDisk.sectorSize = 128;
      break;
    case 180:
      newDisk.numSectors = 720;
      newDisk.sectorSize = 256;
      break;
    case 360:
      newDisk.numSectors = 1440;
      newDisk.sectorSize = 256;
      break;
    case 720:
      newDisk.numSectors = 2880;
      newDisk.sectorSize = 256;
      break;
    case 1440:
      newDisk.numSectors = 5760;
      newDisk.sectorSize = 256;
      break;
    }
  }

  newDisk.hostSlot = selected_host_slot;
  newDisk.deviceSlot = selected_device_slot;
  deviceSlots[selected_device_slot].mode = mode;
  strcpy(newDisk.filename, path);

  set_sio_defaults();
  OS.dcb.dcomnd = 0xE7;
  OS.dcb.dstats = 0x80;
  OS.dcb.dbuf = &newDisk;
  OS.dcb.dtimlo = 0xFE;
  OS.dcb.dbyt = sizeof(newDisk);
  OS.dcb.daux = 0;
  siov();
}

void io_build_directory(unsigned char ds, unsigned long numBlocks, char *v)
{
}

bool io_get_device_enabled_status(unsigned char d)
{
  // adam calls $D1, which doesn't exist in atari.
  return false;
}

void io_update_devices_enabled(bool *e)
{
  int deviceNum;

  // doesn't exist on atari?. Just set to true.
  for (deviceNum = 0; deviceNum < NUM_DEVICE_SLOTS; deviceNum++)
  {
    e[deviceNum] = true;
  }
}

void io_enable_device(unsigned char d)
{
}

void io_disable_device(unsigned char d)
{
}

/**
 * NOTE: On the Fuji, command 0xD8 (copy file) expects the slots to be 1-8, not 0-7 like most things.
 * That's why we add one, since config is tracking the slots as 0-7
 */
void io_copy_file(unsigned char source_slot, unsigned char destination_slot)
{
  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dcomnd = 0xD8;
  OS.dcb.dstats = 0x80;
  OS.dcb.dbuf = &copySpec;
  OS.dcb.dtimlo = 0xFE; // Max timeout
  OS.dcb.dbyt = 256;
  OS.dcb.daux1 = source_slot + 1;
  OS.dcb.daux2 = destination_slot + 1;
  siov();
}

unsigned char io_device_slot_to_device(unsigned char ds)
{
}

/**
 * Get filename for device slot
 */
void io_get_filename_for_device_slot(unsigned char slot, const char *filename)
{
  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dcomnd = 0xDA;
  OS.dcb.dstats = 0x40;
  OS.dcb.dbuf = filename;
  OS.dcb.dtimlo = 0x0F;
  OS.dcb.dbyt = 256;
  OS.dcb.daux1 = slot;
  OS.dcb.daux2 = 0;
  siov();
}

/**
 * Mount all hosts and devices
 */
unsigned char io_mount_all(void)
{
  OS.dcb.ddevic = 0x70;
  OS.dcb.dunit = 1;
  OS.dcb.dcomnd = 0xD7;
  OS.dcb.dstats = 0x00;
  OS.dcb.dbuf = 0x00;
  OS.dcb.dtimlo = 0x0F;
  OS.dcb.dbyt = 0;
  OS.dcb.daux = 0;
  siov();

  return OS.dcb.dstats; // 1 = successful, anything else = error.
}

#endif /* BUILD_ATARI */