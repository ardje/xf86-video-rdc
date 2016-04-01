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
#ifndef __BIOSDEF_H__
#define __BIOSDEF_H__



typedef unsigned char   CBStatus;

typedef unsigned char   UCHAR;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef char*           PCHAR;
typedef unsigned short  USHORT;
typedef long            LONG;
typedef unsigned long   ULONG;
typedef unsigned long long   uint64;



#define FALSE                       0
#define TRUE                        1


#define BIT0                        0x00000001
#define BIT1                        0x00000002
#define BIT2                        0x00000004
#define BIT3                        0x00000008
#define BIT4                        0x00000010
#define BIT5                        0x00000020
#define BIT6                        0x00000040
#define BIT7                        0x00000080
#define BIT8                        0x00000100
#define BIT9                        0x00000200
#define BIT10                       0x00000400
#define BIT11                       0x00000800
#define BIT12                       0x00001000
#define BIT13                       0x00002000
#define BIT14                       0x00004000
#define BIT15                       0x00008000
#define BIT16                       0x00010000
#define BIT17                       0x00020000
#define BIT18                       0x00040000
#define BIT19                       0x00080000
#define BIT20                       0x00100000
#define BIT21                       0x00200000
#define BIT22                       0x00400000
#define BIT23                       0x00800000
#define BIT24                       0x01000000
#define BIT25                       0x02000000
#define BIT26                       0x04000000
#define BIT27                       0x08000000
#define BIT28                       0x10000000
#define BIT29                       0x20000000
#define BIT30                       0x40000000
#define BIT31                       0x80000000


#define CRT_PORT                    0
#define DVP1                        1
#define DVP2                        2
#define DVP12                       3


#define RR60                        0
#define RR50I                       1
#define RR59_94I                    2
#define RR50                        3
#define RR56                        4
#define RR59_94                     5
#define RR24                        6
#define RR70                        7
#define RR75                        8
#define RR80                        9
#define RR85                        10
#define RR90                        11
#define RR100                       12
#define RR120                       13
#define RR72                        14
#define RR65                        15


#define NoneDevice                  0
#define LCDIndex                    1
#define DVIIndex                    2
#define CRTIndex                    3
#define HDMIIndex                   4
#define HDTVIndex                   5
#define TVIndex                     6
#define LCD2Index                   8
#define DVI2Index                   9
#define CRT2Index                   10
#define HDMI2Index                  11
#define HDTV2Index                  12
#define TV2Index                    13
#define DSTNIndex                   14


#define B_LCD                       BIT0
#define B_DVI                       BIT1
#define B_CRT                       BIT2
#define B_HDMI                      BIT3
#define B_HDTV                      BIT4
#define B_TV                        BIT5
#define B_LCD2                      BIT7
#define B_DVI2                      BIT8
#define B_CRT2                      BIT9
#define B_HDMI2                     BIT10
#define B_HDTV2                     BIT11
#define B_TV2                       BIT12
#define B_DSTN                      BIT13


#define ATTR_ADDR                   0x3C0
#define ATTR_DATA_WRITE             0x3C0
#define ATTR_DATA_READ              0x3C1
#define MISC_WRITE                  0x3C2
#define INPUT_STATUS_0_READ         0x3C2
#define VGA_ENABLE                  0x3C3
#define SEQ_INDEX                   0x3C4
#define SEQ_DATA                    0x3C5
#define PEL_MASK                    0x3C6
#define DAC_INDEX_READ              0x3C7
#define DAC_INDEX_WRITE             0x3C8
#define DAC_DATA                    0x3C9
#define FEAT_CTRL_READ              0x3CA
#define MISC_READ                   0x3CC
#define GRAPH_INDEX                 0x3CE
#define GRAPH_DATA                  0x3CF
#define MONO_CRTC_INDEX             0x3B4
#define MONO_CRTC_DATA              0x3B5
#define MONO_INPUT_STATUS1_READ     0x3BA
#define COLOR_CRTC_INDEX            0x3D4
#define COLOR_CRTC_DATA             0x3D5
#define COLOR_INPUT_STATUS1_READ    0x3DA


#define RDC_DRIVER_VERSION          0x00000006
#define RDC_VENDOR_ID               0x17F3
#define M2010_DEVICE_ID             0x2010
#define M2011_DEVICE_ID             0x2011
#define M2012_DEVICE_ID             0x2012 
#define M2013_DEVICE_ID             0x2013 
#define M2014_DEVICE_ID             0x2014 
#define M2200_DEVICE_ID             0x2200 
#define M2010_A0_VENDOR_ID          0x25F0
#define M2010_A0_DEVICE_ID          0x17F3
#define FPGACHIP                    0x1
#define FPGACHECKLOCATION           0x73
#define FPGAVGABIOS                 0x80


#define SCRATCH_01                               0x81 * 0x100
#define     IDX_SCRATCH_01                       SCRATCH_01 + 0xFF
#define     IDX_IGA1_DEV_ID                      SCRATCH_01 + 0x0F
#define     IDX_IGA2_DEV_ID                      SCRATCH_01 + 0xF0

#define SCRATCH_02                               0x82 * 0x100
#define     IDX_SCRATCH_02                       SCRATCH_02 + 0xFF
#define     IDX_DISPLAY1_CHANGED                 SCRATCH_02 + 0x01
#define     IDX_DISPLAY2_CHANGED                 SCRATCH_02 + 0x02
#define     IDX_PWR_SEQ_ON                       SCRATCH_02 + 0x04

#define SCRATCH_03                               0x83 * 0x100
#define     IDX_SCRATCH_03                       SCRATCH_03 + 0xFF
#define     IDX_IGA2_VESA_MODE                   SCRATCH_03 + 0xFF

#define SCRATCH_04                               0x84 * 0x100
#define     IDX_SCRATCH_04                       SCRATCH_04 + 0xFF
#define     IDX_IGA2_VESA_MODE_OVERFLOW          SCRATCH_04 + 0x01
#define     IDX_IGA2_RRATE_ID                    SCRATCH_04 + 0xFE

#define SCRATCH_05                               0x85 * 0x100
#define     IDX_SCRATCH_05                       SCRATCH_05 + 0xFF
#define     IDX_TV1_TYPE                         SCRATCH_05 + 0x0F
#define     IDX_TV1_CONNECTION_TYPE              SCRATCH_05 + 0xF0

#define SCRATCH_06                               0x86 * 0x100
#define     IDX_SCRATCH_06                       SCRATCH_06 + 0xFF
#define     IDX_TV2_TYPE                         SCRATCH_06 + 0x0F
#define     IDX_TV2_CONNECTION_TYPE              SCRATCH_06 + 0xF0

#define SCRATCH_07                               0x87 * 0x100
#define     IDX_SCRATCH_07                       SCRATCH_07 + 0xFF
#define     IDX_HDTV1_TYPE                       SCRATCH_07 + 0x07
#define     IDX_HDTV1_CONNECTION_TYPE            SCRATCH_07 + 0x08
#define     IDX_HDTV2_TYPE                       SCRATCH_07 + 0x70
#define     IDX_HDTV2_CONNECTION_TYPE            SCRATCH_07 + 0x80

#define SCRATCH_08                               0x88 * 0x100
#define     IDX_SCRATCH_08                       SCRATCH_08 + 0xFF
#define     IDX_HDMI1_TYPE                       SCRATCH_08 + 0x0F
#define     IDX_HDMI2_TYPE                       SCRATCH_08 + 0xF0

#define SCRATCH_09                               0x89 * 0x100
#define     IDX_SCRATCH_09                       SCRATCH_09 + 0xFF
#define     IDX_HDMI1_CONNECTION_TYPE            SCRATCH_09 + 0x03
#define     IDX_HDMI2_CONNECTION_TYPE            SCRATCH_09 + 0x30

#define SCRATCH_10                               0x8A * 0x100
#define     IDX_SCRATCH_10                       SCRATCH_10 + 0xFF
#define     IDX_LCD_H_SIZE_OVERFLOW              SCRATCH_10 + 0xF0
#define     IDX_LCD_V_SIZE_OVERFLOW              SCRATCH_10 + 0x0F


#define SCRATCH_11                               0x8B * 0x100
#define     IDX_SCRATCH_11                       SCRATCH_11 + 0xFF
#define     IDX_LCD_H_SIZE                       SCRATCH_11 + 0xFF

#define SCRATCH_12                               0x8C * 0x100
#define     IDX_SCRATCH_12                       SCRATCH_12 + 0xFF
#define     IDX_LCD_V_SIZE                       SCRATCH_12 + 0xFF

#define SCRATCH_13                               0x8D * 0x100
#define     IDX_SCRATCH_13                       SCRATCH_13 + 0xFF
#define     IDX_IGA1_VESA_MODE_OVERFLOW          SCRATCH_13 + 0x01
#define     IDX_IGA1_RRATE_ID                    SCRATCH_13 + 0xFE

#define SCRATCH_14                               0x8E * 0x100
#define     IDX_SCRATCH_14                       SCRATCH_14 + 0xFF
#define     IDX_IGA1_VGA_MODE                    SCRATCH_14 + 0xFF
#define     IDX_IGA1_VESA_MODE                   SCRATCH_14 + 0xFF

#define SCRATCH_15                               0x8F * 0x100
#define     IDX_SCRATCH_15                       SCRATCH_15 + 0xFF
#define     IDX_NEW_IGA1_DEV_ID                  SCRATCH_15 + 0x0F
#define     IDX_NEW_IGA2_DEV_ID                  SCRATCH_15 + 0xF0

#define SCRATCH_16                               0x90 * 0x100
#define     IDX_SCRATCH_16                       SCRATCH_16 + 0xFF
#define     IDX_KEEP_DISPLAY_OFF                 SCRATCH_16 + 0x80
#define     IDX_DPMS_STATE                       SCRATCH_16 + 0x40
#define     IDX_LINEAR_MODE                      SCRATCH_16 + 0x20
#define     IDX_DS_ENABLE                        SCRATCH_16 + 0x10

#define SCRATCH_17                               0x91 * 0x100
#define     IDX_SCRATCH_17                       SCRATCH_17 + 0xFF
#define     IDX_LCD2_H_SIZE_OVERFLOW             SCRATCH_17 + 0xF0
#define     IDX_LCD2_V_SIZE_OVERFLOW             SCRATCH_17 + 0x0F

#define SCRATCH_18                               0x92 * 0x100
#define     IDX_SCRATCH_18                       SCRATCH_18 + 0xFF
#define     IDX_LCD2_H_SIZE                      SCRATCH_18 + 0xFF

#define SCRATCH_19                               0x93 * 0x100
#define     IDX_SCRATCH_19                       SCRATCH_19 + 0xFF
#define     IDX_LCD2_V_SIZE                      SCRATCH_19 + 0xFF

#define SCRATCH_20                               0x94 * 0x100
#define     IDX_SCRATCH_20                       SCRATCH_20 + 0xFF
#define     IDX_LVDS1_TX_ID                      SCRATCH_20 + 0xF0
#define     IDX_LVDS2_TX_ID                      SCRATCH_20 + 0x0F

#define SCRATCH_21                               0x95 * 0x100
#define     IDX_SCRATCH_21                       SCRATCH_21 + 0xFF
#define     IDX_TMDS1_TX_ID                      SCRATCH_21 + 0xF0
#define     IDX_TMDS2_TX_ID                      SCRATCH_21 + 0x0F

#define SCRATCH_22                               0x96 * 0x100
#define     IDX_SCRATCH_22                       SCRATCH_22 + 0xFF
#define     IDX_TV1_ENCODER_ID                   SCRATCH_22 + 0xF0
#define     IDX_TV2_ENCODER_ID                   SCRATCH_22 + 0x0F

#define SCRATCH_23                               0x97 * 0x100
#define     IDX_SCRATCH_23                       SCRATCH_23 + 0xFF
#define     IDX_LCD1_TABLE_INDEX                 SCRATCH_23 + 0xF0
#define     IDX_LCD2_TABLE_INDEX                 SCRATCH_23 + 0x0F

#define SCRATCH_24                               0x98 * 0x100
#define     IDX_SCRATCH_23                       SCRATCH_23 + 0xFF
#define     IDX_TV1_Mode_TABLE_INDEX             SCRATCH_23 + 0xF0
#define     IDX_TV2_Mode_TABLE_INDEX             SCRATCH_23 + 0x0F

#define SCRATCH_25                               0x99 * 0x100
#define     IDX_SCRATCH_25                       SCRATCH_25 + 0xFF
#define     S25_Reserved                         SCRATCH_25 + 0xF0
#define     IDX_BOOT_DEV_ID                      SCRATCH_25 + 0x0F


#define PLLReferenceClock           14318
#define MaxFCKVCO9003A              810000
#define MinFCKVCO9003A              350000
#define MaxFCKVCO4002A              805000
#define MinFCKVCO4002A              350000
#define MaxFCKVCO9001A              1600000
#define MinFCKVCO9001A              833000


#define DeviceON                    0x0
#define DeviceStandBy               0x1
#define DeviceSuspend               0x2
#define DeviceOFF                   0x3


#define MonitorEDID                 0xA0
#define SIIDVITx                    0x70
#define LCDVT1636                   0x80


#define TX_None                     0x0   
#define TX_LCD_VT1636               0x1
#define TX_DVI_SII                  0x2
#define TX_HW                       0x3   
#define TX_TV                       0x4
#define TX_DSTN                     0x5
#define TMDS_ID_ITE6610             0x1
#define TMDS_ID_SII162              0x2
#define TMDS_ID_EP932M              0x3
#define TVEnc_SAA7105               0x6
#define TVEnc_FS473                 0x7


#define TMDS_Power                  0x8
#define TMDS_Power_Bit              BIT0


#define CBIOSI2C_ERROR              0
#define CBIOSI2C_OK                 1


#define QUERYCRT                    0x1
#define QUERYHDMI                   0x2


#define ROM_Offset_Dev_ID           0x40
#define ROM_Offset_Rev_Num          0x15
#define ROM_Offset_Build_Year       0x56
#define ROM_Offset_Build_Month      0x47
#define ROM_Offset_Build_Date       0x4A


#define DispOne                     0x0
#define DispTwo                     0x1


#define DISP1                       1
#define DISP2                       2


#define TV_1                        0x0
#define TV_2                        0x1
#define LCD_1                       0x0
#define LCD_2                       0x1
#define DVI_1                       0x0
#define DVI_2                       0x1
#define CRT_1                       0x0
#define CRT_2                       0x1


#define NTSC                        0
#define PAL                         1

#define CVBS                        1
#define SVideo                      2
#define Component                   3


#define TV_NTSC                     0x1
#define TV_PAL                      0x2
#define TV_SDTV480P                 0x3
#define TV_SDTV576P                 0x4


#define Reg_FS473HPos               0x0
#define Reg_FS473VPos               0x2
#define Reg_FS473HScaler            0xA
#define Reg_FS473HLineBuffer        0x4


#define DiffCCRS                    BIT0
#define DiffHPos                    BIT1
#define DiffVPos                    BIT2
#define DiffVScaler                 BIT3
#define DiffHScaler                 BIT4
#define DiffPloarity                BIT5


#define TVPanningH                  640
#define TVPanningV                  480


#define HDMI640                     0x0
#define HDMI720P                    0x1
#define HDMI1080I                   0x2
#define HDMI480P                    0x3
#define HDMI1080P                   0x4


#define HDMIRGB                     0x0
#define HDMIYCbCR422                0x1
#define HDMIYCbCr444                0x2


#define VBEFunction02               0x4F02 
#define VBEFunction06               0x4F06
#define OEMFunction                 0x4F14
#define VBEFunction15               0x4F15

#define QueryBiosInfo               0x0000 
#define QueryBiosCaps               0x0001
#define QueryExternalDeviceInfo     0x0100
#define QueryDisplayPathInfo        0x0200
#define QueryDeviceConnectStatus    0x0201
#define QuerySupportedMode          0x0202
#define QueryLCDPanelSizeMode       0x0203
#define QueryLCD2PanelSizeMode      0x0283
#define QueryTVConfiguration        0x0400
#define QueryTV2Configuration       0x0480
#define QueryHDTVConfiguration      0x0500
#define QueryHDTV2Configuration     0x0580
#define QueryHDMIConfiguration      0x0600
#define QueryHDMI2Configuration     0x0680
#define QueryHDMISupportMode        0x0601
#define SetActiveDisplayDevice      0x8200
#define SetVESAModeForDisplay2      0x8202
#define SetDevicePowerState         0x8203
#define SetDisplay2Pitch            0x8204
#define SetDisplay1RefreshRate      0x8301
#define SetDisplay2RefreshRate      0x8381
#define SetTVConfiguration          0x8400
#define SetTVFunction               0x8401
#define SetTV2Configuration         0x8480
#define SetHDMIType                 0x8600
#define SetHDMI2Type                0x8680
#define SetHDMIOutputSignal         0x8601
#define SetHDMI2OutputSignal        0x8681
#define SetVideoPOST                0xF100
#define SetEDIDInModeTable          0xF200


#define VBEFunctionCallSuccessful   0x004F
#define VBEFunctionCallFail         0x014F
#define VBEFunctionCallNotSupported 0x024F
#define VBEFunctionCallInvalid      0x034F



#define SR4A                        0x4A
    #define DrivingData             BIT0+BIT1

#define CRDA                        0xDA
#define CRDB                        0xDB
    #define GPIO1                   BIT3
    #define GPIO2                   BIT2

#endif 

