#ifndef __RELAY_H	 
#define __RELAY_H	 
#include "system.h"

#define relay_clk PBout(3)
#define relay_dat PBout(4)

#define RELAY_1 0x20
#define RELAY_2 0x40
#define RELAY_3 0x10
#define RELAY_4 0x08
#define RELAY_5 0x04
#define RELAY_6 0x02
#define RELAY_7 0x01


void relay_init(void);
void send_relay_data(u8 udata);
#endif







