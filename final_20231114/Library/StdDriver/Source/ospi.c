/******************************************************************************
 * @Filename     : ospi.c
 *
 * @Version      : V1.0
 * @Subversion   : $
 * @Last modified: 07/30/2021 16:01:41
 * @Modified by  : Mxy
 *
 * @Brief        : OSPI driver source file
 *
 * Copyright (C) 2020 SKYRELAY Technology co.,Ltd All rights reserved.
 *****************************************************************************/
#include "SKY32B750.h"

#if (ONE_CHIP)
    #define  OSPI_STAND_CONFIG  0x00001005
#elif (TWO_CHIP)
    #define  OSPI_STAND_CONFIG  0x00001035
#endif


void ospi_Cmd(OSPI_T *ospi, XOSPI_T *xospi, uint8_t cmd)
{
    ospi->CS = 0x00000000;

      xospi->DATA8[0] = cmd;
        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
        ospi->INT_STA = 0x00000001;

    ospi->CS = 0x00000001;
}

uint32_t ospi_Status_Op(OSPI_T *ospi, XOSPI_T *xospi, uint8_t cmd, uint8_t status1, uint8_t status2)
{
        //read status reg -> enable qual mode
    ospi->CS = 0x00000000;

      xospi->DATA8[0] = cmd;
        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
        ospi->INT_STA = 0x00000001;

        xospi->DATA8[1] = status1;
        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
        ospi->INT_STA = 0x00000001;

        xospi->DATA8[2] = status2;
        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
        ospi->INT_STA = 0x00000001;

    ospi->CS = 0x00000001;

        return xospi->DATA32[0];
}




void ospi_Init(OSPI_T *ospi)
{
    ospi->COMMON_CFG = OSPI_STAND_CONFIG;

    ospi->FUNC_CFG = 0xEB06FF30;
    ospi->CS = 0x03;
    ospi->INT_STA = 0x07;
}


//read
void ospi_Receive(OSPI_T *ospi, XOSPI_T *xospi, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *rx_buf, uint32_t rx_len, ospi_frame_mode_t ospi_mode)
{
      uint32_t i=0;

    ospi->COMMON_CFG = OSPI_STAND_CONFIG;
      ospi->CS = 0x00000000;

      //cmd & addr
      xospi->DATA32[0] = *cmd_buf;
        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
        ospi->INT_STA = 0x00000001;

      //send dummy
        for(i=0; i<dummy_len; i++)
    {
            xospi->DATA8[4] = 0xFF;
            while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
            ospi->INT_STA = 0x00000001;
        }

        //
        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
        ospi->COMMON_CFG |= (ospi_mode<<OSPI_COMMON_CFG_MODE_Pos);

    //read data
        for(i=0; i<rx_len; i++)
    {
            xospi->DATA8[i] = 0xFF;
            while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
            ospi->INT_STA = 0x00000001;
        *rx_buf++    = (uint8_t)xospi->DATA64[i+1];         //??????????
        }

        ospi->COMMON_CFG = OSPI_STAND_CONFIG;
        ospi->CS = 0x00000001;
}

//write
void ospi_Transmit(OSPI_T *ospi, XOSPI_T *xospi, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *tx_buf, uint32_t tx_len, ospi_frame_mode_t ospi_mode)
{
    uint32_t i=0;

    ospi->COMMON_CFG = OSPI_STAND_CONFIG;
    ospi->CS = 0x00000000;

      //cmd & addr
      xospi->DATA32[0] = *cmd_buf;
        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
        ospi->INT_STA = 0x00000001;

        //
        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
        ospi->COMMON_CFG |= (ospi_mode<<OSPI_COMMON_CFG_MODE_Pos);

      for(i=0; i<tx_len; i++)
        {
            xospi->DATA8[i] = *tx_buf++;
            while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
            ospi->INT_STA = 0x00000001;
        }

    ospi->COMMON_CFG = OSPI_STAND_CONFIG;
    ospi->CS = 0x00000001;
}



//IO
//read
void ospi_Receive_IO(OSPI_T *ospi, XOSPI_T *xospi, uint8_t cmd, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *rx_buf, uint32_t rx_len, ospi_frame_mode_t out_ospi_mode, ospi_frame_mode_t in_ospi_mode)
{
      uint32_t i=0;

    ospi->COMMON_CFG = OSPI_STAND_CONFIG;
      ospi->CS = 0x00000000;

      //cmd
      xospi->DATA8[0] = cmd;
        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
        ospi->INT_STA = 0x00000001;

        //out
        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
        ospi->COMMON_CFG |= (out_ospi_mode<<OSPI_COMMON_CFG_MODE_Pos);

      //addr & A5
      xospi->DATA32[0] = *cmd_buf;
        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
        ospi->INT_STA = 0x00000001;

      //send dummy
        for(i=0; i<dummy_len; i++)
    {
            xospi->DATA8[4] = 0xFF;
            while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
            ospi->INT_STA = 0x00000001;
        }

        //
        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
        ospi->COMMON_CFG |= (in_ospi_mode<<OSPI_COMMON_CFG_MODE_Pos);

    //read data
        for(i=0; i<rx_len; i++)
    {
            xospi->DATA8[i] = 0xFF;
            while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
            ospi->INT_STA = 0x00000001;
        *rx_buf++    = (uint8_t)xospi->DATA64[i+1];         //??????????
        }

        ospi->COMMON_CFG = OSPI_STAND_CONFIG;
        ospi->CS = 0x00000001;
}





void ospi_Enable_Xcode_Mode(void)
{
//must use 8 bit CHIP test, 0xEB command is only in D0
//    OSPI0->COMMON_CFG = 0x000010E5;

//QUAD
#if (ONE_CHIP)
    OSPI0->COMMON_CFG = 0x000010A5;
#elif (TWO_CHIP)
    OSPI0->COMMON_CFG = 0x000010C5;
#endif
//    OSPI0->COMMON_CFG = 0x000010C5;
//    OSPI0->COMMON_CFG = 0x000010A5;

//    OSPI0->COMMON_CFG = 0x00001005;
//      OSPI0->FUNC_CFG =   0x0310FF05;

//    OSPI0->COMMON_CFG = 0x00001055;
//      OSPI0->FUNC_CFG =   0xBB14FF05;

//FPGA
//    OSPI0->FUNC_CFG =   0xEB16FF05;
//chip
    OSPI0->FUNC_CFG =   0xEB1AFF05;


//OSPI0->FUNC_CFG =   0x0310FF05;

//DUAL
//    OSPI0->COMMON_CFG = 0x00001055;
//    OSPI0->FUNC_CFG =   0xBB16FF05;

//    OSPI0->CS = 0x03;
    OSPI0->INT_STA = 0x07;

}


void ospi_Disable_Xcode_Mode(void)
{
    OSPI0->COMMON_CFG = OSPI_STAND_CONFIG;
    OSPI0->FUNC_CFG =   0xEB06FF00;
    OSPI0->CS = 0x03;
    OSPI0->INT_STA = 0x07;
}




//slave





//dma



//read
void o_ospi_Receive(OSPI_T *ospi, XOSPI_T *xospi, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *rx_buf, uint32_t rx_len, ospi_frame_mode_t ospi_mode_fun0, ospi_frame_mode_t ospi_mode_fun1)
{
      uint32_t i=0;

        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
        ospi->COMMON_CFG |= (ospi_mode_fun0<<OSPI_COMMON_CFG_MODE_Pos);
      ospi->CS = 0x00000000;

      //cmd & addr
      xospi->DATA32[0] = *cmd_buf;
        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
        ospi->INT_STA = 0x00000001;

      //send dummy
        for(i=0; i<dummy_len; i++)
    {
            xospi->DATA8[4] = 0xFF;
            while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
            ospi->INT_STA = 0x00000001;
        }

        //
        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
        ospi->COMMON_CFG |= (ospi_mode_fun1<<OSPI_COMMON_CFG_MODE_Pos);

    //read data
        for(i=0; i<rx_len; i++)
    {
            xospi->DATA8[i] = 0xFF;
            while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
            ospi->INT_STA = 0x00000001;
        *rx_buf++    = (uint8_t)xospi->DATA64[i+1];         //??????????
        }

        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
        ospi->COMMON_CFG |= (ospi_mode_fun0<<OSPI_COMMON_CFG_MODE_Pos);
        ospi->CS = 0x00000001;
}


//write
void o_ospi_Transmit(OSPI_T *ospi, XOSPI_T *xospi, uint32_t *cmd_buf, uint8_t dummy_len, uint8_t *tx_buf, uint32_t tx_len, ospi_frame_mode_t ospi_mode_fun0, ospi_frame_mode_t ospi_mode_fun1)
{
    uint32_t i=0,j=0,n=0;

        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
        ospi->COMMON_CFG |= (ospi_mode_fun0<<OSPI_COMMON_CFG_MODE_Pos);
    ospi->CS = 0x00000000;

      //cmd & addr
//      xospi->DATA32[0] = *cmd_buf;
//        while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
//        ospi->INT_STA = 0x00000001;

//        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
//        ospi->COMMON_CFG |= (ospi_mode_fun1<<OSPI_COMMON_CFG_MODE_Pos);

      for(i=0; i<tx_len; i++)
        {
        xospi->DATA8[i] = (uint8_t)*tx_buf++;
            while((ospi->INT_STA&OSPI_STA_IRQ_Msk) != 0x00000001){};
            ospi->INT_STA = 0x00000001;
        }

        ospi->COMMON_CFG &= (~OSPI_COMMON_CFG_MODE_Msk);
        ospi->COMMON_CFG |= (ospi_mode_fun0<<OSPI_COMMON_CFG_MODE_Pos);

    ospi->CS = 0x00000001;
}









