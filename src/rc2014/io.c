#ifdef BUILD_RC2014
/**
 * FujiNet CONFIG for RC2014
 *
 * I/O Routines
 */

#include <conio.h> // for sleep() 
#include <stdlib.h>
#include <string.h>
#include "io.h"
#include "globals.h"
#include "fujinet.h"
#include "fujinet_device.h"

#define FUJI_DEV 0x0F
#undef MOCK_WIFI
#define MOCK_DEVICES
#define MOCK_HOSTS

static uint8_t response[1024];
static FUJINET_RC last_rc = FUJINET_RC_OK;

extern unsigned char source_path;
extern unsigned char path;

#ifdef MOCK_WIFI
AdapterConfig mock_cfg = {
        "my-test-network",
        "localhost",
        { 192, 168, 0, 100 },
        { 192, 168, 0, 1 },
        { 255, 255, 255, 0 },
        { 8, 8, 8, 8 },
        { 1, 2, 3, 4, 5, 6 },
        { 81, 82, 83, 84, 85, 86 },
        "1.0.0-rc2014"
};

SSIDInfo mock_ssid[] = {
        { "my-test-wifi", -30 },
        { "ssid 1", -40 },
        { "Wifi 2", -80 }
};
int mock_ssid_num = 3;

NetConfig mock_netconfig = {
        "my-test-wifi", "password"
};
#endif

#ifdef MOCK_DEVICES
DeviceSlot mock_devices[8] = {
        { 0, 1, "TEST.DSK" },
        { 1, 0, {} },
        { 2, 0, {} },
        { 3, 0, {} },
        { 4, 0, {} },
        { 5, 0, {} },
        { 6, 0, {} },
        { 7, 0, {} }
};
#endif

#ifdef MOCK_HOSTS
HostSlot mock_hosts[8] = {
        { "rc2014-apps.irata.online" },
        { {} },
        { {} },
        { {} },
        { {} },
        { {} },
        { {} },
        { {} }
};
#endif


void io_init(void)
{
  fujinet_init();
}

unsigned char io_status(void)
{
  return last_rc;
}

bool io_error(void)
{
  return last_rc != FUJINET_RC_OK;
}

unsigned char io_get_wifi_status(void)
{
  sleep(1);

#ifdef MOCK_WIFI
  last_rc = FUJINET_RC_OK;
  return 0;
#else
  last_rc = fujinet_get_wifi_status(response);
  return response[0];
#endif
}

NetConfig* io_get_ssid(void)
{
#ifdef MOCK_WIFI
  last_rc = FUJINET_RC_OK;
  return mock_netconfig;
#else
  last_rc = fujinet_get_ssid(response);
  return (NetConfig *)response;
#endif
}

unsigned char io_scan_for_networks(void)
{
#ifdef MOCK_WIFI
  last_rc = FUJINET_RC_OK;
  return mock_ssid_num;
#else
  last_rc = fujinet_scan_for_networks(response);
  return response[0];
#endif
}

SSIDInfo *io_get_scan_result(unsigned char n)
{
#ifdef MOCK_WIFI
  last_rc = FUJINET_RC_OK;
  return &mock_ssid[n];
#else
  last_rc = fujinet_get_scan_result(n, response);
  return (SSIDInfo *)response;
#endif
}

AdapterConfig *io_get_adapter_config(void)
{
#ifdef MOCK_WIFI
  last_rc = FUJINET_RC_OK;
  return mock_cfg;
#else
  last_rc = fujinet_dcb_exec(response);
  return (AdapterConfig *)response;
#endif
}

void io_set_ssid(NetConfig *nc)
{
#ifdef MOCK_WIFI
  memcpy(&mock_cfg, nc, sizeof(NetConfig));
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_set_ssid(nc);
#endif
}

void io_get_device_slots(DeviceSlot *d)
{
#ifdef MOCK_DEVICES
  memcpy(d,mock_devices,sizeof(DeviceSlot) * 8);
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_get_device_slots(d);
#endif
}

void io_get_host_slots(HostSlot *h)
{
#ifdef MOCK_DEVICES
  memcpy(h,mock_hosts,sizeof(HostSlot) * 8);
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_get_host_slots(h);
#endif
}

void io_put_host_slots(HostSlot *h)
{
#ifdef MOCK_DEVICES
  memcpy(mock_hosts, h, sizeof(HostSlot) * 8);
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_put_host_slots(h);
#endif
}

void io_put_device_slots(DeviceSlot *d)
{
#ifdef MOCK_DEVICES
  memcpy(mock_hosts, d, sizeof(DeviceSlot) * 8);
  last_rc = FUJINET_RC_OK;
#else
  last_rc = io_put_device_slots(d);
#endif

  csleep(10);
}

void io_mount_host_slot(unsigned char hs)
{
#ifdef MOCK_DEVICES
  /* do nothing */
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_mount_host_slot(hs);
#endif
}

void io_open_directory(unsigned char hs, char *p, char *f)
{
#ifdef MOCK_DEVICES
  /* do nothing */
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_open_directory(hs, p, f);
#endif
}

char *io_read_directory(unsigned char l, unsigned char a)
{
#ifdef MOCK_DEVICES
  /* do nothing */
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_read_directory(response, l, a);
#endif

  return response;
}

void io_close_directory(void)
{
#ifdef MOCK_DEVICES
  /* do nothing */
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_close_directory();
#endif
}

void io_set_directory_position(DirectoryPosition pos)
{
#ifdef MOCK_DEVICES
  /* do nothing */
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_set_directory_position(pos);
#endif
}

void io_set_device_filename(unsigned char ds, char* e)
{
  struct fujinet_dcb dcb = {};

  dcb.device = 0x70;
  dcb.command = 0xE4;
  dcb.timeout = 15;
  dcb.buffer = (uint8_t *)e;
  dcb.buffer_bytes = 256;
  dcb.aux1 = ds;

#ifdef MOCK_DEVICES
  /* do nothing */
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_dcb_exec(&dcb);
#endif

  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

char *io_get_device_filename(unsigned char ds)
{
  struct fujinet_dcb dcb = {};

  dcb.device = 0x70;
  dcb.command = 0xDA;
  dcb.timeout = 15;
  dcb.buffer = response;
  dcb.buffer_bytes = 256;
  dcb.aux1 = ds;

#ifdef MOCK_DEVICES
  /* do nothing */
  last_rc = FUJINET_RC_OK;
#else
  last_rc = fujinet_dcb_exec(&dcb);
#endif

  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
  return response;
}

void io_create_new(unsigned char selected_host_slot,unsigned char selected_device_slot,unsigned long selected_size,char *path)
{
  char nd[263]={0xE7,0x00,0x00,0x00,0x00,0x00,0x00};
  char *c = &nd[3];
  unsigned long *l = (unsigned long *)c;
  
  nd[1]=selected_host_slot;
  nd[2]=selected_device_slot;
  *l=selected_size;
  strcpy(&nd[7],path);

//  eos_write_character_device(FUJI_DEV,&nd,sizeof(nd));
  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

void io_mount_disk_image(unsigned char ds, unsigned char mode)
{
  char c[3]={0xF8,0x00,0x00};
  c[1]=ds;
  c[2]=mode;

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

void io_set_boot_config(unsigned char toggle)
{
  char c[2]={0xD9,0x00};
  c[1]=toggle;

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

void io_umount_disk_image(unsigned char ds)
{
  char c[2]={0xE9,0x00};
  c[1]=ds;

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

void io_boot(void)
{
//  eos_init();
  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

void io_build_directory(unsigned char ds, unsigned long numBlocks, char *v)
{
  unsigned int nb = numBlocks;

  // End volume label
  v[strlen(v)]=0x03;

  // Adjust device slot to EOS device #
  ds += 4;

  // Set up block 0 to boot right into SmartWriter
  memset(response,0,1024);
  response[0]=0xC3;
  response[1]=0xE7;
  response[2]=0xFC;
//  eos_write_block(ds,0,&response[0]);
//  eos_write_block(ds,0,&response[0]);
//  eos_write_block(ds,0,&response[0]);
//  eos_write_block(ds,0,&response[0]);
//  eos_write_block(ds,0,&response[0]);
//  eos_write_block(ds,0,&response[0]);
//  eos_write_block(ds,0,&response[0]);
//  eos_write_block(ds,0,&response[0]);

  // Write directory
//  eos_initialize_directory(ds, 1, nb, v);
//  eos_initialize_directory(ds, 1, nb, v);
  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

bool io_get_device_enabled_status(unsigned char d)
{
  struct
  {
    unsigned char cmd;
    unsigned char dev;
  } ds;

  ds.cmd = 0xD1; // Get Device status
  ds.dev = d;

//  eos_write_character_device(FUJI_DEV,ds,sizeof(ds));
//  eos_read_character_device(FUJI_DEV,response,sizeof(response));

  last_rc = FUJINET_RC_NOT_IMPLEMENTED;

  return response[0];
}

void io_enable_device(unsigned char d)
{
  struct
  {
    unsigned char cmd;
    unsigned char dev;
  } ed;

  ed.cmd = 0xD5;
  ed.dev = d;

//  eos_write_character_device(FUJI_DEV,ed,sizeof(ed));
  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

void io_disable_device(unsigned char d)
{
  struct
  {
    unsigned char cmd;
    unsigned char dev;
  } dd;

  dd.cmd = 0xD4;
  dd.dev = d;

//  eos_write_character_device(FUJI_DEV,dd,sizeof(dd));
  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

void io_update_devices_enabled(bool *e)
{
  char i;

  for (i=0;i<4;i++)
  {
    e[i]=io_get_device_enabled_status(io_device_slot_to_device(i));
  }
}

void io_copy_file(unsigned char source_slot, unsigned char destination_slot)
{
  char cf[259]={0xD8,0x00,0x00};
  
  cf[1]=source_slot;
  cf[2]=destination_slot;
  strcpy(&cf[3],copySpec);
  
//  eos_write_character_device(FUJI_DEV,cf,sizeof(cf));
  last_rc = FUJINET_RC_NOT_IMPLEMENTED;
}

unsigned char io_device_slot_to_device(unsigned char ds)
{
  return ds+4;
}

bool io_get_wifi_enabled(void)
{
	return true;
}

#endif /* BUILD_RC2014 */
