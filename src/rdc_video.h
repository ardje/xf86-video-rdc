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


#include "xf86.h"


#define MMIOBASE_VIDEO                  0x600
#define VP_VIDEO_SIZE                   0x620
#define VP_RO_REG                       0x624
#define VP_PARAMETERS_QUALITY           0x628
#define VP_COLOR_ENHANCE_CTL1           0x630
#define VP_COLOR_ENHANCE_CTL2           0x634
#define VP_COLOR_ENHANCE_CTL3           0x638
#define VP_COLOR_ENHANCE_CTL4           0x63C
#define VP_H_SCALE                      0x640
#define VP_V_SCALE                      0x644
#define VP_CSC_CTL                      0x648
#define VP_PULL_DOWN                    0x64C
#define VP_SUB_PIC_STRIDE_CONTROL       0x650
#define VP_SUB_PIC_START                0x654
#define VP_SUB_PIC_4X16_RAM_TABLE       0x658
#define VP_STREAM_CTL                   0x660
#define VP_SW_SRC_BUF_LUMA_START        0x664
#define VP_SW_SRC_BUF_CHROMA_START      0x668
#define VP_SRC_BUF_STRIDE               0x66C
#define VP_DEST_FB0                     0x670
#define VP_DEST_FB1                     0x674
#define VP_DEST_FB2                     0x678
#define VP_DEST_STRIDE                  0x67C
#define VP_YV12_Y_Base                  0x664
#define VP_YV12_U_Base                  0x668
#define VP_YV12_V_Base                  0x680
#define VP_ROTATION_CTL                 0x684


#define VP_VIDEO_SIZE_I                 0x08000000
#define VP_RO_REG_I                     0x09000000
#define VP_PARAMETERS_QUALITY_I         0x0A000000
#define VP_COLOR_ENHANCE_CTL1_I         0x0C000000
#define VP_COLOR_ENHANCE_CTL2_I         0x0D000000
#define VP_COLOR_ENHANCE_CTL3_I         0x0E000000
#define VP_COLOR_ENHANCE_CTL4_I         0x0F000000
#define VP_H_SCALE_I                    0x10000000
#define VP_V_SCALE_I                    0x11000000
#define VP_CSC_CTL_I                    0x12000000
#define VP_PULL_DOWN_I                  0x13000000
#define VP_SUB_PIC_STRIDE_CONTROL_I     0x14000000
#define VP_SUB_PIC_START_I              0x15000000
#define VP_SUB_PIC_4X16_RAM_TABLE_I     0x16000000
#define VP_STREAM_CTL_I                 0x18000000
#define VP_SW_SRC_BUF_LUMA_START_I      0x19000000
#define VP_SW_SRC_BUF_CHROMA_START_I    0x1A000000
#define VP_SRC_BUF_STRIDE_I             0x1B000000
#define VP_DEST_FB0_I                   0x1C000000
#define VP_DEST_FB1_I                   0x1D000000
#define VP_DEST_FB2_I                   0x1E000000
#define VP_DEST_STRIDE_I                0x1F000000
#define VP_YV12_V_BASE_I                0x20000000
#define VP_ROTATION_CTL_I               0x21000000


#define VPS_STREAM_YUY2                 0
#define VPS_STREAM_NV12                 0x10000000
#define VPS_STREAM_YV12                 0x20000000
#define VPS_TRIPLE_BUFFER               BIT26
#define VPS_DOUBLE_BUFFER               0
#define VPS_SINGLE_BUFFER               BIT6
#define VPS_SRC_CAPTURE                 0x02000000
#define VPS_SRC_SW                      0
#define VPS_WAVE_MODE                   0
#define VPS_BOB_MODE                    BIT23
#define VPS_SRC_CHROMINANCE_FRAME       0
#define VPS_SRC_CHROMINANCE_FIELD       BIT20
#define VPS_INVERSE_INPUT_FIELD         BIT19
#define VPS_FRAME_TO_FIELD              BIT18
#define VPS_FIELD_TO_FRAME              BIT17
#define VPS_SUB_PIC_FLIP                BIT15
#define VPS_INT_ENABLE                  BIT7
#define VPS_SRC_INPUT_TOP               0
#define VPS_SRC_INPUT_BOTTOM            BIT5
#define VPS_SRC_SW_FLIP                 BIT4
#define VPS_ENABLE                      BIT27
#define VPS_END_FRAME_STATUS            BIT0
#define VPS_SCALE_ENABLE                BIT31
#define VPS_SCALE_DOWN                  BIT30
#define VPS_ENABLE_PULL_DOWN            BIT15
#define VPS_FLIP_CTL_10                 BIT17   
#define VPS_SRC_UV_FIELD_IN_PLANAR      BIT20
#define VPS_SUB_PIC_ENABLE              BIT16
#define VPS_CSC_ENABLE                  BIT30
#define VPS_RGB16_OUTPUT                0
#define VPS_RGB32_OUTPUT                BIT31
#define VPS_ROTATION_ENABLE             BIT31
#define VPS_IDEL_RO_REG                 BIT0
#define VPS_COLOR_HNHANCE               BIT28


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


#define VDP_CTL_I                    0x40000000
#define VDP_REG_UPDATE_MOD_SELECT_I  0x41000000
#define VDP_FIFO_THRESHOLD_I         0x42000000
#define VDP_FETCH_COUNT_I            0x43000000
#define VDP_START_OFFSET_I           0x44000000
#define VDP_SRC_DISP_COUNT_ON_SCR_I  0x45000000
#define VDP_START_LOCATION_I         0x46000000
#define VDP_END_LOCATION_I           0x47000000
#define VDP_FIRST_COLORKEY_I         0x48000000
#define VDP_CHROMA_KEY_LOW_I         0x49000000
#define VDP_CHROMA_KEY_UPPER_I       0x4A000000
#define VDP_ZOOM_CTL_I               0x4B000000
#define VDP_FB0_I                    0x4C000000
#define VDP_FB1_I                    0x4D000000
#define VDP_FB2_I                    0x4E000000
#define VDP_FB_STRIDE_I              0x4F000000
#define VDP_COLOR_ENHANCE_CTL1_I     0x50000000
#define VDP_COLOR_ENHANCE_CTL2_I     0x51000000
#define VDP_COLOR_ENHANCE_CTL3_I     0x52000000
#define VDP_COLOR_ENHANCE_CTL4_I     0x53000000
#define VDP_SECOND_COLORKEY_I        0x54000000


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
#define VDPS_STREAM_RGB16               0x00000004
#define VDPS_CSC_DISENABLE              BIT1
#define VDPS_COLOR_HNHANCE              BIT17

typedef struct {
     
     ULONG          dwVidCtl;                               
     ULONG          dwVideoSrcOffset;                             
     ULONG          dwFetch;                  
     ULONG          dwVDisplayCount;          
     ULONG          dwVSrcPitch;                 
     ULONG          dwWinStartXY;
     ULONG          dwWinEndXY;
     ULONG          dwzoomCtl;
     ULONG          dwColorKey;
     ULONG          dwVIDBuffer[3];
} VIDHWINFO, *VIDHWINFOPtr;


typedef struct {
    
    ULONG           ulVP_VideoSize;
    ULONG           ulVP_Parameters;
    ULONG           ulVP_ColorEnhance[3];
    ULONG           ulVP_HScale;
    ULONG           ulVP_VScale;
    ULONG           ulVP_CSCCtrl;
    ULONG           ulVP_PullDown;
    ULONG           ulVP_SubPic_Stride;
    ULONG           ulVP_SubPic_Start;
    ULONG           ulVP_SubPic_Table;
    ULONG           ulVP_StreamCtrl;
    ULONG           ulVP_SrcBuf[3];     
    ULONG           ulVP_SrcLumaStride;
    FBLinearPtr     pVPDestBuffer[3];
    ULONG           ulVP_DestBuf_Offset[3];
    ULONG           ulVP_Dest_SrcChroma_Stride;
    ULONG           ulVP_RotateCtrl;
} VPOSTHWINFO, *VPOSTHWINFOPtr;


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
    ULONG          colorkey;

    
    
    VIDHWINFO       VidhwInfo;
    
    
    VPOSTHWINFO     VPosthwInfo;
    
    
    
	FBLinearPtr     PackedBuf[3];     
	unsigned long   PackedBufOffset[3];
	unsigned long   PackedBufStride[3]; 

    
    unsigned long   ulVPOSTVideoSrcHorSize;
    unsigned long   ulVPOSTVideoSrcVerSize;
    unsigned long   ulVPOSTVideoDstHorSize;
    unsigned long   ulVPOSTVideoDstVerSize;

    
	int             YUVFormat;
    unsigned long   ulOverlaySrcHorSize;
    unsigned long   ulOverlaySrcVerSize;
    unsigned long   ulOverlayDstHorSize;
    unsigned long   ulOverlayDstVerSize;
    OV_RECTL        rDst;
    OV_RECTL        rSrc;
    
    
    unsigned long   SrcPitch; 
} RDCPortPrivRec, *RDCPortPrivPtr;


#define VP_SRC_MAX_WIDTH            1920



#define PATCH_YV12_WIDTH            784  

