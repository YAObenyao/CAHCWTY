#include "OSPI_Driver.h"
#include "Delay_Driver.h"
#if 1
uint8_t ospi_sendover_flag = 0;
uint8_t ospi_receiveover_flag = 0;
void ospi0_dma_callback(void){
//    ospi_sendover_flag = 1;
     __NOP();
}
void ospi1_dma_callback(void){
//    ospi_receiveover_flag = 1;
    __NOP();
}
void test_dma_callback2(void)
{
    __NOP();
}
volatile uint32_t int_sta = 0;
volatile uint8_t ospi0_flag = 0;
volatile uint8_t ospi1_flag = 0;
//Master
void OSPI0_IRQHandler (void)
{
    int_sta = OSPI0->INT_STA;
    OSPI0->INT_STA = int_sta;
      if((int_sta&0x01) == 0x01)
        ospi_sendover_flag = 1;
}
//Slave
void OSPI1_IRQHandler (void)
{
    int_sta = OSPI1->INT_STA;
    OSPI1->INT_STA = int_sta;
      if((int_sta&0x02) == 0x02)
        ospi_receiveover_flag = 1;
}
/*
˵����
    ������⣺
    OSPI�и�FIFO�����FIFO��8*64bit�����и�ˮ��request����DMA��m2f��ʽ�������ݵ�FIFOʱ��Ҫ���и�request���ܴ��䣬��Ͳ��������м��������е�������
���Ǻ���Ҫ�ȸ�FIFOһ��request��DMA������m2f��ʽ���䣬����request��2�ַ�����һ����ͨ��CPUд��8������FIFO��һ����DMA�ٿ���һ��ͨ����д8������FIFO
����д���ݽ�FIFO��FIFO���Զ������ݷ���ȥ������Ҫ����д��8������ȥ��ĩ��ˮ�ߣ�4�������������ݷ�����֮ǰ�����ݴ�8->0�Ĺ����У�����ˮ��4,������request����requestֻ��1�ľ���ʧ
����Ҫ��ǰ��DMAʹ�ܣ��ȴ�request��������������������
        ��������ʹ��DMAchannel2��Ϊ����8�����ķ�ʽ����m2m��ʽǿ��д��FIFO����DMAchannel0��m2f��ʽ��Ϊ����д�뷽ʽ����ǰ���ú����еĲ���������ͬʱ����������DMA����ʱ
DMAchannel2�ȹ�������д��8����DMAchannel2ֹͣ������DMA1�ȵ���request��������������DMA�����Ĺ����У����OSPI���������ҪCPU����idle״̬���������ܲ����DMA��ռ���ߣ���֤����
DMA��src��ַ�ǿ����Զ����ӵģ����������OSPI����ٶȣ����Բ���ÿ������DMA��ַ��ֻ����DMAʹ�ܼ��ɡ�

*/
void OSPI_MasterInit(void){

    OSPI0_CLK_ENABLE;
    dma_channel_config_t dmaConfigStr={0};

    PinMux_Set(GPIO0, 0, IOMODE_OSPI0_SS);
    PinMux_Set(GPIO0, 1, IOMODE_OSPI0_CLK);
    PinMux_Set(GPIO0, 2, IOMODE_OSPI0_DIO0);
    PinMux_Set(GPIO0, 3, IOMODE_OSPI0_DIO1);
    PinMux_Set(GPIO0, 4, IOMODE_OSPI0_DIO2);
    PinMux_Set(GPIO0, 5, IOMODE_OSPI0_DIO3);
    PinMux_Set(GPIO0, 6, IOMODE_OSPI0_DIO4);
    PinMux_Set(GPIO0, 7, IOMODE_OSPI0_DIO5);
    PinMux_Set(GPIO0, 8, IOMODE_OSPI0_DIO6);
    PinMux_Set(GPIO0, 9, IOMODE_OSPI0_DIO7);
//    OSPI0->CON0 = 0x000041F1;
//    OSPI0->CON1 =   0xEB06FF02;
    OSPI0->CON0         =   (4<<12) |   // fifo_wl
                                                    (0<<9)  |   // slv_en
                                                    (0<<8)  |   // dma_en
                                                    (0xf<<4)|   // mode
                                                    (1<<3)  |   // samp_edge
                                                    (0<<2)  |   // fifo_irq_en
                                                    (0<<1)  |   // slv_end_irq_en
                                                    (0<<0);     // master_tx_irq_en
    OSPI0->CON1     =   (0<<24) |   // read_cmd
                                                    (0<<20) |   // tshsl
                                                    (0<<16) |   // dummy_len
                                                    (0<<8)  |   // dummy_byte
                                                    (0xF<<4)|   // clk_div
                                                    (0<<2)  |   // ins_on
                                                    (1<<1)  |   // master_normal_fifo
                                                    (0<<0);     // xip_mode_en
    OSPI0->CS = 0x01;
    OSPI0->INT_STA = 0x07;
//    OSPI0->CON0 &= (~OSPI_CON0_TRANS_MODE_Msk);
//    OSPI0->CON0 |= (OSPI_O_OCTU<<OSPI_CON0_TRANS_MODE_Pos);

//    NVIC_EnableIRQ(OSPI0_IRQn);

    #if 0
    uint64_t sData[4]={0x11223344,0x55667788,0xaabbccdd,0xeeff1122};
    while(1){
            OSPI_Trans64(sData,2);
            Cnt_Delay_ms(10);
    }
    #endif
    DMA_Disable();
    //config channel 0 tx
    dmaConfigStr.sar = 0;
    dmaConfigStr.dar = (uint32_t)&XOSPI0->DATA64[0];
    dmaConfigStr.ctl_src_tr_width = DMA_TRANS_WIDTH_64;
    dmaConfigStr.ctl_dst_tr_width = DMA_TRANS_WIDTH_64;
    dmaConfigStr.ctl_dinc =  DMA_ADDR_NOCHANGE;
    dmaConfigStr.ctl_sinc =  DMA_ADDR_INCREMENT;
    dmaConfigStr.ctl_dst_msize = DMA_MSIZE_4;
    dmaConfigStr.ctl_src_msize = DMA_MSIZE_1;
    dmaConfigStr.ctl_tt_fc = DMA_MEM2PRF_DMA;
    dmaConfigStr.ctl_block_ts = 0;
    dmaConfigStr.cfg_hs_sel_dst = DMA_HS_HARDWARE;
    dmaConfigStr.cfg_dst_per = DMA_OSPI0;
    dmaConfigStr.ctl_int_en = 1;
    DMA_Channel_Configure(DMA_CHANNEL0, &dmaConfigStr, ospi0_dma_callback);
    //config channel 2 first 8*64bit
    dmaConfigStr.sar = 0;
    dmaConfigStr.dar = (uint32_t)&XOSPI0->DATA64[0];
    dmaConfigStr.ctl_src_tr_width = DMA_TRANS_WIDTH_64;
    dmaConfigStr.ctl_dst_tr_width = DMA_TRANS_WIDTH_64;
    dmaConfigStr.ctl_dinc =  DMA_ADDR_NOCHANGE;
    dmaConfigStr.ctl_sinc =  DMA_ADDR_INCREMENT;
    dmaConfigStr.ctl_dst_msize = DMA_MSIZE_1;
    dmaConfigStr.ctl_src_msize = DMA_MSIZE_1;
    dmaConfigStr.ctl_tt_fc = DMA_MEM2MEM_DMA;
    dmaConfigStr.ctl_block_ts = 8;
    dmaConfigStr.cfg_hs_sel_dst = DMA_HS_HARDWARE;
    dmaConfigStr.cfg_dst_per = DMA_OSPI0;
    dmaConfigStr.ctl_int_en = 0;
    DMA_Channel_Configure(DMA_CHANNEL2, &dmaConfigStr, test_dma_callback2);
//    dma_Enable();
//    while((DMA_INT->RawBlock&(0x01<<DMA_CHANNEL0))!= (0x01<<DMA_CHANNEL0)){}
//    DMA_INT->ClearBlock  = (0x01<<DMA_CHANNEL0);
}

void OSPI_SlaveInit(void){

    OSPI1_CLK_ENABLE;
    dma_channel_config_t dmaConfigStr={0};
    PinMux_Set(GPIO0, 10, IOMODE_OSPI1_SS);
    PinMux_Set(GPIO0, 11, IOMODE_OSPI1_CLK);
    PinMux_Set(GPIO0, 12, IOMODE_OSPI1_DIO0);
    PinMux_Set(GPIO0, 13, IOMODE_OSPI1_DIO1);
    PinMux_Set(GPIO0, 14, IOMODE_OSPI1_DIO2);
    PinMux_Set(GPIO0, 15, IOMODE_OSPI1_DIO3);
    PinMux_Set(GPIO1, 0, IOMODE_OSPI1_DIO4);
    PinMux_Set(GPIO1, 1, IOMODE_OSPI1_DIO5);
    PinMux_Set(GPIO1, 2, IOMODE_OSPI1_DIO6);
    PinMux_Set(GPIO1, 3, IOMODE_OSPI1_DIO7);
    OSPI1->CON0 = 0x00004302;
    OSPI1->CON1 =   0xEB06FF00;
    OSPI1->SDH = 0;
    OSPI1->SDL = 0;
    OSPI1->CON0 &= (~OSPI_CON0_TRANS_MODE_Msk);
    OSPI1->CON0 |= (OSPI_I_OCTU<<OSPI_CON0_TRANS_MODE_Pos);
    OSPI1->CS = 0x01;
//    NVIC_EnableIRQ(OSPI1_IRQn);
    OSPI1->INT_STA = 0x07;
    //config channel 1
    dmaConfigStr.sar = (uint32_t)&XOSPI1->DATA64[0];
    dmaConfigStr.dar =     0;
    dmaConfigStr.ctl_src_tr_width = DMA_TRANS_WIDTH_64;
    dmaConfigStr.ctl_dst_tr_width = DMA_TRANS_WIDTH_64;
    dmaConfigStr.ctl_dinc =  DMA_ADDR_INCREMENT;
    dmaConfigStr.ctl_sinc =  DMA_ADDR_NOCHANGE;
    dmaConfigStr.ctl_dst_msize = DMA_MSIZE_4;
    dmaConfigStr.ctl_src_msize = DMA_MSIZE_4;
    dmaConfigStr.ctl_block_ts = 0;
    dmaConfigStr.ctl_tt_fc = DMA_PRF2MEM_PRF;
    dmaConfigStr.cfg_src_per = DMA_OSPI1;
    dmaConfigStr.ctl_int_en = 0;
    DMA_Disable();
    DMA_Channel_Configure(DMA_CHANNEL1, &dmaConfigStr, ospi1_dma_callback);
//    dma_Enable();
//    while((DMA_INT->RawBlock&(0x01<<DMA_CHANNEL1))!= (0x01<<DMA_CHANNEL1)){}
//    DMA_INT->ClearBlock  = (0x01<<DMA_CHANNEL1);
}
//len>8*64bit
void OSPI_TransDMA64(uint32_t *srcAddr,uint32_t len){
    DMA_CH0->SAR = (uint32_t)&srcAddr[0]+64;
    DMA_CH0->HCTL = (len-8)& DMA_HCTL_BLOCK_TS_Msk;
    DMA_CH2->SAR = (uint32_t)&srcAddr[0];
    OSPI0->CS = 0x00;
    DMA_Channel_Enable(DMA_CHANNEL0);
    DMA_Channel_Enable(DMA_CHANNEL2);
    while((OSPI0->INT_STA & 0x1)==0);
    OSPI0->INT_STA      = 0x1;
    OSPI0->CS = 0x01;
}
void OSPI_RecDMA64(uint32_t *dstAddr,uint32_t len){
    DMA_CH1->DAR = (uint32_t)&dstAddr[0];
    DMA_CH1->HCTL = len& DMA_HCTL_BLOCK_TS_Msk;
//    OSPI1->CS = 0x00;
    DMA_Channel_Enable(DMA_CHANNEL1);
//    OSPI1->CS = 0x01;
}
/*
len = bytes num
len%8=0
*/
void OSPI_Trans64(uint64_t *srcAddr,uint32_t len){
//    uint32_t cnt=0;
    len=len/8;
    OSPI0->INT_STA=7;
    OSPI0->CS = 0x00;

        XOSPI0->DATA8[0]=0x00;
        XOSPI0->DATA8[1]=0x00;
        while((OSPI0->INT_STA & OSPI_NEND_IRQ_Msk)==0);
        OSPI0->INT_STA      = OSPI_NEND_IRQ_Msk;        
        while(len){
            XOSPI0->DATA64[0] = srcAddr[0];
            XOSPI0->DATA64[1] = srcAddr[1];
            XOSPI0->DATA64[2] = srcAddr[2];
            XOSPI0->DATA64[3] = srcAddr[3];
            XOSPI0->DATA64[4] = srcAddr[4];
            XOSPI0->DATA64[5] = srcAddr[5];
            XOSPI0->DATA64[6] = srcAddr[6];
            XOSPI0->DATA64[7] = srcAddr[7];
            srcAddr+=8;
            len-=8;
            while((OSPI0->INT_STA & OSPI_NEND_IRQ_Msk)==0);
            OSPI0->INT_STA      = OSPI_NEND_IRQ_Msk;    
        }
//        if(len){
//            for(cnt=0;cnt<len;cnt++){
//                XOSPI0->DATA64[cnt] = srcAddr[cnt];
//            }
//            while((OSPI0->INT_STA & OSPI_NEND_IRQ_Msk)==0);
//            OSPI0->INT_STA      = OSPI_NEND_IRQ_Msk;        
//        }
    Cnt_Delay_us(1);
    OSPI0->CS = 0x01;

}
#endif










