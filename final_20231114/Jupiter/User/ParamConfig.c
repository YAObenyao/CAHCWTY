#include "ParamConfig.h"
#include "Usart_Protocol.h"
R2_STRUCT r2_str;




/*
    计算分辨率:
    RangeRes=3*10e8/(2*(采样点数/采样率)*(扫频带宽/扫频时间));
    VelRes=λ/(2*N*Tc);λ=c/freq;
    其中freq是初始频率
  c是光速
    λ为波长
  N为chirp个数
  Tc为chirp周期
*/
static void ResolutionCount(void)
{
    float i,j;
    i=SAMPLE_LEN/ADC_SAMPLING_RATE;
    j=BANDWIDTH/SWEEPING_TIME;
    r2_str.RangeRes = 3.0e8f/(2*i*j);
    r2_str.VelRes = 3.0e8f/(FREQ_INIT*2*gstSysPara.usChirpNum*CHIRP_TIME);
}


static void Get_Effective_Param(void)
{
    r2_str.MagicNumb= Read_M32(FLASH_MAGICNUMB_ADDR);//read magic number
    if(r2_str.MagicNumb == 0x5A5A)//magic number is effective
    {
        Read_Param_From_Flash();//reader Param from flash
    }
    else
    {
        Default_Param_Config();//use defalut config and 
        Save_Param_Config();//save default param to flash param by param
    }
    
}

/*
Read Parameters From Flash
 */
void Read_Param_From_Flash(void)
{
    r2_str.Standby_Time = Read_M32(FLASH_STANDBY_TIME_ADDR);
    if(r2_str.Standby_Time > ProtocolTimeMax)
    {
        r2_str.Standby_Time = 1000;
    }
    r2_str.Proj_Func    = Read_M32(FLASH_PROG_FUNC_ADDR);
    if((r2_str.Proj_Func < ProtocolProgFuncMin) || (r2_str.Proj_Func > ProtocolProgFuncMax))
    {
        r2_str.Proj_Func = 1;
    }
    r2_str.WorkMode     = 0;
    
    r2_str.cfarThVG     = Read_M32(FLASH_CFARTHVG_ADDR);
    if(r2_str.cfarThVG > ProtocolCFARTHMax)
    {
        r2_str.cfarThVG = 2;
    }
    r2_str.cfarThVS     = Read_M32(FLASH_CFARTHVS_ADDR);
    if(r2_str.cfarThVS > ProtocolCFARTHMax)
    {
        r2_str.cfarThVS = 6;
    }
    r2_str.cfarThV      = Read_M32(FLASH_CFARTHV_ADDR);
    if((r2_str.cfarThV < ProtocolCFARTHMin) || (r2_str.cfarThV > ProtocolCFARTHMax))
    {
        r2_str.cfarThV = 6;
    }
    
    r2_str.RcfarThVG     = Read_M32(FLASH_RCFARTHVG_ADDR);
    if(r2_str.RcfarThVG > ProtocolCFARTHMax)
    {
        r2_str.RcfarThVG = 3;
    }
    r2_str.RcfarThVS     = Read_M32(FLASH_RCFARTHVS_ADDR);
    if(r2_str.RcfarThVS > ProtocolCFARTHMax)
    {
        r2_str.RcfarThVS = 8;
    }
    r2_str.RcfarThV      = Read_M32(FLASH_RCFARTHV_ADDR);
    if((r2_str.RcfarThV < ProtocolCFARTHMin) || (r2_str.RcfarThV > ProtocolCFARTHMax))
    {
        r2_str.RcfarThV = 6;
    }
    
    r2_str.dpkThres     = Read_M32(FLASH_DPKTH_ADDR);
    if(r2_str.dpkThres > ProtocolDPKTHMax)
    {
        r2_str.dpkThres = 6;
    }
    
    r2_str.numTLV       = Read_M32(FLASH_NUMTLV_ADDR);
    if((r2_str.numTLV < ProtocolNumTlvsMin) || (r2_str.numTLV > ProtocolNumTlvsMax))
    {
        r2_str.numTLV = 1;
    }
    
    r2_str.FireHigh       = Read_M32(FLASH_FIREHIGH_ADDR);
    if((r2_str.FireHigh < 0x55) || (r2_str.FireHigh > 0x68))
    {
        r2_str.FireHigh = 0x00;
    }
    
    r2_str.FuncHigh       = Read_M32(FLASH_FUNCHIGH_ADDR);
    if((r2_str.FuncHigh < 1) || (r2_str.FuncHigh > 0x68))
    {
        r2_str.FuncHigh = 0;
    }
    
    r2_str.SysState       = Read_M32(FLASH_SYSSTATE_ADDR);
    if(r2_str.SysState > 2)
    {
        r2_str.SysState = 0;
    }
    
    r2_str.FuncHigh       = Read_M32(FLASH_OLDFUNCHIGH_ADDR);
    if((r2_str.FuncHigh < 1) || (r2_str.FuncHigh > 0x68))
    {
        r2_str.FuncHigh = 0;
    }
}
/*
Save Parameters From Flash
 */
bool Save_Param_Config(void)
{
    uint8_t wtbuf[16] = {0};
    bool res=TRUE;
    
    //save Standby Time
    Word2Byte(r2_str.Standby_Time, wtbuf);
    if(EFC_ReWrite(FLASH_STANDBY_TIME_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }    
    //save Proj_Func
    Word2Byte(r2_str.Proj_Func, wtbuf);
    if(EFC_ReWrite(FLASH_WORK_MODE_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    //save WorkMode
    Word2Byte(r2_str.WorkMode, wtbuf);
    if(EFC_ReWrite(FLASH_WORK_MODE_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }    
    //save cfar guardSize
    Word2Byte(r2_str.cfarThVG, wtbuf);
    if(EFC_ReWrite(FLASH_CFARTHVG_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    //save cfar searchSize
    Word2Byte(r2_str.cfarThVS, wtbuf);
    if(EFC_ReWrite(FLASH_CFARTHVS_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    //save cfar mulFac
    Word2Byte(r2_str.cfarThV, wtbuf);
    if(EFC_ReWrite(FLASH_CFARTHV_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    
    //save Rcfar guardSize
    Word2Byte(r2_str.RcfarThVG, wtbuf);
    if(EFC_ReWrite(FLASH_RCFARTHVG_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    //save Rcfar searchSize
    Word2Byte(r2_str.RcfarThVS, wtbuf);
    if(EFC_ReWrite(FLASH_RCFARTHVS_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    //save Rcfar mulFac
    Word2Byte(r2_str.RcfarThV, wtbuf);
    if(EFC_ReWrite(FLASH_RCFARTHV_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    
    
    //save cfar dpkThres
    Word2Byte(r2_str.dpkThres, wtbuf);
    if(EFC_ReWrite(FLASH_DPKTH_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    //save protocol numTLV
    Word2Byte(r2_str.numTLV, wtbuf);
    if(EFC_ReWrite(FLASH_NUMTLV_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    //save protocol numTLV
    Word2Byte(r2_str.numTLV, wtbuf);
    if(EFC_ReWrite(FLASH_NUMTLV_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    
    Word2Byte(r2_str.FireHigh, wtbuf);
    if(EFC_ReWrite(FLASH_FIREHIGH_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    
    Word2Byte(r2_str.FuncHigh, wtbuf);
    if(EFC_ReWrite(FLASH_FUNCHIGH_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    
    Word2Byte(r2_str.SysState, wtbuf);
    if(EFC_ReWrite(FLASH_SYSSTATE_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    
    Word2Byte(r2_str.OldFuncHigh, wtbuf);
    if(EFC_ReWrite(FLASH_OLDFUNCHIGH_ADDR, 4, wtbuf) == OK)
    {
        res = res*TRUE;
    }
    else
    {
        res = res*FALSE;
    }
    
    if(res)
    {
        Word2Byte(r2_str.MagicNumb, wtbuf);
        if(EFC_ReWrite(FLASH_MAGICNUMB_ADDR, 4, wtbuf) == OK)
        {
            return true;
        }
        else
        {
            return false;
        }    
    }
    return false;
}


void Default_Param_Config(void)
{
    r2_str.Standby_Time = 1000;
        
    r2_str.Proj_Func = 1;
    r2_str.WorkMode = 0;
    /*App*/
    #ifdef COMPARE_DATA
    r2_str.cfarThVG = 2;//3   2
    r2_str.cfarThVS = 4;//6   4
    r2_str.cfarThV = 4;//6    4
    r2_str.dpkThres = 0;//6   0
    #else
    r2_str.cfarThVG = 2;//3   2
    r2_str.cfarThVS = 6;//6   4
    r2_str.cfarThV = 6;//6    4
    
    r2_str.RcfarThVG = 3;//3   2
    r2_str.RcfarThVS = 8;//6   4
    r2_str.RcfarThV = 6;//6    4
    
    r2_str.dpkThres = 6;//6   0
    #endif
    r2_str.numTLV = 1;
    r2_str.FireHigh = 0x00;
    r2_str.FuncHigh = 0;
    r2_str.OldFuncHigh = 0;
    r2_str.SysState = 0;
    r2_str.MagicNumb = 0x5A5A;

}

void Printf_Param(void){
        printf("SoftVerison=SKY32B750_7711B-AIP_OSPI_2T8R_SampleV1.0.3.0\r\n");
        printf("RangeRes=%fm\r\n",r2_str.RangeRes);
        printf("VelRes=%fm/s\r\n",r2_str.VelRes);
        /*Read Flash*/
        printf("TIME=%dms\r\n",r2_str.Standby_Time);
        printf("PROG=%02d\r\n",r2_str.Proj_Func);
        printf("cfarThVG=%02d\r\n", r2_str.cfarThVG);
        printf("cfarThVS=%02d\r\n", r2_str.cfarThVS);
        printf("cfarThV=%02d\r\n", r2_str.cfarThV);
        printf("dpkThres=%02d\r\n", r2_str.dpkThres);
        printf("numTLV=%02d\r\n", r2_str.numTLV);
}

/*
    参数初始化：
*/
void ParamConfigInit(void)
{
        /*下面代码上电后只执行一次*/
        memset(&r2_str,0,sizeof(R2_STRUCT)); //清掉 r2结构体，地址95K，size 1K
        ResolutionCount();    //计算距离分辨率
        Get_Effective_Param();
        //Read_Param_From_Flash();
        //Default_Param_Config();
        //Printf_Param();
}



























