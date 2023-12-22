#include "App_Def.h"

/*
*/
#include "App_Func.h"
#include "list.h"
int AccTime=3;

uint32_t *calib_apBuf_Cpx = (uint32_t *)(CALIB_AP_CPX_ADDR);
uint32_t *calib_apBuf_Pf = (uint32_t *)(CALIB_AP_PF_ADDR);
uint16_t *winRange = (uint16_t *)(WIN_RANGE_ADDR);
uint16_t *winVel = (uint16_t *)(WIN_VEL1_ADDR);
uint32_t *sincBuf_Cpx = (uint32_t *)(SINCBUF_CPX_ADDR);
uint32_t *sincBuf_Pf = (uint32_t *)(SINCBUF_PF_ADDR);
uint32_t *steeringVec_Pf = (uint32_t *)(STEERING_VEC_PF_ADDR);
uint32_t *FFT1D_Buf = (uint32_t *)(FFT1D_CACHE_ADDR);
uint32_t *FFT1D_ABS_Buf = (uint32_t *) (FFT1D_CACHE_ADDR+BB_ABS_OFFSET);
uint16_t *abuf16=(uint16_t *)(0x30000000);

uint32_t *FFT2D_Buf = (uint32_t *)(FFT2D_CACHE_ADDR);
uint32_t *FFT2D_ABS_Buf = (uint32_t *)(FFT2D_CACHE_ADDR+BB_ABS_OFFSET);

uint32_t *FFT2D_COLLECT_CACHE_Buf = (uint32_t *)(FFT2D_COLLECT_CACHE_ADDR+BB_ABS_OFFSET);

uint32_t *FFT3D_Buf = (uint32_t *)(FFT3D_CACHE_ADDR);
uint32_t *FFT4D_Pre = (uint32_t *)(FFT4D_PRE_ADDR);
BB_DPKRST_T *bb_dpkRst_str = (BB_DPKRST_T *)(DPK_DST_FIRST_ADDR);
BB_CFARRST_T *bb_cfarRst_str = (BB_CFARRST_T *)(CFAR_CACHE_ADDR);
STRUCT_FRAMERST *frameRst =  (STRUCT_FRAMERST *)(FRAME_RST_ADDR);
STRUCT_COLLECT  *clllectresult = (STRUCT_COLLECT *)(COLLECT_RST_ADDR);
MmwDemo_detectedObj  *Groupresult =   (MmwDemo_detectedObj *)(GROUP_RST_ADDR);

uint8_t *ProtocolPackBuff = (uint8_t *)(PROTOCOL_PACK_ADDR);
//uint8_t ProtocolPackBuff[200];
uint32_t ProtocolSendLen = 0;
uint32_t cfarTargetNum[NUM_RX*NUM_TX] = {0};
uint64_t VcfarRangeTarget[NUM_RX*NUM_TX] = {0};
uint32_t cfarCollectTargetNum=0;
uint8_t cfarCollectAntNumber[NUM_CHIRP*NUM_RX*NUM_TX]={0};
uint8_t cfarCollectVelocity[NUM_CHIRP*NUM_RX*NUM_TX]={0};
uint8_t ACCAntChannelNumer[NUM_RX*NUM_TX]={0};
uint32_t AntObjNumer[4]={0};
uint64_t VcfarRanngResult[2][64][4]={0};//[2][4][64] every vel channel has independence range cell
uint16_t frameTime = 0;

STRUCT_COLLECTPOINT ChosenOne[4];

STRUCT_COLLECTPOINT ChosenOneCompensate[4];
/*const*/
const uint32_t calib_ap[4]={
0x10001000,0x10001000,0x10001000,0x10001000};    
const float calib_ap_Pre[4*2]={
1.000000000000000,0.000000000000000,1.000000000000000,0.000000000000000,1.000000000000000,0.000000000000000,1.000000000000000,0.000000000000000};
const uint32_t nco_fcw[4] ={
0x00000000,0x00000000,0x00000000,0x00000000};//,0x007FFEA8,0x0000022D,0x00000550,0x0000026F };//0x007FF852,0x007FF4DD,0x007FF9EC,0x007FF396,0x007FF948,0x007FFD50,0x00000127,0x007FF9FC
//};//budong;

const uint16_t winRange_Pre[512] = {
0x0009,0x0003,0x0003,0x0003,0x0004,0x0004,0x0005,0x0005,0x0006,0x0007,0x0007,0x0008,0x0009,0x000A,0x000B,0x000C,0x000D,0x000E,0x000F,0x0011,0x0012,0x0014,0x0015,0x0017,0x0019,0x001B,0x001D,0x001F,0x0021,0x0023,0x0026,0x0029,0x002B,0x002E,0x0031,0x0035,0x0038,0x003B,0x003F,0x0043,0x0047,0x004B,0x0050,0x0055,0x0059,0x005F,0x0064,0x0069,0x006F,0x0075,0x007B,0x0082,0x0089,0x0090,0x0097,0x009F,0x00A7,0x00AF,0x00B7,0x00C0,0x00C9,0x00D3,0x00DD,0x00E7,0x00F2,0x00FC,0x0108,0x0113,0x011F,0x012C,0x0139,0x0146,0x0154,0x0162,0x0170,0x017F,0x018F,0x019F,0x01AF,0x01C0,0x01D1,0x01E3,0x01F5,0x0208,0x021B,0x022F,0x0244,0x0258,0x026E,0x0284,0x029A,0x02B2,0x02C9,0x02E2,0x02FA,0x0314,0x032E,0x0348,0x0364,0x037F,0x039C,0x03B9,0x03D7,0x03F5,0x0414,0x0434,0x0454,0x0475,0x0496,0x04B8,0x04DB,0x04FF,0x0523,0x0548,0x056D,0x0593,0x05BA,0x05E1,0x060A,0x0632,0x065C,0x0686,0x06B1,0x06DC,0x0708,0x0735,0x0762,0x0790,0x07BF,0x07EE,0x081E,0x084F,0x0880,0x08B2,0x08E4,0x0918,0x094B,0x0980,0x09B4,0x09EA,0x0A20,0x0A57,0x0A8E,0x0AC5,0x0AFE,0x0B36,0x0B70,0x0BA9,0x0BE4,0x0C1E,0x0C5A,0x0C95,0x0CD1,0x0D0E,0x0D4B,0x0D88,0x0DC6,0x0E04,0x0E43,0x0E82,0x0EC1,0x0F00,0x0F40,0x0F80,0x0FC0,0x1001,0x1041,0x1082,0x10C3,0x1105,0x1146,0x1188,0x11C9,0x120B,0x124D,0x128E,0x12D0,0x1312,0x1354,0x1395,0x13D7,0x1419,0x145A,0x149B,0x14DD,0x151E,0x155E,0x159F,0x15DF,0x161F,0x165F,0x169E,0x16DD,0x171C,0x175A,0x1798,0x17D5,0x1812,0x184F,0x188B,0x18C6,0x1901,0x193B,0x1975,0x19AE,0x19E6,0x1A1E,0x1A55,0x1A8B,0x1AC1,0x1AF5,0x1B29,0x1B5D,0x1B8F,0x1BC0,0x1BF1,0x1C21,0x1C50,0x1C7D,0x1CAA,0x1CD6,0x1D01,0x1D2B,0x1D54,0x1D7C,0x1DA3,0x1DC9,0x1DED,0x1E11,0x1E33,0x1E55,0x1E75,0x1E94,0x1EB2,0x1ECE,0x1EE9,0x1F04,0x1F1D,0x1F34,0x1F4B,0x1F60,0x1F74,0x1F86,0x1F98,0x1FA8,0x1FB6,0x1FC4,0x1FD0,0x1FDA,0x1FE4,0x1FEC,0x1FF3,0x1FF8,0x1FFC,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFC,0x1FF8,0x1FF3,0x1FEC,0x1FE4,0x1FDA,0x1FD0,0x1FC4,0x1FB6,0x1FA8,0x1F98,0x1F86,0x1F74,0x1F60,0x1F4B,0x1F34,0x1F1D,0x1F04,0x1EE9,0x1ECE,0x1EB2,0x1E94,0x1E75,0x1E55,0x1E33,0x1E11,0x1DED,0x1DC9,0x1DA3,0x1D7C,0x1D54,0x1D2B,0x1D01,0x1CD6,0x1CAA,0x1C7D,0x1C50,0x1C21,0x1BF1,0x1BC0,0x1B8F,0x1B5D,0x1B29,0x1AF5,0x1AC1,0x1A8B,0x1A55,0x1A1E,0x19E6,0x19AE,0x1975,0x193B,0x1901,0x18C6,0x188B,0x184F,0x1812,0x17D5,0x1798,0x175A,0x171C,0x16DD,0x169E,0x165F,0x161F,0x15DF,0x159F,0x155E,0x151E,0x14DD,0x149B,0x145A,0x1419,0x13D7,0x1395,0x1354,0x1312,0x12D0,0x128E,0x124D,0x120B,0x11C9,0x1188,0x1146,0x1105,0x10C3,0x1082,0x1041,0x1001,0x0FC0,0x0F80,0x0F40,0x0F00,0x0EC1,0x0E82,0x0E43,0x0E04,0x0DC6,0x0D88,0x0D4B,0x0D0E,0x0CD1,0x0C95,0x0C5A,0x0C1E,0x0BE4,0x0BA9,0x0B70,0x0B36,0x0AFE,0x0AC5,0x0A8E,0x0A57,0x0A20,0x09EA,0x09B4,0x0980,0x094B,0x0918,0x08E4,0x08B2,0x0880,0x084F,0x081E,0x07EE,0x07BF,0x0790,0x0762,0x0735,0x0708,0x06DC,0x06B1,0x0686,0x065C,0x0632,0x060A,0x05E1,0x05BA,0x0593,0x056D,0x0548,0x0523,0x04FF,0x04DB,0x04B8,0x0496,0x0475,0x0454,0x0434,0x0414,0x03F5,0x03D7,0x03B9,0x039C,0x037F,0x0364,0x0348,0x032E,0x0314,0x02FA,0x02E2,0x02C9,0x02B2,0x029A,0x0284,0x026E,0x0258,0x0244,0x022F,0x021B,0x0208,0x01F5,0x01E3,0x01D1,0x01C0,0x01AF,0x019F,0x018F,0x017F,0x0170,0x0162,0x0154,0x0146,0x0139,0x012C,0x011F,0x0113,0x0108,0x00FC,0x00F2,0x00E7,0x00DD,0x00D3,0x00C9,0x00C0,0x00B7,0x00AF,0x00A7,0x009F,0x0097,0x0090,0x0089,0x0082,0x007B,0x0075,0x006F,0x0069,0x0064,0x005F,0x0059,0x0055,0x0050,0x004B,0x0047,0x0043,0x003F,0x003B,0x0038,0x0035,0x0031,0x002E,0x002B,0x0029,0x0026,0x0023,0x0021,0x001F,0x001D,0x001B,0x0019,0x0017,0x0015,0x0014,0x0012,0x0011,0x000F,0x000E,0x000D,0x000C,0x000B,0x000A,0x0009,0x0008,0x0007,0x0007,0x0006,0x0005,0x0005,0x0004,0x0004,0x0003,0x0003,0x0003,0x0009
};
const uint16_t winVelPre[64*4] = {
0x0001,0x0005,0x000B,0x0014,0x001F,0x002C,0x003C,0x004E,0x0063,0x007A,0x0093,0x00AF,0x00CD,0x00EE,0x0110,0x0135,0x015D,0x0186,0x01B2,0x01E0,0x0210,0x0242,0x0277,0x02AD,0x02E6,0x0320,0x035C,0x039B,0x03DB,0x041D,0x0461,0x04A7,0x04EE,0x0537,0x0582,0x05CF,0x061D,0x066C,0x06BD,0x070F,0x0763,0x07B8,0x080E,0x0866,0x08BF,0x0918,0x0973,0x09CF,0x0A2C,0x0A8A,0x0AE8,0x0B48,0x0BA8,0x0C08,0x0C6A,0x0CCB,0x0D2E,0x0D91,0x0DF4,0x0E57,0x0EBB,0x0F1F,0x0F83,0x0FE7,0x104B,0x10AF,0x1113,0x1177,0x11DB,0x123E,0x12A1,0x1303,0x1366,0x13C7,0x1428,0x1489,0x14E8,0x1547,0x15A5,0x1603,0x165F,0x16BA,0x1715,0x176E,0x17C6,0x181D,0x1873,0x18C7,0x191A,0x196C,0x19BC,0x1A0B,0x1A58,0x1AA3,0x1AED,0x1B36,0x1B7C,0x1BC1,0x1C04,0x1C45,0x1C85,0x1CC2,0x1CFD,0x1D37,0x1D6E,0x1DA4,0x1DD7,0x1E08,0x1E37,0x1E64,0x1E8F,0x1EB7,0x1EDD,0x1F01,0x1F23,0x1F42,0x1F5F,0x1F7A,0x1F92,0x1FA8,0x1FBB,0x1FCC,0x1FDB,0x1FE7,0x1FF1,0x1FF8,0x1FFD,0x1FFF,0x1FFF,0x1FFD,0x1FF8,0x1FF1,0x1FE7,0x1FDB,0x1FCC,0x1FBB,0x1FA8,0x1F92,0x1F7A,0x1F5F,0x1F42,0x1F23,0x1F01,0x1EDD,0x1EB7,0x1E8F,0x1E64,0x1E37,0x1E08,0x1DD7,0x1DA4,0x1D6E,0x1D37,0x1CFD,0x1CC2,0x1C85,0x1C45,0x1C04,0x1BC1,0x1B7C,0x1B36,0x1AED,0x1AA3,0x1A58,0x1A0B,0x19BC,0x196C,0x191A,0x18C7,0x1873,0x181D,0x17C6,0x176E,0x1715,0x16BA,0x165F,0x1603,0x15A5,0x1547,0x14E8,0x1489,0x1428,0x13C7,0x1366,0x1303,0x12A1,0x123E,0x11DB,0x1177,0x1113,0x10AF,0x104B,0x0FE7,0x0F83,0x0F1F,0x0EBB,0x0E57,0x0DF4,0x0D91,0x0D2E,0x0CCB,0x0C6A,0x0C08,0x0BA8,0x0B48,0x0AE8,0x0A8A,0x0A2C,0x09CF,0x0973,0x0918,0x08BF,0x0866,0x080E,0x07B8,0x0763,0x070F,0x06BD,0x066C,0x061D,0x05CF,0x0582,0x0537,0x04EE,0x04A7,0x0461,0x041D,0x03DB,0x039B,0x035C,0x0320,0x02E6,0x02AD,0x0277,0x0242,0x0210,0x01E0,0x01B2,0x0186,0x015D,0x0135,0x0110,0x00EE,0x00CD,0x00AF,0x0093,0x007A,0x0063,0x004E,0x003C,0x002C,0x001F,0x0014,0x000B,0x0005,0x0001
};

const uint16_t winRange_self[256] = {0,0,2,4,7,11,16,22,29,37,46,56,66,78,91,106,121,137,155,174,194,215,238,263,288,315,344,374,406,439,474,511,549,589,631,675,721,768,818,869,923,978,1035,1095,1156,1220,1286,1353,1423,1495,1569,1645,1723,1803,1885,1969,2055,2143,2233,2325,2419,2514,2611,2710,2811,2913,3016,3121,3228,3335,3444,3554,3666,3778,3891,4004,4119,4234,4349,4465,4581,4697,4813,4929,5045,5161,5276,5391,5505,5618,5730,5841,5951,6059,6166,6272,6376,6478,6578,6676,6772,6866,6957,7046,7132,7215,7296,7374,7448,7520,7588,7653,7715,7773,7828,7879,7926,7970,8010,8046,8078,8106,8131,8151,8167,8179,8187,8191,8191,8187,8179,8167,8151,8131,8106,8078,8046,8010,7970,7926,7879,7828,7773,7715,7653,7588,7520,7448,7374,7296,7215,7132,7046,6957,6866,6772,6676,6578,6478,6376,6272,6166,6059,5951,5841,5730,5618,5505,5391,5276,5161,5045,4929,4813,4697,4581,4465,4349,4234,4119,4004,3891,3778,3666,3554,3444,3335,3228,3121,3016,2913,2811,2710,2611,2514,2419,2325,2233,2143,2055,1969,1885,1803,1723,1645,1569,1495,1423,1353,1286,1220,1156,1095,1035,978,923,869,818,768,721,675,631,589,549,511,474,439,406,374,344,315,288,263,238,215,194,174,155,137,121,106,91,78,66,56,46,37,29,22,16,11,7,4,2,0,0
};
const uint16_t winRange_doppler_self[1024] = {0,0,0,0,0,1,1,1,2,2,3,3,4,5,5,6,7,8,9,10,11,12,14,15,16,18,19,20,22,24,25,27,29,31,33,35,37,39,41,43,45,48,50,53,55,58,61,63,66,69,72,75,78,81,84,88,91,94,98,101,105,109,112,116,120,124,128,132,136,141,145,149,154,159,163,168,173,178,183,188,193,198,203,209,214,220,225,231,237,243,249,255,261,267,273,280,286,293,300,306,313,320,327,334,342,349,356,364,372,379,387,395,403,411,419,428,436,445,453,462,471,480,489,498,507,517,526,536,546,555,565,575,585,596,606,617,627,638,649,660,671,682,693,704,716,728,739,751,763,775,787,800,812,825,838,850,863,876,890,903,916,930,944,957,971,985,1000,1014,1028,1043,1058,1072,1087,1102,1118,1133,1148,1164,1180,1195,1211,1228,1244,1260,1277,1293,1310,1327,1344,1361,1378,1396,1413,1431,1448,1466,1484,1503,1521,1539,1558,1576,1595,1614,1633,1652,1672,1691,1711,1730,1750,1770,1790,1810,1831,1851,1872,1892,1913,1934,1955,1976,1998,2019,2041,2062,2084,2106,2128,2150,2173,2195,2218,2240,2263,2286,2309,2332,2355,2378,2402,2425,2449,2473,2497,2521,2545,2569,2593,2618,2642,2667,2692,2716,2741,2766,2792,2817,2842,2868,2893,2919,2944,2970,2996,3022,3048,3074,3101,3127,3153,3180,3206,3233,3260,3287,3314,3341,3368,3395,3422,3449,3477,3504,3531,3559,3587,3614,3642,3670,3698,3726,3754,3782,3810,3838,3866,3894,3923,3951,3979,4008,4036,4065,4093,4122,4150,4179,4208,4236,4265,4294,4323,4352,4380,4409,4438,4467,4496,4525,4554,4583,4612,4641,4670,4699,4727,4756,4785,4814,4843,4872,4901,4930,4959,4988,5017,5046,5075,5103,5132,5161,5190,5218,5247,5276,5304,5333,5361,5390,5418,5447,5475,5504,5532,5560,5588,5616,5644,5672,5700,5728,5756,5784,5811,5839,5866,5894,5921,5948,5976,6003,6030,6057,6083,6110,6137,6163,6190,6216,6243,6269,6295,6321,6347,6372,6398,6424,6449,6474,6499,6524,6549,6574,6599,6623,6648,6672,6696,6720,6744,6768,6791,6815,6838,6861,6884,6907,6930,6952,6975,6997,7019,7041,7063,7084,7106,7127,7148,7169,7190,7210,7231,7251,7271,7291,7311,7330,7349,7369,7388,7406,7425,7443,7461,7479,7497,7515,7532,7549,7566,7583,7600,7616,7632,7648,7664,7680,7695,7710,7725,7740,7754,7768,7782,7796,7810,7823,7836,7849,7862,7874,7887,7899,7910,7922,7933,7944,7955,7966,7976,7986,7996,8006,8016,8025,8034,8042,8051,8059,8067,8075,8082,8090,8097,8103,8110,8116,8122,8128,8134,8139,8144,8149,8153,8158,8162,8165,8169,8172,8175,8178,8181,8183,8185,8187,8188,8189,8190,8191,8192,8192,8192,8192,8191,8190,8189,8188,8187,8185,8183,8181,8178,8175,8172,8169,8165,8162,8158,8153,8149,8144,8139,8134,8128,8122,8116,8110,8103,8097,8090,8082,8075,8067,8059,8051,8042,8034,8025,8016,8006,7996,7986,7976,7966,7955,7944,7933,7922,7910,7899,7887,7874,7862,7849,7836,7823,7810,7796,7782,7768,7754,7740,7725,7710,7695,7680,7664,7648,7632,7616,7600,7583,7566,7549,7532,7515,7497,7479,7461,7443,7425,7406,7388,7369,7349,7330,7311,7291,7271,7251,7231,7210,7190,7169,7148,7127,7106,7084,7063,7041,7019,6997,6975,6952,6930,6907,6884,6861,6838,6815,6791,6768,6744,6720,6696,6672,6648,6623,6599,6574,6549,6524,6499,6474,6449,6424,6398,6372,6347,6321,6295,6269,6243,6216,6190,6163,6137,6110,6083,6057,6030,6003,5976,5948,5921,5894,5866,5839,5811,5784,5756,5728,5700,5672,5644,5616,5588,5560,5532,5504,5475,5447,5418,5390,5361,5333,5304,5276,5247,5218,5190,5161,5132,5103,5075,5046,5017,4988,4959,4930,4901,4872,4843,4814,4785,4756,4727,4699,4670,4641,4612,4583,4554,4525,4496,4467,4438,4409,4380,4352,4323,4294,4265,4236,4208,4179,4150,4122,4093,4065,4036,4008,3979,3951,3923,3894,3866,3838,3810,3782,3754,3726,3698,3670,3642,3614,3587,3559,3531,3504,3477,3449,3422,3395,3368,3341,3314,3287,3260,3233,3206,3180,3153,3127,3101,3074,3048,3022,2996,2970,2944,2919,2893,2868,2842,2817,2792,2766,2741,2716,2692,2667,2642,2618,2593,2569,2545,2521,2497,2473,2449,2425,2402,2378,2355,2332,2309,2286,2263,2240,2218,2195,2173,2150,2128,2106,2084,2062,2041,2019,1998,1976,1955,1934,1913,1892,1872,1851,1831,1810,1790,1770,1750,1730,1711,1691,1672,1652,1633,1614,1595,1576,1558,1539,1521,1503,1484,1466,1448,1431,1413,1396,1378,1361,1344,1327,1310,1293,1277,1260,1244,1228,1211,1195,1180,1164,1148,1133,1118,1102,1087,1072,1058,1043,1028,1014,1000,985,
971,957,944,930,916,903,890,876,863,850,838,825,812,800,787,775,763,751,739,728,716,704,693,682,671,660,649,638,627,617,606,596,585,575,565,555,546,536,526,517,507,498,489,480,471,462,453,445,436,428,419,411,403,395,387,379,372,364,356,349,342,334,327,320,313,306,300,293,286,280,273,267,261,255,249,243,237,231,225,220,214,209,203,198,193,188,183,178,173,168,163,159,154,149,145,141,136,132,128,124,120,116,112,109,105,101,98,94,91,88,84,81,78,75,72,69,66,63,61,58,55,53,50,48,45,43,41,39,37,35,33,31,29,27,25,24,22,20,19,18,16,15,14,12,11,10,9,8,7,6,5,5,4,3,3,2,2,1,1,1,0,0,0,0,0
};

const uint16_t winV_self[64]={0,7,30,68,124,199,296,417,564,741,949,1189,1464,1772,2114,2487,2888,3315,3762,4223,4692,5161,5623,6069,6492,6883,7235,7540,7792,7986,8117,8184,8184,8117,7986,7792,7540,7235,6883,6492,6069,5623,5161,4692,4223,3762,3315,2888,2487,2114,1772,1464,1189,949,741,564,417,296,199,124,68,30,7,0
};

/*static*/
//static uint32_t head[2]={0x04030201,0x08070605};
//float sTemp[9] = {
//0,1,-1,100,-100,32764,-32764,0.0001220703125000000,-0.0001220703125000000
//};
float sTestFltReal = -32768.0f;
float sTestFltImag = -32764.0f;
int   sTestIntReal = -8192*32768;
int   sTestIntImag = -8192*32768;
//uint32_t *sTestR = (uint32_t *)&sTestFltReal;
//uint32_t *sTestI = (uint32_t *)&sTestFltImag;
uint32_t *sTestR = (uint32_t *)&sTestIntReal;
uint32_t *sTestI = (uint32_t *)&sTestIntImag;
uint32_t sTestRR = 0;
extern STRUCT_TARGET Target_result[4];
extern unsigned char ucFireSignal;
void mmWave_FireProc(void);

/*Init*/
void BB_Init(void)
{
    BB_CLOCK_EN;
    SYSC->RST &= ~0x200000;
    SYSC->RST |= 0x200000;

//    MPU->RASR = ARM_MPU_RASR(0, ARM_MPU_AP_FULL, 2, 0, 0, 0, 0, ARM_MPU_REGION_SIZE_4MB);
    /*IRQ*/
    BB_IRQ_Init();
    /*dataPrep*/
    BB_PrepConfig(&bb_prep);
    /*ZO*/
    BB_OPGATE_EN(BB_GATE_P1MUL);
    BB_ZO_Cfg0(BB_ZO_ZLEN_6,7,BB_ZO_MODE_2,0xFFFF);//zLen,mulFac,mode,thresSum
    /*P1MUL*/
    BB_P1MUL_Cfg1(0,BB_FFT_BASE);  // dstIntraInc,dstBaseAddr
//    BB_P1MUL_Cfg1(NUM_BYTE32,P1MUL_CACHE_ADDR);  // dstIntraInc,dstBaseAddr
    /*buf_cfg0*/
    
    //wr_flt2pf_lsf_en,wr_flt2pf_sfb,convert_cpx_mode,rdPowMode
    BUF_CFG0(BUF_CFG0_WR_FLT2PF_LSF_EN_RIGHT,0,BUF_CFG0_CONVERT_CPX_MODE_FLOAT,BUF_CFG0_RD_POWMODE_ABS);
//    BUF_CFG0(BUF_CFG0_WR_FLT2PF_LSF_EN_RIGHT,0,BUF_CFG0_CONVERT_CPX_MODE_FIX,BUF_CFG0_RD_POWMODE_ABS);
    /*FFT*/
    BB_FFT2D_ZP_Init();
    /*CFAR*/
    //BB_CFAR_2DCONFIG();
    
    
    /*calibAP*/
    //    DMA_Start_TransferM2M_32((uint32_t *)calib_ap_Pre,(uint32_t *)(calib_apBuf_Cpx),NUM_ANT*2,DMA_CHANNEL0);
    BB_WBUF_W32(CALIB_AP_CPX_ADDR,(uint32_t *)calib_ap_Pre,gstSysPara.ucRxNum*gstSysPara.ucTxNum*2);
//    BB_float2pf(&calib_apBuf_Cpx[0],&calib_apBuf_Pf[0],NUM_ANT);
    /*winRange*/
    
    BB_WBUF_W16(WIN_RANGE_ADDR,(uint16_t *)&winRange_self[0],gstSysPara.usAdcSampPoint);
    /*winVel*/
    for(uint32_t i=0; i<gstSysPara.usChirpNum; i++)
    {
        Write_M16(WIN_VEL1_ADDR+2*i, winVelPre[i*4]);
        Write_M16(WIN_VEL2_ADDR+2*i, winVelPre[i*4+1]);
        Write_M16(WIN_VEL3_ADDR+2*i, winVelPre[i*4+2]);
        Write_M16(WIN_VEL4_ADDR+2*i, winVelPre[i*4+3]);
    }
}
void Func_Init(void)
{
    /*BB*/
    RF_CHIPEN_Disable();//disable RFCHIP by set register to zero
    BB_Init();//Init BB module by default param
}

extern uint16_t HeadSendBuf[5];


uint16_t CfarSendBuf[5]={0};
uint16_t d1Cnt = 0;

#define DATASENDINTERVAL  0
#define POINTSENDINTERVAL 0

#define DEBUGOUT 0

void Target2D_Process(void)
{
    uint32_t ulCollectNum;
    //NVIC_DisableIRQ(USCI4_IRQn);
    mmWave_BbPrep();
    mmWave_FftRange();
    mmWave_FftVelocity();
    ulCollectNum = mmWave_CfarVelocity();
    if(FALSE == mmWave_CfarRange(ulCollectNum))
    {
        return;
    }
    mmWave_TargetProc();
}

uint32_t MmwDemo_cfarPeakGroupingCfarQualified(                                
																MmwDemo_detectedObj*  objOut,
                                MmwDemo_objRaw_t * objRaw,
                                uint32_t numDetectedObjects,
                                uint32_t numDopplerBins)
{
    int32_t i;
    uint32_t numObjOut = 0;
    uint32_t rangeIdx, dopplerIdx, peakVal,ante;
    uint32_t kernel[9], detectedObjFlag;
    int32_t k;
    int32_t l;
    uint32_t startInd, stepInd, endInd;
 
#define WRAP_DOPPLER_IDX(_x) ((_x) & (numDopplerBins-1))
#define WRAP_DWN_LIST_IDX(_x) ((_x) >= numDetectedObjects ? ((_x) - numDetectedObjects) : (_x))
#define WRAP_UP_LIST_IDX(_x) ((_x) < 0 ? ((_x) + numDetectedObjects) : (_x))
 
#ifdef GROUPINALLDIRECTION
    /* Grouping both in Range and Doppler direction */
    startInd = 0;
    stepInd = 1;
    endInd = 8;
#endif
#ifdef GROUPINRANGEDIRECTION
    /* Grouping only in Range direction */
    startInd = 1;
    stepInd = 3;
    endInd = 7;
#endif
#ifdef GROUPINDOPPLERDIRECTION
    /* Grouping only in Doppler direction */
    startInd = 3;
    stepInd = 1;
    endInd = 5;
#endif
 
    /* Start checking  */
    for(i = 0; i < numDetectedObjects ; i++)
    {
        detectedObjFlag = 0;
        rangeIdx = objRaw[i].rangeIdx;
        dopplerIdx = objRaw[i].dopplerIdx;
        peakVal = objRaw[i].peakVal;
				ante = objRaw[i].Antenna;
 
        if((rangeIdx <= MAXRANGEIDX) && (rangeIdx >= MINRANGEIDX))
        {
            detectedObjFlag = 1;
            memset((void *) kernel, 0, 9*sizeof(uint32_t));
 
            /* Fill the middle column of the kernel */
            kernel[4] = peakVal;
 
            if (i > 0)
            {
                if ((objRaw[i-1].rangeIdx == (rangeIdx-1)) &&
                    (objRaw[i-1].dopplerIdx == dopplerIdx))
                {
                    kernel[1] = objRaw[i-1].peakVal;
                }
            }
 
            if (i < (numDetectedObjects - 1))
            {
                if ((objRaw[i+1].rangeIdx == (rangeIdx+1)) &&
                    (objRaw[i+1].dopplerIdx == dopplerIdx))
                {
                    kernel[7] = objRaw[i+1].peakVal;
                }
            }
 
            /* Fill the left column of the kernel */
            k = WRAP_UP_LIST_IDX(i-1);
            for (l = 0; l < numDetectedObjects; l++)
            {
                if (objRaw[k].dopplerIdx == WRAP_DOPPLER_IDX(dopplerIdx - 2))
                {
                    break;
                }
                if ((objRaw[k].rangeIdx == (rangeIdx + 1)) &&
                    (objRaw[k].dopplerIdx == WRAP_DOPPLER_IDX(dopplerIdx - 1)))
                {
                    kernel[6] = objRaw[k].peakVal;
                }
                else if ((objRaw[k].rangeIdx == (rangeIdx)) &&
                    (objRaw[k].dopplerIdx == WRAP_DOPPLER_IDX(dopplerIdx - 1)))
                {
                    kernel[3] = objRaw[k].peakVal;
                }
                else if ((objRaw[k].rangeIdx == (rangeIdx - 1)) &&
                    (objRaw[k].dopplerIdx == WRAP_DOPPLER_IDX(dopplerIdx - 1)))
                {
                    kernel[0] = objRaw[k].peakVal;
                }
                k = WRAP_UP_LIST_IDX(k-1);
            }
 
            /* Fill the right column of the kernel */
            k = WRAP_DWN_LIST_IDX(i+1);
            for (l = 0; l < numDetectedObjects; l++)
            {
                if (objRaw[k].dopplerIdx == WRAP_DOPPLER_IDX(dopplerIdx + 2))
                {
                    break;
                }
                if ((objRaw[k].rangeIdx == (rangeIdx - 1)) &&
                    (objRaw[k].dopplerIdx == WRAP_DOPPLER_IDX(dopplerIdx + 1)))
                {
                    kernel[2] = objRaw[k].peakVal;
                }
                else if ((objRaw[k].rangeIdx == (rangeIdx)) &&
                    (objRaw[k].dopplerIdx == WRAP_DOPPLER_IDX(dopplerIdx + 1)))
                {
                    kernel[5] = objRaw[k].peakVal;
                }
                else if ((objRaw[k].rangeIdx == (rangeIdx + 1)) &&
                    (objRaw[k].dopplerIdx == WRAP_DOPPLER_IDX(dopplerIdx + 1)))
                {
                    kernel[8] = objRaw[k].peakVal;
                }
                k = WRAP_DWN_LIST_IDX(k+1);
            }
 
            /* Compare the detected object to its neighbors.
             * Detected object is at index 4*/
            for (k = startInd; k <= endInd; k += stepInd)
            {
                if(kernel[k] > kernel[4])
                {
                    detectedObjFlag = 0;
                }
            }
        }
        if(detectedObjFlag == 1)
        {
            objOut[numObjOut].rangeIdx = rangeIdx;
            objOut[numObjOut].dopplerIdx = dopplerIdx;
            objOut[numObjOut].peakVal = peakVal;
						objOut[numObjOut].Antenna=ante;
            numObjOut++;
        }
        if (numObjOut >= CFAR_MAX_NUM)
        {
            break;
        }
    }
 
    return(numObjOut);
}

void mmWave_Process(STRUCT_TARGET * target_result)
{
    Target2D_Process();
    memset(target_result,0xFF,sizeof(STRUCT_TARGET)*4);
    if(ChosenOne[0].Range<1 && ChosenOne[1].Range<1)
    {
        AccTime=3;
    }

    if(ChosenOne[0].Range>0 || ChosenOne[1].Range>0)
    {
        if(AccTime>0)
        {
                AccTime--;
        }
        if(AccTime<1)
        {
            target_result[ChosenOne[0].Antenna-1].Range=ChosenOne[0].Range;
            target_result[ChosenOne[0].Antenna-1].powABS=ChosenOne[0].powABS;
            target_result[ChosenOne[1].Antenna-1].Range=ChosenOne[1].Range;
            target_result[ChosenOne[1].Antenna-1].powABS=ChosenOne[1].powABS;
        }
    }
	mmWave_FireProc();
}

uint16_t mmWave_Init(void)
{
    uint16_t ReadValue=0;
    RF_Clock40M_En();
    Func_Init();
    ReadValue=RF_ReadRegSPI(CS1_PIN,ANA_CFG8_ADDR);
    return ReadValue;
}
/******************************************************
*Fucnc: Start RF chip,Get ADC result
*Data src:  BB_PrepConfig-->ADC0;ADC1;ADC6;ADC7
*Data dest: BB_PrepConfig-->ADC_SAMPLE_ADDR
*Data format: (2*512)*4*64
******************************************************/    
void mmWave_BbPrep()
{
/*************************************************************************************************
* conception:       
*                            BaseAddr: the start point. i.e ADC_SAMPLE_ADDR
*                            IntraInc: every data ponit increment. i.e ADC_BYTE
*                            InterInc: every Sampling period data ponit increment. i.e ADC_MAX_LEN
*                            jumpCnt:  Space between 2 same data area(chrip) .ie NUM_CHIRP
**************************************************************************************************/
#ifdef SIMULTANEOUSLY
    RF_SPI_Init();
    BB_ISR_CLEAR(BB_ISR_PREP_RAMP_END|BB_ISR_PREP_QUARTER_END|BB_ISR_PREP_FRAME_END);
    BB_OPGATE_EN(BB_GATE_PREP);
    waveGenStart();
    while( (bb_regb_str->ISR & (BB_ISR_PREP_FRAME_END))==0 );//wait untile frame raw ADC data get.
    bb_regb_str->ISR = BB_ISR_PREP_FRAME_END;
    RF_CHIPEN_Disable();
    BB_OPGATE_DIS(BB_GATE_PREP);
    BB_OPCLEAR(BB_CLEAR_PREP);
#else
    //TMRA_CLK_DISABLE;
    //NVIC_DisableIRQ(USCI4_IRQn);	
    BB_ISR_CLEAR(BB_ISR_PREP_RAMP_END|BB_ISR_PREP_QUARTER_END|BB_ISR_PREP_FRAME_END);
    Chip1_ADC_Init();
    Chip1_BB_PrepConfig(&bb_prep);
    rc6011_init_chip1();
    waveGenStart1();
    BB_OPGATE_EN(BB_GATE_PREP);
    while( (bb_regb_str->ISR & (BB_ISR_PREP_FRAME_END))==0 );//wait untile frame raw ADC data get.
    RF_CHIP1_Disable();
    BB_OPGATE_DIS(BB_GATE_PREP);
    //NVIC_EnableIRQ(USCI4_IRQn);
    //TMRA_CLK_ENABLE;
        
#ifdef MULTICHIPS
    
    BB_ISR_CLEAR(BB_ISR_PREP_RAMP_END|BB_ISR_PREP_QUARTER_END|BB_ISR_PREP_FRAME_END);
    Chip2_ADC_Init();//Chip2_ADC_Init();
    Chip2_BB_PrepConfig(&bb_prep);//BB2_Init();//Chip2_BB_PrepConfig(&bb_prep);    
    rc6011_init_chip1_2();
    waveGenStart2();
    BB_OPGATE_EN(BB_GATE_PREP);
    while( (bb_regb_str->ISR & (BB_ISR_PREP_FRAME_END))==0 );//wait untile frame raw ADC data get.
    RF_CHIP2_Disable();
    BB_OPGATE_DIS(BB_GATE_PREP);
#endif
#endif
    
        
#if DEBUGOUT     
    uint32_t ulTemp;
    ulTemp = gstSysPara.ucRxNum*gstSysPara.ucTxNum*gstSysPara.usAdcSampPoint*NUM_BYTE16*gstSysPara.usChirpNum;
    HeadSendBuf[0]=0x55AA;
    HeadSendBuf[1]=1;
    HeadSendBuf[2]=1;
    HeadSendBuf[3]=(uint16_t)((ulTemp>>16)&0xffff);
    HeadSendBuf[4]=(uint16_t)(ulTemp&0xffff);
    USART_Transmit_Bytes(&USART0, (uint8_t*)&HeadSendBuf[0],5*2);
    for(uint32_t chirpNow = 0 ;chirpNow<gstSysPara.usChirpNum;chirpNow++)
    {
        for(uint32_t antNow=0;antNow<gstSysPara.ucRxNum*gstSysPara.ucTxNum;antNow++)
        {
            USART_Transmit_Bytes(&USART0,(uint8_t *)ADC_SAMPLE_ADDR+chirpNow*bb_prep.cfg2.jumpInc+antNow*USE_RANGE*NUM_BYTE32,gstSysPara.usAdcSampPoint*ADC_BYTE);
            Cnt_Delay_ms(DATASENDINTERVAL);
        }
    }
#endif

}
void mmWave_FftRange()
{
    uint64_t sDMA_Cfg2 = (uint64_t)ADC_BYTE<<BB_BIT32_POS |(uint64_t)(ADC_SAMPLE_ADDR); 
    uint64_t sFFT_Cfg2 = (uint64_t)NUM_BYTE32 << BB_BIT32_POS|(uint64_t)FFT1D_CACHE_ADDR;
    uint32_t sChirp_Cnt = 0;
    uint32_t sAnt_Cnt = 0;
    uint32_t sQuarter_Cnt = 0;

    BB_OPCLEAR(BB_CLEAR_DMA);
    //twiceMode,minmaxMode,flt2pf_lsf_en,flt2pf_sfb,jumpCnt,interCnt,intraCnt,wmode,dsize
    BB_DMA_Cfg0(DMA_CFG0_TWICEMODE_NORMAL,DMA_CFG0_MINMAX_MODE_ABS,DMA_CFG0_FLT2PF_LSF_EN_RIGHT,0,0,0,gstSysPara.usAdcSampPoint-1,DMA_CFG0_WMODE_ORI,ADC_BYTE-1);
    BB_DMA_Cfg1(0,0);//srcJumpInc,srcInterInc
    BB_DMA_Cfg3(0,0);//dstJumpInc,dstInterInc
    BB_DMA_Cfg4(0,BB_ZO_BASE); //dstIntraInc,dstBaseAddr
    BB_FFT_Cfg0(0,0,FFT1D_USE_A,FFT1D_USE_B,gstSysPara.usAdcSampPoint-1,FFTPT_256,FFT_UNLOAD_EN,FFT_MODE_FORWARD);
    BB_FFT_ZP_Clear();
    //winBaseAddr,outLsfEn,outSfb,inLsfEn,inSfb,winSize,winEn
    BB_FFT_Cfg1(WIN_RANGE_ADDR,FFT_CFG1_OUTLSFEN_RIGHT,0,FFT_CFG1_INLSFEN_RIGHT,0,FFT_CFG1_WIN_SIZE_14BIT,FFT_CFG1_WIN_EN); 
    BB_FFT_CMD0(FFT_CMD0_CLEAR_IBUF);
    BB_OPGATE_EN(BB_GATE_P1MUL|BB_GATE_DMA|BB_GATE_FFT);
    
    /******************************************************
    *Fucnc: ZO(BB_ZO_Cfg0)+P1MUL(P1MUL_CFG0)+DMA+FFT1D(FFT with range dimension)
    *Data src:  ADC_SAMPLE_ADDR
    *Data dest: FFT1D_CACHE_ADDR
    *Cube format: (4*256)*4*64
    ******************************************************/
    for(uint32_t quarterNow=0; quarterNow<4; quarterNow++)
    {
        //while( (bb_regb_str->ISR & (BB_ISR_PREP_QUARTER_END|BB_ISR_PREP_FRAME_END))==0 );
        bb_regb_str->ISR = BB_ISR_PREP_QUARTER_END;//CLEAR Quarter end
        for(uint32_t antNow=0; antNow<gstSysPara.ucRxNum*gstSysPara.ucTxNum; antNow++)
        {
            bb_regb_str->P1MUL_CFG0 =    (uint64_t)P1MUL_CFG0_NCOMODE_COS_SIN<<BB_BIT56_POS|((uint64_t)nco_fcw[antNow] << BB_BIT32_POS) | (uint64_t)calib_ap[antNow];
            for(uint32_t chirpNow=0; chirpNow<NUM_CHIRP_QUARTER; chirpNow++)
            {//16
                bb_regb_str->DMA_CFG2   = sDMA_Cfg2;
                bb_regb_str->FFT_CFG2   = sFFT_Cfg2;
                BB_OPCLEAR(BB_CLEAR_P1MUL);
                BB_ZO_Cfg0(BB_ZO_ZLEN_6,7,BB_ZO_MODE_2,0xFFFF);//zLen,mulFac,mode,thresSum
                BB_FFT_CMD0(FFT_CMD0_CLEAR_IBUF);
                BB_ISR_CLEAR(BB_ISR_FFT_UNLOAD_END);
                BB_OPTRIG(BB_TRIG_DMA);
                sChirp_Cnt++;
                if(sChirp_Cnt==NUM_CHIRP_QUARTER)//16
                {
                    sChirp_Cnt = 0;
                    sAnt_Cnt++;
                    if(sAnt_Cnt==gstSysPara.ucRxNum*gstSysPara.ucTxNum)
                    {
                        sAnt_Cnt = 0;
                        sQuarter_Cnt++;
                    }
                }
                sDMA_Cfg2 = (uint64_t)ADC_BYTE<<BB_BIT32_POS |(uint64_t)(ADC_SAMPLE_ADDR+sAnt_Cnt*bb_prep.cfg2.interInc+sChirp_Cnt*bb_prep.cfg2.jumpInc+sQuarter_Cnt*NUM_CHIRP_QUARTER*bb_prep.cfg2.jumpInc);    //srcIntraInc,srcBaseAddr
                sFFT_Cfg2 = (uint64_t)NUM_BYTE32<<BB_BIT32_POS |(uint64_t)(FFT1D_CACHE_ADDR+sAnt_Cnt*USE_RANGE*NUM_BYTE32+sChirp_Cnt*bb_prep.cfg2.jumpInc+sQuarter_Cnt*NUM_CHIRP_QUARTER*bb_prep.cfg2.jumpInc); //dstIntraInc,dstBaseAddr
                BB_waitISR(BB_ISR_FFT_UNLOAD_END);
            }
        }
    }
    sAnt_Cnt = 0;
    sChirp_Cnt = 0;
    BB_OPGATE_DIS(BB_GATE_P1MUL|BB_GATE_DMA|BB_GATE_FFT);
    
#if 0 //FFT1D
    Temp=USE_RANGE*gstSysPara.ucRxNum*gstSysPara.ucTxNum*NUM_BYTE32*gstSysPara.usChirpNum;
    HeadSendBuf[0]=0x55AA;
    HeadSendBuf[1]=2;
    HeadSendBuf[2]=1;
    HeadSendBuf[3]=(uint16_t)((Temp>>16)&0xffff);
    HeadSendBuf[4]=(uint16_t)(Temp&0xffff);
    USART_Transmit_Bytes(&USART0, (uint8_t*)&HeadSendBuf[0],5*2);    
    for (uint32_t chirpNow = 0;chirpNow<gstSysPara.usChirpNum;chirpNow++)
    {
        for(uint32_t AntNow = 0;AntNow<gstSysPara.ucRxNum*gstSysPara.ucTxNum;AntNow++)
        {
            USART_Transmit_Bytes(&USART0,(uint8_t *)FFT1D_CACHE_ADDR+chirpNow*bb_prep.cfg2.jumpInc+AntNow*bb_prep.cfg2.interInc,USE_RANGE*NUM_BYTE32);
            Cnt_Delay_ms(DATASENDINTERVAL);
        }
    }
#endif

}
/******************************************************
*Func: 2DFFT
*Data src:  FFT1D_CACHE_ADDR
*Data dest: FFT2D_CACHE_ADDR
*dest Cube format:(256)*4*64
******************************************************/
void mmWave_FftVelocity()
{
    uint8_t  ucTxIdx,ucRxIdx,ucAntIdx,RangInd;
    uint32_t k=0;
    uint32_t q=0;
    BB_OPCLEAR(BB_CLEAR_DMA);
    BB_DMA_Cfg0(DMA_CFG0_TWICEMODE_NORMAL,DMA_CFG0_MINMAX_MODE_ABS,DMA_CFG0_FLT2PF_LSF_EN_RIGHT,0,0,0,gstSysPara.usChirpNum-1,DMA_CFG0_WMODE_ORI,DMA_CFG0_DSIZE_32BIT);
    BB_DMA_Cfg1(0,0);//srcJumpInc,srcInterInc
    BB_DMA_Cfg3(0,0);//dstJumpInc,dstInterInc
    BB_DMA_Cfg4(0,BB_FFT_BASE); //dstIntraInc,dstBaseAddr
    //rsfOutput,rsfInput,*useA,useB*,inPt,fftPt,unloadEn,mo
    BB_FFT_Cfg0(0,0,FFT2D_USE_A,FFT2D_USE_B,gstSysPara.usChirpNum-1,gstSysPara.usVeloFftPt,FFT_UNLOAD_EN,FFT_MODE_FORWARD);
    BB_FFT_ZP_Clear();
    BB_FFT_Cfg1(WIN_VEL1_ADDR,FFT_CFG1_OUTLSFEN_RIGHT,0,FFT_CFG1_INLSFEN_RIGHT,0,FFT_CFG1_WIN_SIZE_14BIT,FFT_CFG1_WIN_EN); //winBaseAddr,winSize,winEn
    BB_FFT_CMD0(FFT_CMD0_CLEAR_IBUF);
    BB_ISR_CLEAR(BB_CLEAR_FFT);
    BB_OPGATE_EN(BB_GATE_DMA|BB_GATE_FFT);
        
    for(ucTxIdx = 0;ucTxIdx<gstSysPara.ucTxNum;ucTxIdx++)
    {
        for(ucRxIdx=0;ucRxIdx<gstSysPara.ucRxNum;ucRxIdx++)
        {    
            ucAntIdx=ucTxIdx*gstSysPara.ucRxNum+ucRxIdx;
            for(RangInd=0;RangInd<USE_RANGE;RangInd++)
            {
                bb_regb_str->DMA_CFG2 = ((uint64_t)bb_prep.cfg2.jumpInc)<<BB_BIT32_POS|(uint64_t)(FFT1D_CACHE_ADDR+USE_RANGE*ucAntIdx*NUM_BYTE32+RangInd*NUM_BYTE32);
                bb_regb_str->FFT_CFG2 = ((uint64_t)bb_prep.cfg2.jumpInc)<<BB_BIT32_POS|(uint64_t)(FFT2D_CACHE_ADDR+USE_RANGE*ucAntIdx*NUM_BYTE32+RangInd*NUM_BYTE32);
                BB_FFT_CMD0(FFT_CMD0_CLEAR_IBUF);
                BB_ISR_CLEAR(BB_ISR_FFT_UNLOAD_END);
                BB_OPTRIG(BB_TRIG_DMA);
                BB_waitISR(BB_ISR_FFT_UNLOAD_END);
            }
        }
    }    
    BB_OPGATE_DIS(BB_GATE_DMA|BB_GATE_FFT);
    //循环前缀/后缀 4点
    //guard 2
    //8
    for(k=0;k<4;k++)
    {
        for(q=0;q<2*64;q++)
        {
            *(((uint32_t *)FFT2D_CACHE_ADDR)+k+q*USE_RANGE)=*(((uint32_t *)FFT2D_CACHE_ADDR)+(q+1)*USE_RANGE-1-k);
        }
    }
        
    //*(((uint32_t *)FFT2D_CACHE_ADDR))=0x18002000;
        
#if DEBUGOUT //FFT2D
    Temp=USE_RANGE*gstSysPara.ucRxNum*gstSysPara.ucTxNum*NUM_BYTE32*gstSysPara.usChirpNum;
    HeadSendBuf[0]=0x55AA;
    HeadSendBuf[1]=4;
    HeadSendBuf[2]=1;
    HeadSendBuf[3]=(uint16_t)((Temp>>16)&0xffff);
    HeadSendBuf[4]=(uint16_t)(Temp&0xffff);
    USART_Transmit_Bytes(&USART0, (uint8_t*)&HeadSendBuf[0],5*2);
    for (uint32_t chirpNow = 0;chirpNow<gstSysPara.usChirpNum;chirpNow++)
    {
        for(uint32_t AntNow = 0;AntNow<gstSysPara.ucRxNum*gstSysPara.ucTxNum;AntNow++)
        {
            USART_Transmit_Bytes(&USART0,(uint8_t *)FFT2D_CACHE_ADDR+chirpNow*bb_prep.cfg2.jumpInc+AntNow*bb_prep.cfg2.interInc,USE_RANGE*NUM_BYTE32);
            Cnt_Delay_ms(DATASENDINTERVAL);
        }
    }
#endif    
}
/******************************************************
*Fucnc: Velocity CFAR
*Data src:  FFT1D_CACHE_ADDR
*Data dest: CFAR_CACHE_ADDR
*dest format:BB_CFARRST_T*CFAR_STA0
******************************************************/
uint32_t mmWave_CfarVelocity()
{
    uint32_t ulCollectNum=0,ulChirpIdx;
    uint8_t  ucAntIdx;
    
    for(ucAntIdx=0;ucAntIdx<gstSysPara.ucRxNum*gstSysPara.ucTxNum;ucAntIdx++)
    {
        BB_VEL_CFAR_CONFIG(ucAntIdx);
        BB_OPCLEAR(BB_CLEAR_CFAR);
        BB_OPGATE_EN(BB_GATE_CFAR); //val,sw
        BB_ISR_CLEAR(BB_ISR_CFAR_END);
        BB_OPTRIG(BB_TRIG_CFAR); //val
        BB_waitISR(BB_ISR_CFAR_END);
        cfarTargetNum[ucAntIdx]= bb_regb_str->CFAR_STA0;
        VcfarRangeTarget[ucAntIdx] =0;
        for(int j=0;j<64;j++)
        {
            VcfarRanngResult[0][j][ucAntIdx]=0;
            VcfarRanngResult[1][j][ucAntIdx]=0;
        }
        BB_OPGATE_DIS(BB_GATE_CFAR); //val,sw
        //Cnt_Delay_ms(100);
    }
    frameRst->targetNum=0;
    for(ucAntIdx=0;ucAntIdx<gstSysPara.ucRxNum*gstSysPara.ucTxNum;ucAntIdx++)
    {
        bb_cfarRst_str = (BB_CFARRST_T *)(CFAR_CACHE_ADDR+ucAntIdx*sizeof(BB_CFARRST_T)*CFAR_MAX_NUM);
        for(uint32_t cfarNow = 0;cfarNow<cfarTargetNum[ucAntIdx];cfarNow++)
        {
            VcfarRangeTarget[ucAntIdx] |= (((uint64_t)0x01)<<bb_cfarRst_str[cfarNow].intraIdx);//Record the Doppler unit where the current antenna contains the target
            if(bb_cfarRst_str[cfarNow].interIdx >63)
            {
                VcfarRanngResult[1][bb_cfarRst_str[cfarNow].intraIdx][ucAntIdx] |= (((uint64_t)0x01)<<(bb_cfarRst_str[cfarNow].interIdx-64));//save present cfar result
            }
            else
            {
                VcfarRanngResult[0][bb_cfarRst_str[cfarNow].intraIdx][ucAntIdx] |= (((uint64_t)0x01)<<(bb_cfarRst_str[cfarNow].interIdx));//save present cfar result
            }
            frameRst->target[frameRst->targetNum].d1Idx = bb_cfarRst_str[cfarNow].intraIdx+1;//Vel
            frameRst->target[frameRst->targetNum].d2Idx = bb_cfarRst_str[cfarNow].interIdx+1;//Range
            frameRst->target[frameRst->targetNum].d3Idx = ucAntIdx+1;//antenna
                
            //frameRst->target[frameRst->targetNum].powABS = FFT2D_ABS_Buf[bb_cfarRst_str[cfarNow].interIdx+bb_cfarRst_str[cfarNow].intraIdx*bb_prep.cfg2.jumpInc/NUM_BYTE32];        
            frameRst->targetNum++;
        }
    }
    
#if DEBUGOUT
    uint8_t ucTarIdx;
    uint32_t ulTemp;
    ulTemp=frameRst->targetNum*3*2;
    HeadSendBuf[0]=0x55AA;
    HeadSendBuf[1]=3;
    HeadSendBuf[2]=1;
    HeadSendBuf[3]=(uint16_t)((ulTemp>>16)&0xffff);
    HeadSendBuf[4]=(uint16_t)(ulTemp&0xffff);
    USART_Transmit_Bytes(&USART0, (uint8_t*)&HeadSendBuf[0],5*2);
    for(ucTarIdx = 0 ;ucTarIdx <frameRst->targetNum;ucTarIdx++)
    {
        CfarSendBuf[0]=frameRst->target[ucTarIdx].d1Idx;
        CfarSendBuf[1]=frameRst->target[ucTarIdx].d2Idx;
        CfarSendBuf[2]=frameRst->target[ucTarIdx].d3Idx;
        USART_Transmit_Bytes(&USART0,(uint8_t *)CfarSendBuf,3*2);
        Cnt_Delay_ms(POINTSENDINTERVAL);
    }
#endif
        
    memset(cfarCollectAntNumber,0,sizeof(cfarCollectAntNumber));
    memset(cfarCollectVelocity,0,sizeof(cfarCollectVelocity));
        
    memset(ACCAntChannelNumer,0,sizeof(ACCAntChannelNumer));
    for(ucAntIdx=0;ucAntIdx<gstSysPara.ucRxNum*gstSysPara.ucTxNum;ucAntIdx++)
    {
        for(ulChirpIdx = 0;ulChirpIdx<gstSysPara.usChirpNum; ulChirpIdx++)
        {
            if(VcfarRangeTarget[ucAntIdx]&((uint64_t)0x1<<ulChirpIdx))
            {
                cfarCollectAntNumber[ulCollectNum]=ucAntIdx;//Save channel==>AntNumber
                cfarCollectVelocity[ulCollectNum]=ulChirpIdx;//Save channel==>Velocity
                    
                memcpy((void *)(FFT2D_COLLECT_CACHE_ADDR+ulCollectNum*bb_prep.cfg2.interInc),(void *)(FFT2D_CACHE_ADDR+ulChirpIdx*bb_prep.cfg2.jumpInc+ucAntIdx*bb_prep.cfg2.interInc),bb_prep.cfg2.interInc);
                ulCollectNum++;
                ACCAntChannelNumer[ucAntIdx]++;
            }
        }
    }
    return ulCollectNum;
}
/******************************************************
*Func: Range CFAR
*Data src:  FFT2D_ACC_CACHE_ADDR
*Data dest: CFAR_CACHE_ADDR
*dest format:BB_CFARRST_T*CFAR_STA0
******************************************************/    
bool mmWave_CfarRange(uint32_t ulCollectNum)
{
    uint32_t Temp128=0;
    uint32_t Temp2=0;
    BB_RANG_CFAR_CONFIG(ulCollectNum);
    BB_OPCLEAR(BB_CLEAR_CFAR);
    BB_OPGATE_EN(BB_GATE_CFAR); //val,sw
    BB_ISR_CLEAR(BB_ISR_CFAR_END);
    BB_OPTRIG(BB_TRIG_CFAR); //val
    BB_waitISR(BB_ISR_CFAR_END);
    cfarCollectTargetNum=bb_regb_str->CFAR_STA0;
    BB_OPGATE_DIS(BB_GATE_CFAR); //val,sw

    clllectresult->targetNum=0;
    bb_cfarRst_str = (BB_CFARRST_T *)(CFAR_CACHE_ADDR);
    memset(AntObjNumer,0,sizeof(AntObjNumer));
    for(uint32_t cfarNow = 0;cfarNow<cfarCollectTargetNum;cfarNow++)
    {
        if(bb_cfarRst_str[cfarNow].intraIdx>63)
        {
            Temp128=bb_cfarRst_str[cfarNow].intraIdx-64;
            Temp2=1;
        }
        else
        {
            Temp128=bb_cfarRst_str[cfarNow].intraIdx;
            Temp2=0;
        }
        if(VcfarRanngResult[Temp2][cfarCollectVelocity[bb_cfarRst_str[cfarNow].interIdx]][cfarCollectAntNumber[bb_cfarRst_str[cfarNow].interIdx]]&(((uint64_t)0x01)<<(Temp128)))//judge if early cfar result has this point
        {                
            clllectresult->target[clllectresult->targetNum].Range   = bb_cfarRst_str[cfarNow].intraIdx+1;//(bb_cfarRst_str[cfarNow].intraIdx+4)*r2_str.RangeRes;//bb_cfarRst_str[cfarNow].intraIdx;//
            clllectresult->target[clllectresult->targetNum].Antenna = cfarCollectAntNumber[bb_cfarRst_str[cfarNow].interIdx]+1;
            clllectresult->target[clllectresult->targetNum].Velocity = cfarCollectVelocity[bb_cfarRst_str[cfarNow].interIdx]+1;
            clllectresult->target[clllectresult->targetNum].powABS = FFT2D_COLLECT_CACHE_Buf[bb_cfarRst_str[cfarNow].intraIdx+bb_cfarRst_str[cfarNow].interIdx*USE_RANGE];        
            clllectresult->targetNum++;
            AntObjNumer[cfarCollectAntNumber[bb_cfarRst_str[cfarNow].interIdx]]++;
        }
    }
#if DEBUGOUT
    Temp=clllectresult->targetNum*5*2;
    HeadSendBuf[0]=0x55AA;
    HeadSendBuf[1]=6;
    HeadSendBuf[2]=1;
    HeadSendBuf[3]=(uint16_t)((Temp>>16)&0xffff);
    HeadSendBuf[4]=(uint16_t)(Temp&0xffff);
    USART_Transmit_Bytes(&USART0, (uint8_t*)&HeadSendBuf[0],5*2);
    for(uint32_t k = 0 ;k <clllectresult->targetNum;k++)
    {
        CfarSendBuf[0]=clllectresult->target[k].Velocity;
        CfarSendBuf[1]=clllectresult->target[k].Range;
        CfarSendBuf[2]=clllectresult->target[k].Antenna;//
        CfarSendBuf[3]=(uint16_t)(clllectresult->target[k].powABS>>16);//
        CfarSendBuf[4]=(uint16_t)(clllectresult->target[k].powABS&0xffff);
        USART_Transmit_Bytes(&USART0,(uint8_t *)CfarSendBuf,5*2);
        Cnt_Delay_ms(POINTSENDINTERVAL);
    }
#endif
    if(clllectresult->targetNum<1 || AntObjNumer[0]<1 || AntObjNumer[1]<1)
    {
        memset(ChosenOne,0,sizeof(ChosenOne));
        return FALSE;
    }
    return TRUE;
}
/******************************************************
    *Fucnc: Peak Group
    *Data src:   clllectresult
    *Data dest:  Groupresult
    *dest format: MmwDemo_objRaw_t
******************************************************/    
void mmWave_TargetProc()
{
    MmwDemo_objRaw_t * GroupPtr=NULL;
    unsigned int AntPowerchose[4]={0};
    unsigned int ChoseIndex[4]={0xffff,0xffff,0xffff,0xffff};
    uint32_t AnteGroupCount=0;
    uint32_t GroupNumber=0;
    AnteGroupCount=0;
    GroupNumber=0;
    //    unsigned temp17=0;
    MmwDemo_detectedObj*  OneobjOut;
    memset((void *)Groupresult,0,sizeof(STRUCT_COLLECT));
    for(uint32_t AnteNowIndex=0;AnteNowIndex<4;AnteNowIndex++)
    {
        if(AntObjNumer[AnteNowIndex]<2)
        {
            if(AntObjNumer[AnteNowIndex]>0)
            {
                OneobjOut=Groupresult+GroupNumber;
                OneobjOut[0].rangeIdx=clllectresult->target[AnteGroupCount].Range;
                OneobjOut[0].dopplerIdx = clllectresult->target[AnteGroupCount].Velocity;
                OneobjOut[0].peakVal =  clllectresult->target[AnteGroupCount].powABS;
                OneobjOut[0].Antenna=        clllectresult->target[AnteGroupCount].Antenna;
                AnteGroupCount=AnteGroupCount+AntObjNumer[AnteNowIndex];
                GroupNumber++;
            }
            continue;
        }
            
    //        temp17=(unsigned int)(Groupresult+GroupNumber);
        GroupPtr=(MmwDemo_objRaw_t *)(&(clllectresult->target[AnteGroupCount]));
        GroupNumber+=MmwDemo_cfarPeakGroupingCfarQualified((Groupresult+GroupNumber),GroupPtr,AntObjNumer[AnteNowIndex],64);
            
        AnteGroupCount=AnteGroupCount+AntObjNumer[AnteNowIndex];
    }
        
#if DEBUGOUT
    Temp=GroupNumber*5*2;
    HeadSendBuf[0]=0x55AA;
    HeadSendBuf[1]=8;
    HeadSendBuf[2]=1;
    HeadSendBuf[3]=(uint16_t)((Temp>>16)&0xffff);
    HeadSendBuf[4]=(uint16_t)(Temp&0xffff);
    USART_Transmit_Bytes(&USART0, (uint8_t*)&HeadSendBuf[0],5*2);
    for(uint32_t k = 0 ;k <GroupNumber;k++)
    {
        CfarSendBuf[0]=Groupresult[k].dopplerIdx;
        CfarSendBuf[1]=Groupresult[k].rangeIdx;
        CfarSendBuf[2]=Groupresult[k].Antenna;//
        CfarSendBuf[3]=(uint16_t)(Groupresult[k].peakVal>>16);//
        CfarSendBuf[4]=(uint16_t)(Groupresult[k].peakVal&0xffff);
        USART_Transmit_Bytes(&USART0,(uint8_t *)CfarSendBuf,5*2);
        Cnt_Delay_ms(POINTSENDINTERVAL);
    }
#endif
        
    memset(ChosenOne,0,sizeof(ChosenOne));
        
    //    PowerMaxTemp=0;
    //    PowerMaxTargetNumber=0;
        
    for(uint32_t NumTotal=0;NumTotal<GroupNumber;NumTotal++)
    {
        if(Groupresult[NumTotal].Antenna==1)
        {
            if(Groupresult[NumTotal].peakVal>AntPowerchose[0])
            {
                AntPowerchose[0]=Groupresult[NumTotal].peakVal;
                ChoseIndex[0]=NumTotal;
            }
        }
        else if(Groupresult[NumTotal].Antenna==2)
        {
            if(Groupresult[NumTotal].peakVal>AntPowerchose[1])
            {
                AntPowerchose[1]=Groupresult[NumTotal].peakVal;
                ChoseIndex[1]=NumTotal;
            }
        }
        else if(Groupresult[NumTotal].Antenna==3)
        {
            if(Groupresult[NumTotal].peakVal>AntPowerchose[2])
            {
                AntPowerchose[2]=Groupresult[NumTotal].peakVal;
                ChoseIndex[2]=NumTotal;
            }
        }
        else if(Groupresult[NumTotal].Antenna==4)
        {
            if(Groupresult[NumTotal].peakVal>AntPowerchose[3])
            {
                AntPowerchose[3]=Groupresult[NumTotal].peakVal;
                ChoseIndex[3]=NumTotal;
            }
        }
    }
    for(uint32_t loopnumber=0;loopnumber<4;loopnumber++)
    {
        if(ChoseIndex[loopnumber]>255)
        {
            continue;
        }
        ChosenOne[loopnumber].Antenna=Groupresult[ChoseIndex[loopnumber]].Antenna;
        ChosenOne[loopnumber].powABS=Groupresult[ChoseIndex[loopnumber]].peakVal;
        ChosenOne[loopnumber].Range=Groupresult[ChoseIndex[loopnumber]].rangeIdx;
        ChosenOne[loopnumber].Velocity=Groupresult[ChoseIndex[loopnumber]].dopplerIdx;
    }
    
        
#if DEBUGOUT
    Temp=4*5*2;
    HeadSendBuf[0]=0x55AA;
    HeadSendBuf[1]=9;
    HeadSendBuf[2]=1;
    HeadSendBuf[3]=(uint16_t)((Temp>>16)&0xffff);
    HeadSendBuf[4]=(uint16_t)(Temp&0xffff);
    USART_Transmit_Bytes(&USART0, (uint8_t*)&HeadSendBuf[0],5*2);
    for(uint32_t k = 0 ;k <4;k++)
    {
        CfarSendBuf[0]=ChosenOne[k].Velocity;
        CfarSendBuf[1]=ChosenOne[k].Range;
        CfarSendBuf[2]=ChosenOne[k].Antenna;//
        CfarSendBuf[3]=(uint16_t)(ChosenOne[k].powABS>>16);//
        CfarSendBuf[4]=(uint16_t)(ChosenOne[k].powABS&0xffff);//
        USART_Transmit_Bytes(&USART0,(uint8_t *)CfarSendBuf,5*2);
        Cnt_Delay_ms(POINTSENDINTERVAL);
    }
#endif
        
    return;
}
void mmWave_FireProc()
{
    uint16_t usFuncTemp=0xFF;

    if(Target_result[0].Range<Target_result[1].Range)
    {
        usFuncTemp = (Target_result[0].Range&0xFF);
    }
    else
    {
        usFuncTemp = (Target_result[1].Range&0xFF);
    }
    if(usFuncTemp == 5)
    {
        usFuncTemp = 0xFF;
    }
    if(usFuncTemp<=gstSysPara.ucTargetFireIdx)
    {
        ucFireSignal = 0xFF;
        usFuncTemp = usFuncTemp*0.24;

        FIRE_WritePin=1;        
        Prod_SendFireMsgK101W(ucFireSignal,usFuncTemp);
        LED3_WritePin=1;
        gstSysPara.ucRptCnt = 0;
    }
    else if(usFuncTemp != 0xFF)
    {
        Prod_SendFireMsgK101W(ucFireSignal,usFuncTemp);
        gstSysPara.ucRptCnt = 0;
    }
    else if(gstSysPara.ucRptEn)
    {
        gstSysPara.ucRptCnt++;
        if(gstSysPara.ucRptCnt >=3)
        {
            Prod_SendPeriodicMsgK101W();
            gstSysPara.ucRptCnt = 0;
        }
    }
}
