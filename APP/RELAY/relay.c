#include "relay.h"

void relay_init(void)
{
//	GPIO_InitTypeDef  GPIO_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO, ENABLE);	 //使能PB,PE端口时钟

//	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
//	
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4;				 
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
//	GPIO_Init(GPIOB, &GPIO_InitStructure);	

//	send_relay_data(0x00);
}
void delay_relay(u16 i)
{
	while(i--);
}

void send_relay_data(u8 udata)
{
//	u8 i;
//	for( i = 0; i < 8; i ++ )
//	{
//		if( udata & 0x80 )
//		{
//			relay_dat=1;
//		}
//		else
//		{
//			relay_dat=0;
//		}
//		delay_relay(1);
//		relay_clk=0;
//		delay_relay(1);
//		relay_clk=1;
//		udata = udata<<1;
//	}
}




