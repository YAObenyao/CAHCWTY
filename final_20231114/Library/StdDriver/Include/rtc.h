/******************************************************************************
 * @File         : rtc.h
  *
 * @Version      : V1.0
 * @Revision     : none
 * @Created date : 2021/10/18 18:13:30
 * @Last modified: 2021/10/18 18:13:43
 * @Modified by  : Mxy
 *
 * @Brief        : rtc driver source file
 *
 * Copyright (C) 2017~2023 Skyrelay Technology Corp. All rights reserved.
 *****************************************************************************/

#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C"
{
#endif

//Error code
#define RTC_INVALID_PARA       0x0C01

#define ONCE_MODE              0
#define REPEAT_MODE            1
#define SOURCE_RC32K           0
#define SOURCE_OSC32K          1
#define RTC_SELECT_LRC32K      LXTAL_DISABLE
#define RTC_SELECT_OSC32K      LXTAL_ENABLE
#define RTC_CLEAR_COUNTER      (RTC->CLR = RTC_CLR_Msk)

uint16_t rtc_Enable(uint8_t u8ClkSrc);
void rtc_Disable(void);
void rtc_Get_Current_Value(uint8_t *pval);
void rtc_Reset(void);
void rtc_Wakeup_ms(uint32_t u32Msec, uint8_t u8Mode);
void rtc_Wakeup_sec(uint32_t u32sec, uint8_t u8Mode);

#ifdef __cplusplus
}
#endif

#endif //__RTC_H__

/*** (C) COPYRIGHT 2023 Skyrelay Technology Corp. ***/
