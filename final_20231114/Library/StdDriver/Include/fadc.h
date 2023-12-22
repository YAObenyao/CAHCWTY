/******************************************************************************
 * @Filename     : fadc.h
 *
 * @Version      : V1.0
 * @Subversion   : $
 * @Last modified: 05/09/2022 16:01:41
 * @Modified by  : CB
 *
 * @Brief        : FADC driver header file
 *
 * Copyright (C) 2022 SKYRELAY Technology co.,Ltd All rights reserved.
 *****************************************************************************/
#ifndef __FADC_H__
#define __FADC_H__
#include "SKY32B750.h"

#ifdef __cplusplus
extern "C"
{
#endif

#define FADC_SPI_CLK_DIV(div)               FADC->SPI_DIV = (FADC->SPI_DIV & ~ADC_SPI_CLK_DIV_Msk) | (div << ADC_SPI_CLK_DIV_Pos)

#define FADC_SPI_CLK_ENABLE                 FADC->SPI_EN = 1
#define FADC_SPI_CLK_DISABLE                FADC->SPI_EN = 0

void FADC_SPI_WriteSingle(uint8_t csn,uint8_t addr,uint8_t wdata);
uint8_t FADC_SPI_ReadSingle(uint8_t csn, uint8_t addr);

#ifdef __cplusplus
}
#endif

#endif // __FADC_H__

/*** (C) COPYRIGHT 2022 SKYRELAY Technology co.,Ltd ***/
