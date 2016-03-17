/*
 * Copyright (C) 2009 RDC Semiconductor Co.,Ltd
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For technical support : 
 *     <rdc_xorg@rdc.com.tw>
 */
 

PORT_CONFIG *pPortConfig;
MODE_INFO   *pLCDTable;
MODE_INFO   *pVESATable;
void *PCIDataStruct;
void *BiosInfoData;

bool bCRTSUPPORT = false;
bool bLCDSUPPORT = false;
bool bDVISUPPORT = false;
bool bTVSUPPORT = false;

bool bDS_SUPPORT = false;


void *Display1VESAModeInitRegs;


void *POSTInItRegs;
void *DDRII400Tbl;
void *DDRII533Tbl;
void *ExtendRegs2;



REG_OP HTotal1[] = {
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


REG_OP HSource1[] = {
            {SR, 0xFF,                0x4B, 0},
            {SR, BIT3+BIT2+BIT1+BIT0, 0x4C, 8},
            {NR,0x0,0x0,0x0}
};

REG_OP HSource2[] = {
            {SR, 0xFF,                0x4D, 0},
            {SR, BIT3+BIT2+BIT1+BIT0, 0x4E, 8},
            {NR,0x0,0x0,0x0}
};

REG_OP HScalingFactor1[] = {
            {SR, 0xFF,                0x59, 0},
            {SR, BIT3+BIT2+BIT1+BIT0, 0x5B, 8},
            {NR,0x0,0x0,0x0}
};
        
REG_OP VScalingFactor1[] = {
            {SR, 0xFF,                0x5A, 0},
            {SR, BIT6+BIT5+BIT4,      0x5B, 8},
            {NR,0x0,0x0,0x0}
};
        
REG_OP HScalingFactor2[] = {
            {SR, 0xFF,                0x51, 0},
            {SR, BIT3+BIT2+BIT1+BIT0, 0x53, 8},
            {NR,0x0,0x0,0x0}
};

REG_OP VScalingFactor2[] = {
            {SR, 0xFF,                0x52, 0},
            {SR, BIT6+BIT5+BIT4,      0x53, 8},
            {NR,0x0,0x0,0x0}
};

REG_OP HorDownScaleFactor1[] = {
        {SR, 0xFF,                    0x75, 0},
        {SR, BIT1+BIT0,               0x74, 8},
        {NR,0x0,0x0,0x0}
};
        
REG_OP VerDownScaleFactor1[] = {
        {SR, 0xFF,                    0x71, 0},
        {SR, BIT1+BIT0,               0x70, 8},
        {NR,0x0,0x0,0x0}
};

REG_OP HorDownScaleSrcSize1[] = {
            {SR, 0xFF,                0x77, 0},
            {SR, BIT2+BIT1+BIT0,      0x76, 8},
            {NR,0x0,0x0,0x0}
};

REG_OP VerDownScaleSrcSize1[] = {
            {SR, 0xFF,                0x73, 0},
            {SR, BIT2+BIT1+BIT0,      0x72, 8},
            {NR,0x0,0x0,0x0}
};

VESA_TABLE CInt10VESATable[] = {
        {
            {1280, 720, 0x147, 0x148, 0x149, 1},
            {74250, RR60, PHS+PVS, 370, 1390, 40,  30,  725,  5}
        },
        {
            {1280, 768, 0x129, 0x12A, 0x12B, 1},
            { 79500, RR60, NHS+PVS, 384, 1344, 128,  30,  771,  7}
        },
        {
            {1280, 960, 0x135, 0x136, 0x137, 1},
            {108000, RR60, PHS+PVS, 520, 1376, 112, 40,  961,  3}
        },
        {
            {1280, 1024, 0x107, 0x11A, 0x11B, 1},
            {108000, RR60, PHS+PVS, 408, 1328, 112, 42, 1025, 3}
        },
        {
            {1360, 768, 0x13B, 0x13C, 0x13D, 1},
            {85500, RR60, NHS+PVS, 432, 1424, 112, 27, 771, 6}            
        },
        {
            {1366, 768, 0x141, 0x142, 0x143, 1},
            { 85860, RR60, NHS+PVS, 434, 1440, 144,  27,  769,  3}
        },
        {
            {1400, 960, 0x13E, 0x13F, 0x140, 1},
            {111169, RR60, NHS+PVS, 464, 1480, 152, 34, 961, 3}
        },
        {
            {1400,1050, 0x138, 0x139, 0x13A, 1},
            {121750, RR60, NHS+PVS, 464, 1488, 144, 39, 1053, 4}
        },
        {
            {1440, 900, 0x14A, 0x14B, 0x14C, 1},
            {106472, RR60, NHS+PVS, 464, 1520, 152, 32, 901, 3}
        },
        {
            {1440, 960, 0x14D, 0x14E, 0x14F, 1},
            {121750, RR60, NHS+PVS, 480, 1528, 152, 34, 961, 3}
        },
        {
            {1600, 1200, 0x150, 0x151, 0x152, 1},
            {162000, RR60, PHS+PVS, 560, 1664, 192, 50, 1201, 3}
        },
        {
            {1680, 1050, 0x153, 0x154, 0x155, 1},
            {119000, RR60, NHS+PVS, 160, 1728, 32, 30, 1053, 6}
        },
        {
            {1920, 1080, 0x156, 0x157, 0x158, 1},
            {172798, RR60, NHS+PVS, 656, 2040, 208, 38, 1081, 3}
        },
        {
            {1920, 1200, 0x159, 0x15A, 0x15B, 1},
            {193250, RR60, NHS+PVS, 676, 2056, 200, 45, 1203, 6}
        },
        {
            {1920, 1440, 0x15C, 0x15D, 0x15E, 1},
            {234000, RR60, NHS+PVS, 680, 2048, 208, 60, 1441, 3}
        },

        {
            {0xffff,1050, 0x138, 0x139, 0x13A, 1},
            {121750, RR60, NHS+PVS, 464, 1488, 144, 39, 1053, 4}
        }
};


