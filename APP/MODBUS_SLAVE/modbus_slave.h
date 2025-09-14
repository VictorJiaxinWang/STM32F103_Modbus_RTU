#ifndef __MODBUS_SLAVE_H
#define __MODBUS_SLAVE_H

#include "system.h"


#define REG_HOLDING_START     0x0001	//���ּĴ�����ʼ��ַ
#define REG_HOLDING_NREGS     6			//���ּĴ�������

#define REG_COILS_START       0x0001	//��Ȧ��ʼ��ַ
#define REG_COILS_SIZE        (8*21)	//��Ȧ����

#define REG_DISCRETE_START    0x0001	//���ؼĴ�����ʵ��ַ
#define REG_DISCRETE_SIZE     (16*21)			//���ؼĴ�������


extern uint16_t usRegHoldingBuf[REG_HOLDING_NREGS];	//������ʱ����				
//��Ȧ״̬
extern uint8_t ucRegCoilsBuf[REG_COILS_SIZE/8];
//��ɢ�Ĵ�������
extern uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE/8];



// #define MODBUS_SLAVE_SEND_PIN PAout(0)
#define MODBUS_SLAVE_SEND_PIN PGout(3)

#define modbus_slave_send_en() MODBUS_SLAVE_SEND_PIN=1
#define modbus_slave_receive_en() MODBUS_SLAVE_SEND_PIN=0


void modbus_slave_uart_init(u32 bound);
void modbus_slave_timer_init(unsigned short period);
#endif




















