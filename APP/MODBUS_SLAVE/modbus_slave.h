#ifndef __MODBUS_SLAVE_H
#define __MODBUS_SLAVE_H

#include "system.h"


#define REG_HOLDING_START     0x0001	//保持寄存器起始地址
#define REG_HOLDING_NREGS     6			//保持寄存器数量

#define REG_COILS_START       0x0001	//线圈起始地址
#define REG_COILS_SIZE        (8*21)	//线圈数量

#define REG_DISCRETE_START    0x0001	//开关寄存器其实地址
#define REG_DISCRETE_SIZE     (16*21)			//开关寄存器数量


extern uint16_t usRegHoldingBuf[REG_HOLDING_NREGS];	//年月日时分秒				
//线圈状态
extern uint8_t ucRegCoilsBuf[REG_COILS_SIZE/8];
//离散寄存器内容
extern uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE/8];



// #define MODBUS_SLAVE_SEND_PIN PAout(0)
#define MODBUS_SLAVE_SEND_PIN PGout(3)

#define modbus_slave_send_en() MODBUS_SLAVE_SEND_PIN=1
#define modbus_slave_receive_en() MODBUS_SLAVE_SEND_PIN=0


void modbus_slave_uart_init(u32 bound);
void modbus_slave_timer_init(unsigned short period);
#endif




















