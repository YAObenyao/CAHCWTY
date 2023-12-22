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
 
void TimerInit(void)//连续计数模式
{
    
    //SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;/
    TMRA_CLK_ENABLE;
    //模仿 USCI 初始化函数，先清除 所有中断标志位
    TMRA->TAI = 0xFF;//寄存器描述，写 1  ，会清除寄存器位
        
    TMRA->TACTL |= 1<<3 ;//         IDEX 分频 5；
//    TMRA->TACTL |= 3<<6 ;//     ID分频 8；时钟源 4M  ，0.25us
    
    //TMRA->TACTL = 0x9A;//         IDEX 分频 5；
    //TMRA->TACTL=2;
    TMRA->TACCR0 = 60000;//UP 模式下   TAR计数值跟该值比较，产生TAIFG中断，10ms
    
    TMRA->TAIE  |= 0x01<<7;//使能 TAIFG 中断 
//    //TMRA->TAIE  |=0xFF;
//    
//    TMRA->TACTL |= 0x01<<0;//清除TAR计数值
    TMRA->TACTL |= 0x01<<1;//设置成向上计数模式，按照文档说法，配置完该模式后，TAR寄存器就开始计数了(UP模式下在 0~TACCRO值间循环)
    //定时器时钟不进行其它分频操作
    NVIC_EnableIRQ(TMRA_IRQn);
}
 

void TMRA_IRQHandler(void)//0.5ms
{
    
    TMRA->TACTL |= 0x01<<0;//清除TAR计数值
    //TMRA->TACTL = 0;//关闭定时器
    
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
    TMRA->TAI =0xFF;//清除所有中断标志位
    //TMRA->TACTL |= 0x01<<1;//开启定时器
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
