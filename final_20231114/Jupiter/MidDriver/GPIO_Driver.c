#include "GPIO_Driver.h"
#include "main.h"
void GPIO_Init(void){

        //RF1 引脚初始化
    GPIO_SetMode(SPI0_CS1_GPIO_Port, SPI0_CS1_Pin, GPIO_PMODE_OUTPUT);
    GPIO_SetMode(RF1_CHIPEN_GPIO_Port, RF1_CHIPEN_Pin, GPIO_PMODE_OUTPUT);
        GPIO_SetMode(RF1_PWEN_GPIO_Port, RF1_PWEN_Pin, GPIO_PMODE_OUTPUT);
        
        //RF2 引脚初始化
        GPIO_SetMode(SPI0_CS2_GPIO_Port, SPI0_CS2_Pin, GPIO_PMODE_OUTPUT);
//    GPIO_SetMode(RF2_CHIPEN_GPIO_Port, RF2_CHIPEN_Pin, GPIO_PMODE_OUTPUT);
        GPIO_SetMode(RF2_PWEN_GPIO_Port, RF2_PWEN_Pin, GPIO_PMODE_OUTPUT);
    
        
    

        //测试  同步信号 初始化有无效果
        GPIO_SetMode(GPIO1, GPIO_PIN_0, GPIO_PMODE_INPUT);
        GPIO1_0 = 0;

//    CHPIEN_WritePin = 0;
    SPI0_CS1_WritePin = 1;
    SPI0_CS2_WritePin = 1;
//    SPI0_CS3_WritePin = 1;
//    SPI0_CS4_WritePin = 1;
//    TEST_WritePin = 0;
    
        GPIO_SetMode(LED2_GPIO_Port, LED2_Pin, GPIO_PMODE_OUTPUT);
        LED2_WritePin = 0;
    
        GPIO_SetMode(LED3_GPIO_Port, LED3_Pin, GPIO_PMODE_OUTPUT);
        LED3_WritePin = 0;
        
        //FIRE_SIG
        GPIO_SetMode(FIRE_GPIO_Port, FIRE_Pin, GPIO_PMODE_OUTPUT);        
        FIRE_WritePin =  0;
        
        //FIRE_LOW_SIG
        GPIO_SetMode(FIRE_LOW_GPIO_Port, FIRE_LOW_Pin, GPIO_PMODE_OUTPUT);        
        FIRE_LOW_WritePin =  0;
        
        GPIO_PullUp(GPIO1,GPIO_PIN_3);
}

