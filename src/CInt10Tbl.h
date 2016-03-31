/* 
 * Copyright (C) 2009 RDC Semiconductor Co.,Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For technical support : 
 *     <rdc_xorg@rdc.com.tw>
 */


#define DISABLE     BIT0                


#define Mode_Disable BIT0               
#define PHS         0                   
#define NHS         BIT1
#define PVS         0                   
#define NVS         BIT2
#define HB          BIT3                
#define VB          BIT4                
#define RBK         BIT5                



#define ROT         BIT6
#define EN_DIT      BIT7                
#define OpenLDI     BIT8                
#define _2_CH       BIT9                
#define SW_PS       BIT10               
#define NCLK        BIT11               


#define Dev_Support BIT0                
#define TX_DIT      BIT1
#define TX_PS       BIT2
#define DSTN_COLOR  BIT3
#define DSTN_MONO   0
#define DSTN_6X4    BIT4
#define DSTN_3X2    0
#define TX_MSB      BIT5
#define Dev_Boot    BIT6



#define CR          0x0000
#define SR          0x0001
#define GR          0x0002
#define AR          0x0003
#define MR          0x0004
#define NR          0x00FF  
#define BITS        BIT7


#define wDefaultModeNum 0x118


RRATE_TABLE RRateTable0640x0480[] = {
        { 25175, RR60, NHS+NVS+HB+VB,            800, 656, 752,  525, 490, 492},
        { 31500, RR72, NHS+NVS,                  832, 664, 704,  504, 489, 492},
        { 31500, RR75, NHS+NVS,                  840, 656, 720,  500, 481, 484},
        { 36000, RR85, NHS+NVS,                  832, 696, 750,  509, 481, 484}
};

RRATE_TABLE RRateTable0800x0600[] = {
        { 36000, RR56, PHS+PVS,                 1024, 824, 896,  625, 601, 604},
        { 40000, RR60, PHS+PVS,                 1056, 840, 968,  628, 601, 604},
        { 50000, RR72, PHS+PVS,                 1040, 856, 976,  666, 637, 640},
        { 49500, RR75, PHS+PVS,                 1056, 816, 896,  625, 601, 604},
        { 56250, RR85, PHS+PVS,                 1048, 832, 896,  631, 601, 604}
};

RRATE_TABLE RRateTable0720x0480[] = {
        { 27027, RR60, NHS+NVS,                  858, 736, 798,  525, 489, 495}
};

RRATE_TABLE RRateTable0720x0576[] = {
        { 27000, RR50, NHS+NVS,                  864, 732, 796,  625, 590, 595}
};



RRATE_TABLE RRateTable1024x0768[] = {
        { 65000, RR60, NHS+NVS,                1344, 1048, 1184, 806, 771, 777},
        { 75000, RR70, NHS+NVS,                1328, 1048, 1184, 806, 771, 777},
        { 78750, RR75, PHS+PVS,                1312, 1040, 1136, 800, 769, 772},
        { 94500, RR85, PHS+PVS,                1376, 1072, 1168, 808, 769, 772}
};

RRATE_TABLE RRateTable1280x0720[] = {
        { 74250, RR60, PHS+PVS,                 1648, 1390, 1430,  750,  725,  730},
};

RRATE_TABLE RRateTable1280x0768[] = {
        { 79500, RR60, NHS+PVS,                 1664, 1344, 1472,  798,  771,  778}
};

RRATE_TABLE RRateTable1280x0960[] = {
        {108000, RR60, PHS+PVS,                 1800, 1376, 1488, 1000, 961, 964}
};

RRATE_TABLE RRateTable1280x1024[] = {
        {108000, RR60, PHS+PVS,                 1688, 1328, 1440, 1066, 1025, 1028}
};

RRATE_TABLE RRateTable1360x0768[] = {
        { 85500, RR60, NHS+PVS,                 1792, 1424, 1536,  795,  771,  777}
};

RRATE_TABLE RRateTable1366x0768[] = {
        { 85860, RR60, PHS+PVS,                 1800, 1440, 1584,  795,  769,  772}
};

RRATE_TABLE RRateTable1400x0960[] = {
        {111169, RR60, NHS+PVS,                 1864, 1480, 1632,  994,  961, 964}
};

RRATE_TABLE RRateTable1400x1050[] = {
        {121750, RR60, NHS+PVS,                 1864, 1488, 1632, 1089, 1053, 1057}
};

RRATE_TABLE RRateTable1440x0900[] = {
        {106472, RR60, NHS+PVS,                 1904, 1520, 1672,  932,  901,  904}
};

RRATE_TABLE RRateTable1440x0960[] = {
        {121750, RR60, NHS+PVS,                 1920, 1528, 1680,  994,  961,  964}
};

RRATE_TABLE RRateTable1600x0900[] = {
        {118997, RR60, PHS+PVS,                 2128, 1696, 1864, 932, 901, 904}
};

RRATE_TABLE RRateTable1600x1200[] = {
        {162000, RR60, PHS+PVS,                 2160, 1664, 1856, 1250, 1201, 1204}
};

RRATE_TABLE RRateTable1680x1050[] = {
        {119000, RR60, NHS+PVS,                 1840, 1728, 1760, 1080, 1053, 1059}
};

RRATE_TABLE RRateTable1920x1080[] = {
        {148500, RR60, PHS+PVS,                 2200, 2008, 2052, 1125, 1084, 1089}
};

RRATE_TABLE RRateTable1920x1200[] = {
        {193250, RR60, NHS+PVS,                 2592, 2056, 2256, 1245, 1203, 1209}
};

RRATE_TABLE RRateTable1920x1440[] = {
        {234000, RR60, NHS+PVS,                 2600, 2048, 2256, 1500, 1441, 1444}
};

RRATE_TABLE RRateTableHDMI0640[] = {
        { 25175, RR60, NHS+NVS,                  800,  656,  752,  525,  490,  492}
};

RRATE_TABLE RRateTableHDMI0480[] = {
        { 27000, RR60, NHS+NVS,                  858,  736,  798,  525,  485,  491}
};

RRATE_TABLE RRateTableHDMI0720[] = {
        { 74250, RR60, PHS+PVS,                 1650, 1390, 1430,  750,  725,  730}
};

RRATE_TABLE RRateTableHDMI1080[] = {
        {148500, RR60, PHS+PVS,                 2200, 2008, 2052, 1125, 1084, 1089}
};

MODE_INFO VESATable[] = {
        { 640, 480, 0x101, 0x111, 0x112, RRateTable0640x0480, sizeof(RRateTable0640x0480)/sizeof(RRATE_TABLE)},
        { 720, 480, 0x126, 0x127, 0x128, RRateTable0720x0480, sizeof(RRateTable0720x0480)/sizeof(RRATE_TABLE)},
        { 720, 576, 0x144, 0x145, 0x146, RRateTable0720x0576, sizeof(RRateTable0720x0576)/sizeof(RRATE_TABLE)},
        { 800, 600, 0x103, 0x114, 0x115, RRateTable0800x0600, sizeof(RRateTable0800x0600)/sizeof(RRATE_TABLE)},

        {1024, 768, 0x105, 0x117, 0x118, RRateTable1024x0768, sizeof(RRateTable1024x0768)/sizeof(RRATE_TABLE)},
        {1280, 720, 0x147, 0x148, 0x149, RRateTable1280x0720, sizeof(RRateTable1280x0720)/sizeof(RRATE_TABLE)},
        {1280, 768, 0x129, 0x12A, 0x12B, RRateTable1280x0768, sizeof(RRateTable1280x0768)/sizeof(RRATE_TABLE)},
        {1280, 960, 0x135, 0x136, 0x137, RRateTable1280x0960, sizeof(RRateTable1280x0960)/sizeof(RRATE_TABLE)},
        {1280,1024, 0x107, 0x11A, 0x11B, RRateTable1280x1024, sizeof(RRateTable1280x1024)/sizeof(RRATE_TABLE)},
        {1360, 768, 0x13B, 0x13C, 0x13D, RRateTable1360x0768, sizeof(RRateTable1360x0768)/sizeof(RRATE_TABLE)},
        {1366, 768, 0x141, 0x142, 0x143, RRateTable1366x0768, sizeof(RRateTable1366x0768)/sizeof(RRATE_TABLE)},
        {1400, 960, 0x13E, 0x13F, 0x140, RRateTable1400x0960, sizeof(RRateTable1400x0960)/sizeof(RRATE_TABLE)},
        {1400,1050, 0x138, 0x139, 0x13A, RRateTable1400x1050, sizeof(RRateTable1400x1050)/sizeof(RRATE_TABLE)},
        {1440, 900, 0x14A, 0x14B, 0x14C, RRateTable1440x0900, sizeof(RRateTable1440x0900)/sizeof(RRATE_TABLE)},
        {1440, 960, 0x14D, 0x14E, 0x14F, RRateTable1440x0960, sizeof(RRateTable1440x0960)/sizeof(RRATE_TABLE)},
        {1600, 900, 0x161, 0x162, 0x163, RRateTable1600x0900, sizeof(RRateTable1600x0900)/sizeof(RRATE_TABLE)},
        {1600,1200, 0x150, 0x151, 0x152, RRateTable1600x1200, sizeof(RRateTable1600x1200)/sizeof(RRATE_TABLE)},
        {1680,1050, 0x153, 0x154, 0x155, RRateTable1680x1050, sizeof(RRateTable1680x1050)/sizeof(RRATE_TABLE)},
        {1920,1080, 0x156, 0x157, 0x158, RRateTable1920x1080, sizeof(RRateTable1920x1080)/sizeof(RRATE_TABLE)},
        {1920,1200, 0x159, 0x15A, 0x15B, RRateTable1920x1200, sizeof(RRateTable1920x1200)/sizeof(RRATE_TABLE)},
        {1920,1440, 0x15C, 0x15D, 0x15E, RRateTable1920x1440, sizeof(RRateTable1920x1440)/sizeof(RRATE_TABLE)},
        {0, 0, 0, 0, 0, NULL, 0}
};
        
MODE_INFO HDMITable[] = {
        { 640, 480, 0x101, 0x111, 0x112, RRateTableHDMI0640,  sizeof(RRateTableHDMI0640)/sizeof(RRATE_TABLE)}, 
        {1280, 720, 0x147, 0x148, 0x149, RRateTableHDMI0720,  sizeof(RRateTableHDMI0720)/sizeof(RRATE_TABLE)}, 
        {1920, 540, 0x0  , 0x0  , 0x0  , RRateTableHDMI1080,  sizeof(RRateTableHDMI1080)/sizeof(RRATE_TABLE)}, 
        { 720, 480, 0x126, 0x127, 0x128, RRateTableHDMI0480,  sizeof(RRateTableHDMI0480)/sizeof(RRATE_TABLE)}, 
        {1920,1080, 0x156, 0x157, 0x158, RRateTableHDMI1080,  sizeof(RRateTableHDMI1080)/sizeof(RRATE_TABLE)}, 


};


REG_PACKAGE Display1VESAModeInitRegs[] = {
        
        {SR, 0x00, 0x03, 0xFF},
        {SR, 0x01, 0x01, ~BIT5},
        {SR, 0x02, 0x0F, 0xFF},
        {SR, 0x03, 0x00, 0xFF},
        {SR, 0x04, 0x0E, 0xFF},
        {SR, 0x4F, 0x00, BIT3+BIT1}, 
        
        {CR, 0x05, 0x00, BIT6+BIT5},
        {CR, 0x08, 0x00, 0x7F},
        {CR, 0x09, 0x00, 0x9F},
        {CR, 0x17, 0x23, 0x7F},
        {CR, 0xA0, 0x70, BIT6+BIT5+BIT4},
        {CR, 0xAE, 0x80, BIT7},
        {CR, 0xA8, 0x00, BIT1},
        {CR, 0xD1, 0x80, BIT7},
        
        {GR, 0x06, 0x05, 0xFF},
        
        {NR, 0x00, 0x00, 0x00}
};

REG_PACKAGE ExtendRegs[] = {
        
        {SR, 0x11, 0x00, 0xFF},     
        {SR, 0x30, 0x00, 0xFF},     
        
        
        {CR, 0x03, 0x80, 0xF0},     
        {CR, 0x33, 0x71, 0xFF},     

        {CR, 0x0C, 0x00, 0xFF},     
        {CR, 0x0D, 0x00, 0xFF},     
        {CR, 0xB0, 0x00, 0xFF},     
        {CR, 0xAF, 0x00, 0xFF},     
        
        {CR, 0x17, 0x80, 0xFF},     

        {CR, 0x36, 0x00, 0xFF},     
        {CR, 0x37, 0x00, 0xFF},     
        {CR, 0x38, 0x00, 0xFF},     
        {CR, 0x3B, 0x00, 0xFF},     
        
        {CR, 0x82, 0x01, 0xFF},     

        {CR, 0xA3, 0x00, 0xFF},     
        {CR, 0xA4, 0x00, 0xFF},     
        {CR, 0xA5, 0x00, 0xFF},     
        
        
        
        {SR, 0x01, 0x20, BIT5},                               
        {SR, 0x2F, 0x04, BIT2+BIT1+BIT0},          
        {SR, 0x31, 0x00, BIT3+BIT2+BIT1+BIT0},     
        {SR, 0x32, 0x01, BIT1+BIT0},                      
        {SR, 0x50, 0x08, 0x1F},                              
        {SR, 0x58, 0x08, 0x1F},                              

        
        {CR, 0x3E, 0x00, BIT1+BIT0},     
        {CR, 0xA8, 0x80, BIT7+BIT4},     

        {CR, 0xB4, 0x01, BIT0},                               
        {CR, 0xB6, 0x28, BIT5+BIT3+BIT1+BIT0},     
        {CR, 0xBD, 0x00, BIT2+BIT1+BIT0},             
        {CR, 0xBF, 0x00, BIT2+BIT1+BIT0},             
        {CR, 0xC1, 0x00, BIT2+BIT1+BIT0},             
        {CR, 0xD9, 0x00, BIT2+BIT1+BIT0},             
        {CR, 0xD0, 0x00, BIT5+BIT3},                     
        
        {NR, 0x00, 0x00, 0x00}
};



REG_OP HTotal1[] = {
            {SR,BIT2+BIT1+BIT0,0x33,0},   
            {CR, 0xFF, 0x00,3},                 
            {CR, BIT0, 0xAC,11},                  
            {NR,0x0,0x0,0x0}
};


REG_OP HDispEnd1[] = {
            {SR, BIT6+BIT5+BIT4, 0x33,0},
            {CR, 0xFF, 0x01,3},
            {CR, BIT2, 0xAC,11},
            {NR,0x0,0x0,0x0}
};


REG_OP HBnkSt1[] = {
            {SR, BIT2+BIT1+BIT0, 0x34,0},
            {CR, 0xFF, 0x02,3},
            {CR, BIT4, 0xAC,11},
            {NR,0x0,0x0,0x0}
};


REG_OP HBnkEnd1[] = {
            {SR, BIT6+BIT5+BIT4, 0x34,0},
            {CR, 0x1F, 0x03,3},
            {CR, BIT7, 0x05,8},
            {CR, BIT0, 0xAD,9},
            {NR,0x0,0x0,0x0}
};


REG_OP HSyncSt1[] = {
            {SR, BIT2+BIT1+BIT0, 0x35,0},
            {CR, 0xFF, 0x04,3},
            {CR, BIT6, 0xAC,11},
            {NR,0x0,0x0,0x0}
};


REG_OP HSyncEnd1[] = {
            {SR, BIT6+BIT5+BIT4, 0x35,0},
            {CR, 0x1F, 0x05,3},
            {CR, BIT2, 0xAD,8},
            {NR,0x0,0x0,0x0}
};


REG_OP VTotal1[] = {
            {CR, 0xFF, 0x06,0},
            {CR, BIT0, 0x07,8},
            {CR, BIT5, 0x07,9},
            {CR, BIT0, 0xAE,10},
            {NR,0x0,0x0,0x0}
};


REG_OP VDispEnd1[] = {
            {CR, 0xFF, 0x12,0},
            {CR, BIT1, 0x07,8},
            {CR, BIT6, 0x07,9},
            {CR, BIT1, 0xAE,10},
            {NR,0x0,0x0,0x0}
};


REG_OP VBnkSt1[] = {
            {CR, 0xFF, 0x15,0},
            {CR, BIT3, 0x07,8},
            {CR, BIT5, 0x09,9},
            {CR, BIT2, 0xAE,10},
            {NR,0x0,0x0,0x0}
};


REG_OP VBnkEnd1[] = {
            {CR, 0xFF, 0x16,0},
            {CR, BIT4, 0xAE,8},
            {NR,0x0,0x0,0x0}
};


REG_OP VSyncSt1[] = {
             {CR, 0xFF, 0x10,0},
             {CR, BIT2, 0x07,8},
             {CR, BIT7, 0x07,9},
             {CR, BIT3, 0xAE,10},
             {NR,0x0,0x0,0x0}
};


REG_OP VSyncEnd1[] = {
             {CR, BIT3+BIT2+BIT1+BIT0, 0x11,0},
             {CR, BIT6+BIT5, 0xAE,4},
             {NR,0x0,0x0,0x0}
};


REG_OP Pitch1[] = {
             {CR, 0xFF, 0x13,0},
             {CR, 0x3F, 0xB0,8},
             {NR,0x0,0x0,0x0}
};




REG_OP HTotal2[] = {
            {SR, BIT2+BIT1+BIT0,0x36,0},   
            {CR, 0xFF, 0x23,3},                 
            {CR, BIT0, 0x29,11},                  
            {NR,0x0,0x0,0x0}
};


REG_OP HDispEnd2[] = {
            {SR, BIT6+BIT5+BIT4, 0x36,0},
            {CR, 0xFF, 0x24,3},
            {CR, BIT2, 0x29,11},
            {NR,0x0,0x0,0x0}
};


REG_OP HBnkSt2[] = {
            {SR, BIT2+BIT1+BIT0, 0x37,0},
            {CR, 0xFF, 0x25,3},
            {CR, BIT4, 0x29,11},
            {NR,0x0,0x0,0x0}
};


REG_OP HBnkEnd2[] = {
            {SR, BIT6+BIT5+BIT4, 0x37,0},
            {CR, 0x1F, 0x26,3},
            {CR, BIT7, 0x28,8},
            {CR, BIT0, 0x2A,9},
            {NR,0x0,0x0,0x0}
};


REG_OP HSyncSt2[] = {
            {SR, BIT2+BIT1+BIT0, 0x38,0},
            {CR, 0xFF, 0x27,3},
            {CR, BIT6, 0x29,11},
            {NR,0x0,0x0,0x0}
};


REG_OP HSyncEnd2[] = {
            {SR, BIT6+BIT5+BIT4, 0x38,0},
            {CR, 0x1F, 0x28,3},
            {CR, BIT2, 0x2A,8},
            {NR,0x0,0x0,0x0}
};


REG_OP VTotal2[] = {
            {CR, 0xFF, 0x2B,0},
            {CR, BIT0, 0x31,8},
            {CR, BIT5, 0x31,9},
            {CR, BIT0, 0x32,10},
            {NR,0x0,0x0,0x0}
};


REG_OP VDispEnd2[] = {
            {CR, 0xFF, 0x2C,0},
            {CR, BIT1, 0x31,8},
            {CR, BIT6, 0x31,9},
            {CR, BIT1, 0x32,10},
            {NR,0x0,0x0,0x0}
};


REG_OP VBnkSt2[] = {
            {CR, 0xFF, 0x2D,0},
            {CR, BIT3, 0x31,8},
            {CR, BIT5, 0x29,9},
            {CR, BIT2, 0x32,10},
            {NR,0x0,0x0,0x0}
};


REG_OP VBnkEnd2[] = {
            {CR, 0xFF, 0x2E,0},
            {CR, BIT4, 0x32,8},
            {NR,0x0,0x0,0x0}
};


REG_OP VSyncSt2[] = {
             {CR, 0xFF, 0x2F,0},
             {CR, BIT2, 0x31,8},
             {CR, BIT7, 0x31,9},
             {CR, BIT3, 0x32,10},
             {NR,0x0,0x0,0x0}
};


REG_OP VSyncEnd2[] = {
             {CR, BIT3+BIT2+BIT1+BIT0, 0x30,0},
             {CR, BIT6+BIT5, 0x32,4},
             {NR,0x0,0x0,0x0}
};


REG_OP Pitch2[] = {
             {CR, 0xFF, 0x3A,0},
             {CR, 0x3F, 0x3B,8},
             {NR,0x0,0x0,0x0}
};



PANEL_INFO LCDTable[] = {
        { 0, 0, 0, 0, 0, 0, { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0}}}
};


RRATE_TABLE RRateTableEDID1[] = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0}
};

MODE_INFO VESAEDIDTable[] = {
        { 0, 0, 0x1A0, 0x1A1, 0x1A2, RRateTableEDID1, sizeof(RRateTableEDID1)/sizeof(RRATE_TABLE)}
};



PORT_CONFIG PortConfig[]={
        {0,0,0,0,0}, 
        {0,0,0,0,0}, 
        {0,0,0,0,0}, 
        {0,0,0,0,0}  
};

TxEncDetect *pTMDSTxTable = NULL;

