#include "main.h"
#include "Led.h"

extern STRUCT_FRAMERST *frameRst;
STRUCT_ObjectWeight Objwei;
STRUCT_TARGET Target_result[4];
STRUCT_SYS_PARA     gstSysPara;

/******************Clock Block Diagram**********************
//SYSCLK default is from HRC(92Mhz) until PLL is enable
HRC(360MHz max)---------------clk_hrc              ----/DIV---iclkcan
or-----SYSCLK(320M)
XTAL_40M---->PLL(320MHz max)--clk_pll              ----/DIV(AHB)---FCLK 320MHZ
---HCLK_g_dma
---HCLK_g_qspi0/1
---HCLK_g_ospi0/1
---HCLK_g_bb
--------------------/DIV(APB)---PCLK---pclk_g_usci0/1/2/3/4 pclk=160MHZ
pclk_g_TimerA/B
pclk_g_fadc/sadc
pclk_g_ao
pclk_g_i2c
pclk_g_crc/trng
pclk_g_pinmux
pclk_g_wwdg
-----clk_wdt
LRC(32KHz)----------
-----clk_lrc
clk_rtc
XTAL_32K(32.768KHZ)    -----clk_32k                        
********************************************/
void Sys_CalcVeloFftPt(uint16_t usChirpNum)
{
    if(16 == usChirpNum)
    {
        gstSysPara.usVeloFftPt = FFTPT_16;
    }
    else if(usChirpNum == 32)
    {
        gstSysPara.usVeloFftPt = FFTPT_32;
    }
    else if(usChirpNum == 64)
    {
        gstSysPara.usVeloFftPt = FFTPT_64;
    }
    else if(usChirpNum == 128)
    {
        gstSysPara.usVeloFftPt = FFTPT_128;
    }
    else if(usChirpNum == 256)
    {
        gstSysPara.usVeloFftPt = FFTPT_256;
    }
    else if(usChirpNum == 512)
    {
        gstSysPara.usVeloFftPt = FFTPT_512;
    }
    else if(usChirpNum == 1024)
    {
        gstSysPara.usVeloFftPt = FFTPT_1024;
    }
    else if(usChirpNum == 2048)
    {
        gstSysPara.usVeloFftPt = FFTPT_2048;
    }
    else if(usChirpNum == 4096)
    {
        gstSysPara.usVeloFftPt = FFTPT_4096;
    }
}
void Sys_Init(void)
{
    LRC_DISABLE;   //LRCOFF WDT OFF RTC DEPEND ON  XTL32K
    LXTAL_DISABLE; //disable XTAL_32K module(XTAL_32K).RTC is off.
    CAN0_CLK_DISABLE;
    CAN1_CLK_DISABLE;
//    AON_SYSC->ANA_CFG0 = 0xA0181050; //default:0xA0181050
//    AON_SYSC->ANA_CFG1 = 0x2D004A; //default:0x2D004A
    SET_WKUP_MASK0(0xFFFFFFFF);//Mask positive edge wake up source(disable),0.0~0.15
    SET_WKUN_MASK0(0xFFFFFFFF);//Mask Negative edge wake up source,0.0~0.15
    CLR_WKUP_MASK0(GPIO1_10_WKU_MSK);  //GPIO1.10 Wakeup Enable(RAMPSYNC_1)
    CLR_WKUN_MASK0(GPIO1_10_WKU_MSK);  //GPIO1.10 Wakeup Enable(RAMPSYNC_1)
    SET_WKUP_MASK1(0xFFFFFFFF);///Mask positive edge wake up source(disable)
    SET_WKUN_MASK1(0xFFFFFFFF);//Mask Negative edge wake up source,2.0~3.15
    SET_WKUP_MASK2(0xFFFFFFFF);//Mask RTC and WDT wakeup source
    Set_Flash_WS(0xF);  //Set wait time before access flash,wait 0x0F HCLK
    Set_SysClk_Div(0, 1);   //FCLK and HCLK=SYSCLK. PCLK=HCLK/2
    HRC_System_Clock_Update();// Read  sysc_clk_div register to updata hclk and pclk and can_clk from hrc
        
    Set_CanClk_Div(0);//smx:没有这句话没法仿真// can_clk=sysclk/2,try delec
  
    GPIO_Init();
    SPI_Init();
    //RF_Clock40M_En();   
    SYS_CLK_HRC_Init();    //USE XTAL_40M TO GET real hrc_clk and lrc_clk 
    SYS_CLK_PLL_Init(); //
    //DMA_Config_Init(); //Do not use for now
    //OSPI_MasterInit();//跟LED部分引脚功能冲突，不可用
    ADC_Init();//FADC initialize
    gstSysPara.ucRxNum = NUM_RX;
    gstSysPara.ucTxNum = NUM_TX;
    gstSysPara.usChirpNum = NUM_CHIRP;
    gstSysPara.usAdcSampPoint = ADC_MAX_LEN;
    gstSysPara.usHighFireDist = FIRE_HIGHTHRE;
    gstSysPara.usLowFireDist = FIRE_LOWTHRE;
    gstSysPara.usRfNum = 1;
    gstSysPara.usProductId = PROD_K101W;
    gstSysPara.ucRptCnt = 0;
    gstSysPara.ucRptEn = FALSE;
    Sys_CalcVeloFftPt(gstSysPara.usChirpNum);
}
void Sys_SelfCheck(void)
{

    switch(gstSysPara.usProductId)
    {
        case PROD_K101W:
        {
            Prod_SelfCheckK101W();
        }
            break;

        default:
            break;
    }
        
}

void ObjectProcess()
{
    uint32_t FindFlag;
    uint32_t SerchIndx;
    uint32_t FireWeight;
    uint32_t i;
    
    FindFlag=0;
    SerchIndx=0;
    FireWeight=0;
    i=0;
    for(i=0;i < frameRst->targetNum;i++)
    {
        //if(frameRst->target[i].range > RANGE_DOWN_DIFFERENCE || frameRst->target[i].range < RANGE_UP_DIFFERENCE)
        {
            Objwei.unObjectweight[Objwei.Index]=1;
            Objwei.Index++;
            if(Objwei.Index>= OBSER_WIN)
            {
                Objwei.Index=0;
            }
            FindFlag=1;
            break;
        }
    }
    if(FindFlag != 1)
    {
        Objwei.unObjectweight[Objwei.Index]=0;
        Objwei.Index++;
        if(Objwei.Index>= OBSER_WIN)
        {
            Objwei.Index=0;
        }
    }
    
    SerchIndx=Objwei.Index;
    for(i=0;i < OBSER_WIN;i++)
    {
        FireWeight+=Objwei.unObjectweight[SerchIndx];
        SerchIndx++;
        if(SerchIndx>= OBSER_WIN)
        {
                SerchIndx=0;
        }
    }
    if(FireWeight>gstSysPara.usLowFireDist)
    {
        FIRE_WritePin=1;
    }
    else
    {
        FIRE_WritePin=0;
    }
}


uint8_t SendBuf[128]={0};
uint16_t HeadSendBuf[4]={0};
uint64_t TestSendBuf[16]={0};
extern uint16_t winRange_Pre[512];
extern const uint16_t winVelPre[64*4];
unsigned int value;
unsigned char TargetFreIndex=13;//YAO CHANGE
unsigned char ucFireSignal=0;
unsigned char InitState=0;
unsigned char Timerflag=1;
int main(void)
{
//	SCB->VTOR=0x2000;
    uint16_t InitRestult=0;
    //uint32_t OldFuncTemp=0;
    //StartSignalDetect();    
    Sys_Init();//MCU minimal system initialization      
    ParamConfigInit();//System parameter acquisition
    MPU_LocalConfig();
    USART_Init(); //systemClock/4        
    InitRestult=mmWave_Init();
    //TimerInit();//使用中断进行工作状态指示
    Sys_SelfCheck();
    //TMRA_CLK_DISABLE;
    //TMRA_CLK_ENABLE;
    if(!InitRestult)
    {
        InitState=0xff;
    }
        
    while(1)    
    {    
        XFProtocol_Analysis_Handle();
        mmWave_Process(&Target_result[0]);     
    }
        
}
