/******************************************************************************
 * @file Led.c
 * @brief 
 * @author liyang
 * @version 0.1
 * @date 2022-12-26
 * 
 * Copyright (c) 2022,xxx xxx Co., Ltd.
******************************************************************************/
#include "Led.h"
#include "GPIO_Driver.h"
 
/*-----------------------------  Private types  -----------------------------*/
/*---------------------------  Private constants  ---------------------------*/
/*---------------------------  Private variables  ---------------------------*/
static uint32_t time_it_falg;
static uint8_t ChangeFlage=0;
unsigned int long long GlobalTimeSlice=0;
/*-----------------------------  Private macro  -----------------------------*/
/*---------------------------  Private functions  ---------------------------*/
 
void TimerInit(void)//��������ģʽ
{
    
    //SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;/
    TMRA_CLK_ENABLE;
    //ģ�� USCI ��ʼ������������� �����жϱ�־λ
    TMRA->TAI = 0xFF;//�Ĵ���������д 1  ��������Ĵ���λ
        
    TMRA->TACTL |= 1<<3 ;//         IDEX ��Ƶ 5��
//    TMRA->TACTL |= 3<<6 ;//     ID��Ƶ 8��ʱ��Դ 4M  ��0.25us
    
    //TMRA->TACTL = 0x9A;//         IDEX ��Ƶ 5��
    //TMRA->TACTL=2;
    TMRA->TACCR0 = 60000;//UP ģʽ��   TAR����ֵ����ֵ�Ƚϣ�����TAIFG�жϣ�10ms
    
    TMRA->TAIE  |= 0x01<<7;//ʹ�� TAIFG �ж� 
//    //TMRA->TAIE  |=0xFF;
//    
//    TMRA->TACTL |= 0x01<<0;//���TAR����ֵ
    TMRA->TACTL |= 0x01<<1;//���ó����ϼ���ģʽ�������ĵ�˵�����������ģʽ��TAR�Ĵ����Ϳ�ʼ������(UPģʽ���� 0~TACCROֵ��ѭ��)
    //��ʱ��ʱ�Ӳ�����������Ƶ����
    NVIC_EnableIRQ(TMRA_IRQn);
}
 

void TMRA_IRQHandler(void)//0.5ms
{
    
    TMRA->TACTL |= 0x01<<0;//���TAR����ֵ
    //TMRA->TACTL = 0;//�رն�ʱ��
    
    time_it_falg++;
    GlobalTimeSlice++;
    if(time_it_falg>1000)
    {
        ChangeFlage++;
        if(ChangeFlage%2==1)
        {
            LED2_WritePin = 1;
            //LED3_WritePin = 1; 
        }
        else
        {
            LED2_WritePin = 0;
            //LED3_WritePin = 0; 
        }
        time_it_falg=0;
    }
    TMRA->TAI =0xFF;//��������жϱ�־λ
    //TMRA->TACTL |= 0x01<<1;//������ʱ��
}

void Cnt_Delay(uint32_t u32Cnt)
{
    uint32_t i, j;
    for (j = 0; j < u32Cnt; j++)
    {
        for(i = 0; i < 100; i++); 
    }
}

void StartSignalDetect(void)
{
        unsigned int value=0;
        unsigned int validCount=0;
        GPIO_SetMode(START_GPIO_Port, START_Pin, GPIO_PMODE_INPUT);
    
        while(1)
        {        
            value=GPIO0_6;
            if(value==0)
            {
                validCount++;
                if(validCount>500)
                {
                    break;
                }
            }
            else{
                validCount=0;
            }
            Cnt_Delay(100);// 1ms
        }
}
 
/******************************************************************************
          End of the File (EOF). Do not put anything after this part!
******************************************************************************/
