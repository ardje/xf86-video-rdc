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
 


#ifdef FPGA
#define MAX_HResolution     1024
#define MAX_VResolution     768
#else
#define MAX_HResolution     1920
#define MAX_VResolution     1200
#endif
 

#define TextModeIndex       0
#define EGAModeIndex        1
#define VGAModeIndex        2
#define HiCModeIndex        3
#define TrueCModeIndex      4


#define VCLK25_175          0x00
#define VCLK28_322          0x01
#define VCLK31_5            0x02
#define VCLK36              0x03
#define VCLK40              0x04
#define VCLK49_5            0x05
#define VCLK50              0x06
#define VCLK56_25           0x07
#define VCLK65              0x08
#define VCLK75              0x09
#define VCLK78_75           0x0A
#define VCLK94_5            0x0B
#define VCLK108             0x0C
#define VCLK135             0x0D
#define VCLK157_5           0x0E
#define VCLK162             0x0F

#define VCLK154             0x10


#define Charx8Dot           0x00000001
#define HalfDCLK            0x00000002
#define DoubleScanMode      0x00000004
#define LineCompareOff      0x00000008
#define SyncPP              0x00000000
#define SyncPN              0x00000040
#define SyncNP              0x00000080
#define SyncNN              0x000000C0
#define HBorder             0x00000020
#define VBorder             0x00000010


#define DAC_NUM_TEXT        64
#define DAC_NUM_EGA         64
#define DAC_NUM_VGA         256


#define LCD_TIMING          0x00010000


typedef struct {
    UCHAR   MISC;
    UCHAR   SEQ[4];
    UCHAR   CRTC[25];
    UCHAR   AR[20];
    UCHAR   GR[9];
    
} VBIOS_STDTABLE_STRUCT, *PVBIOS_STDTABLE_STRUCT;

typedef struct {
    UCHAR   ucNS;
    UCHAR   ucMS;
    UCHAR   ucRS;
} VBIOS_DCLK_INFO, *PVBIOS_DCLK_INFO;

typedef struct _RRateInfo{
    float fRefreshRate;
    Bool  BInterlaced;
    UCHAR ucRRateIndex;
}RRateInfo;

typedef struct _MODE_PRIVATE
{
    USHORT  Mode_ID_8bpp;
    USHORT  Mode_ID_16bpp;
    USHORT  Mode_ID_32bpp;
    UCHAR   ucRRate_ID;
} MODE_PRIVATE;


