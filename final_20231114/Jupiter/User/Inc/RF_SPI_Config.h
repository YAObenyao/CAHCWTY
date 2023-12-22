#ifndef __RF_SPI_CONFIG_H
#define __RF_SPI_CONFIG_H


#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "RC7701N32.h"
#include "GPIO_Driver.h"
#include "Delay_Driver.h"
#include "SPI_Driver.h"
#include "App_Def.h"



//#define MULTICHIPS





typedef struct{
    uint8_t num;
    uint8_t PA_Buf[8];
}STRUCT_PA;






extern  uint8_t waveStartFlag;
extern STRUCT_PA pa_str;

extern uint16_t RF_ReadRegSPI(uint8_t seqNum,uint8_t u8Addr);
void RF_WriteRegSPI(uint8_t seqNum,uint8_t u8Addr, uint16_t u16Data);
extern uint16_t RF_ReadWaveSPI(uint8_t seqNum,uint8_t Row_Addr, uint8_t Col_Addr);
void RF_WriteWaveSPI(uint8_t seqNum,uint8_t Row_Addr, uint8_t Col_Addr, uint16_t u16Data);
void RF_WriteCmdSPI(uint8_t seqNum,uint8_t u8Cmd);

void RF_SPI_Init(void);
void RF_CHIPEN_Disable(void);
void RF_CHIPEN(void);

void waveGenStart(void);
void waveGenStop(void);

void RF_PA_Set(uint8_t sPA_In,STRUCT_PA *sPA_ST);
void RF_PA_FASTSW(uint8_t paNum);
void RF_WriteRegSPI_Fast(uint8_t seqNum,uint8_t u8Addr, uint16_t u16Data);



void RF_Clock40M_En(void);

void RF_CLOSE(void);
void rc6011_init_chip1(void);
void rc6011_init_chip1_2(void);
void waveGenStart1(void);
void waveGenStart2(void);
void RF_CHIP1_Disable(void);
void RF_CHIP2_Disable(void);

void rc6011_init_doppler_chip1(void);
void rc6011_init_doppler_chip2(void);











#endif

