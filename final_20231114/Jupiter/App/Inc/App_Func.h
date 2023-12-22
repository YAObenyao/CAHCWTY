/* Define to prevent recursive inclusion -------------------------------------*/
#include "App_Def.h"

#ifndef _APP_FUNC_H
#define _APP_FUNC_H
/*Local*/
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/*Driver*/
#include "IRQ_Handler.h"
#include "OSPI_Driver.h"
/*User*/
#include "BB_Config.h"
#include "ParamConfig.h"
void Func_Init(void);
/***************************************************************
 * @funcname   Target2D_Process
 * @brief      2D target process function
 * @author     liyang
 * @version    0.0.1
 * @date       20221222
 **************************************************************/
void Target2D_Process(void);

void mmWave_Process(STRUCT_TARGET * target_result);
uint16_t mmWave_Init(void);
void mmWave_FftRange(void);
void mmWave_BbPrep(void);
void mmWave_FftVelocity(void);
uint32_t mmWave_CfarVelocity(void);
bool mmWave_CfarRange(uint32_t ulCollectNum);
void mmWave_TargetProc(void);

/////////////////////////////////////////////////////////////////
///////////////Gorgeous dividing line
/////////////////////////////////////////////////////////////////

//#define GROUPINDOPPLERDIRECTION 
//#define GROUPINRANGEDIRECTION 
#define GROUPINALLDIRECTION 

#define MAXRANGEIDX    128
#define MINRANGEIDX    0

typedef volatile struct MmwDemo_detectedObj_t
{
    uint16_t    Antenna;
    uint16_t    rangeIdx;    /*!< @brief Range index */
    uint16_t    dopplerIdx;   /*!< @brief Doppler index. Note that it is changed
                                 to signed integer in order to handle extended maximum velocity.
                                 Neagative values correspond to the object moving toward
                                 sensor, and positive values correspond to the
                                 object moving away from the sensor */
    uint32_t    peakVal;      /*!< @brief Peak value */
} MmwDemo_detectedObj; 

typedef volatile struct MmwDemo_objRaw_t
{
    uint16_t    Antenna;
    uint16_t    rangeIdx;    /*!< @brief Range index */
    uint16_t    dopplerIdx;   /*!< @brief Doppler index. Note that it is changed
                                 to signed integer in order to handle extended maximum velocity.
                                 Neagative values correspond to the object moving toward
                                 sensor, and positive values correspond to the
                                 object moving away from the sensor */
    uint32_t    peakVal;      /*!< @brief Peak value */
} MmwDemo_objRaw_t; 

uint32_t MmwDemo_cfarPeakGroupingCfarQualified(                                
                                                                MmwDemo_detectedObj*  objOut,
                                MmwDemo_objRaw_t * objRaw,
                                uint32_t numDetectedObjects,
                                uint32_t numDopplerBins);
#endif

