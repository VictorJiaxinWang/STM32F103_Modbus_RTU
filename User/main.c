

#include "system.h"
#include "SysTick.h"
#include "led.h"
#include "usart.h"
#include "rs485.h"

#include "modbus_slave.h"
#include "mb.h"
#include "mbutils.h"

/*******************************************************************************
* �� �� ��         : main
* ��������		   : ������
* ��    ��         : ��
* ��    ��         : ��
*******************************************************************************/
int main()
{
	u8 i=0;

	SysTick_Init(72);
	// NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  //�ж����ȼ����� ��2��
	LED_Init();
	// USART1_Init(9600);
	// RS485_Init(9600);
	
	eMBInit(MB_RTU, 0x01, 0x01, 9600, MB_PAR_NONE); //MODBUS��ʼ��
	eMBEnable();  			//MODBUSʹ��
	
	while(1)
	{
		
		(void)eMBPoll();
		
		i++;
		if(i%20==0)
		{
			led1=!led1;
		}
		
		delay_ms(10);
	}
}
