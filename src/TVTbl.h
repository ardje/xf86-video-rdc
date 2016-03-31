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

#define TVMode3              0
#define TVMode6x4            3
#define TVMode7x4            4
#define TVMode7x5            5
#define TVMode8x6            6
#define TVMode1024           7

#define VBE_Mode_3           0x3
#define Mode640x480_8bpp     0x101
#define Mode640x480_16bpp    0x111
#define Mode640x480_32bpp    0x112
#define Mode720x480_8bpp     0x126
#define Mode720x480_16bpp    0x127
#define Mode720x480_32bpp    0x128
#define Mode720x576_8bpp     0x144
#define Mode720x576_16bpp    0x145
#define Mode720x576_32bpp    0x146
#define Mode800x600_8bpp     0x103
#define Mode800x600_16bpp    0x114
#define Mode800x600_32bpp    0x115
#define Mode1024x768_8bpp    0x105
#define Mode1024x768_16bpp   0x117
#define Mode1024x768_32bpp   0x118
#define HDMI720P8bpp         0x147
#define HDMI720P16bpp        0x148
#define HDMI720P32bpp        0x149
#define HDMI1080P8bpp        0x156
#define HDMI1080P16bpp       0x157
#define HDMI1080P32bpp       0x158

#define TVVESAModeCnt        8 
#define TVSupportModeCnt     5 
#define TMDSSupportModeCnt   5

#define SAA7105EncData       0x2D
#define SAA7105DACPowerDown  0x61
#define FS473DACPower        0xD4

#define PLL25175             25175  
#define PLL40000             40000  
#define PLL65000             65000  
#define PLL74250             74250  
#define PLL148500           148500  


typedef struct _SAA7105_Mode_TABLE {
    unsigned char   ucTVModeIndex;
    unsigned short  Mode_ID_8bpp;
    unsigned short  Mode_ID_16bpp;
    unsigned short  Mode_ID_32bpp;
    unsigned char   Reg5Ato66[13];
    unsigned char   Reg6Cto7C[17];
    unsigned char   Reg81to85[5];
    unsigned char   Reg90to9F[16];
    unsigned short  usHTotal;
    unsigned short  usHDisplayEnd;
    unsigned short  usHBlankingStart;
    unsigned short  usHBlankingEnd;
    unsigned short  usHSyncStart;
    unsigned short  usHSyncEnd;
    unsigned short  usVTotal;
    unsigned short  usVDisplayEnd;
    unsigned short  usVBlankingStart;
    unsigned short  usVBlankingEnd;
    unsigned short  usVSyncStart;
    unsigned short  usVSyncEnd;
}SAA7105_Mode_TABLE;


typedef struct _FS473_Mode_TABLE {
    unsigned char   ucTVModeIndex;
    unsigned short  Mode_ID_8bpp;
    unsigned short  Mode_ID_16bpp;
    unsigned short  Mode_ID_32bpp;
    unsigned char   Reg00to0F[16];
    unsigned char   Reg12to19[8];
    unsigned char   RegC6toD1[12];
    unsigned char   Reg3Cto3D[2];
    unsigned short  usHTotal;
    unsigned short  usHDisplayEnd;
    unsigned short  usHBlankingStart;
    unsigned short  usHBlankingEnd;
    unsigned short  usHSyncStart;
    unsigned short  usHSyncEnd;
    unsigned short  usVTotal;
    unsigned short  usVDisplayEnd;
    unsigned short  usVBlankingStart;
    unsigned short  usVBlankingEnd;
    unsigned short  usVSyncStart;
    unsigned short  usVSyncEnd;
}FS473_Mode_TABLE;

typedef struct _SAA7105_I2C_REG{
    BYTE    ucIndex;
    BYTE    ucData;
} SAA7105_I2C_REG;

typedef struct _FS473_I2C_REG{
    BYTE    ucIndex;
    BYTE    ucDataLow;
    BYTE    ucDataHigh;
} FS473_I2C_REG;

typedef struct _FSVSRegdata{
    WORD    VTotal;
    BYTE    ucData[20];
} FSVSRegdata;

typedef struct _FSVScaler{
    BYTE ucTVType;
    WORD wHRes;
    WORD wVRes;
    FSVSRegdata *ScalerTable;
} FSVScaler;

typedef struct _FSPLLPatch{
    DWORD   dwPixelClock;
    BYTE    ucData[10];
} FSPLLPatch;

typedef struct _SAAPLLPatch{
    DWORD   dwPixelClock;
    BYTE    ucData[4];
} SAAPLLPatch;

SAA7105_I2C_REG SAA7105InitReg[]={
    {0x2D,0x08},
    {0x3A,0x0C},
    {0x16,0x0F},
    {0x17,0x18},
    {0x18,0x18},
    {0x19,0x18},
    {0x54,0x02},
    {0x55,0x0F},
    {0x56,0xC3},
    {0x58,0x02},
    {0x59,0x30},
    {0x7E,0x0C},
    {0x7F,0x0D},
    {0xF9,0x00},
    {0xFA,0x06},
    {0xFD,0x81},
    {0xFF,0xFF}
};

FS473_I2C_REG FS473InitReg[]={
    {0x10,0x08,0x00},
    {0x34,0xa0,0x00},
    {0xA0,0x04,0x00},
    {0x52,0x01,0x00},
    {0x46,0x00,0x09},
    {0xA2,0x4D,0x00},
    {0xA4,0x96,0x00},
    {0xA6,0x1D,0x00},
    {0xA8,0xA0,0x00},
    {0xAA,0xDB,0x00},
    {0xAC,0x7E,0x00},
    {0x20,0x08,0x00},
    {0x22,0x0c,0x00},
    {0x9a,0x00,0x00},
    {0x48,0x00,0x00},
    {0x44,0x00,0x00},
    {0x4A,0x7E,0x7E},
    {0x6a,0x00,0x00},
    {0x78,0x00,0xB4},
    {0x7A,0x15,0x15},
    {0x80,0x08,0x00},
    {0xB4,0x00,0x00},
    {0xB6,0xF0,0x00},
    {0xC2,0x00,0x00},
    {0xd4,0x03,0x00},
    {0xFF,0xFF,0xFF}
};

FS473_I2C_REG FS473_NTSCReg[]={
    {0x40,0x1F,0x7C},   
    {0x42,0xF0,0x21},    
    {0x4C,0x44,0x44},   
    {0x4E,0x76,0x76},
    {0x50,0x48,0x00},
    {0x54,0x00,0x4C},   
    {0x56,0x02,0x40},
    {0x64,0x02,0xDC},   
    {0x66,0xCB,0xCB},
    {0x5E,0x01,0x83},   
    {0x70,0x16,0x20},   
    {0x7E,0x24,0x00},   
    {0xFF,0xFF,0xFF}    
};

FS473_I2C_REG FS473_PALReg[]={
    {0x40,0xCB,0xA8},   
    {0x42,0x09,0x2A},    
    {0x4C,0x40,0x40},   
    {0x4E,0x8A,0x8A},
    {0x50,0x2C,0x1F},
    {0x54,0x02,0x43},   
    {0x56,0x02,0x43},
    {0x64,0x02,0xDC},   
    {0x66,0x91,0x91},
    {0x5E,0x01,0x9C},   
    {0x70,0x1A,0x18},   
    {0x7E,0x59,0x59},   
    {0xFF,0xFF,0xFF}    
};

SAA7105_Mode_TABLE CBIOS_NTSC_SAA7105_TV_Mode_Table[TVSupportModeCnt]={
    {TVMode3, VBE_Mode_3, 0x0,0x0, 
            {0x25,0x76,0xaf,0x30,0x3c,0x6e,0x00,0x11,0x3f,0x1f,0x7c,0xf0,0x21},
            {0x01,0x20,0x00,0x06,0x44,0x44,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x1a,0xfb,0x00},
            {0x78,0x89,0x25,0xa5,0x88},
            {0x67,0x7e,0x50,0x50,0x04,0xd0,0x2a,0x21,0x87,0x93,0x71,0x5b,0x7d,0x99,0x99,0x3b},
            824, 640, 648, 816, 712, 762, 570, 480, 488, 562, 468, 495
    },
    {TVMode6x4, Mode640x480_8bpp, Mode640x480_16bpp,Mode640x480_32bpp, 
        {0x25,0x76,0xaf,0x30,0x3c,0x2e,0x25,0x11,0x3f,0x1f,0x7c,0xf0,0x21},
        {0x01,0x26,0x00,0x06,0x60,0x28,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x18,0xfb,0x00},
        {0xea,0x87,0x21,0xa5,0x88},
        {0x81,0x40,0x28,0x28,0x04,0x7d,0x2a,0x20,0x2f,0x94,0x4f,0x20,0x7f,0x90,0x90,0x3b},
        800, 640, 640, 800, 657, 760, 590, 480, 480, 590, 534, 547
    },
    {TVMode7x4, Mode720x480_8bpp, Mode720x480_16bpp, Mode720x480_32bpp,
        {0x25,0x76,0xaf,0x30,0x3c,0xee,0x25,0x11,0x3f,0x1f,0x7c,0xf0,0x21},
        {0x00,0x2d,0x90,0x06,0x42,0x42,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x14,0xfa,0x00},
        {0x30,0x02,0x23,0xA5,0x88},
        {0x51,0x68,0x34,0x36,0x04,0xe8,0x69,0x25,0x7f,0x23,0x39,0xa4,0x7e,0x00,0x00,0x80},
        896, 720, 720, 896, 736, 808, 550, 480, 480, 550, 483, 495
    },
    {TVMode8x6, Mode800x600_8bpp, Mode800x600_16bpp, Mode800x600_32bpp,
        {0x00,0x76,0xaf,0x30,0x3c,0x2e,0x25,0x11,0x3f,0x1f,0x7c,0xf0,0x21},
        {0x01,0x28,0x00,0x06,0x5c,0x28,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x1b,0xf8,0x00},
        {0xa2,0xb0,0x38,0xa5,0x88},
        {0xcf,0x95,0x3d,0x3d,0x04,0x7d,0x2a,0x20,0x2f,0x54,0x2a,0x98,0x5c,0xe9,0xea,0x2a},
        1064, 800, 800, 1064, 840, 972, 750, 600, 600, 750, 670, 700
    },
    {TVMode1024, Mode1024x768_8bpp, Mode1024x768_16bpp, Mode1024x768_32bpp,
        {0x00,0x76,0xa5,0x3a,0x2e,0x2e,0x00,0x15,0x3f,0x1f,0x7c,0xf0,0x21},
        {0x01,0x24,0x00,0x02,0x58,0x34,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x18,0xf5,0x00},
        {0x0a,0xbf,0x4d,0xa5,0x88},
        {0x39,0x00,0x69,0x69,0x08,0x00,0x28,0x21,0x73,0x04,0xb8,0x64,0x49,0x80,0x80,0x2a},
        1152, 1024, 1024, 1152, 1032, 1090, 950, 768, 768, 950,  850, 900
    }    
};

SAA7105_Mode_TABLE CBIOS_PAL_SAA7105_TV_Mode_Table[TVSupportModeCnt]={
    {TVMode3, VBE_Mode_3, 0x0,0x0, 
        {0x00,0x7d,0xaf,0x33,0x35,0x75,0x00,0x02,0x2f,0xcb,0x8a,0x09,0x2a},
        {0x01,0x23,0x80,0x02,0x7e,0x29,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x20,0x22,0x40},
        {0x5c,0x55,0x29,0xa5,0x88},
        {0xda,0x68,0x3d,0x3d,0x04,0xe6,0x2b,0x21,0xec,0x96,0x4a,0x01,0x8d,0x5d,0x5d,0x4c},
        992, 640, 648, 984, 792, 844, 625, 400, 408, 209, 497, 529
    },
    {TVMode6x4, Mode640x480_8bpp, Mode640x480_16bpp,Mode640x480_32bpp, 
        {0x00,0x7d,0xaf,0x33,0x35,0x35,0x00,0x02,0x2f,0xcb,0x8a,0x09,0x2a},
        {0x01,0x25,0x80,0x02,0x68,0x44,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x25,0x25,0x40},
        {0xaa,0x84,0x1d,0xa4,0x88},
        {0xb5,0x80,0x48,0x48,0x08,0xe5,0x29,0x20,0x8f,0x06,0xc7,0x6e,0x87,0x32,0x32,0x4c},
        840, 640, 640, 840, 658, 768, 593, 480, 480, 593, 518, 528
    },
    {TVMode7x5, Mode720x576_8bpp, Mode720x576_16bpp, Mode720x576_32bpp,
        {0x00,0x7d,0xaf,0x33,0x35,0x75,0x00,0x02,0x2f,0xcb,0x8a,0x09,0x2a},
        {0x01,0x23,0x80,0x02,0x58,0x5C,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x13,0x30,0x40},
        {0x0a,0xd7,0x23,0xa5,0x88},
        {0x7F,0x68,0x28,0x28,0x04,0xb8,0x2a,0x20,0x98,0x04,0x46,0x21,0x7e,0x00,0x00,0x2a},
        864, 720, 720, 864, 732, 796, 700, 576, 576, 700, 615, 621
    },
    {TVMode8x6, Mode800x600_8bpp, Mode800x600_16bpp, Mode800x600_32bpp,
        {0x00,0x7d,0xaf,0x33,0x35,0x35,0x00,0x02,0x2f,0xcb,0x8a,0x09,0x2a},
        {0x01,0x26,0x00,0x02,0x68,0x44,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x25,0x25,0x40},
        {0x3f,0xe9,0x2b,0xa5,0x88},
        {0x89,0x90,0x49,0x49,0x04,0xed,0x2a,0x24,0xe7,0x03,0x71,0xba,0x6c,0x60,0x60,0x3b},
        1000, 800, 800, 1000, 858, 940, 741, 600, 600, 741, 625, 665
    },
    {TVMode1024, Mode1024x768_8bpp, Mode1024x768_16bpp, Mode1024x768_32bpp,
        {0x00,0x7d,0xaf,0x33,0x35,0x35,0x00,0x02,0x2f,0xcb,0x8a,0x09,0x2a},
        {0x01,0x22,0x00,0x02,0x68,0x44,0x61,0x90,0x94,0xb0,0x78,0xf9,0x2a,0xfa,0x25,0x25,0x40},
        {0x36,0x8e,0x43,0xa5,0x88},
        {0x9a,0x00,0x66,0x66,0x08,0x00,0x28,0x24,0xaf,0x04,0xb8,0x45,0x59,0xa1,0xa1,0x2a},
        1200, 1024, 1024, 1200, 1041, 1121, 950, 768, 768, 950, 828, 843
    }
};

FS473_Mode_TABLE CBIOS_NTSC_FS473_TV_Mode_Table[TVSupportModeCnt]={
    {TVMode3, VBE_Mode_3, 0x0,0x0,                                                         
        {0x00,0x00,0x18,0x00,0x94,0x02,0x54,0x2b,0x00,0x00,0x00,0x08,0x00,0x00,0x30,0x04}, 
        {0x36,0x52,0x00,0x00,0x79,0x4b,0x00,0x00},                                         
        {0x1b,0x00,0xb8,0x22,0x87,0x00,0x85,0x00,0x87,0x00,0x00,0x00},                     
        {0x00,0x09},                                                                       
        800, 640, 640, 800, 660, 692, 460, 400, 400, 460, 430, 431                         
    },
    {TVMode6x4, Mode640x480_8bpp, Mode640x480_16bpp,Mode640x480_32bpp,                     
        {0x5b,0x00,0x18,0x00,0x88,0x02,0x29,0xf2,0x07,0x00,0x00,0x06,0x00,0x00,0x30,0x04}, 
        {0x9b,0x5d,0x00,0x00,0x78,0x55,0x00,0x00},                                         
        {0x14,0x00,0x18,0x33,0x89,0x00,0x88,0x00,0x89,0x00,0x00,0x00},                     
        {0x00,0x09},                                                                       
        800, 640, 640, 800, 660, 692, 555, 480, 480, 555, 517, 518                         
    },
    {TVMode7x4, Mode720x480_8bpp, Mode720x480_16bpp, Mode720x480_32bpp,                    
        {0x00,0x00,0x20,0x00,0xfa,0x02,0x9b,0xf6,0x07,0x00,0xf9,0x00,0x00,0x00,0x30,0x04}, 
        {0x70,0x14,0x00,0x00,0x8d,0x13,0x00,0x00},                                         
        {0x15,0x00,0x4d,0x21,0x03,0x00,0x83,0x00,0x83,0x00,0x00,0x00},                     
        {0x00,0x09},                                                                       
        864, 720, 720, 864, 747, 779, 545, 480, 480, 545, 512, 513                         
    },
    {TVMode8x6, Mode800x600_8bpp, Mode800x600_16bpp, Mode800x600_32bpp,                    
        {0x8b,0x00,0x1b,0x00,0x82,0x03,0x34,0xc4,0x07,0x00,0xeb,0x00,0x00,0x00,0x30,0x00}, 
        {0x90,0x08,0x00,0x00,0x55,0x05,0x00,0x00},                                         
        {0x19,0x00,0x78,0x22,0x83,0x00,0x83,0x00,0x83,0x00,0x00,0x00},                     
        {0x00,0x08},                                                                       
        1056, 800, 800, 1056, 896, 928, 685, 600, 600, 685, 642, 643                       
    },
    {TVMode1024, Mode1024x768_8bpp, Mode1024x768_16bpp, Mode1024x768_32bpp,                
        {0x32,0x00,0x26,0x00,0x2c,0x02,0x02,0x97,0x07,0x00,0x00,0x27,0x00,0x00,0xb0,0x00}, 
        {0x20,0x0b,0x00,0x00,0x61,0x08,0x00,0x00},                                         
        {0x1d,0x00,0x98,0x23,0x05,0x00,0x85,0x00,0x82,0x00,0x0a,0x00},                     
        {0x00,0x0b},                                                                       
        1344, 1024, 1024, 1344, 1152, 1216, 890, 768, 768, 890,  829, 830                  
    }
};

FS473_Mode_TABLE CBIOS_PAL_FS473_TV_Mode_Table[TVSupportModeCnt]={
    {TVMode3, VBE_Mode_3, 0x0,0x0,                                                         
        {0x30,0x00,0x28,0x00,0xee,0x02,0xd3,0x5b,0x00,0x00,0x00,0x08,0x00,0x01,0x30,0x04}, 
        {0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00},                                         
        {0x1a,0x00,0xbc,0x22,0x89,0x00,0x09,0x00,0x89,0x00,0x00,0x00},                     
        {0x00,0x09},                                                                       
        800, 640, 640, 800, 656, 752, 460, 400, 400, 460, 410, 412                         
    },
    {TVMode6x4, Mode640x480_8bpp, Mode640x480_16bpp,Mode640x480_32bpp,                     
        {0x1a,0x00,0x2c,0x00,0x9f,0x02,0x93,0x25,0x00,0x00,0x00,0x0a,0x00,0x01,0x30,0x04}, 
        {0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00},                                         
        {0x1a,0x00,0x91,0x21,0x04,0x00,0x04,0x00,0x84,0x00,0x00,0x00},                     
        {0x00,0x0b},                                                                       
        800, 640, 640, 800, 656, 752, 545, 480, 480, 545, 490, 492                         
    },
    {TVMode7x5, Mode720x576_8bpp, Mode720x576_16bpp, Mode720x576_32bpp,                    
        {0x30,0x00,0x20,0x00,0xfc,0x02,0x6c,0xf2,0x07,0x00,0xf6,0x00,0x00,0x01,0x30,0x04}, 
        {0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00},                                         
        {0x18,0x00,0xed,0x21,0x84,0x00,0x04,0x00,0x84,0x00,0x00,0x00},                     
        {0x00,0x0b},                                                                       
        864, 720, 720, 864, 732, 796, 660, 576, 576, 660, 615, 621                         
    },
    {TVMode8x6, Mode800x600_8bpp, Mode800x600_16bpp, Mode800x600_32bpp,                    
        {0x41,0x00,0x43,0x00,0x38,0x03,0x93,0xe9,0x07,0x00,0xed,0x00,0x00,0x01,0x30,0x00}, 
        {0xe3,0x05,0x00,0x00,0x65,0x04,0x00,0x00},                                         
        {0x1a,0x00,0x40,0x23,0x05,0x00,0x85,0x00,0x85,0x00,0x00,0x00},                     
        {0x00,0x0b},                                                                       
        1056, 800, 800, 1056, 840, 968, 685, 600, 600, 685, 601, 605                       
    },
    {TVMode1024, Mode1024x768_8bpp, Mode1024x768_16bpp, Mode1024x768_32bpp,                
        {0x42,0x00,0x57,0x00,0x24,0x02,0xc6,0xb3,0x07,0x00,0x00,0x2b,0x00,0x01,0xb0,0x00}, 
        {0xde,0x04,0x00,0x00,0x65,0x04,0x00,0x00},                                         
        {0x1e,0x00,0x14,0x23,0x05,0x00,0x85,0x00,0x82,0x00,0x0a,0x00},                     
        {0x00,0x0b},                                                                       
        1344, 1024, 1024, 1344, 1048, 1184, 890, 768, 768, 890, 771, 777                   
    }
};


FSVSRegdata FSVSNTSC640x480[5]= {
   
        {565, 0x19, 0x00, 0x58, 0x44, 0x8A, 0x00, 0x8A, 0x00, 0x8A, 0x00, 0x50, 0x23, 0x00, 0x00, 0x31, 0x23, 0x00, 0x00, 0xd0, 0xed},
        {560, 0x16, 0x00, 0xb8, 0x22, 0x87, 0x00, 0x86, 0x00, 0x87, 0x00, 0x00, 0x28, 0x00, 0x00, 0x31, 0x23, 0x00, 0x00, 0x00, 0xf0},
        {555, 0x14, 0x00, 0x18, 0x33, 0x89, 0x00, 0x88, 0x00, 0x89, 0x00, 0x9b, 0x5d, 0x00, 0x00, 0x78, 0x55, 0x00, 0x00, 0x29, 0xf2},
        {550, 0x17, 0x00, 0x28, 0x23, 0x88, 0x00, 0x87, 0x00, 0x88, 0x00, 0x64, 0x00, 0x00, 0x00, 0x5b, 0x00, 0x00, 0x00, 0x5d, 0xf4},
        {545, 0x18, 0x00, 0xc1, 0x21, 0x84, 0x00, 0x83, 0x00, 0x84, 0x00, 0x94, 0x2a, 0x00, 0x00, 0x31, 0x23, 0x00, 0x00, 0x9b, 0xf6}
};

FSVSRegdata FSVSNTSC800x600[5]= {
   
        {706, 0x1c, 0x00, 0x1d, 0x22, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00, 0x20, 0x2c, 0x00, 0x00, 0xa9, 0x1a, 0x00, 0x00, 0x5e, 0xbe},
        {699, 0x1e, 0x00, 0xd4, 0x23, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x90, 0x0e, 0x00, 0x00, 0xe3, 0x08, 0x00, 0x00, 0x46, 0xc0},
        {692, 0x1a, 0x00, 0x48, 0x23, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x40, 0x2b, 0x00, 0x00, 0xa9, 0x1a, 0x00, 0x00, 0x38, 0xc2},
        {685, 0x19, 0x00, 0x78, 0x22, 0x83, 0x00, 0x83, 0x00, 0x83, 0x00, 0x90, 0x08, 0x00, 0x00, 0x55, 0x05, 0x00, 0x00, 0x34, 0xc4},
        {678, 0x25, 0x00, 0x94, 0x34, 0x84, 0x00, 0x84, 0x00, 0x84, 0x00, 0x20, 0x0e, 0x00, 0x00, 0xe3, 0x08, 0x00, 0x00, 0x3a, 0xc6}
};

FSVSRegdata FSVSNTSC1024x768[5]= {
   
        {904, 0x25, 0x00, 0x11, 0x23, 0x03, 0x00, 0x83, 0x00, 0x81, 0x00, 0x80, 0x38, 0x00, 0x00, 0xe5, 0x29, 0x00, 0x00, 0xac, 0x94},
        {897, 0x21, 0x00, 0xb5, 0x22, 0x03, 0x00, 0x83, 0x00, 0x81, 0x00, 0x70, 0x01, 0x00, 0x00, 0x13, 0x01, 0x00, 0x00, 0xd5, 0x95},
        {890, 0x1d, 0x00, 0x98, 0x23, 0x05, 0x00, 0x85, 0x00, 0x82, 0x00, 0x20, 0x0b, 0x00, 0x00, 0x61, 0x08, 0x00, 0x00, 0x02, 0x97},
        {883, 0x19, 0x00, 0x01, 0x22, 0x03, 0x00, 0x83, 0x00, 0x81, 0x00, 0x30, 0x37, 0x00, 0x00, 0xe5, 0x29, 0x00, 0x00, 0x35, 0x98},
        {876, 0x15, 0x00, 0xa9, 0x21, 0x03, 0x00, 0x83, 0x00, 0x81, 0x00, 0x40, 0x12, 0x00, 0x00, 0xf7, 0x0d, 0x00, 0x00, 0x6c, 0x99}
};

FSVSRegdata FSVSPAL640x480[5]= {
   
        {560, 0x1a, 0x00, 0x9d, 0x21, 0x84, 0x00, 0x04, 0x00, 0x84, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0xb6, 0x1d},
        {555, 0x18, 0x00, 0xc0, 0x22, 0x88, 0x00, 0x08, 0x00, 0x88, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x49, 0x20},
        {550, 0x14, 0x00, 0x44, 0x22, 0x88, 0x00, 0x08, 0x00, 0x88, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0xe8, 0x22},
        {545, 0x1a, 0x00, 0x91, 0x21, 0x84, 0x00, 0x04, 0x00, 0x84, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x93, 0x25},
        {540, 0x14, 0x00, 0x2d, 0x21, 0x84, 0x00, 0x04, 0x00, 0x84, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x4b, 0x28}
};

FSVSRegdata FSVSPAL800x600[5]= {
   
        {699, 0x1e, 0x00, 0x28, 0x45, 0x87, 0x00, 0x87, 0x00, 0x87, 0x00, 0x03, 0x0a, 0x00, 0x00, 0x53, 0x07, 0x00, 0x00, 0xe5, 0xe4},
        {692, 0x16, 0x00, 0xc0, 0x43, 0x87, 0x00, 0x87, 0x00, 0x87, 0x00, 0xbc, 0x1d, 0x00, 0x00, 0xf9, 0x15, 0x00, 0x00, 0x36, 0xe7},
        {685, 0x1a, 0x00, 0x40, 0x23, 0x85, 0x00, 0x85, 0x00, 0x85, 0x00, 0xe3, 0x05, 0x00, 0x00, 0x65, 0x04, 0x00, 0x00, 0x93, 0xe9},
        {678, 0x15, 0x00, 0x98, 0x22, 0x85, 0x00, 0x85, 0x00, 0x85, 0x00, 0xb6, 0x09, 0x00, 0x00, 0x53, 0x07, 0x00, 0x00, 0xfc, 0xeb},
        {671, 0x1a, 0x00, 0xbc, 0x23, 0x86, 0x00, 0x86, 0x00, 0x86, 0x00, 0xd5, 0x1c, 0x00, 0x00, 0xf9, 0x15, 0x00, 0x00, 0x73, 0xee}
};

FSVSRegdata FSVSPAL1024x768[5]= {
   
        {897, 0x20, 0x00, 0x50, 0x23, 0x05, 0x00, 0x85, 0x00, 0x82, 0x00, 0x2d, 0x08, 0x00, 0x00, 0x53, 0x07, 0x00, 0x00, 0x5f, 0xb2},
        {890, 0x1e, 0x00, 0x14, 0x23, 0x05, 0x00, 0x85, 0x00, 0x82, 0x00, 0xde, 0x04, 0x00, 0x00, 0x65, 0x04, 0x00, 0x00, 0xc6, 0xb3},
        {883, 0x1a, 0x00, 0xa4, 0x22, 0x05, 0x00, 0x85, 0x00, 0x82, 0x00, 0x25, 0x18, 0x00, 0x00, 0xf9, 0x15, 0x00, 0x00, 0x33, 0xb5},
        {876, 0x24, 0x00, 0xa4, 0x33, 0x05, 0x00, 0x85, 0x00, 0x82, 0x00, 0xfc, 0x07, 0x00, 0x00, 0x53, 0x07, 0x00, 0x00, 0xa5, 0xb6},
        {869, 0x16, 0x00, 0xf8, 0x22, 0x07, 0x00, 0x87, 0x00, 0x83, 0x00, 0xc3, 0x17, 0x00, 0x00, 0xf9, 0x15, 0x00, 0x00, 0x1e, 0xb8}
};

FSVSRegdata FSVSNTSC720x480[5]= {
   
        {565, 0x1c, 0x00, 0x4c, 0x23, 0x06, 0x00, 0x86, 0x00, 0x86, 0x00, 0x30, 0x15, 0x00, 0x00, 0x8d, 0x13, 0x00, 0x00, 0xe0, 0xed},
        {560, 0x14, 0x00, 0x08, 0x33, 0x08, 0x00, 0x88, 0x00, 0x88, 0x00, 0x00, 0x03, 0x00, 0x00, 0xcb, 0x02, 0x00, 0x00, 0x00, 0xf0},
        {555, 0x1e, 0x00, 0xed, 0x21, 0x03, 0x00, 0x83, 0x00, 0x83, 0x00, 0xd0, 0x14, 0x00, 0x00, 0x8d, 0x13, 0x00, 0x00, 0x29, 0xf2},
        {550, 0x19, 0x00, 0xdc, 0x22, 0x06, 0x00, 0x06, 0x00, 0x86, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x5d, 0xf4},
        {545, 0x15, 0x00, 0x4d, 0x21, 0x03, 0x00, 0x83, 0x00, 0x83, 0x00, 0x70, 0x14, 0x00, 0x00, 0x8d, 0x13, 0x00, 0x00, 0x9b, 0xf6}
};

FSVSRegdata FSVSPAL720x576[5]= {
   
        {672, 0x1e, 0x00, 0xfc, 0x22, 0x05, 0x00, 0x85, 0x00, 0x85, 0x00, 0xa0, 0x02, 0x00, 0x00, 0x71, 0x02, 0x00, 0x00, 0x18, 0xee},
        {666, 0x15, 0x00, 0x28, 0x33, 0x08, 0x00, 0x88, 0x00, 0x88, 0x00, 0x9a, 0x02, 0x00, 0x00, 0x71, 0x02, 0x00, 0x00, 0x3d, 0xf0},
        {660, 0x18, 0x00, 0xed, 0x21, 0x04, 0x00, 0x04, 0x00, 0x84, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x6c, 0xf2},
        {654, 0x17, 0x00, 0x64, 0x33, 0x08, 0x00, 0x88, 0x00, 0x88, 0x00, 0x8e, 0x02, 0x00, 0x00, 0x71, 0x02, 0x00, 0x00, 0xa5, 0xf4},
        {648, 0x1e, 0x00, 0x60, 0x23, 0x06, 0x00, 0x86, 0x00, 0x86, 0x00, 0x88, 0x02, 0x00, 0x00, 0x71, 0x02, 0x00, 0x00, 0xe9, 0xf6}
};

FSVScaler FocusVScaleringTable[TVVESAModeCnt]={
        {NTSC,  640, 480, FSVSNTSC640x480},
        {NTSC,  720, 480, FSVSNTSC720x480},
        {NTSC,  800, 600, FSVSNTSC800x600},
        {NTSC, 1024, 768, FSVSNTSC1024x768},
        {PAL ,  640, 480, FSVSPAL640x480},
        {PAL ,  720, 576, FSVSPAL720x576},
        {PAL ,  800, 600, FSVSPAL800x600},
        {PAL , 1024, 768, FSVSPAL1024x768}
};

FSPLLPatch FocusPLLPatch[TMDSSupportModeCnt]={
        { PLL25175, 0x26, 0x00, 0x1c, 0x56, 0x8a, 0x00, 0x0a, 0x00, 0x8a, 0x00},
        { PLL40000, 0x17, 0x00, 0x85, 0x21, 0x82, 0x00, 0x82, 0x00, 0x82, 0x00},
        { PLL65000, 0x14, 0x00, 0x71, 0x21, 0x83, 0x00, 0x83, 0x00, 0x81, 0x00},
        { PLL74250, 0x1C, 0x00, 0x98, 0x23, 0x85, 0x00, 0x85, 0x00, 0x82, 0x00},
        {PLL148500, 0x14, 0x00, 0x78, 0x43, 0x87, 0x00, 0x07, 0x00, 0x81, 0x00}
};

SAAPLLPatch Saa7105PLLPatch[TMDSSupportModeCnt]={
        { PLL25175, 0xDE, 0xDD, 0x1D, 0xA5},
        { PLL40000, 0x9B, 0x28, 0x2F, 0xA5},
        { PLL65000, 0x39, 0x08, 0x4D, 0xA5},
        { PLL74250, 0x01, 0x00, 0x58, 0xA5},
        {PLL148500, 0x01, 0x00, 0x58, 0xA0}
};

