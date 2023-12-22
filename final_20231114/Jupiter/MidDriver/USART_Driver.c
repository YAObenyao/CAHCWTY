#include "USART_Driver.h"
#include "main.h"
STRUCT_USART strUsart,stTxUart;
usci_handler_t  USART0;

void USART_Init(void)
{

    USCI4_CLK_ENABLE;
    PinMux_Set(GPIO1, USART0_TX_Pin, IOMODE_USCI4_IO1);
    PinMux_Set(GPIO1, USART0_RX_Pin, IOMODE_USCI4_IO2);

    USART0.instance = USCI4;
    USART0.u_init.uart_data_len = USCI_UART_DATA_LEN_8BIT;   //8 bit
    USART0.u_init.uart_parity_en = USCI_UART_PAR_EN_OFF;
    USART0.u_init.uart_lsb = USCI_UART_LSB_LSB;              //LSB
    USART0.u_init.uart_baud_rate = 115200;
    USART0.u_init.uart_stop_bit = USCI_UART_STOP_BIT_1;

    USCI_UART_Init(&USART0);

    NVIC_EnableIRQ(USCI4_IRQn);
    USART0.instance->ISR = 0x1F;
    USART0.instance->IMR &= ~USCI_IM_RXNE_Msk;
    memset(&strUsart,0,sizeof(STRUCT_USART));
}

void USART_Transmit_Bytes(usci_handler_t *usci, uint8_t *tx_buf, int32_t size)
{
    USCI_UART_Transmit(usci, tx_buf,size);
}
#if 1
bool USART_Transmit_Int(usci_handler_t *usci, uint8_t *tx_buf, int32_t size)
{
    USCI_UART_Transmit(usci, tx_buf,size);
	return TRUE;
}
#else
bool USART_Transmit_Int(usci_handler_t *usci, uint8_t *tx_buf, int32_t size)
{
    if((stTxUart.RecFinish ==FALSE)||(size<=0))
    {
        return FALSE;
    }
    if(size>USART_REC_MAX_LEN)
    {
        size = USART_REC_MAX_LEN;
    }
    stTxUart.writeCnt++;
    stTxUart.RecLen = size -1;
    USCI_UART_TransmitOneByte(usci, tx_buf);
    return TRUE;
}
#endif
/*Printf*/
int sendchar(int ch)
{
//usci3
//    if (ch == '\n')
//    {
//            USCI3->TRX = '\n';
//            while((USCI3->ISR & USCI_IS_TXE_Msk)==0);
//            USCI3->ISR = USCI_IS_TXE_Msk;
//            USCI3->TRX = '\r';
//            while((USCI3->ISR & USCI_IS_TE_Msk)==0);
//            USCI3->ISR = USCI_IS_TE_Msk;
//    }
    USCI4->TRX = ch;
    while((USCI4->ISR & USCI_IS_TXE_Msk)==0);
//    USCI3->ISR = USCI_IS_TE_Msk;
    return 0;
}

int getkey(void)
{
#ifdef DBG_PRINTF
    while((USCI4->ISR & USCI_IS_RXNE_Msk)==0);
     // USCI0->ISR = USCI_IS_RXNE_Msk;

    return (USCI4->TRX & BYTEMsk);
#else
    return (0);
#endif
}

struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;
int fputc(int ch, FILE *f)
{
  return (sendchar(ch));
}

int fgetc(FILE *f)
{
  return (sendchar(getkey()));
}
int ferror(FILE *f)
{
  /* Your implementation of ferror */
  return EOF;
}
void _ttywrch(int ch)
{
  sendchar (ch);
}
void _sys_exit(int x)
{
 x = x;
}

uint8_t usci_uART_rx_it(usci_handler_t *usci)
{
        uint8_t retry = 0;
        strUsart.timeoutStart = 1;
        strUsart.timeoutCnt = 0;
        while((usci->instance->ISR & USCI_IS_RXNE_Msk)==0)
        {
                retry++;
                if(retry > 200){
                    return 0;
                }
        }
        return usci->instance->TRX;
}
uint32_t errornum = 0;
bool   bConfigMode = FALSE;
void USCI4_IRQHandler(void)
{
    uint32_t reg_isr = 0;
    reg_isr = USCI4->ISR;
    //RX
    if(reg_isr&USCI_IS_RXNE_Msk)
    {
        strUsart.timeoutStart = 1;
        strUsart.timeoutCnt = 0;
        strUsart.RecBuff[strUsart.RecLen] = USART0.instance->TRX;
        if(strUsart.RecBuff[0] == 0xAA)
        {
            strUsart.RecLen++;
            if(strUsart.RecLen >= strUsart.RecBuff[2]+3)
            {
                strUsart.timeoutStart = 0;
                strUsart.timeoutCnt = 0;
                strUsart.RecLen=0;
                strUsart.RecFinish = 1;
				if(strUsart.RecBuff[1] == 0x60)
				{
					bConfigMode = TRUE;
				}
            }
			
        }
		else
		{
                //strUsart.timeoutStart = 0;
                //strUsart.timeoutCnt = 0;
                //USCI4_CLK_DISABLE;
                strUsart.RecLen=0;
			    strUsart.timeoutCnt = 0;
				errornum++;
                //strUsart.RecFinish = 1;
		}
        //ERR
    }
#if 0
    if(reg_isr&USCI_IS_TXE_Msk)
    {
        stTxUart.timeoutStart = 1;
        stTxUart.timeoutCnt=0;
        USART0.instance->TRX = stTxUart.RecBuff[stTxUart.writeCnt];
        stTxUart.writeCnt++;
        if(stTxUart.writeCnt >= stTxUart.RecLen)
        {
            stTxUart.RecFinish = 1;
            stTxUart.timeoutStart = 0;
            stTxUart.timeoutCnt = 0;
            stTxUart.RecLen=0;
            stTxUart.writeCnt = 0;
        }
    }
#endif
    if(reg_isr&USCI_IS_ERR_Msk)
    {
        //printf("[CONS]config reg_isr ERROR!\r\n");
        USCI4->ISR=USCI_IS_ERR_Msk;
    }
}
