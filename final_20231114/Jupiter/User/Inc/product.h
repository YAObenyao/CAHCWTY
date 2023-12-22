/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _PRODUCT_H
#define _PRODUCT_H
/*User*/
#include "stdio.h"

#define  COMMAND_HEARD  0xAA60
#define  REPORT_HEARD   0xAA70
#define  DEFAULT_HEIGHT 0x55
void Sys_SelfCheckK101W(void);
uint8_t Prod_CalcFireHeight(uint8_t ucInfo);
void Prod_SelfCheckK101W(void);
void Prod_CommHandleK101W(uint8_t *ptrBuff);
void Prod_SendFireMsgK101W(uint8_t ucFireFlag,uint8_t ucFireHeight);
void Prod_SendPeriodicMsgK101W(void);
uint8_t Prod_CalcRptHeight(uint8_t ucInfo);

#endif /*_MAIN_H */

/************************ (C) COPYRIGHT  *****END OF FILE****/

