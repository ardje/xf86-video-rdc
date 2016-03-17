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
 

#include "xf86.h"

// VIDEO Register definition VDISP
#define VDP_CTL                         0x700
#define VDP_REG_UPDATE_MOD_SELECT       0x704
#define VDP_FIFO_THRESHOLD              0x708
#define VDP_FETCH_COUNT                 0x70C
#define VDP_START_OFFSET                0x710
#define VDP_SRC_DISP_COUNT_ON_SCR       0x714
#define VDP_START_LOCATION              0x718
#define VDP_END_LOCATION                0x71C
#define VDP_FIRST_COLORKEY              0x720
#define VDP_CHROMA_KEY_LOW              0x724
#define VDP_CHROMA_KEY_UPPER            0x728
#define VDP_ZOOM_CTL                    0x72C
#define VDP_FB0                         0x730
#define VDP_FB1                         0x734
#define VDP_FB2                         0x738
#define VDP_FB_STRIDE                   0x73C
#define VDP_COLOR_ENHANCE_CTL1          0x740
#define VDP_COLOR_ENHANCE_CTL2          0x744
#define VDP_COLOR_ENHANCE_CTL3          0x748
#define VDP_COLOR_ENHANCE_CTL4          0x74C
#define VDP_SECOND_COLORKEY             0x750

#define VID_Enable                      1
#define VID_Disable                     0

// VIDEO states
#define VDPS_ENABLE                     BIT0
#define VDPS_SW_FLIP                    0x00010000
#define VDPS_HW_FLIP                    0
#define VDPS_DISP_REQ_EXPIRE            0x00000140
#define VDPS_DISP_PRE_FETCH             BIT20
#define VDPS_DISP1                      0x0
#define VDPS_DISP2                      BIT18
#define VDPS_FIRE                       BIT7
#define VDPS_FIFO_THRESHOLD             0x60006000
#define VDPS_ENABLE_CK                  BIT30
#define VDPS_ENABLE_CHK                 BIT31
#define VDPS_H_ZOOM_ENABLE              BIT31
#define VDPS_H_INTERP                   BIT30
#define VDPS_V_ZOOM_ENABLE              BIT15
#define VDPS_V_INTERP                   BIT14

typedef struct {
     
     ULONG dwVidCtl;                               
     ULONG dwVideoSrcOffset;                             
     ULONG dwFetch;                  
     ULONG dwVDisplayCount;          
     ULONG dwVSrcPitch;                 
     ULONG dwWinStartXY;
     ULONG dwWinEndXY;
     ULONG dwzoomCtl;
     ULONG dwColorKey;
     ULONG dwVIDBuffer[3];
} VIDHWINFO, *VIDHWINFOPtr;


typedef struct _OV_RECTL
{
    unsigned long     left;
    unsigned long     top;
    unsigned long     right;
    unsigned long     bottom;
} OV_RECTL, *OV_RECTLPtr;



typedef struct {
    long            brightness;
    long            saturation;
    long            contrast;
    long            hue;
    RegionRec       clip;
    CARD32          colorkey;

    VIDHWINFO       VidhwInfo;
	FBLinearPtr     PackedBuf0;
	unsigned long   Packed0Offset;
	
    OV_RECTL        rDst;
    OV_RECTL        rSrc;
    unsigned long   SrcPitch;
} RDCPortPrivRec, *RDCPortPrivPtr;

