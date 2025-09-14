#include "modbus_slave.h"
#include "mb.h"
#include "mbutils.h"
#include "rtc.h" 
#include "string.h"
#include "relay.h"
#include "mydef.h"
//���ּĴ�������		
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x0001,0x0002,0x0003,0x0004,0x0005,0x0006};	//������ʱ����				
//��Ȧ״̬
uint8_t ucRegCoilsBuf[REG_COILS_SIZE/8] = {0x00};
//��ɢ�Ĵ�������
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE/8] = {0x00}; 

//void modbus_slave_uart_init(u32 bound)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	USART_InitTypeDef USART_InitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;

//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);//ʹ��GPIOA,Dʱ��
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);//ʹ��USART3ʱ��

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;				 //PD7�˿�����
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOA, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;	//PA2
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//��������
//	GPIO_Init(GPIOB, &GPIO_InitStructure);

//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;//PA3
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
//	GPIO_Init(GPIOB, &GPIO_InitStructure);  

//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,ENABLE);//��λ����2
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_USART3,DISABLE);//ֹͣ��λ


//	USART_InitStructure.USART_BaudRate = bound;//����������
//	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8λ���ݳ���
//	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
//	USART_InitStructure.USART_Parity = USART_Parity_No;///��żУ��λ
//	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
//	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ

//	USART_Init(USART3, &USART_InitStructure); ; //��ʼ������

//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn; //ʹ�ܴ���3�ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�2��
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3; //�����ȼ�2��
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //ʹ���ⲿ�ж�ͨ��
//	NVIC_Init(&NVIC_InitStructure); //����NVIC_InitStruct��ָ���Ĳ�����ʼ������NVIC�Ĵ���

//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�����ж�

//	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ��� 
//	
//	modbus_slave_receive_en();

//}

void modbus_slave_timer_init(unsigned short period)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = period; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =(1800 - 1); //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM3�ж�,��������ж�
//		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  //��ռ���ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�3��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM3, ENABLE);  //ʹ��TIMx
	
}

/****************************************************************************
* ��	  �ƣ�eMBRegHoldingCB 
* ��    �ܣ���Ӧ�������У�06 д���ּĴ��� eMBFuncWriteHoldingRegister 
*													16 д������ּĴ��� eMBFuncWriteMultipleHoldingRegister
*													03 �����ּĴ��� eMBFuncReadHoldingRegister
*													23 ��д������ּĴ��� eMBFuncReadWriteMultipleHoldingRegister
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNRegs: Ҫ��д�ļĴ�������
*						eMode: ������
* ���ڲ�����
* ע	  �⣺4 ��
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
			case MB_REG_READ://�� MB_REG_READ = 0
				while(usNRegs > 0)
				{
					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] >> 8);            
					*pucRegBuffer++ = (u8)(usRegHoldingBuf[iRegIndex] & 0xFF); 
					iRegIndex++;
					usNRegs--;					
				}                            
			break;
			case MB_REG_WRITE://д MB_REG_WRITE = 0
				memcpy(usRegHoldingBufSave,usRegHoldingBuf,REG_HOLDING_NREGS);
				while(usNRegs > 0)
				{         
					usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
					usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
					iRegIndex++;
					usNRegs--;
				}
				if(usRegHoldingBuf[0]>2099||usRegHoldingBuf[0]<2020)  //��
				{
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[1]>12||usRegHoldingBuf[1]<=0)	//��
				{	
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[2]>31||usRegHoldingBuf[2]<=0)	//��
				{
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[3]>23)	//ʱ
				{
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[4]>59)	//��
				{
					valueErrorFlag=1;
				}
				else if(usRegHoldingBuf[5]>59)	//��
				{
					valueErrorFlag=1;
				}
				
				if(valueErrorFlag)
				{
					memcpy(usRegHoldingBuf,usRegHoldingBufSave,REG_HOLDING_NREGS);
					eStatus = MB_EINVAL;
				}
				else
					// RTC_Set(usRegHoldingBuf[0],usRegHoldingBuf[1],usRegHoldingBuf[2],usRegHoldingBuf[3],usRegHoldingBuf[4],usRegHoldingBuf[5]);//����RTCʱ��
			break;				
		}
	}
	else//����
	{
		eStatus = MB_ENOREG;
	}	
	
	return eStatus;
}

/****************************************************************************
* ��	  �ƣ�eMBRegCoilsCB 
* ��    �ܣ���Ӧ�������У�01 ����Ȧ eMBFuncReadCoils
*						  05 д��Ȧ eMBFuncWriteCoil
*						  15 д�����Ȧ eMBFuncWriteMultipleCoils
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: ��Ȧ��ַ
*						usNCoils: Ҫ��д����Ȧ����
*						eMode: ������
* ���ڲ�����
* ע	  �⣺��̵��� 
*						0 ��
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
* ��	  �ƣ�eMBRegDiscreteCB 
* ��    �ܣ���ȡ��ɢ�Ĵ�������Ӧ�������У�02 ����ɢ�Ĵ��� eMBFuncReadDiscreteInputs
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNDiscrete: Ҫ��ȡ�ļĴ�������
* ���ڲ�����
* ע	  �⣺1 ��
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










