/******************************************************************************
 * @Filename     : fadc.c
 *
 * @Version      : V1.0
 * @Subversion   : $
 * @Last modified: 05/08/2022 16:01:41
 * @Modified by  : CB
 *
 * @Brief        : FADC driver source file
 *
 * Copyright (C) 2022 SKYRELAY Technology co.,Ltd All rights reserved.
 *****************************************************************************/
#include "SKY32B750.h"

/**
 * @brief
 * @DateTime  2022-05-09T16:03:08+0800
 * @param     csn                       Select FADC0 ~ FADC7
 * @param     addr                      Address to be write
 * @param     wdata                     Data to be write
 */
void FADC_SPI_WriteSingle(uint8_t csn,uint8_t addr,uint8_t wdata)
{
    uint8_t adc_addr = addr & 0x3F;
    FADC->SPI_CS = 0xFF;
    FADC->SPI_DI = adc_addr;
    FADC->SPI_CS = csn;
    while((FADC->SPI_STA & 0x01) == 0x01);  // data valid
    FADC->SPI_DI = wdata;
    while((FADC->SPI_STA & 0x03) != 0x02);  // data valid
    FADC->SPI_CS = 0xFF;
}

/**
 * @brief
 * @DateTime  2022-05-09T16:08:24+0800
 * @param     csn                       Select FADC0 ~ FADC7
 * @param     addr                      Address to be read
 * @return                              Readout data
 */
uint8_t FADC_SPI_ReadSingle(uint8_t csn, uint8_t addr)
{
    uint8_t rdata;
    uint8_t adc_addr;
    //// ADC read & check process
    FADC->SPI_CS = 0xFF;
    adc_addr = adc_addr | 0x40;
    while((FADC->SPI_STA & 0x01) == 0x01);  // data valid
    FADC->SPI_DI = adc_addr;
    FADC->SPI_CS = csn; // select ADC channel
    while((FADC->SPI_STA & 0x01) == 0x01);  // data valid
    FADC->SPI_DI = 0xFF;
    while((FADC->SPI_STA & 0x03) != 0x02);
    FADC->SPI_CS = 0xFF; // select ADC channel
    rdata = FADC->SPI_DO;
    return rdata;
}

/*** (C) COPYRIGHT 2022 SKYRELAY Technology co.,Ltd ***/
