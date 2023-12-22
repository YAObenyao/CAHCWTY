/******************************************************************************
 * @file led.h
 * @brief 
 * @author liyang
 * @version 0.1
 * @date 2022-12-26
 * 
 * Copyright (c) 2022,xxx xxx Co., Ltd.
******************************************************************************/
/*-----------------  Define to prevent recursive inclusion  -----------------*/
#ifndef __LED_H
#define __LED_H
 
#ifdef __cplusplus
extern "C"
{
#endif
 
/*-------------------------------  Includes  --------------------------------*/
#include <stdio.h>
/*core*/
#include "SKY32B750.h"


/*-----------------------------  Exported types  ----------------------------*/
/*--------------------------  Exported constants  ---------------------------*/
/*-----------------------------  Exported macro  ----------------------------*/
/*--------------------------  Exported functions  ---------------------------*/

/***************************************************************
 * @funcname   TimerInit
 * @brief      LED 
 * @author     liyang
 * @version    0.0.1
 * @date       20221222
 **************************************************************/
void TimerInit(void);
void StartSignalDetect(void);

#define DELAYTIME 100

 
#ifdef __cplusplus
}
#endif  /*__cpluspuls*/
 
#endif /*__LED_H*/
 
/******************************************************************************
          End of the File (EOF). Do not put anything after this part!
******************************************************************************/
