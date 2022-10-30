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

#define FUJI_DEV 0x0F
#define MOCK_WIFI

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
  struct fujinet_dcb dcb = {};

  dcb.command = 0xFA;
  dcb.response = response;
  dcb.response_bytes = 1;
  dcb.timeout = 15;

  sleep(1);
#ifdef MOCK_WIFI
  return 0;
#else
  fujinet_dcb_exec(&dcb);

  return response[0];
#endif
}

NetConfig* io_get_ssid(void)
{
  struct fujinet_dcb dcb = {};

  dcb.command = 0xFE;
  dcb.response = response;
  dcb.response_bytes = sizeof(NetConfig);
  dcb.timeout = 15;


#ifdef MOCK_WIFI
  return mock_netconfig;
#else
  fujinet_dcb_exec(&dcb);

  return (NetConfig *)response;
#endif
}

unsigned char io_scan_for_networks(void)
{
  struct fujinet_dcb dcb = {};

  dcb.command = 0xFD;
  dcb.response = response;
  dcb.response_bytes = 4;
  dcb.timeout = 15;


#ifdef MOCK_WIFI
  return mock_ssid_num;
#else
  fujinet_dcb_exec(&dcb);

  return response[0];
#endif
}

SSIDInfo *io_get_scan_result(unsigned char n)
{
  struct fujinet_dcb dcb = {};

  dcb.command = 0xFC;
  dcb.response = response;
  dcb.response_bytes = sizeof(SSIDInfo);
  dcb.timeout = 15;
  dcb.aux1 = n;

#ifdef MOCK_WIFI
  return &mock_ssid[n];
#else
  fujinet_dcb_exec(&dcb);

  return (SSIDInfo *)response;
#endif
}

AdapterConfig *io_get_adapter_config(void)
{
  struct fujinet_dcb dcb = {};

  dcb.command = 0xE8;
  dcb.response = response;
  dcb.response_bytes = sizeof(AdapterConfig);
  dcb.timeout = 15;


#ifdef MOCK_WIFI
  return mock_cfg;
#else
  fujinet_dcb_exec(&dcb);

  return (AdapterConfig *)response;
#endif
}

void io_set_ssid(NetConfig *nc)
{
  unsigned char c[98]={0xFB};

  memcpy(&c[1],nc,sizeof(NetConfig));

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));

}

void io_get_device_slots(DeviceSlot *d)
{
  unsigned char c=0xF2;

//  io_command_and_response(&c,1);

  memcpy(d,response,304);
}

void io_get_host_slots(HostSlot *h)
{
  unsigned char c=0xF4;

//  io_command_and_response(&c,1);
  
  memcpy(h,response,256);
}

void io_put_host_slots(HostSlot *h)
{
  unsigned char c[257]={0xF3};

  memcpy(&c[1],h,256);

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
}

void io_put_device_slots(DeviceSlot *d)
{
  unsigned char c[305]={0xF1};

  memcpy(&c[1],d,304);

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
  csleep(10);
}

void io_mount_host_slot(unsigned char hs)
{
//  unsigned char c[2]={0xF9,0x00};
//
//  c[1] = hs;
//
//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
}

void io_open_directory(unsigned char hs, char *p, char *f)
{
//  char c[258];
//  char *e;
//
//  memset(&c,0,258);
//  c[0]=0xF7;
//  c[1]=hs;
//  strcpy(&c[2],p);
//  e=&c[2];
//
//  if (f[0]!=0x00)
//    {
//      while (*e != 0x00)
//	e++;
//
//      e++;
//      strcpy(e,f);
//    }
//
//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
}

char *io_read_directory(unsigned char l, unsigned char a)
{
  unsigned char c[3]={0xF6,0x00,0x00};
  c[1]=l;
  c[2]=a;
  //io_command_and_response(&c,3);
  return response;
}

void io_close_directory(void)
{
  unsigned char c=0xF5;

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
}

void io_set_directory_position(DirectoryPosition pos)
{
  unsigned char c[3]={0xE4,0x00,0x00};

  memcpy(&c[1],&pos,sizeof(DirectoryPosition));

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
}

void io_set_device_filename(unsigned char ds, char* e)
{
  char c[258]={0xE2,0x00};
  
  c[1] = ds;

  strcpy(&c[2],e);

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
}

char *io_get_device_filename(unsigned char ds)
{
  char c[2]={0xDA,0x00};

  c[1] = ds;

//  io_command_and_response(&c,2);

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
}

void io_mount_disk_image(unsigned char ds, unsigned char mode)
{
  char c[3]={0xF8,0x00,0x00};
  c[1]=ds;
  c[2]=mode;

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
}

void io_set_boot_config(unsigned char toggle)
{
  char c[2]={0xD9,0x00};
  c[1]=toggle;

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
}

void io_umount_disk_image(unsigned char ds)
{
  char c[2]={0xE9,0x00};
  c[1]=ds;

//  eos_write_character_device(FUJI_DEV,&c,sizeof(c));
}

void io_boot(void)
{
//  eos_init();
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
