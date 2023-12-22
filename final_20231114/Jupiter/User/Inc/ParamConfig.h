/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PARAMCONFIG_H
#define _PARAMCONFIG_H

#include <stdint.h>
#include <string.h>

#include "App_Def.h"
#include "USART_Driver.h"



//#include "stm32g4xx_ll_iwdg.h"
#define SAMPLE_LEN           (512)     //采样点数，DownFac前面
#define ADC_SAMPLING_RATE    (20.0f)    //采样率 MHz
#define BANDWIDTH            (1750000000) //bandwith hz
#define SWEEPING_TIME        (30)       //us
#define CHIRP_TIME           ((10+SWEEPING_TIME+5)*NUM_TX/1e6)//The time of one chirp
#define FREQ_INIT            (613e8)//Initial frequency

enum
{
    STANDBY_TIME = 0,
    PROG_FUNC,
    WORK_MODE,
    CFARTHVG,
    CFARTHVS,
    CFARTHV,
      RCFARTHVG,
    RCFARTHVS,
    RCFARTHV,
    DPKTH,
    NUMTLV,
        FIREHIGH,
        FUNCHIGH,
        SYSSTATE,
        OLDFUNCHIGH,
        MAGICNUMB,
};

#define FLASH_PARAM_HEAD_ADDR        (INFOFLASH_ADDR_BEGIN)  //0x80000
#define FLASH_STANDBY_TIME_ADDR        (FLASH_PARAM_HEAD_ADDR + STANDBY_TIME * 4)
#define FLASH_PROG_FUNC_ADDR        (FLASH_PARAM_HEAD_ADDR + PROG_FUNC * 4)
#define FLASH_WORK_MODE_ADDR        (FLASH_PARAM_HEAD_ADDR + WORK_MODE * 4)
#define FLASH_CFARTHVG_ADDR         (FLASH_PARAM_HEAD_ADDR + CFARTHVG * 4)
#define FLASH_CFARTHVS_ADDR         (FLASH_PARAM_HEAD_ADDR + CFARTHVS * 4)
#define FLASH_CFARTHV_ADDR             (FLASH_PARAM_HEAD_ADDR + CFARTHV * 4)
#define FLASH_RCFARTHVG_ADDR         (FLASH_PARAM_HEAD_ADDR + RCFARTHVG * 4)
#define FLASH_RCFARTHVS_ADDR         (FLASH_PARAM_HEAD_ADDR + RCFARTHVS * 4)
#define FLASH_RCFARTHV_ADDR         (FLASH_PARAM_HEAD_ADDR + RCFARTHV * 4)
#define FLASH_DPKTH_ADDR             (FLASH_PARAM_HEAD_ADDR + DPKTH * 4)
#define FLASH_NUMTLV_ADDR            (FLASH_PARAM_HEAD_ADDR + NUMTLV * 4)
#define FLASH_FIREHIGH_ADDR            (FLASH_PARAM_HEAD_ADDR + FIREHIGH * 4)
#define FLASH_FUNCHIGH_ADDR            (FLASH_PARAM_HEAD_ADDR + FUNCHIGH * 4)
#define FLASH_SYSSTATE_ADDR            (FLASH_PARAM_HEAD_ADDR + SYSSTATE * 4)
#define FLASH_OLDFUNCHIGH_ADDR            (FLASH_PARAM_HEAD_ADDR + OLDFUNCHIGH * 4)
#define FLASH_MAGICNUMB_ADDR            (FLASH_PARAM_HEAD_ADDR + MAGICNUMB * 4)

typedef struct
{
    uint32_t WorkMode;
    float RangeRes;
    float VelRes;

    uint32_t Standby_Time;
    uint32_t Proj_Func;
    uint32_t BautRate;
    /*app*/
    uint32_t cfarThVG;
    uint32_t cfarThVS;
  uint32_t cfarThV;
    uint32_t RcfarThVG;
    uint32_t RcfarThVS;
  uint32_t RcfarThV;
    uint32_t numTLV;
    uint32_t dpkThres;
    uint32_t FireHigh;
    uint32_t FuncHigh;
    uint32_t SysState;
    uint32_t OldFuncHigh;
    uint32_t MagicNumb;
} R2_STRUCT;




extern R2_STRUCT r2_str;





void ParamConfigInit(void);
void Read_Param_From_Flash(void);
void Default_Param_Config(void);

/***************************************************************
 * @funcname   Save_Param_Config
 * @brief      Save Param to flash param by param.this function can update by write param struct to flash just once. 
 * @author     liyang
 * @version    0.0.1
 * @date       20221222
 **************************************************************/
bool Save_Param_Config(void);







#endif /*_PARAMCONFIG_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
