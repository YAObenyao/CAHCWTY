#include "main.h"
unsigned soft_version[6]={0x03,0x01,0xE7,0x07,0x5,0x12};
unsigned char TargetFreSend=0x55;
extern STRUCT_TARGET Target_result[4];
extern unsigned char ucFireSignal;

void Prod_SelfCheckK101W(void)
{
    uint8_t aucSendBackBuf[16]={0};
    uint8_t ucSelfIdx;
    uint16_t usTargetFire;

    //读取之前装订的信息。自检消息传给后端
    usTargetFire = DEFAULT_HEIGHT;
    
    gstSysPara.ucTargetFireIdx = Prod_CalcFireHeight(usTargetFire);
    for(ucSelfIdx=0;ucSelfIdx <5;ucSelfIdx++)
    {
        aucSendBackBuf[0] = 0xAA;
        aucSendBackBuf[1] = 0x70;
        aucSendBackBuf[2] = usTargetFire;
        aucSendBackBuf[3] = 0x03;
        aucSendBackBuf[4] = 0x01;
        aucSendBackBuf[5] = getBcc(aucSendBackBuf,5);
        USART_Transmit_Bytes(&USART0, (uint8_t*)&aucSendBackBuf[0],6);
        Cnt_Delay_ms(4);
    }
    
}
uint8_t Prod_CalcFireHeight(uint8_t ucInfo)
{
    uint8_t ucTargetFreTemp[3] = {3,1,2};
    uint8_t ucTargetHight;
    double   fTargetHight;
    if(ucInfo<=0x57)
    {
        ucTargetHight = ucTargetFreTemp[ucInfo-0x55];
    }
    else
    {
        ucTargetHight=ucInfo-0x54;
    }
    fTargetHight = ucTargetHight;
    fTargetHight = fTargetHight/0.24;
    ucTargetHight = (fTargetHight+0.5);
    return ucTargetHight;
    
}
uint8_t Prod_CalcRptHeight(uint8_t ucInfo)
{
    uint8_t usRptHeight;
    if(ucInfo==3)
    {
        usRptHeight=0x55;
    }
    else if(ucInfo==1)
    {
        usRptHeight=0x56;
    }
    else if(ucInfo==2)
    {
        usRptHeight=0x57;
    }
    else
    {
        usRptHeight=ucInfo+0x54;
    }
    return usRptHeight;
    
}
void Prod_CommHandleK101W(uint8_t *ptrBuff)
{
    uint8_t ucActCheck=0,ucCheck;
    uint8_t ucComLen = *(ptrBuff+2);
    uint8_t ucComand = *(ptrBuff + 3);
    uint16_t usHeader;
    uint8_t aucSendBackBuf[16]={0};
    uint8_t wtbuf[16] = {0};
    
    usHeader = (*(ptrBuff))<<8;
    usHeader += *(ptrBuff+1);
    if(ucComLen+2 > USART_REC_MAX_LEN)
    {
        return;
    }
    ucCheck = *(ptrBuff+ucComLen+2);
    ucActCheck = getBcc(ptrBuff,ucComLen+2);
    if(usHeader == COMMAND_HEARD && ucCheck == ucActCheck)
    {
        aucSendBackBuf[0]=(uint8_t)(REPORT_HEARD>>8);
        aucSendBackBuf[1]=REPORT_HEARD&0xFF;
        //装订或查询
        if(ucComand <= 0x68 && ucComand >= 0x55)//z_height "0X55-0X68,0X68--->0X69:YZF"
        {
            memset(wtbuf,0,sizeof(wtbuf));
            Word2Byte(ucComand, wtbuf);
            ucFireSignal=0;
            gstSysPara.ucTargetFireIdx = Prod_CalcFireHeight(ucComand);
            TargetFreSend = ucComand;
            aucSendBackBuf[2] = TargetFreSend;
            aucSendBackBuf[3] = soft_version[0];
            aucSendBackBuf[4] = soft_version[1];
            aucSendBackBuf[5] = getBcc(aucSendBackBuf,5);
            USART_Transmit_Bytes(&USART0, (uint8_t*)&aucSendBackBuf[0],6);
            return;                        
        }        
#if 1
        else if(ucComand == 0x71)//signle read
        {
            gstSysPara.ucRptEn = TRUE;
            gstSysPara.ucRptCnt = 0;
            Prod_SendPeriodicMsgK101W();
            return;                        
        }
#endif
        else if(ucComand == 0x75)//debug mode
        {        
            aucSendBackBuf[0]=0x5A;
            aucSendBackBuf[1]=0x5A;
            if (Target_result[0].Range == 5)
            {
                aucSendBackBuf[2]=0x00;
            }
            else
            {
                aucSendBackBuf[2]=(uint8_t)(Target_result[0].Range&0xff);
            }
            if (Target_result[1].Range == 5)
            {
                aucSendBackBuf[3]=0x00;
            }
            else
            {
                aucSendBackBuf[3]=(uint8_t)(Target_result[1].Range&0xff);
            }
            USART_Transmit_Bytes(&USART0, (uint8_t*)&aucSendBackBuf[0],4);                
            return;                        
        }
    }
}

void Prod_SendFireMsgK101W(uint8_t ucFireFlag,uint8_t ucFireHeight)
{
    uint8_t aucSendBackBuf[16]={0};
    
    aucSendBackBuf[0]=(uint8_t)(REPORT_HEARD>>8);
    aucSendBackBuf[1]=REPORT_HEARD&0xFF;
    aucSendBackBuf[2]=0x72;
    if(ucFireHeight == 0xFF)
    {
        aucSendBackBuf[3]=ucFireHeight;
    }
    else
    {
        aucSendBackBuf[3]=Prod_CalcRptHeight(ucFireHeight);;
    }
    aucSendBackBuf[4]=ucFireFlag;
    aucSendBackBuf[5]=getBcc(aucSendBackBuf,5);
    USART_Transmit_Bytes(&USART0, (uint8_t*)&aucSendBackBuf[0],6);                
    
}
void Prod_SendPeriodicMsgK101W(void)
{
    uint8_t aucSendBackBuf[16]={0};
    
    aucSendBackBuf[0]=(uint8_t)(REPORT_HEARD>>8);
    aucSendBackBuf[1]=REPORT_HEARD&0xFF;
    aucSendBackBuf[2]=0x73;
    aucSendBackBuf[3]=0;
    aucSendBackBuf[4]=0;
    aucSendBackBuf[5]=getBcc(aucSendBackBuf,5);
    USART_Transmit_Bytes(&USART0, (uint8_t*)&aucSendBackBuf[0],6);                
    
}

