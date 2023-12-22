/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _MAIN_H
#define _MAIN_H
/*Local*/
#include <stdio.h>
/*core*/
#include "SKY32B750.h"
/*Driver*/
#include "mpu.h"
/*MidDriver*/
#include "SPI_Driver.h"
#include "GPIO_Driver.h"
#include "USART_Driver.h"
#include "MPU_Driver.h"
#include "Delay_Driver.h"
#include "DMA_Driver.h"
#include "ADC_Driver.h"
#include "SysClk_Driver.h"
/*User*/
#include "RF_SPI_Config.h"
#include "BB_Config.h"
#include "Usart_Protocol.h"
#include "ParamConfig.h"
#include "product.h"
/*App*/
#include "App_Func.h"



#define OBSER_WIN 5
#define RANGE_UP_DIFFERENCE   1.5f
#define RANGE_DOWN_DIFFERENCE 1.0f
#define FIRE_HIGHTHRE           5
#define FIRE_LOWTHRE            3
typedef enum
{
    PROD_K101W = 0         
}ENUM_PRODUCT_ID;
typedef uint16_t  UINT16_PRODUCT_ID_ENUM;
typedef struct
{
    uint8_t Index;
    uint8_t unObjectweight[OBSER_WIN];
}STRUCT_ObjectWeight;
typedef struct
{
    uint8_t   ucTxNum;
    uint8_t   ucRxNum;
    uint8_t   ucTargetFireIdx;
    uint8_t   ucFireSignal;

    uint8_t   ucRptCnt;
    uint8_t   ucRptEn;
    uint16_t  usChirpNum;
    
    uint16_t  usVeloFftPt;
    uint16_t  usHighFireDist;
    
    uint16_t  usLowFireDist;
    uint16_t  usAdcSampPoint;
    
    uint16_t  usRfNum;
    uint16_t  usProductId;
    
    
    uint32_t ulFlashStatusyao;
}STRUCT_SYS_PARA;
extern STRUCT_SYS_PARA gstSysPara;

#endif /*_MAIN_H */

/************************ (C) COPYRIGHT  *****END OF FILE****/























