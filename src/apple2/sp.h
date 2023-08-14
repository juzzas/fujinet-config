#ifdef BUILD_APPLE2
/**
 * FujiNet CONFIG for #Apple2
 *
 * SmartPort MLI Routines
 */

#ifndef SP_H
#define SP_H

#include <stdint.h>

#ifdef __ORCAC__
extern uint8_t *sp_payload;
#else
extern uint8_t sp_payload[1024];
#endif
extern uint16_t sp_count, sp_dispatch;
extern uint8_t sp_dest;
extern uint8_t sp_error;

int8_t sp_status(uint8_t dest, uint8_t statcode);
int8_t sp_control(uint8_t dest, uint8_t ctrlcode);
int8_t sp_find_fuji(void);
uint8_t sp_find_slot(void);
uint16_t sp_dispatch_address(uint8_t slot);
void sp_init(void);
#ifdef __ORCAC__
void sp_done(void);
#endif
void sp_list_devs(void);

#endif /* SP_H */
#endif /* BUILD_APPLE2 */

