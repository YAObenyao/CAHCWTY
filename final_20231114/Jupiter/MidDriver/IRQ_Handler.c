#include "IRQ_Handler.h"
uint32_t SysTick_Counter = 0;
uint32_t firstChirpCnt = 0;
uint8_t PA_Cnt = 0;
void BB_IRQHandler(void)
{
    //���PREP_RAMP_END��־
    if(bb_regb_str->ISR & BB_ISR_PREP_RAMP_END)
    {
        bb_regb_str->ISR = BB_ISR_PREP_RAMP_END;            //�����ǰ��    
        //�ۼ�
    }
    else if(bb_regb_str->ISR & BB_ISR_PREP_FRAME_END)
    {
        //bb_regb_str->ISR = BB_ISR_PREP_FRAME_END;            //�����ǰ��   
        //����PREP
    
    }
    
#ifdef RF_RC7701N32
    PA_Cnt++;
    RF_PA_FASTSW(PA_Cnt);    
    if(PA_Cnt>=gstSysPara.ucTxNum)
    {
        PA_Cnt = 0;
    }
#endif
    if(firstChirpCnt==0)    
    {
        BB_OPCLEAR(BB_CLEAR_PREP);
        firstChirpCnt = 1;
    }
    //�ۺ�
    {
    }
}
    
void SysTick_Handler(void){
//    uint8_t sTemp=TEST_WritePin;
//    TEST_WritePin = 1-sTemp;
    SysTick_Counter++;
}

