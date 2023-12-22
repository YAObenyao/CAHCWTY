/******************************************************************************
 * @Filename     : i2c.c
 *
 * @Version      : V1.0
 * @Subversion   : $
 * @Last modified: 08/24/2021 16:01:41
 * @Modified by  : Mxy
 *
 * @Brief        : I2C driver source file
 *
 * Copyright (C) 2020 SKYRELAY Technology co.,Ltd All rights reserved.
 *****************************************************************************/
#include "SKY32B750.h"




const i2c_slave_handler_t *slave_handler;


/**
 * @brief       Set i2c params
 *
 * @param[in]   i2c                 i2c number
 * @param[in]   slave_address       i2c slave device address
 * @param[in]   address_width       address width 7bit or 10bit
 * @param[in]   i2c_clk             i2c clk rate
 */
void i2c_Init(I2C_T *i2c, uint32_t slave_address, uint32_t address_width, uint32_t i2c_clk)
{
    uint32_t i2c_apb_freq = 0;
      uint16_t period_clk_cnt = 0;
      uint8_t speed = 0;

//Big Board APB=25M, Small Board APB=5M
      i2c_apb_freq = 5000000;

      if(i2c_clk<=100000)
              speed = I2C_BS_STANDARD;
        else if((i2c_clk>100000)&&(i2c_clk<=400000))
              speed = I2C_BS_FAST;
        else
              speed = I2C_BS_HIGHSPEED;

      period_clk_cnt = (i2c_apb_freq / i2c_clk / 2);
    if(period_clk_cnt == 0)
        period_clk_cnt = 1;

    i2c->I2C_ENABLE = 0;
      i2c->CON = (I2C_CON_MASTER_MODE_Msk | I2C_CON_SLAVE_DISABLE_Msk | I2C_CON_RESTART_EN_Msk |
                 (address_width == 10? I2C_CON_10BITADDR_SLAVE_Msk:0) | (speed << I2C_CON_SPEED_Pos));

    if(speed == I2C_BS_STANDARD)
        {
                i2c->SS_SCL_HCNT = (period_clk_cnt & I2C_SS_SCL_HCNT_COUNT_Msk);
                i2c->SS_SCL_LCNT = (period_clk_cnt & I2C_SS_SCL_LCNT_COUNT_Msk);
        }
    else if(speed == I2C_BS_FAST)
        {
                i2c->FS_SCL_HCNT = (period_clk_cnt & I2C_FS_SCL_HCNT_COUNT_Msk);
                i2c->FS_SCL_LCNT = (period_clk_cnt & I2C_FS_SCL_LCNT_COUNT_Msk);
        }
        else
        {
                i2c->HS_SCL_HCNT = (period_clk_cnt & I2C_HS_SCL_HCNT_COUNT_Msk);
                i2c->HS_SCL_LCNT = (period_clk_cnt & I2C_HS_SCL_LCNT_COUNT_Msk);
        }

    i2c->TAR = ((slave_address>>1) & I2C_TAR_TAR_Msk);
    i2c->INTR_MASK = 0;
    i2c->I2C_ENABLE = I2C_ENABLE_ENABLE_Msk;
}


/**
 * @brief       I2c Transmit data
 *
 * @param[in]   i2c             i2c number
 * @param[in]   send_buf        Transmit data
 * @param[in]   send_buf_len    Transmit data length
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
 */
int i2c_Transmit_Data(I2C_T *i2c, const uint8_t *send_buf, uint8_t send_buf_len)
{
    uint8_t fifo_len, index;

    i2c->CLR_TX_ABART = I2C_CLR_TX_ABRT_CLR;
    while(send_buf_len)
        {
            fifo_len = 8 - i2c->TXFLR;
            fifo_len = send_buf_len < fifo_len ? send_buf_len : fifo_len;
        for(index = 0; index < fifo_len; index++)
            i2c->DATA_CMD = *send_buf++;
        if(i2c->TX_ABRT_SOURCE != 0)
            return NOK;
        send_buf_len -= fifo_len;
        }

    while ((i2c->STATUS & I2C_STATUS_ACTIVITY_Msk) || !(i2c->STATUS & I2C_STATUS_TFE_Msk));
    if(i2c->TX_ABRT_SOURCE != 0)
        return NOK;

    return OK;
}

/**
 * @brief       I2c receive data
 *
 * @param[in]   i2c                 i2c number
 * @param[in]   send_buf            send data address
 * @param[in]   send_buf_len        length of send buf
 * @param[in]   receive_buf         receive buf address
 * @param[in]   receive_buf_len     length of receive buf
 *
 * @return      result
 *     - 0      Success
 *     - Other  Fail
*/
int i2c_Receive_Data(I2C_T *i2c, const uint8_t *send_buf, uint8_t send_buf_len, uint8_t *receive_buf, uint8_t receive_buf_len)
{
    uint8_t fifo_len, index;
    uint8_t rx_len = receive_buf_len;

    while (send_buf_len)
    {
        fifo_len = 8 - i2c->TXFLR;
        fifo_len = send_buf_len < fifo_len ? send_buf_len : fifo_len;
        for (index = 0; index < fifo_len; index++)
            i2c->DATA_CMD = *send_buf++;
        if(i2c->TX_ABRT_SOURCE != 0)
            return NOK;
        send_buf_len -= fifo_len;
    }

    while (receive_buf_len || rx_len)
    {
        fifo_len = i2c->RXFLR;
        fifo_len = rx_len < fifo_len ? rx_len : fifo_len;
        for (index = 0; index < fifo_len; index++)
            *receive_buf++ = (uint8_t)i2c->DATA_CMD;
        rx_len -= fifo_len;
        fifo_len = 8 - i2c->TXFLR;
        fifo_len = receive_buf_len < fifo_len ? receive_buf_len : fifo_len;
        for (index = 0; index < fifo_len; index++)
            i2c->DATA_CMD = I2C_DATA_CMD_CMD_Msk;
        if(i2c->TX_ABRT_SOURCE != 0)
            return NOK;
        receive_buf_len -= fifo_len;
    }

    return OK;
}


/**
 * @brief       Init i2c as slave mode.
 *
 * @param[in]   i2c                 i2c number
 * @param[in]   slave_address       i2c slave device address
 * @param[in]   address_width       address width 7bit or 10bit
 * @param[in]   handler             Handle of i2c slave interrupt function.
 */
void i2c_Init_As_Slave(I2C_T *i2c, uint32_t slave_address, uint32_t address_width, const i2c_slave_handler_t *handler)
{

      slave_handler = handler;

    i2c->I2C_ENABLE = 0;
     // i2c->CON = (address_width == 10? I2C_CON_10BITADDR_SLAVE_Msk:0) | (0x01 << I2C_CON_SPEED_Pos) | I2C_CON_STOP_DET_IFADDRESSED;
        i2c->CON = (address_width == 10? I2C_CON_10BITADDR_SLAVE_Msk:0) | (0x01 << I2C_CON_SPEED_Pos);
    i2c->SS_SCL_HCNT = (50 & I2C_SS_SCL_HCNT_COUNT_Msk);
    i2c->SS_SCL_LCNT = (50 & I2C_SS_SCL_LCNT_COUNT_Msk);
    i2c->SAR = ((slave_address) & I2C_SAR_SAR_Msk);
        i2c->RX_TL = 0&I2C_RX_TL_VALUE_Msk;
        i2c->TX_TL = 0&I2C_TX_TL_VALUE_Msk;
//    i2c->INTR_MASK = I2C_INTR_MASK_RX_FULL_Msk | I2C_INTR_MASK_START_DET_Msk | I2C_INTR_MASK_STOP_DET_Msk | I2C_INTR_MASK_RD_REQ_Msk;
      i2c->INTR_MASK = I2C_INTR_MASK_RX_FULL_Msk | I2C_INTR_MASK_RD_REQ_Msk;
      NVIC_EnableIRQ(I2C_IRQn);
    i2c->I2C_ENABLE = I2C_ENABLE_ENABLE_Msk;
}


void I2C_IRQHandler(void)
{
    uint32_t status = I2C->INTR_STAT;

    if(status & I2C_INTR_STAT_START_DET_Msk)
    {
        slave_handler->on_event(I2C_EV_START);
        I2C->CLR_START_DET = I2C_CLR_START_DET_CLR;
    }
    if(status & I2C_INTR_STAT_STOP_DET_Msk)
    {
        slave_handler->on_event(I2C_EV_STOP);
        I2C->CLR_STOP_DET = I2C_CLR_STOP_DET_CLR;
    }
    if(status & I2C_INTR_STAT_RX_FULL_Msk)
    {
        slave_handler->on_receive(I2C->DATA_CMD);
    }
    if(status & I2C_INTR_STAT_RD_REQ_Msk)
    {
             I2C->DATA_CMD = 0x45;
             I2C->DATA_CMD = 0x55;
             I2C->DATA_CMD = 0xBB;
             I2C->DATA_CMD = 0xAC;
             I2C->DATA_CMD = 0x12;
             I2C->DATA_CMD = 0x53;
             I2C->DATA_CMD = 0xF1;
        I2C->DATA_CMD = slave_handler->on_transmit();
        I2C->CLR_RD_REQ = I2C_CLR_RD_REQ_CLR;
    }

}












