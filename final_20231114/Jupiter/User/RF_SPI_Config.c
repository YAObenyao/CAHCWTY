#include "RF_SPI_Config.h"
#include "USART_Driver.h"
//#include "main.h"


STRUCT_PA pa_str;
uint8_t waveStartFlag = 0;
static void RF_WaveSet(uint8_t seqNum,uint8_t Row_Addr,uint16_t A1, uint16_t A2, uint16_t B1, uint16_t B2, uint16_t C1, uint16_t C2, uint16_t R, uint16_t V){
    //uint16_t WaveData[8]={0};
    //uint16_t i=0;
    RF_WriteWaveSPI(seqNum,Row_Addr,0x07,A1);
    //WaveData[i++]=RF_ReadWaveSPI(seqNum,Row_Addr, 0x07);
    RF_WriteWaveSPI(seqNum,Row_Addr,0x06,A2);
    //WaveData[i++]=RF_ReadWaveSPI(seqNum,Row_Addr, 0x06);
    RF_WriteWaveSPI(seqNum,Row_Addr,0x05,B1);
    //WaveData[i++]=RF_ReadWaveSPI(seqNum,Row_Addr, 0x05);
    RF_WriteWaveSPI(seqNum,Row_Addr,0x04,B2);
    //WaveData[i++]=RF_ReadWaveSPI(seqNum,Row_Addr, 0x04);
    RF_WriteWaveSPI(seqNum,Row_Addr,0x03,C1);
    //WaveData[i++]=RF_ReadWaveSPI(seqNum,Row_Addr, 0x03);
    RF_WriteWaveSPI(seqNum,Row_Addr,0x02,C2);
    //WaveData[i++]=RF_ReadWaveSPI(seqNum,Row_Addr, 0x02);
    RF_WriteWaveSPI(seqNum,Row_Addr,0x01,R);
    //WaveData[i++]=RF_ReadWaveSPI(seqNum,Row_Addr, 0x01);
    RF_WriteWaveSPI(seqNum,Row_Addr,0x00,V);
    //WaveData[i++]=RF_ReadWaveSPI(seqNum,Row_Addr, 0x00);
}
//void waveGenClear(void)/*{{{*/
//{
//    RF_WriteCmdSPI(1,0x03);  //waveform generator clear all
//
//}/*}}}*/
void waveGenStop(void)/*{{{*/
{
    RF_WriteCmdSPI(CS1_PIN,0x02);  //RF1 waveform generator start  smx:手册18页，直接命令模式，函数内部 |0x80,给出低四位就能控制开启/关闭
#ifdef MULTICHIPS
    RF_WriteCmdSPI(CS2_PIN,0x02);   //RF2 waveform generator start
#endif    
}/*}}}*/

void waveGenStop1(void)/*{{{*/
{
    RF_WriteCmdSPI(CS1_PIN,0x02); 
}/*}}}*/
void waveGenStop2(void)/*{{{*/
{
    RF_WriteCmdSPI(CS2_PIN,0x02); 
}/*}}}*/

void waveGenStart(void)/*{{{*/
{
    RF_WriteCmdSPI(CS1_PIN,0x00);  //RF1 waveform generator start
#ifdef MULTICHIPS
    RF_WriteCmdSPI(CS2_PIN,0x00);  //RF2 waveform generator start
#endif
}/*}}}*/

void waveGenStart1(void)/*{{{*/
{
    RF_WriteCmdSPI(CS1_PIN,0x00);  //RF1 waveform generator start
}/*}}}*/

void waveGenStart2(void)/*{{{*/
{
    RF_WriteCmdSPI(CS2_PIN,0x00);  
}/*}}}*/

void waveGenClear_RF1()
{
    RF_WriteCmdSPI(CS1_PIN,0x03); 
}

void waveGenClear_RF2()
{
    RF_WriteCmdSPI(CS2_PIN,0x03);
}

void RF_PA_FASTSW(uint8_t paNum){
    switch(paNum){
        case 0:   //不开PA
            RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG22_ADDR,0xFE00); //channel en
            break;
        case 1:        //PA1
            RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG22_ADDR,0xFE88); //channel en
            break;
        case 2:        //PA2
            RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG22_ADDR,0xFE44); //channel en
            break;
        case 3:   //PA3
            RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG22_ADDR,0xFE22); //channel en
            break;
        case 4:   //PA4
            RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG22_ADDR,0xFE11); //channel en
            break;
        default:
            break;
    }
}

void rc6011_init_chip1()
{
    NVIC_DisableIRQ(USCI4_IRQn);

    waveGenClear_RF1();
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG0_ADDR,0x3300);//ori:33c0;weilli:3300//PA Power[15:12]=12dBm //12dBm[0011]; 11dBm[0000]; 10dBm[0100]; 8dBm[1000]
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG1_ADDR,0xF000);//ori:C005;weilli:F000//Single out
    //RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG2_ADDR,0x0001);//clk output enable 20210618
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG8_ADDR,0x0813);//ori:91f;weilli:f1f;onechannle:911// bben[4:0]:[ch4 ch3 ch2 ch1]==>bit[3 2 1 0];bit[4]=channel power enable;bbvcm[10:8] 1.1V[000]; 1V[001]; 900mV011]; 800mV[111]
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG9_ADDR,0x87f);//0xf05 ori:81f;weilli:77F//High pass/low pass bandwidth, pls refer to datasheet
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG10_ADDR,0x7770);//ori:1110;weilli:7330//adjustable RX gain.<6:4> PGA1 gain 000:0dB  001:6dB 011:12dB 111:18dB 474
    
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG18_ADDR,0x0007);//ori:4007;weilli:0007//PLL Enable
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG19_ADDR,0x014C);//Used for Test
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG20_ADDR,0x3200);//PLL_cap_adj
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG21_ADDR,0x0070); //ori:93F4;weilli:0070//Used for Test
    
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG22_ADDR,0xCC40);//ori:FEF0;weilli:ff80;onechannle:0x8C40//Rx Tx LO enable.
    
    //ramp sync 脉冲宽度
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_SYNCW_CFG0_ADDR,0x0500);//sync_w2[15:8];sync_w1[7:0];0~255*8*Tref,8*Tref/step
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_SYNCW_CFG1_ADDR,0x0032);//sync_w4[15:8];sync_w3[7:0];0~255*8*Tref,8*Tref/step
        
    //PLLACC脉宽设置
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_ACCW_CFG0_ADDR,0x00FF);//Tacc_w2[15:8];Tacc_w1[7:0];0~255*8*Tref,8*Tref/step
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_ACCW_CFG1_ADDR,0x0000);//Tacc_w3[7:0];0~255*8*Tref,8*Tref/step
        
    //调频设置
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_WM_CFG0_ADDR,0x3000);//ramp_mode:1 from start
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_WM_CFG1_ADDR,0x0000);//0x0101 //0:1个  0100:2个 0200:3个 波形 [6:0] Bounce start address
    
    //初始频率设置  
     RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x73E0);  // 60g0 x73F9 0x73F5
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73B8); //60GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73c7); //60.3GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73D1);    //60.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73E0);    //60.8GHz    from weilli    
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73EA);    //61GHz from weilli
//        RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x73F9);    //61.3GHz    from weilli
//    RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7403);    //61.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7412);    //61.8GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x741C);    //62GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x742B);    //62.3GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7435);    //62.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7444);    //62.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x744E);    //63GHz from weilli
//    RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x745D);    //63.3GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7467);    //63.5GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7476);    //63.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7480); //64GHz from weilli

//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x73B8);  // 60g
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x73EA);  //61g
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x741C);  //62g
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x7480);  //64g
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x74E4);  //66g
//    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_PLL_CFG0_ADDR,0x76A6);    //75g
//    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_PLL_CFG0_ADDR,0x76BF);  //75.5g 
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x76D8);  //76g
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG1_ADDR,0x0000);
        
        
    //seqNum;Row_Addr,A_F,A_T,B_F,B_T,C_F,C_T,R,V
/******************Clock Block Diagram**********************
//Fref= 40MHz Tref=25ns
//S=(A_F[14:0]-1)*1.22kHZ*40    A_F[15]=0,Forward FM;A_F[15]=0,Forward FM;A_F[15]=1 Negative FM
//t=(A_T[15:2]-1)*8*25ns
//B=A_F[14:0]*A_T[15:2]*8*1.22kHZ
//
//R[9:0]:unit cycle Times;R[15:14]:A segment Tacc Configure;R[13:12]:B segment Tacc Configure;R[11:10]:C segment Tacc Configure
//V[2:1]:A segment vcoband[2:1];V[5:4]:B segment vcoband[2:1];V[8:7]:C segment vcoband[2:1]
//V[15:12]:enable Tx channel[4:1]
********************************************/
    ////A11us_B16.8us_1.75G_C2us
    RF_WaveSet(CS1_PIN,0,0x0000,0x014+1,0x3e0,0x218,0x00,0x00,0x4401,0x2092);  //no PA
    //RF_WaveSet(CS1_PIN,0,0x0000,0x00dc+1,0x0650,0x0084,0x8650,0x0084+1,0x5401,0x2092);  //PA2 //vco band 31Ghz~33Ghz
        
    //A10us_B_75ns_2.25G_C5us
    //RF_WaveSet(CS1_PIN,0,0x0000,0x00C8+1,0x0266,0x05DC+0,0xA400,0x0064+0,0x4401,0x1092);//from weilli
    NVIC_EnableIRQ(USCI4_IRQn);
}

void rc6011_init_chip2()
{
    NVIC_DisableIRQ(USCI4_IRQn);
    //uint16_t ReadValue[18]={0};
    //uint16_t i=0;
    waveGenClear_RF2();
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG0_ADDR,0x3300);//ori:33c0;weilli:3300//PA Power[15:12]=12dBm //12dBm[0011]; 11dBm[0000]; 10dBm[0100]; 8dBm[1000]
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG0_ADDR);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG1_ADDR,0xF000);//ori:C005;weilli:F000//Single out
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG1_ADDR);
    //RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG2_ADDR,0x0001);//clk output enable 20210618
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG8_ADDR,0x0873);// ori:91f;weilli:f1f;onechannle:911// bben[4:0]:[ch4 ch3 ch2 ch1]==>bit[3 2 1 0];bit[4]=channel power enable;bbvcm[10:8] 1.1V[000]; 1V[001]; 900mV011]; 800mV[111]
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG8_ADDR);
    
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG9_ADDR,0x81f);//775 ori:81f;weilli:77F//High pass/low pass bandwidth, pls refer to datasheet
    
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG9_ADDR);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG10_ADDR,0x7770);//ori:1110;weilli:7330//adjustable RX gain.<6:4> PGA1 gain 000:0dB  001:6dB 011:12dB 111:18dB 7700
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG10_ADDR);
    
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG18_ADDR,0x0007);//ori:4007;weilli:0007//PLL Enable
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG18_ADDR);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG19_ADDR,0x014C);//Used for Test
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG19_ADDR);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG20_ADDR,0x3200);//PLL_cap_adj
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG20_ADDR);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG21_ADDR,0x0070); //ori:93F4;weilli:0070//Used for Test
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG21_ADDR);
    
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG22_ADDR,0xCC40);//ori:FEF0;weilli:ff80;onechannle:0x8C40//Rx Tx LO enable.
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,ANA_CFG22_ADDR);
    
    //ramp sync 脉冲宽度
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_SYNCW_CFG0_ADDR,0xFF00);//sync_w2[15:8];sync_w1[7:0];0~255*8*Tref,8*Tref/step
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,RAMP_SYNCW_CFG0_ADDR);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_SYNCW_CFG1_ADDR,0x0001);//sync_w4[15:8];sync_w3[7:0];0~255*8*Tref,8*Tref/step
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,RAMP_SYNCW_CFG1_ADDR);
        
    //PLLACC脉宽设置
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_ACCW_CFG0_ADDR,0x00FF);//Tacc_w2[15:8];Tacc_w1[7:0];0~255*8*Tref,8*Tref/step
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,RAMP_ACCW_CFG0_ADDR);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_ACCW_CFG1_ADDR,0x0000);//Tacc_w3[7:0];0~255*8*Tref,8*Tref/step
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,RAMP_ACCW_CFG1_ADDR);
        
    //调频设置
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_WM_CFG0_ADDR,0x3000);//ramp_mode:1 from start
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,RAMP_WM_CFG0_ADDR);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_WM_CFG1_ADDR,0x0000);//0x0101 //0:1个  0100:2个 0200:3个 波形 [6:0] Bounce start address
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,RAMP_WM_CFG1_ADDR);
    
    //初始频率设置  
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_PLL_CFG0_ADDR,0x741C);  // 62.3 0x743A
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73B8); //60GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73c7); //60.3GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73D1);    //60.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73E0);    //60.8GHz    from weilli    
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73EA);    //61GHz from weilli
//        RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x73F9);    //61.3GHz    from weilli
//    RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7403);    //61.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7412);    //61.8GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x741C);    //62GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x742B);    //62.3GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7435);    //62.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7444);    //62.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x744E);    //63GHz from weilli
//    RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x745D);    //63.3GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7467);    //63.5GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7476);    //63.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7480); //64GHz from weilli


//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x741C);    //62GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x742B);    //62.3GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7435);    //62.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7444);    //62.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x744E);    //63GHz from weilli
//    RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x745D);    //63.3GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7467);    //63.5GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7476);    //63.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7480); //64GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x748F); //64.3GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7499); //64.5GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x74A8); //64.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x74B2); //65GHz from weilli

//      RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x73B8);  // 60g
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x73EA);  //61g
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x741C);  //62g
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x7480);  //64g  65G 74B2
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x74E4);  //66g
//        RF_WriteRegSPI_Fast(CS2_PIN,RAMP_PLL_CFG0_ADDR,0x76A6);    //75g
//        RF_WriteRegSPI_Fast(CS2_PIN,RAMP_PLL_CFG0_ADDR,0x76BF);  //75.5g 
//    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x76D8);  //76g
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_PLL_CFG1_ADDR,0x0000);
    //ReadValue[i++]=RF_ReadRegSPI(CS2_PIN,RAMP_PLL_CFG1_ADDR);
        
        
//seqNum;Row_Addr,A_F,A_T,B_F,B_T,C_F,C_T,R,V
/******************Clock Block Diagram**********************
//Fref= 40MHz Tref=25ns
//S=(A_F[14:0]-1)*1.22kHZ    A_F[15]=0,Forward FM;A_F[15]=0,Forward FM;A_F[15]=1 Negative FM
//t=(A_T[15:2]-1)*8*25ns
//B=A_F[14:0]*A_T[15:2]*8*1.22kHZ
//
//R[9:0]:unit cycle Times;R[15:14]:A segment Tacc Configure;R[13:12]:B segment Tacc Configure;R[11:10]:C segment Tacc Configure
//V[2:1]:A segment vcoband[2:1];V[5:4]:B segment vcoband[2:1];V[8:7]:C segment vcoband[2:1]
//V[15:12]:enable Tx channel[4:1]
********************************************/
////A11us_B30us_1.75G_C2us
    RF_WaveSet(CS2_PIN,0,0x0000,0x008c+1,0x3e0,0x150,0x0000,0x0078+1,0x4401,0x2124);  //no PA 8c   78
    //RF_WaveSet(CS2_PIN,0,0x0000,0x00dc+1,0x0650,0x0084,0x8650,0x0084+1,0x5401,0x2124);  //PA2 //vco band 30Ghz~32Ghz 92
    //160
    //A10us_B_75ns_2.25G_C5us
    //RF_WaveSet(CS1_PIN,0,0x0000,0x00C8+1,0x0266,0x05DC+0,0xA400,0x0064+0,0x4401,0x1092);//from weilli
    NVIC_EnableIRQ(USCI4_IRQn);
}


void rc6011_init_chip1_2()
{
    NVIC_DisableIRQ(USCI4_IRQn);
    waveGenClear_RF2();
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG0_ADDR,0x3300);//ori:33c0;weilli:3300//PA Power[15:12]=12dBm //12dBm[0011]; 11dBm[0000]; 10dBm[0100]; 8dBm[1000]
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG1_ADDR,0xF000);//ori:C005;weilli:F000//Single out
    //RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG2_ADDR,0x0001);//clk output enable 20210618
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG8_ADDR,0x0813);//ori:91f;weilli:f1f;onechannle:911// bben[4:0]:[ch4 ch3 ch2 ch1]==>bit[3 2 1 0];bit[4]=channel power enable;bbvcm[10:8] 1.1V[000]; 1V[001]; 900mV011]; 800mV[111]
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG9_ADDR,0x81F);//0xf05 ori:81f;weilli:77F//High pass/low pass bandwidth, pls refer to datasheet
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG10_ADDR,0x7770);//ori:1110;weilli:7330//adjustable RX gain.<6:4> PGA1 gain 000:0dB  001:6dB 011:12dB 111:18dB 474
    
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG18_ADDR,0x0007);//ori:4007;weilli:0007//PLL Enable
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG19_ADDR,0x014C);//Used for Test
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG20_ADDR,0x3200);//PLL_cap_adj
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG21_ADDR,0x0070); //ori:93F4;weilli:0070//Used for Test
    
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG22_ADDR,0xCC40);//ori:FEF0;weilli:ff80;onechannle:0x8C40//Rx Tx LO enable.
    
    //ramp sync 脉冲宽度
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_SYNCW_CFG0_ADDR,0xFF00);//sync_w2[15:8];sync_w1[7:0];0~255*8*Tref,8*Tref/step
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_SYNCW_CFG1_ADDR,0x0032);//sync_w4[15:8];sync_w3[7:0];0~255*8*Tref,8*Tref/step
        
    //PLLACC脉宽设置
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_ACCW_CFG0_ADDR,0x00FF);//Tacc_w2[15:8];Tacc_w1[7:0];0~255*8*Tref,8*Tref/step
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_ACCW_CFG1_ADDR,0x0000);//Tacc_w3[7:0];0~255*8*Tref,8*Tref/step
        
    //调频设置
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_WM_CFG0_ADDR,0x3000);//ramp_mode:1 from start
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_WM_CFG1_ADDR,0x0000);//0x0101 //0:1个  0100:2个 0200:3个 波形 [6:0] Bounce start address
    
    //初始频率设置  
     RF_WriteRegSPI_Fast(CS2_PIN,RAMP_PLL_CFG0_ADDR,0x7420);  // 60g0 0x742B 743A 0x73E0 0x741C 0x7435
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73B8); //60GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73c7); //60.3GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73D1);    //60.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73E0);    //60.8GHz    from weilli    
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x73EA);    //61GHz from weilli
//        RF_WriteRegSPI_Fast(CS1_PIN,RAMP_PLL_CFG0_ADDR,0x73F9);    //61.3GHz    from weilli
//    RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7403);    //61.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7412);    //61.8GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x741C);    //62GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x742B);    //62.3GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7435);    //62.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7444);    //62.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x744E);    //63GHz from weilli
//    RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x745D);    //63.3GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7467);    //63.5GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7476);    //63.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7480); //64GHz from weilli

//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x741C);    //62GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x742B);    //62.3GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7435);    //62.5GHz from weilli
//        RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7444);    //62.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x744E);    //63GHz from weilli
//    RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x745D);    //63.3GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7467);    //63.5GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7476);    //63.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7480); //64GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x748F); //64.3GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x7499); //64.5GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x74A8); //64.8GHz from weilli
//      RC7711B_WriteRegSPI(RAMP_PLL_CFG0_ADDR,0x74B2); //65GHz from weilli
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_PLL_CFG1_ADDR,0x0000);
        
        
        //seqNum;Row_Addr,A_F,A_T,B_F,B_T,C_F,C_T,R,V
        /******************Clock Block Diagram**********************
        //Fref= 40MHz Tref=25ns
        //S=(A_F[14:0]-1)*1.22kHZ*40    A_F[15]=0,Forward FM;A_F[15]=0,Forward FM;A_F[15]=1 Negative FM
        //t=(A_T[15:2]-1)*8*25ns
        //B=A_F[14:0]*A_T[15:2]*8*1.22kHZ
        //
        //R[9:0]:unit cycle Times;R[15:14]:A segment Tacc Configure;R[13:12]:B segment Tacc Configure;R[11:10]:C segment Tacc Configure
        //V[2:1]:A segment vcoband[2:1];V[5:4]:B segment vcoband[2:1];V[8:7]:C segment vcoband[2:1]
        //V[15:12]:enable Tx channel[4:1]
        ********************************************/
        
        ////A11us_B30us_1.75G_C2us
        RF_WaveSet(CS2_PIN,0,0x0000,0x00dc+1,0x3e0,0x150,0x00,0x028+1,0x4401,0x2092);  //no PA
        //RF_WaveSet(CS1_PIN,0,0x0000,0x00dc+1,0x0650,0x0084,0x8650,0x0084+1,0x5401,0x2092);  //PA2 //vco band 31Ghz~33Ghz
        //A10us_B_75ns_2.25G_C5us
        //RF_WaveSet(CS1_PIN,0,0x0000,0x00C8+1,0x0266,0x05DC+0,0xA400,0x0064+0,0x4401,0x1092);//from weilli
        NVIC_EnableIRQ(USCI4_IRQn);
}


//Enable two chip power supply.
void RF_CHIPEN(void)
{
    RF1_PWEN_WritePin = 1;
    HW_Delay_ms(1);    
    RF1_CHPIEN_WritePin = 1;
    
    HW_Delay_ms(1);
    
#ifdef MULTICHIPS
    RF2_PWEN_WritePin = 1;
    HW_Delay_ms(1); //上电后延时一段时间,再使能RF芯片，否则会通讯失败
    RF2_CHPIEN_WritePin = 1;
#endif
    //使用硬件延时后会关闭 systick 中断，BB处理需要该中断进行计时
    //SysTick_Init();
}

void RF_CLOSE(void)
{
    RF1_PWEN_WritePin = 0;
    HW_Delay_ms(1);    
    RF1_CHPIEN_WritePin = 0;
    
    HW_Delay_ms(1);
    RF2_PWEN_WritePin = 0;
    HW_Delay_ms(1); //上电后延时一段时间,再使能RF芯片，否则会通讯失败
//    RF2_CHPIEN_WritePin = 0;
    //使用硬件延时后会关闭 systick 中断，BB处理需要该中断进行计时
    //SysTick_Init();
}

void RF_CHIPEN_Disable(void)
{
    waveGenStop();
    
#ifdef RF_RC7701N32
    for (uint32_t i=0;i<4;i++)
    { //4chip
        RF_WriteRegSPI_Fast(i,ANA_CFG0_ADDR, 0);
        RF_WriteRegSPI_Fast(i,ANA_CFG1_ADDR, 0);
        RF_WriteRegSPI_Fast(i,ANA_CFG2_ADDR, 0);
        RF_WriteRegSPI_Fast(i,ANA_CFG6_ADDR, 0);
        RF_WriteRegSPI_Fast(i,ANA_CFG7_ADDR, 0); //LO band
        RF_WriteRegSPI_Fast(i,ANA_CFG8_ADDR, 0);
        RF_WriteRegSPI_Fast(i,ANA_CFG9_ADDR, 0); //change bias
        RF_WriteRegSPI_Fast(i,ANA_CFG10_ADDR,0); //1.0V LDO
        RF_WriteRegSPI_Fast(i,ANA_CFG11_ADDR,0); //LNA1
        RF_WriteRegSPI_Fast(i,ANA_CFG12_ADDR,0); //LNA2
        RF_WriteRegSPI_Fast(i,ANA_CFG13_ADDR,0); //LNA3
        RF_WriteRegSPI_Fast(i,ANA_CFG14_ADDR,0); //0xF010 C010 只开RX1   7010 开RX123    A010开RX2
        RF_WriteRegSPI_Fast(i,ANA_CFG15_ADDR,0); //baseband channel 1 fc<5:0> and gc B277   f333  0xF277
        RF_WriteRegSPI_Fast(i,ANA_CFG16_ADDR,0); //baseband channel 2 fc and gc
        RF_WriteRegSPI_Fast(i,ANA_CFG17_ADDR,0); //baseband channel 3 fc and gc    
        RF_WriteRegSPI_Fast(i,ANA_CFG18_ADDR,0);    //0x5A15 最小，1A15最大
        RF_WriteRegSPI_Fast(i,ANA_CFG19_ADDR,0);    //0x5A15 最小，1A15最大        
        RF_WriteRegSPI_Fast(i,ANA_CFG20_ADDR,0); //pa peakdet  //VTEMP/ ATST2 / ATST1  40M:001A    20M:011A  10M:021A
        RF_WriteRegSPI_Fast(i,ANA_CFG22_ADDR,0);    
    }
#else
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG1_ADDR,0);
    //RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG2_ADDR,0x0001);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG8_ADDR,0);//RX baseband channel
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG9_ADDR,0);//047F:1.5 50 200
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG10_ADDR,0); //0x1330
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG18_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG19_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG20_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG21_ADDR,0); //93F4
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG22_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_SYNCW_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_SYNCW_CFG1_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_ACCW_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_ACCW_CFG1_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_WM_CFG0_ADDR,0); 
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_WM_CFG1_ADDR,0); //0:1个  0100:2个 0200:3个 波形        
    /*chip2*/
#ifdef MULTICHIPS
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG1_ADDR,0);
    //RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG2_ADDR,0x0001);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG8_ADDR,0);//RX baseband channel
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG9_ADDR,0);//047F:1.5 50 200
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG10_ADDR,0); //0x1330
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG18_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG19_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG20_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG21_ADDR,0); //93F4
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG22_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_SYNCW_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_SYNCW_CFG1_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_ACCW_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_ACCW_CFG1_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_WM_CFG0_ADDR,0); 
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_WM_CFG1_ADDR,0); //0:1个  0100:2个 0200:3个 波形        
#endif
#endif
}
void RF_PA_SET(uint8_t sPA_In,STRUCT_PA *sPA_ST)
{
    memset(sPA_ST,0,sizeof(STRUCT_PA));
    for (uint32_t i=0;i<8;i++)
    {
        if(sPA_In&(1<<(7-i)))
        {
            sPA_ST->PA_Buf[sPA_ST->num]= i+1;
            sPA_ST->num++;
        }
    }
//    memset(&sPA_ST->PA_Buf[0],0,8);
//    sPA_ST->PA_Buf[2] = 1;
}
void RF_SPI_Init(void)
{
    rc6011_init_chip1();
#ifdef MULTICHIPS
    rc6011_init_chip2();
#endif
}

void RF1_SPI_Init(void)
{
    rc6011_init_chip1();
}

void RF2_SPI_Init(void)
{
    rc6011_init_chip2();
}

void RF_Clock40M_En(void){
    RF_CHIPEN();
#ifdef RF_RC7701N32
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG21_ADDR,0x0001);
#else
        
 #endif
}


void RF_CHIP1_Disable(void)
{
    waveGenStop1();
    
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG1_ADDR,0);
    //RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG2_ADDR,0x0001);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG8_ADDR,0);//RX baseband channel
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG9_ADDR,0);//047F:1.5 50 200
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG10_ADDR,0); //0x1330
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG18_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG19_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG20_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG21_ADDR,0); //93F4
    RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG22_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_SYNCW_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_SYNCW_CFG1_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_ACCW_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_ACCW_CFG1_ADDR,0);
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_WM_CFG0_ADDR,0); 
    RF_WriteRegSPI_Fast(CS1_PIN,RAMP_WM_CFG1_ADDR,0); //0:1个  0100:2个 0200:3个 波形        

}

void RF_CHIP2_Disable(void)
{
    waveGenStop2();

    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG1_ADDR,0);
    //RF_WriteRegSPI_Fast(CS1_PIN,ANA_CFG2_ADDR,0x0001);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG8_ADDR,0);//RX baseband channel
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG9_ADDR,0);//047F:1.5 50 200
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG10_ADDR,0); //0x1330
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG18_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG19_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG20_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG21_ADDR,0); //93F4
    RF_WriteRegSPI_Fast(CS2_PIN,ANA_CFG22_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_SYNCW_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_SYNCW_CFG1_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_ACCW_CFG0_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_ACCW_CFG1_ADDR,0);
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_WM_CFG0_ADDR,0); 
    RF_WriteRegSPI_Fast(CS2_PIN,RAMP_WM_CFG1_ADDR,0); //0:1个  0100:2个 0200:3个 波形        
}
