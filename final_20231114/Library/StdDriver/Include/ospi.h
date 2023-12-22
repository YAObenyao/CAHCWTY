/******************************************************************************
 * @Filename     : ospi.h
 *
 * @Version      : V1.0
 * @Subversion   : $
 * @Last modified: 07/29/2021 16:01:41
 * @Modified by  : Mxy
 *
 * @Brief        : OSPI driver header file
 *
 * Copyright (C) 2020 SKYRELAY Technology co.,Ltd All rights reserved.
 *****************************************************************************/
#ifndef __OSPI_H__
#define __OSPI_H__

#include "SKY32B750.h"



#ifdef __cplusplus
extern "C"
{
#endif

//User config from hardware, use 1 or 2 chip, or 3, 4 Chips
#define ONE_CHIP  1
#define TWO_CHIP  0

#define XOSPI0_BASE             ((uint32_t)0x63000000)          //// EXE OSPI0 base address
#define XOSPI1_BASE             ((uint32_t)0x64000000)          //// EXE OSPI1 base address

#pragma anon_unions
typedef struct {

    union
    {
            __IO uint64_t DATA64[8];
      __IO uint32_t DATA32[16];
      __IO uint8_t  DATA8[64];
    };
} XOSPI_T;

#define XOSPI0                     ((XOSPI_T *) XOSPI0_BASE)              //// 0x50080000 Pointer to OSPI register structure
#define XOSPI1                     ((XOSPI_T *) XOSPI1_BASE)              //// 0x50090000 Pointer to OSPI register structure

typedef enum
{
      OSPI_SING      = 0x00ul,         /* (cs,clk,out1,in1,NO,NO,NO,NO,NO,NO) */            //-----------
      OSPI_DSING     = 0x01ul,         /* (cs,clk,out1,in1,out1,in1,NO,NO,NO,NO) */
      OSPI_QSING     = 0x02ul,         /* (cs,clk,out1,in1,out1,in1,out1,in1,out1,in1) */
      OSPI_OSING     = 0x03ul,         /* (cs,clk,out1,in1,NO,NO,out1,in1,NO,NO) */            //---------
      OSPI_I_DUAL    = 0x04ul,         /* (cs,clk,in1,in2,NO,NO,NO,NO,NO,NO) */
      OSPI_O_DUAL    = 0x05ul,         /* (cs,clk,out1,out2,NO,NO,NO,NO,NO,NO) */
      OSPI_I_DDUAL   = 0x06ul,         /* (cs,clk,in1,in2,NO,NO,in1,in2,NO,NO) */
      OSPI_O_DDUAL   = 0x07ul,         /* (cs,clk,out1,out2,NO,NO,out1,out2,NO,NO) */
      OSPI_I_QDUAL   = 0x08ul,         /* (cs,clk,in1,in2,in1,in2,in1,in2,in1,in2) */
      OSPI_O_QDUAL   = 0x09ul,         /* (cs,clk,out1,out2,out1,out2,out1,out2,out1,out2) */
      OSPI_I_QUAD    = 0x0Aul,         /* (cs,clk,in1,in2,in3,in4,NO,NO,NO,NO) */
      OSPI_O_QUAD    = 0x0Bul,         /* (cs,clk,out1,out2,out3,out4,NO,NO,NO,NO) */
      OSPI_I_DQUAD   = 0x0Cul,         /* (cs,clk,in1,in2,in3,in4,in1,in2,in3,in4) */             //---------
      OSPI_O_DQUAD   = 0x0Dul,         /* (cs,clk,out1,out2,out3,out4,out1,out2,out3,out4) */      //---------
      OSPI_I_OCTU    = 0x0Eul,         /* (cs,clk,in1,in2,in3,in4,in5,in6,in7,in8) */
      OSPI_O_OCTU    = 0x0Ful,         /* (cs,clk,out1,out2,out3,out4,out5,out6,out7,out8) */
    OSPI_MAX       = OSPI_SING,
} ospi_frame_mode_t;



void ospi_Init(OSPI_T *ospi);
void ospi_Cmd(OSPI_T *ospi, XOSPI_T *xospi, uint8_t cmd);
uint32_t ospi_Status_Op(OSPI_T *ospi, XOSPI_T *xospi, uint8_t cmd, uint8_t status1, uint8_t status2);
void ospi_Receive(OSPI_T *ospi, XOSPI_T *xospi, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *rx_buf, uint32_t rx_len, ospi_frame_mode_t ospi_mode);
void ospi_Transmit(OSPI_T *ospi, XOSPI_T *xospi, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *tx_buf, uint32_t tx_len, ospi_frame_mode_t ospi_mode);
void ospi_Enable_Xcode_Mode(void);
void ospi_Disable_Xcode_Mode(void);

void ospi_Receive_IO(OSPI_T *ospi, XOSPI_T *xospi, uint8_t cmd, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *rx_buf, uint32_t rx_len, ospi_frame_mode_t out_ospi_mode, ospi_frame_mode_t in_ospi_mode);


void o_ospi_Receive(OSPI_T *ospi, XOSPI_T *xospi, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *rx_buf, uint32_t rx_len, ospi_frame_mode_t ospi_mode_fun0, ospi_frame_mode_t ospi_mode_fun1);
void o_ospi_Transmit(OSPI_T *ospi, XOSPI_T *xospi, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *tx_buf, uint32_t tx_len, ospi_frame_mode_t ospi_mode_fun0, ospi_frame_mode_t ospi_mode_fun1);


#ifdef __cplusplus
}
#endif


#endif


