#include "Delay_Driver.h"
#include "SysClk_Driver.h"
#include "SKY32B750.h"
#include "App_Def.h"

/**
 * @Breif  : SW_Delay_us : delay n mircoseconds use software
 *
 * @Param  : u32Cnt: delay time
 */
void Cnt_Delay_us(uint32_t u32Cnt)
{
    uint32_t i, j;
    for (j = 0; j < u32Cnt; j++)
    {
        for(i = 0; i < 160; i++); //160
    }
}

/**
 * @Breif  : SW_Delay_ms : delay n milliseconds use software
 *
 * @Param  : u32Cnt: delay time
 */
void Cnt_Delay_ms(uint32_t u32Cnt)
{
    uint32_t j;
    uint32_t usCnt=hclk/206000;//smx:320MHZ下，软件定时系数
    for (j = 0; j < u32Cnt; j++)
    {
       Cnt_Delay_us(usCnt);
    }
}

//1ms= SYS_CLK / 1000
void SysTick_Init(void){
//    SysTick->LOAD = (uint32_t)SYS_CLK / 1000;
    SysTick->LOAD = (uint32_t)hclk / 1000;
    NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
    SysTick->VAL  = 0;   // Clear counter
        SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;//smx:配置 512页寄存器，时钟源写入 1，选择外部时钟？
    NVIC_EnableIRQ(SysTick_IRQn);
}
