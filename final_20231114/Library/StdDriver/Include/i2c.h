/******************************************************************************
 * @Filename     : i2c.h
 *
 * @Version      : V1.0
 * @Subversion   : $
 * @Last modified: 08/24/2021 16:01:41
 * @Modified by  : Mxy
 *
 * @Brief        : I2C driver header file
 *
 * Copyright (C) 2020 SKYRELAY Technology co.,Ltd All rights reserved.
 *****************************************************************************/
#ifndef __I2C_H__
#define __I2C_H__
#include "SKY32B750.h"

#ifdef __cplusplus
extern "C"
{
#endif


typedef enum
{
    I2C_BS_STANDARD = 1,
    I2C_BS_FAST = 2,
    I2C_BS_HIGHSPEED = 3
}i2c_bus_speed_mode_t;

typedef enum
{
    I2C_EV_START,
    I2C_EV_RESTART,
    I2C_EV_STOP
}i2c_event_t;

typedef struct
{
    void(*on_receive)(uint32_t data);
    uint32_t(*on_transmit)();
    void(*on_event)(i2c_event_t event);
}i2c_slave_handler_t;


/**
 * @brief       Set i2c params
 *
 * @param[in]   i2c                 i2c number
 * @param[in]   slave_address       i2c slave device address
 * @param[in]   address_width       address width 7bit or 10bit
 * @param[in]   i2c_clk             i2c clk rate
 */
void i2c_Init(I2C_T *i2c, uint32_t slave_address, uint32_t address_width, uint32_t i2c_clk);

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
int i2c_Transmit_Data(I2C_T *i2c, const uint8_t *send_buf, uint8_t send_buf_len);

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
int i2c_Receive_Data(I2C_T *i2c, const uint8_t *send_buf, uint8_t send_buf_len, uint8_t *receive_buf, uint8_t receive_buf_len);

/**
 * @brief       Init i2c as slave mode.
 *
 * @param[in]   i2c                 i2c number
 * @param[in]   slave_address       i2c slave device address
 * @param[in]   address_width       address width 7bit or 10bit
 * @param[in]   handler             Handle of i2c slave interrupt function.
 */
void i2c_Init_As_Slave(I2C_T *i2c, uint32_t slave_address, uint32_t address_width, const i2c_slave_handler_t *handler);


#ifdef __cplusplus
}
#endif


#endif




