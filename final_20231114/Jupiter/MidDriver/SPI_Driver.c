#include "SPI_Driver.h"
#include "main.h"

usci_handler_t SPI0;

uint8_t SPI_write_data[10] = {0};
uint8_t SPI_read_data[10] = {0};

void SPI_Init(void){
    USCI3_CLK_ENABLE;
    //CS1 CS2 CS3
    // pull up
    PORT01_PIN_CTRL->UE |= 0xFFFF; //open input for analog 2.0-2.15
    GPIO_PullUp(GPIO0, SPI0_SCLK_PinMsk);
    GPIO_PullUp(GPIO0, SPI0_MOSI_PinMsk);
    GPIO_PullUp(GPIO0, SPI0_MISO_PinMsk);
    //CLK MOSI MISO
    PinMux_Set(SPI0_SCLK_Port, SPI0_SCLK_PinNum, IOMODE_USCI3_IO0);
    PinMux_Set(SPI0_MOSI_Port, SPI0_MOSI_PinNum, IOMODE_USCI3_IO1);
    PinMux_Set(SPI0_MISO_Port, SPI0_MISO_PinNum, IOMODE_USCI3_IO2);

    SPI0.instance = USCI3;
    SPI0.instance->CMD = USCI_CMD_CLK_ON;
    SPI0.s_init.spi_mode = USCI_SPI_WORK_MODE_MASTER;
    SPI0.s_init.spi_clkp = USCI_SPI_CLKP_IDLE_LOW;
    SPI0.s_init.spi_posedge = USCI_SPI_TRX_NEG_NEGEDGE;
    SPI0.s_init.spi_msb = USCI_SPI_LSB_MSB;
    SPI0.s_init.spi_data_len = USCI_SPI_DATA_LEN_8BIT;
    SPI0.usci_clock_div = 4;  //freq=pclk/2/(div+1)   pclk 系统时钟的1/2

    USCI_SPI_Init(&SPI0);
//    printf("spitest\n");
    #if 0
        RF_CHIPEN();
        RF_SPI_Init();
        while(1);
    #endif
}
void RF_SPI_CS_CTRL(uint8_t seqNum,uint8_t status){
    switch(seqNum){
        case CS1_PIN:
                SPI0_CS1_WritePin = status;
            break;
        case CS2_PIN:
                SPI0_CS2_WritePin = status;
            break;
//        case CS3_PIN:
//                SPI0_CS3_WritePin = status;
//            break;
//        case CS4_PIN:
//                SPI0_CS4_WritePin = status;
//            break;
        default:
            break;
    }
}

void RF_WriteCmdSPI(uint8_t seqNum,uint8_t u8Cmd)
{
  u8Cmd = (u8Cmd & 0x3F) | 0x80;     //sky6605 cmd
    RF_SPI_CS_CTRL(seqNum,0);          
  USCI_SPI_Transmit(&SPI0,&u8Cmd,4);
    RF_SPI_CS_CTRL(seqNum,1);
}

uint16_t RF_ReadRegSPI(uint8_t seqNum,uint8_t u8Addr)
{
    uint16_t u16Data;
    SPI_write_data[0] = (u8Addr & 0x3F) | 0x40;  //sky6605 SPI1 write
    SPI_write_data[1] = 0xFF;                    //send dumy data
    SPI_write_data[2] = 0xFF;                    //send dumy data
//        GPIO_PIN_ADDR(seqNum, 0) = 0;                                             //À­µÍ
        RF_SPI_CS_CTRL(seqNum,0); 
    SPI0.instance->ISR = USCI_IS_TE_Msk|USCI_IS_RXNE_Msk;
    SPI0.instance->TRX = SPI_write_data[0];
        while((SPI0.instance->ISR & USCI_IS_RXNE_Msk)==0);
        SPI_read_data[0] = SPI0.instance->TRX;
    SPI0.instance->TRX = SPI_write_data[1];
        while((SPI0.instance->ISR & USCI_IS_RXNE_Msk)==0);
    SPI_read_data[1] = SPI0.instance->TRX;
    SPI0.instance->TRX = SPI_write_data[2];
    while((SPI0.instance->ISR & USCI_IS_RXNE_Msk)==0);
    SPI_read_data[2] = SPI0.instance->TRX;



//        usci_Spi_TRx(&SPI0,SPI_write_data,3,SPI_read_data);
//        while((SPI0.instance->ISR & USCI_IS_TE_Msk)==0);
//        GPIO_PIN_ADDR(seqNum, 0) = 1;                                             //À­¸ß
//      SPI0_CS1_WritePin=1;
        RF_SPI_CS_CTRL(seqNum,1); 
    u16Data = (SPI_read_data[1] << 8) | SPI_read_data[2];
    //printf("addr=%02x,data=%04x\n",u8Addr,u16Data);
    return u16Data;
}

void RF_WriteRegSPI_Fast(uint8_t seqNum,uint8_t u8Addr, uint16_t u16Data)
{
    SPI_write_data[0] = (u8Addr & 0x3F);     //sky6605 SPI1 write
    SPI_write_data[1] = u16Data >> 8;
    SPI_write_data[2] = u16Data;
//    SPI0_CS1_WritePin=0;
        RF_SPI_CS_CTRL(seqNum,0); 
//    GPIO_PIN_ADDR(seqNum, 0) = 0;    //À­µÍ
    SPI0.instance->ISR = USCI_IS_TE_Msk;
    SPI0.instance->TRX = SPI_write_data[0];
    while((SPI0.instance->ISR & USCI_IS_TXE_Msk)==0);
    SPI0.instance->TRX = SPI_write_data[1];
    while((SPI0.instance->ISR & USCI_IS_TXE_Msk)==0);
    SPI0.instance->TRX = SPI_write_data[2];
    while((SPI0.instance->ISR & USCI_IS_TXE_Msk)==0);
    while((SPI0.instance->ISR & USCI_IS_TE_Msk)==0);
//    GPIO_PIN_ADDR(seqNum, 0) = 1;    //À­¸ß
//    SPI0_CS1_WritePin=1;
        RF_SPI_CS_CTRL(seqNum,1); 
    SPI0.instance->ISR = USCI_IS_TE_Msk;
    #if 0
        uint16_t rtn=RF_ReadRegSPI(seqNum,u8Addr);
        printf("cs=%d,addr=%02X,readData=%04X,oriData=%04X\n",seqNum,u8Addr,rtn,u16Data);
        if(rtn!=u16Data){
            printf("error\n");
        }
    #endif
}
#ifdef RF_RC7701N32

uint16_t RF_ReadWaveSPI(uint8_t seqNum,uint8_t Row_Addr, uint8_t Col_Addr){
    uint16_t u16Data;
    SPI_write_data[0] = 0x7E;
    SPI_write_data[1] = (Row_Addr&0x1F)<<3 | (Col_Addr&0x07);
    SPI_write_data[2] = 0xFF;
    SPI_write_data[3] = 0xFF;
    RF_SPI_CS_CTRL(seqNum,0);
        USCI_SPI_TRx(&SPI0,SPI_write_data,4,SPI_read_data);
    RF_SPI_CS_CTRL(seqNum,1);
    u16Data=((uint16_t)SPI_read_data[2] << 8) | SPI_read_data[3];
   // printf("seq=%d,row=%d,col=%d,data=%04X\n",seqNum,Row_Addr,Col_Addr,u16Data);
    return u16Data;
}
void RF_WriteWaveSPI(uint8_t seqNum,uint8_t Row_Addr, uint8_t Col_Addr, uint16_t u16Data)
{
    SPI_write_data[0] = 0x3E;
    SPI_write_data[1] = (Row_Addr&0x1F)<<3 | (Col_Addr&0x07);
    SPI_write_data[2] = u16Data >> 8;
    SPI_write_data[3] = u16Data;
    RF_SPI_CS_CTRL(seqNum,0);
        USCI_SPI_Transmit(&SPI0,SPI_write_data,4);
    RF_SPI_CS_CTRL(seqNum,1);
    #if 0
    RF_ReadWaveSPI(seqNum,Row_Addr,Col_Addr);
    #endif
}
#else
uint16_t RF_ReadWaveSPI(uint8_t seqNum,uint8_t Row_Addr, uint8_t Col_Addr)
{
    uint16_t u16Data;
    SPI_write_data[0] = 0x7E;
    SPI_write_data[1] = Row_Addr&0x7F;
    SPI_write_data[2] = (Col_Addr<<5)&0xE0;
    SPI_write_data[3] = 0xFF;
    SPI_write_data[4] = 0xFF;
    RF_SPI_CS_CTRL(seqNum,0);                  
    USCI_SPI_TRx(&SPI0,SPI_write_data,5,SPI_read_data);
    RF_SPI_CS_CTRL(seqNum,1);
        u16Data=(uint16_t)(SPI_read_data[3]<<8) | SPI_read_data[4];
    //for(uint8_t i=0;i<16;i++){
        //u16Data = (SPI_read_data[3+i*2] << 8) | SPI_read_data[3+i*2+1];
       // printf("Raddr=%02x,Caddr=%02x,data=%04x\n",Row_Addr,i,u16Data);
    //}
    return u16Data;
}

void RF_WriteWaveSPI(uint8_t seqNum,uint8_t Row_Addr, uint8_t Col_Addr, uint16_t u16Data)
{
    SPI_write_data[0] = 0x3E;
    SPI_write_data[1] = Row_Addr&0x7F;
    SPI_write_data[2] = (Col_Addr<<5)&0xE0;
    SPI_write_data[3] = (u16Data >> 8)&0xFF;
    SPI_write_data[4] = u16Data&0xFF;
    RF_SPI_CS_CTRL(seqNum,0);                                          //À­µÍ
    USCI_SPI_Transmit(&SPI0,SPI_write_data,5);                                                //À­¸ß
    RF_SPI_CS_CTRL(seqNum,1);

    #if 0
        RCC7711B_ReadWaveSPI(Row_Addr,Col_Addr);
    #endif
}
#endif
