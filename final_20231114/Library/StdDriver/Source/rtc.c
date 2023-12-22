/******************************************************************************
 * @File         : rtc.c
 *
 * @Version      : V1.0
 * @Revision     : none
 * @Created date : 2021/10/18 18:13:30
 * @Last modified: 2021/10/18 18:13:43
 * @Modified by  : Mxy
 *
 * @Brief        : rtc driver source file
 *
 * Copyright (C) 2017~2016 Skyrelay Technology Corp. All rights reserved.
 *****************************************************************************/

#include "SKY32B750.h"

uint32_t rtc_clk = 32768;

uint16_t rtc_Enable(uint8_t u8ClkSrc)
{
    if (u8ClkSrc == SOURCE_RC32K)
    {
        RTC_SELECT_LRC32K;
        //wdt and rtc disable
        if (AON_SYSC->LRC_EN == PD_LRC_ALL_PATTERN)
        {
            //rtc enable only
            WDT_DISABLE;
        }
        //rtc disable
        else if (AON_SYSC->LRC_EN == PD_LRC_RTC_PATTERN)
        {
            //wdt and rtc enable
            LRC_ENABLE;
        }
        //rtc enable
        else {}
        return OK;
    }
    else if (u8ClkSrc == SOURCE_OSC32K)
    {
        PORT01_PIN_CTRL->AE |= BIT18;    //Port1.2
        PORT01_PIN_CTRL->AE |= BIT19;    //Port1.3
        RTC_SELECT_OSC32K;
        return OK;
    }
    else
    {
        return RTC_INVALID_PARA;
    }
}

void rtc_Disable(void)
{
    RTC_SELECT_LRC32K;
//    PORT0->AE &= (~BIT5);
//    PORT0->AT &= (~BIT5);
//    PORT0->AE &= (~BIT6);
//    PORT0->AT &= (~BIT6);
    //rtc and wdt disable
    if (AON_SYSC->LRC_EN == PD_LRC_ALL_PATTERN) {}
    //wdt disable
    else if (AON_SYSC->LRC_EN == PD_LRC_WDT_PATTERN)
    {
        //rtc and wdt disable
        AON_SYSC->LRC_EN = PD_LRC_ALL_PATTERN;
    }
    //wdt enable
    else
    {
        //rtc disable only
        AON_SYSC->LRC_EN = PD_LRC_RTC_PATTERN;
    }
}

void rtc_Get_Current_Value(uint8_t *pval)
{
    pval[0] = RTC->CNT0;
    pval[1] = RTC->CNT1;
}

void rtc_Reset(void)
{
    RTC->WKU_CMP = 0x00;
    RTC->PUS_CFG = 0x00;
    RTC->WKU_CFG = 0x00;
    RTC_CLEAR_COUNTER;
    HW_Delay_ms(2);
}

void rtc_Wakeup_ms(uint32_t u32Msec, uint8_t u8Mode)
{
    uint32_t val;

    val = (u32Msec * rtc_clk) / 16000;
    RTC->WKU_CMP = (uint16_t)val;
    RTC->WKU_CFG &= (~RTC_CLK_SEL_Msk);

    if (u8Mode == REPEAT_MODE)
    {
        RTC->WKU_CFG |= RTC_WKU_MODE_Msk;
    }
    else
    {
        RTC->WKU_CFG &= (~RTC_WKU_MODE_Msk);
    }
}

void rtc_Wakeup_sec(uint32_t u32sec, uint8_t u8Mode)
{
    uint32_t val;
    val = (((u32sec * rtc_clk * 1000) / 16000 ) / 1024) ;
    RTC->WKU_CMP = (uint16_t)val;
    RTC->WKU_CFG |= RTC_CLK_SEL_Msk;
    if (u8Mode == REPEAT_MODE)
    {
        RTC->WKU_CFG |= RTC_WKU_MODE_Msk;
    }
    else
    {
        RTC->WKU_CFG &= (~RTC_WKU_MODE_Msk);
    }
}
