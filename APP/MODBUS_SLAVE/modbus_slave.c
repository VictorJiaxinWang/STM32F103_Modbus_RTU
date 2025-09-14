#include "modbus_slave.h"
#include "mb.h"
#include "mbutils.h"
#include "rtc.h" 
#include "string.h"
#include "relay.h"
#include "mydef.h"
//保持寄存器内容		
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x0001,0x0002,0x0003,0x0004,0x0005,0x0006};	//年月日时分秒				
//线圈状态
uint8_t ucRegCoilsBuf[REG_COILS_SIZE/8] = {0x00};
//离散寄存器内容
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE/8] = {0x00}; 

//void modbus_slave_uart_init(u32 bound)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);//使能GPIOA,D时钟
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//使能USART3时钟

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //PD7端口配置
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PA2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA3
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
//	GPIO_Init(GPIOB, &GPIO_InitStructure);  

//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,ENABLE);//复位串口2
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,DISABLE);//停止复位


//	USART_InitStructure.USART_BaudRate = bound;//波特率设置
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据长度
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
//	USART_InitStructure.USART_Parity = USART_Parity_No;///奇偶校验位
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//收发模式

//	USART_Init(USART3, &USART_InitStructure); ; //初始化串口

//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //使能串口3中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //先占优先级2级
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //从优先级2级
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //使能外部中断通道
//	NVIC_Init(&NVIC_InitStructure); //根据NVIC_InitStruct中指定的参数初始化外设NVIC寄存器

//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//开启中断

//	USART_Cmd(USART3, ENABLE);                    //使能串口 
//	
//	modbus_slave_receive_en();

//}

void modbus_slave_timer_init(unsigned short period)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //时钟使能
	
	//定时器TIM3初始化
	TIM_TimeBaseStructure.TIM_Period = period; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =(1800 - 1); //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //根据指定的参数初始化TIMx的时间基数单位
 
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //使能指定的TIM3中断,允许更新中断
//		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //先占优先级0级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //从优先级3级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  //初始化NVIC寄存器

	TIM_Cmd(TIM3, ENABLE);  //使能TIMx
	
}

/****************************************************************************
* 名	  称：eMBRegHoldingCB 
* 功    能：对应功能码有：06 写保持寄存器 eMBFuncWriteHoldingRegister 
*													16 写多个保持寄存器 eMBFuncWriteMultipleHoldingRegister
*													03 读保持寄存器 eMBFuncReadHoldingRegister
*													23 读写多个保持寄存器 eMBFuncReadWriteMultipleHoldingRegister
* 入口参数：pucRegBuffer: 数据缓存区，用于响应主机   
*						usAddress: 寄存器地址
*						usNRegs: 要读写的寄存器个数
*						eMode: 功能码
* 出口参数：
* 注	  意：4 区
****************************************************************************/
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;
	u16 usRegHoldingBufSave[REG_HOLDING_NREGS];
	u8 valueErrorFlag=0;

	if((usAddress >= REG_HOLDING_START)&&\
		((usAddress+usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - REG_HOLDING_START);
		switch(eMode)
		{                                       
			case MB_REG_READ://读 MB_REG_READ = 0
				while(usNRegs > 0)
				{
					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] >> 8);            
					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] & 0xFF); 
					iRegIndex++;
					usNRegs--;					
				}                            
			break;
			case MB_REG_WRITE://写 MB_REG_WRITE = 0
				memcpy(usRegHoldingBufSave,usRegHoldingBuf,REG_HOLDING_NREGS);
				while(usNRegs > 0)
				{         
					usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
					usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
					iRegIndex++;
					usNRegs--;
				}
				if(usRegHoldingBuf[0]>2099||usRegHoldingBuf[0]<2020)  //年
				{
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[1]>12||usRegHoldingBuf[1]<=0)	//月
				{	
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[2]>31||usRegHoldingBuf[2]<=0)	//日
				{
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[3]>23)	//时
				{
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[4]>59)	//分
				{
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[5]>59)	//秒
				{
					valueErrorFlag=1;
				}
				
				if(valueErrorFlag)
				{
					memcpy(usRegHoldingBuf,usRegHoldingBufSave,REG_HOLDING_NREGS);
					eStatus = MB_EINVAL;
				}
				else
					// RTC_Set(usRegHoldingBuf[0],usRegHoldingBuf[1],usRegHoldingBuf[2],usRegHoldingBuf[3],usRegHoldingBuf[4],usRegHoldingBuf[5]);//设置RTC时间
			break;				
		}
	}
	else//错误
	{
		eStatus = MB_ENOREG;
	}	
	
	return eStatus;
}

/****************************************************************************
* 名	  称：eMBRegCoilsCB 
* 功    能：对应功能码有：01 读线圈 eMBFuncReadCoils
*						  05 写线圈 eMBFuncWriteCoil
*						  15 写多个线圈 eMBFuncWriteMultipleCoils
* 入口参数：pucRegBuffer: 数据缓存区，用于响应主机   
*						usAddress: 线圈地址
*						usNCoils: 要读写的线圈个数
*						eMode: 功能码
* 出口参数：
* 注	  意：如继电器 
*						0 区
****************************************************************************/
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
               eMBRegisterMode eMode )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iNCoils = ( int )usNCoils;
    unsigned short  usBitOffset;
	u8 relayValue;

	#define 	relayBit0        (((BIT8*) & ucRegCoilsBuf[0]) -> b0)
	#define 	relayBit1        (((BIT8*) & ucRegCoilsBuf[0]) -> b1)
	#define 	relayBit2        (((BIT8*) & ucRegCoilsBuf[0]) -> b2)
	#define 	relayBit3        (((BIT8*) & ucRegCoilsBuf[0]) -> b3)
	#define	    relayBit4        (((BIT8*) & ucRegCoilsBuf[0]) -> b4)
	#define 	relayBit5        (((BIT8*) & ucRegCoilsBuf[0]) -> b5)
	#define 	relayBit6        (((BIT8*) & ucRegCoilsBuf[0]) -> b6)
	#define 	relayBit7        (((BIT8*) & ucRegCoilsBuf[0]) -> b7) 

    /* Check if we have registers mapped at this block. */
    if( ( usAddress >= REG_COILS_START ) &&
        ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_COILS_START );
        switch ( eMode )
        {
                /* Read current values and pass to protocol stack. */
            case MB_REG_READ:
                while( iNCoils > 0 )
                {
                    *pucRegBuffer++ =
                        xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                        ( unsigned char )( iNCoils >
                                                           8 ? 8 :
                                                           iNCoils ) );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
                break;

                /* Update current register values. */
            case MB_REG_WRITE:
                while( iNCoils > 0 )
                {
                    xMBUtilSetBits( ucRegCoilsBuf, usBitOffset, 
                                    ( unsigned char )( iNCoils > 8 ? 8 : iNCoils ),
                                    *pucRegBuffer++ );
                    iNCoils -= 8;
                    usBitOffset += 8;
                }
				relayValue=0x00;
				if(relayBit0)
					relayValue|=RELAY_1;
				else
					relayValue&=(u8)(~RELAY_1);
				if(relayBit1)
					relayValue|=RELAY_2;
				else
					relayValue&=(u8)(~RELAY_2);
				if(relayBit2)
					relayValue|=RELAY_3;
				else
					relayValue&=(u8)(~RELAY_3);
				if(relayBit3)
					relayValue|=RELAY_4;
				else
					relayValue&=(u8)(~RELAY_4);
				if(relayBit4)
					relayValue|=RELAY_5;
				else
					relayValue&=(u8)(~RELAY_5);
				if(relayBit5)
					relayValue|=RELAY_6;
				else
					relayValue&=(u8)(~RELAY_6);
				if(relayBit6)
					relayValue|=RELAY_7;
				else
					relayValue&=(u8)(~RELAY_7);
				send_relay_data(relayValue);
                break;
        }

    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}
/****************************************************************************
* 名	  称：eMBRegDiscreteCB 
* 功    能：读取离散寄存器，对应功能码有：02 读离散寄存器 eMBFuncReadDiscreteInputs
* 入口参数：pucRegBuffer: 数据缓存区，用于响应主机   
*						usAddress: 寄存器地址
*						usNDiscrete: 要读取的寄存器个数
* 出口参数：
* 注	  意：1 区
****************************************************************************/
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    short           iNDiscrete = ( short )usNDiscrete;
    unsigned short  usBitOffset;

    /* Check if we have registers mapped at this block. */
    if( ( usAddress >= REG_DISCRETE_START ) &&
        ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
    {
        usBitOffset = ( unsigned short )( usAddress - REG_DISCRETE_START );
        while( iNDiscrete > 0 )
        {
            *pucRegBuffer++ =
                xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                                ( unsigned char )( iNDiscrete >
                                                   8 ? 8 : iNDiscrete ) );
            iNDiscrete -= 8;
            usBitOffset += 8;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }
    return eStatus;
}










