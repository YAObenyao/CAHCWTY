#include "Usart_Protocol.h"
STRUCT_UDATA udata;

unsigned int long long GlobalTimeSliceOld=0;
extern STRUCT_TARGET Target_result[4];
extern unsigned char TargetFreIndex;
unsigned int debugMode=0;
extern unsigned int long long GlobalTimeSlice;
extern unsigned char InitState;
unsigned char initsendcount=0;
extern unsigned char ucFireSignal;
extern unsigned int countj;
void Word2Byte(uint32_t w_val, uint8_t* b_val)
{
    b_val[0] = w_val & 0xFF;
    b_val[1] = (w_val >> 8) & 0xFF;
    b_val[2] = (w_val >> 16) & 0xFF;
    b_val[3] = (w_val >> 24) & 0xFF;
}

void XFProtocol_Analysis_Handle(void)
{
    strUsart.timeoutCnt++;
    if(strUsart.RecFinish)
    {
        switch(gstSysPara.usProductId)
        {
            case PROD_K101W:
            {
                Prod_CommHandleK101W(&strUsart.RecBuff[0]);
            }
            break;
            default:
                
                break;
        }
        strUsart.RecLen=0;
        strUsart.RecFinish = 0;
		strUsart.timeoutStart = 0;
    }
	else if(strUsart.timeoutCnt > 2)
	{
        strUsart.RecLen=0;
        strUsart.RecFinish = 0;
		strUsart.timeoutStart = 0;		
	}
}
uint8_t getBcc(uint8_t *data, uint16_t length)
{
//    uint8_t i;
    uint8_t bcc = 0;        // Initial value
    while(length--)
    {
        bcc ^= *data++;
    }        
    return bcc;
}
