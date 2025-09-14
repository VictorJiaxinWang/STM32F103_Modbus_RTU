/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id: portserial.c,v 1.1 2006/08/22 21:35:13 wolti Exp $
 */

#include "port.h"
#include "stm32f10x.h"
#include "modbus_slave.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
// #include "rtthread.h"
#include "rs485.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    /* If xRXEnable enable serial receive interrupts. If xTxENable enable
     * transmitter empty interrupts.
     */
	if(xRxEnable == TRUE)
	{
		modbus_slave_receive_en();
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
	}
	else
	{
		USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	}
	
	if(xTxEnable == TRUE)
	{
		modbus_slave_send_en();
		USART_ITConfig(USART2, USART_IT_TC, ENABLE);
	}
	else
	{
		USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	}
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
	// modbus_slave_uart_init((uint16_t)ulBaudRate);  
	RS485_Init(ulBaudRate);
	
	return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    /* Put a byte in the UARTs transmit buffer. This function is called
     * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
     * called. */
	USART_SendData(USART2, ucByte);
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    /* Return the byte in the UARTs receive buffer. This function is called
     * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
     */
  *pucByte = USART_ReceiveData(USART2); 
	//rt_kprintf("0x%x ",*pucByte);
	return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

/**
  * @brief  This function handles usart1 Handler.
  * @param  None
  * @retval None
  */
//void USART2_IRQHandler(void)
//{
//	// rt_interrupt_enter();
//	//���������ж�
//	if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
//	{
//		prvvUARTRxISR(); 
//		//����жϱ�־λ    
//		USART_ClearITPendingBit(USART2, USART_IT_RXNE);   
//	}

//	if(USART_GetITStatus(USART2, USART_IT_ORE) == SET)
//	{  
//		USART_ClearITPendingBit(USART2, USART_IT_ORE);
//		prvvUARTRxISR(); 	
//	}

//	//��������ж�
//	if(USART_GetITStatus(USART2, USART_IT_TC) == SET)
//	{
//		prvvUARTTxReadyISR();
//		//����жϱ�־
//		USART_ClearITPendingBit(USART2, USART_IT_TC);
//	}
//	// rt_interrupt_leave();
//}

void USART2_IRQHandler(void)
{
    //rt_interrupt_enter();  // ??RT-Thread,????

    // 1. ??RXNE(????)
    if(USART_GetITStatus(USART2, USART_IT_RXNE) == SET)
    {
        prvvUARTRxISR(); 
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }

    // 2. ??ORE(????)
    if(USART_GetITStatus(USART2, USART_IT_ORE) == SET)
    {
        volatile uint32_t temp = USART2->DR; // ??!?DR??ORE
        USART_ClearITPendingBit(USART2, USART_IT_ORE);
    }

    // 3. ??TC(????)
    if(USART_GetITStatus(USART2, USART_IT_TC) == SET)
    {
        prvvUARTTxReadyISR();
        USART_ClearITPendingBit(USART2, USART_IT_TC);
    }

    // 4. ????????(FE, NE, IDLE)
    if(USART_GetFlagStatus(USART2, USART_FLAG_FE)) {
        volatile uint32_t temp = USART2->DR;
        USART_ClearITPendingBit(USART2, USART_IT_FE);
    }
    if(USART_GetFlagStatus(USART2, USART_FLAG_NE)) {
        volatile uint32_t temp = USART2->DR;
        USART_ClearITPendingBit(USART2, USART_IT_NE);
    }
    if(USART_GetFlagStatus(USART2, USART_FLAG_IDLE)) {
        volatile uint32_t temp = USART2->DR;
        USART_ClearITPendingBit(USART2, USART_IT_IDLE);
    }

    // rt_interrupt_leave();  // ??RT-Thread,????
}
