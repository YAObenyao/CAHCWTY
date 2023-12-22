/******************************************************************************
 * @Filename     : qspi.c
 *
 * @Version      : V1.0
 * @Subversion   : $
 * @Last modified: 06/15/2022 16:01:41
 * @Modified by  : CB
 *
 * @Brief        : QSPI driver source file
 *
 * Copyright (C) 2022 SKYRELAY Technology co.,Ltd All rights reserved.
 *****************************************************************************/
#include "SKY32B750.h"

static void QSPI_Ctrl_WithMask(QSPI_T *qspi, uint32_t mask, uint32_t value, uint8_t pos)
{
  uint32_t reg = 0;
  reg = qspi->CON;
  reg &= ~mask;
  reg |= (mask & (value << pos));
  qspi->CON = reg;
}

/**
 * @Breif  : QSPI_Init     : Qspi init function
 *
 * @Param  : ctrl_reg      : Set qspi->CON value, Normal mode
 *
 * @Returns: qspi_status   : QSPI_PARA_ERROR
 *                         : QSPI_OK
 */
enum qspi_status QSPI_Init(QSPI_T *qspi, uint32_t ctrl_reg)
{
  if (ctrl_reg == NULL)
    return QSPI_PARA_ERROR;

  qspi->CON = ctrl_reg;

  return QSPI_OK;
}

/**
 * @Breif  : QSPI_Receive  : Qspi Single mode Receive function
 *
 * @Param  : *cmd_buf      : pointer to Command data buffer
 * @Param  : cmd_len       : qspi command data byte length
 * @Param  : *rx_buf       : pointer to rx data buffer
 * @Param  : rx_len        : rx data byte length
 *
 * @Returns: qspi_status   : QSPI_PARA_ERROR
 *                         : QSPI_OK
 */
enum qspi_status QSPI_Receive(QSPI_T *qspi, uint8_t *cmd_buf, uint8_t cmd_len, uint8_t *rx_buf, uint32_t rx_len)
{
  if ((cmd_buf == NULL) || (cmd_len <= 0) || (rx_buf == NULL) || (rx_len <= 0))
    return QSPI_PARA_ERROR;

  qspi->ST = QSPI_ST_IRQ_Msk;  //Clear IRQ

  qspi->CS = (QSPI_CS_CS_LOW << QSPI_CS_CS_Pos);
  while (cmd_len--)
  {
    qspi->DATA8[0] = *cmd_buf++;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }
  while (rx_len--)
  {
    qspi->DATA8[0] = QSPI_DUMMY_DATA;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
    *rx_buf++ = qspi->DATA8[0];
  }
  qspi->CS = (QSPI_CS_CS_HIGH << QSPI_CS_CS_Pos);

  return QSPI_OK;
}

/**
 * @Breif  : QSPI_Transmit : Qspi Single mode Transmit function
 *
 * @Param  : *cmd_buf      : pointer to Command data buffer
 * @Param  : cmd_len       : qspi command data byte length
 * @Param  : *tx_buf       : pointer to tx data buffer
 * @Param  : tx_len        : tx data byte length
 *
 * @Returns: qspi_status : QSPI_PARA_ERROR
 *                         : QSPI_OK
 */
enum qspi_status QSPI_Transmit(QSPI_T *qspi, uint8_t *cmd_buf, uint8_t cmd_len, uint8_t *tx_buf, uint32_t tx_len)
{
  if ((cmd_buf == NULL) || (cmd_len <= 0))
    return QSPI_PARA_ERROR;

  qspi->ST = QSPI_ST_IRQ_Msk; //Clear IRQ

  qspi->CS = (QSPI_CS_CS_LOW << QSPI_CS_CS_Pos);
  while (cmd_len--)
  {
    qspi->DATA8[0] = *cmd_buf++;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }
  while (tx_len--)
  {
    qspi->DATA8[0] = *tx_buf++;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }
  qspi->CS = (QSPI_CS_CS_HIGH << QSPI_CS_CS_Pos);

  return QSPI_OK;
}

/**
 * @Breif  : QSPI_Receive_Enhanced : Qspi DUAL and QUAD mode Receive function
 *
 * @Param  : *cmd_buf      : pointer to Command data buffer
 * @Param  : cmd_len       : qspi command data byte length
 * @Param  : *rx_buf       : pointer to rx data buffer
 * @Param  : rx_len        : rx data byte length
 * @Param  : qspi_mode     : QSPI_DUAL_INPUT or QSPI_QUAD_INPUT
 *
 * @Returns: qspi_status : QSPI_PARA_ERROR
 *                         : QSPI_OK
 */
enum qspi_status QSPI_Receive_Enhanced(QSPI_T *qspi, uint32_t *cmd_buf, uint8_t cmd_len, uint8_t *rx_buf, uint32_t rx_len, enum qspi_frame_mode qspi_mode)
{
  if ((cmd_buf == NULL) || (cmd_len <= 0) || (rx_buf == NULL) || (rx_len <= 0))
    return QSPI_PARA_ERROR;

  qspi->ST = QSPI_ST_IRQ_Msk;  //Clear IRQ

  qspi->CS = (QSPI_CS_CS_LOW << QSPI_CS_CS_Pos);
  while (cmd_len--)
  {
    qspi->DATA = *cmd_buf++;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }
  qspi->DATA8[0] = QSPI_DUMMY_DATA;
  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, qspi_mode, QSPI_CON_MODE_Pos);
  while (rx_len--)
  {
    qspi->DATA8[0] = QSPI_DUMMY_DATA;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
    *rx_buf++ = qspi->DATA8[0];
  }
  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, QSPI_STANDARD, QSPI_CON_MODE_Pos);
  qspi->CS = (QSPI_CS_CS_HIGH << QSPI_CS_CS_Pos);

  return QSPI_OK;
}

/**
 * @Breif  : QSPI_Transmit_Enhanced : Qspi DUAL and QUAD mode Transmit function
 *
 * @Param  : *cmd_buf      : pointer to Command data buffer
 * @Param  : cmd_len       : qspi command data byte length
 * @Param  : *tx_buf       : pointer to tx data buffer
 * @Param  : tx_len        : tx data byte length
 * @Param  : qspi_mode     : QSPI_DUAL_OUTPUT or QSPI_QUAD_OUTPUT
 *
 * @Returns: qspi_status : QSPI_PARA_ERROR
 *                         : QSPI_OK
 */
enum qspi_status QSPI_Transmit_Enhanced(QSPI_T *qspi, uint32_t *cmd_buf, uint8_t cmd_len, uint8_t *tx_buf, uint32_t tx_len, enum qspi_frame_mode qspi_mode)
{
  if ((cmd_buf == NULL) || (cmd_len <= 0) || (tx_buf == NULL) || (tx_len <= 0))
    return QSPI_PARA_ERROR;

  qspi->ST = QSPI_ST_IRQ_Msk;  //Clear IRQ

  qspi->CS = (QSPI_CS_CS_LOW << QSPI_CS_CS_Pos);
  while (cmd_len--)
  {
    qspi->DATA = *cmd_buf++;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }
  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, qspi_mode, QSPI_CON_MODE_Pos);
  while (tx_len--)
  {
    qspi->DATA8[0] = *tx_buf++;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }
  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, QSPI_STANDARD, QSPI_CON_MODE_Pos);
  qspi->CS = (QSPI_CS_CS_HIGH << QSPI_CS_CS_Pos);

  return QSPI_OK;
}

/**
 * @Breif  : QSPI_Receive_Enhanced_IO_First : Qspi QUAD Fast mode Receive function
 * User    : (QSPI_Receive_Enhanced_IO_First + QSPI_Receive_Enhanced_IO)
 * @Param  : *cmd_buf      : pointer to Command data buffer
 * @Param  : cmd_len       : qspi command data byte length
 * @Param  : *rx_buf       : pointer to rx data buffer
 * @Param  : rx_len        : rx data byte length
 * @Param  : qspi_mode     : QSPI_QUAD_INPUT    :  QUAD_FAST,
 *
 * @Returns: qspi_status : QSPI_PARA_ERROR
 *                         : QSPI_OK
 */
enum qspi_status QSPI_Receive_Enhanced_IO_First(QSPI_T *qspi, uint32_t *cmd_buf, uint8_t cmd_len, uint8_t *rx_buf, uint32_t rx_len, enum qspi_frame_mode qspi_mode)
{
  uint8_t i = 0;

  if ((cmd_buf == NULL) || (cmd_len <= 0) || (rx_buf == NULL) || (rx_len <= 0))
    return QSPI_PARA_ERROR;

  qspi->ST = QSPI_ST_IRQ_Msk;  //Clear IRQ

  qspi->CS = (QSPI_CS_CS_LOW << QSPI_CS_CS_Pos);
  //send command
  qspi->DATA8[0] = (uint8_t) *(cmd_buf + 1);
  while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
  qspi->ST = QSPI_ST_IRQ_Msk;

  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, QSPI_QUAD_OUTPUT, QSPI_CON_MODE_Pos);

  //send ADDR and P,
  while (cmd_len--)
  {
    qspi->DATA = *cmd_buf++;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }

  //send Dummy
  for (i = 0; i < 4; i++)
  {
    qspi->DATA8[0] = QSPI_CON_READ_MODE_FIRST_STEP;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }

  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, qspi_mode, QSPI_CON_MODE_Pos);
  //rx
  while (rx_len--)
  {
    qspi->DATA8[0] = QSPI_DUMMY_DATA;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
    *rx_buf++ = qspi->DATA8[0];
  }
  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, QSPI_STANDARD, QSPI_CON_MODE_Pos);
  qspi->CS = (QSPI_CS_CS_HIGH << QSPI_CS_CS_Pos);

  return QSPI_OK;
}

/**
 * @Breif  : QSPI_Receive_Enhanced_IO : Qspi QUAD Fast mode Receive function
 * User  : (QSPI_Receive_Enhanced_IO_First + QSPI_Receive_Enhanced_IO)
 * @Param  : *cmd_buf      : pointer to Command data buffer
 * @Param  : cmd_len       : qspi command data byte length
 * @Param  : *rx_buf       : pointer to rx data buffer
 * @Param  : rx_len        : rx data byte length
 * @Param  : qspi_mode     : QSPI_QUAD_INPUT    :  QUAD_FAST,
 *
 * @Returns: qspi_status : QSPI_PARA_ERROR
 *                         : QSPI_OK
 */
enum qspi_status QSPI_Receive_Enhanced_IO(QSPI_T *qspi, uint32_t *cmd_buf, uint8_t cmd_len, uint8_t *rx_buf, uint32_t rx_len, enum qspi_frame_mode qspi_mode)
{
  uint8_t i = 0;

  if ((cmd_buf == NULL) || (cmd_len <= 0) || (rx_buf == NULL) || (rx_len <= 0))
    return QSPI_PARA_ERROR;

  qspi->ST = QSPI_ST_IRQ_Msk;
  qspi->CS = (QSPI_CS_CS_LOW << QSPI_CS_CS_Pos);

  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, QSPI_QUAD_OUTPUT, QSPI_CON_MODE_Pos);
  //send ADDR and P, and Dummy
  while (cmd_len--)
  {
    qspi->DATA = *cmd_buf++;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }
  //send Dummy
  for (i = 0; i < 4; i++)
  {
    qspi->DATA8[0] = QSPI_CON_READ_MODE_SECOND_STEP;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
  }
  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, qspi_mode, QSPI_CON_MODE_Pos);
  while (rx_len--)
  {
    qspi->DATA8[0] = QSPI_DUMMY_DATA;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
    *rx_buf++ = qspi->DATA8[0];
  }
  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, QSPI_STANDARD, QSPI_CON_MODE_Pos);
  qspi->CS = (QSPI_CS_CS_HIGH << QSPI_CS_CS_Pos);

  return QSPI_OK;
}

/**
 * @Breif  : QSPI_Receive_Enhanced_IO_DUAL : Qspi DUAL Fast mode Receive function
 *
 * @Param  : *cmd_buf      : pointer to Command data buffer
 * @Param  : cmd_len       : qspi command data byte length
 * @Param  : *rx_buf       : pointer to rx data buffer
 * @Param  : rx_len        : rx data byte length
 * @Param  : qspi_mode     : QSPI_DUAL_INPUT    :  DUAL_FAST,
 *
 * @Returns: qspi_status : QSPI_PARA_ERROR
 *                         : QSPI_OK
 */
enum qspi_status QSPI_Receive_Enhanced_IO_DUAL(QSPI_T *qspi, uint32_t *cmd_buf, uint8_t cmd_len, uint8_t *rx_buf, uint32_t rx_len, enum qspi_frame_mode qspi_mode)
{
  uint8_t i = 0;

  if ((cmd_buf == NULL) || (cmd_len <= 0) || (rx_buf == NULL) || (rx_len <= 0))
    return QSPI_PARA_ERROR;

  qspi->ST = QSPI_ST_IRQ_Msk;
  qspi->CS = (QSPI_CS_CS_LOW << QSPI_CS_CS_Pos);

  //send command
  qspi->DATA8[0] = (uint8_t) * (cmd_buf + 1);
  while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
  qspi->ST = QSPI_ST_IRQ_Msk;

  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, qspi_mode, QSPI_CON_MODE_Pos);

  //send ADDR and P,
  while (cmd_len--)
  {
    qspi->DATA = *cmd_buf++;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0)
      ;
    qspi->ST = QSPI_ST_IRQ_Msk;
  }

  //send Dummy
  for (i = 0; i < 2; i++)
  {
    qspi->DATA8[0] = QSPI_CON_READ_MODE_FIRST_STEP;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0)
      ;
    qspi->ST = QSPI_ST_IRQ_Msk;
  }

  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, qspi_mode, QSPI_CON_MODE_Pos);
  //rx
  while (rx_len--)
  {
    qspi->DATA8[0] = QSPI_DUMMY_DATA;
    while ((qspi->ST & QSPI_ST_IRQ_Msk) == 0);
    qspi->ST = QSPI_ST_IRQ_Msk;
    *rx_buf++ = qspi->DATA8[0];
  }
  QSPI_Ctrl_WithMask(qspi, QSPI_CON_MODE_Msk, QSPI_STANDARD, QSPI_CON_MODE_Pos);
  qspi->CS = (QSPI_CS_CS_HIGH << QSPI_CS_CS_Pos);

  return QSPI_OK;
}

/**
 * @Breif  : QSPI_Enable_Xcode_Mode : Qspi Enable XIP function
 *
 * @Param  : ctrl_reg      : Set qspi->CON value, XIP mode
 *
 * @Returns: qspi_status : QSPI_PARA_ERROR
 *                         : QSPI_OK
 */
enum qspi_status QSPI_Enable_Xcode_Mode(QSPI_T *qspi, uint32_t ctrl_reg)
{
  if (ctrl_reg == NULL)
    return QSPI_PARA_ERROR;

  qspi->CON = ctrl_reg;
  qspi->CS = 0x0000;
  qspi->CS |= (QSPI_CS_CMD_ON_Msk | QSPI_CS_CS_HIGH);

  return QSPI_OK;
}

/**
 * @Breif  : QSPI_Disable_Xcode_Mode : Qspi Disable XIP function
 *
 * @Param  : ctrl_reg      : Set qspi->CON value, 0x00
 *
 * @Returns: qspi_status : QSPI_OK
 *
 */
enum qspi_status QSPI_Disable_Xcode_Mode(QSPI_T *qspi)
{
  qspi->CON = QSPI_STANDARD;
  return QSPI_OK;
}

/*** (C) COPYRIGHT 2022 SKYRELAY Technology co.,Ltd ***/
