#include "DMA_Driver.h"
dma_channel_config_t dmaConfigStr;
void DMA_Config_Init(void){
    DMA_Clear_All_Irq();
    DMA_Init();
}

void DMA_Start_TransferM2M_32(uint32_t *srcAddr,uint32_t *dstAddr,uint32_t Len,enum dma_channel_number channel){

    DMA_CLK_ENABLE;

    dmaConfigStr.sar = (uint32_t)&srcAddr[0];
    dmaConfigStr.dar = (uint32_t)&dstAddr[0];
    dmaConfigStr.ctl_src_tr_width = DMA_TRANS_WIDTH_32;
    dmaConfigStr.ctl_dst_tr_width = DMA_TRANS_WIDTH_32;
    dmaConfigStr.ctl_dinc =  DMA_ADDR_INCREMENT;
    dmaConfigStr.ctl_sinc =  DMA_ADDR_INCREMENT;
    dmaConfigStr.ctl_dst_msize = DMA_MSIZE_1;
    dmaConfigStr.ctl_src_msize = DMA_MSIZE_1;
    dmaConfigStr.ctl_tt_fc = DMA_MEM2MEM_DMA;
    dmaConfigStr.ctl_block_ts = Len;

    DMA_Disable();
    DMA_Channel_Configure(channel, &dmaConfigStr, NULL);
    DMA_Enable();
    DMA_Channel_Enable(DMA_CHANNEL0);
    while((DMA_INT->RawBlock&(0x01<<channel))!= (0x01<<channel)){}
    DMA_INT->ClearBlock  = (0x01<<channel);
}
