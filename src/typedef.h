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
 


#define X_INFO 0
#define X_WARNING 1
#define X_ERROR 2


typedef unsigned char UCHAR;
typedef unsigned short USHORT;
typedef unsigned long ULONG;

typedef enum _bool
{
    FALSE = 0,
    TRUE = 1
}bool;

#ifndef ErrorLevel
    #define ErrorLevel             0    
#endif

#ifndef DefaultLevel
    #define DefaultLevel           4    
#endif

#ifndef InfoLevel
    #define InfoLevel              5    
#endif

#ifndef InternalLevel
    #define InternalLevel          6    
#endif


#define ATTR_ADDR                   0X0040
#define ATTR_DATA_WRITE             0X0040
#define ATTR_DATA_READ              0X0041
#define MISC_WRITE                  0X0042
#define INPUT_STATUS_0_READ         0X0042
#define RIO_VGA_ENABLE              0X0043
#define SEQ_INDEX                   0X0044
#define SEQ_DATA                    0X0045
#define PEL_MASK                    0X0046
#define DAC_INDEX_READ              0X0047
#define FEAT_CTRL_READ              0X004A
#define MISC_READ                   0X004C
#define GRAPH_INDEX                 0X004E
#define GRAPH_DATA                  0X004F

#define MONO_CRTC_INDEX             0X0034
#define MONO_CRTC_DATA              0X0035
#define MONO_INPUT_STATUS1_READ     0X003A

#define COLOR_CRTC_INDEX            0X0054
#define COLOR_CRTC_DATA             0X0055
#define COLOR_INPUT_STATUS1_READ    0X005A


#define VBIOSVerNum                 0x00000




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
#define     IDX_TV1_CONNECTION_TYPE              SCRATCH_05 + 0x30

#define SCRATCH_06                               0x86 * 0x100
#define     IDX_SCRATCH_06                       SCRATCH_06 + 0xFF
#define     IDX_TV2_TYPE                         SCRATCH_06 + 0x0F
#define     IDX_TV2_CONNECTION_TYPE              SCRATCH_06 + 0x30

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

#define SCRATCH_17                               0x91 * 0x100
#define     IDX_SCRATCH_17                       SCRATCH_17 + 0xFF
#define     IDX_LCD2_H_SIZE_OVERFLOW             SCRATCH_10 + 0xF0
#define     IDX_LCD2_V_SIZE_OVERFLOW             SCRATCH_10 + 0x0F

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


#define CRT_PORT    0
#define DVP1        1
#define DVP2        2
#define DVP12       3


#define RR60        0
#define RR50I       1
#define RR59_94I    2
#define RR50        3
#define RR56        4
#define RR59_94     5
#define RR24        6
#define RR70        7
#define RR75        8
#define RR80        9
#define RR85        10
#define RR90        11
#define RR100       12
#define RR120       13
#define RR72        14
#define RR65        15


#define LCD_ID      1
#define DVI_ID      2
#define CRT_ID      3
#define HDMI_ID     4
#define HDTV_ID     5
#define TV_ID       6
#define LCD2_ID     8
#define DVI2_ID     9
#define CRT2_ID     10
#define HDMI2_ID    11
#define HDTV2_ID    12
#define TV2_ID      13


#define B_LCD       BIT0
#define B_DVI       BIT1
#define B_CRT       BIT2
#define B_HDMI      BIT3
#define B_HDTV      BIT4
#define B_TV        BIT5
#define B_LCD2      BIT7
#define B_DVI2      BIT8
#define B_CRT2      BIT9
#define B_HDMI2     BIT10
#define B_HDTV2     BIT11
#define B_TV2       BIT12




#define PLLReferenceClock           14318
#define MaxFCKVCO9003A              810000
#define MinFCKVCO9003A              350000
#define MaxFCKVCO9001A              1600000
#define MinFCKVCO9001A              833000


#define DPMS__ON                     0
#define DPMS__STANDBY                1
#define DPMS__SUSPEND                2
#define DPMS__OFF                    3



#define SEQ         BIT1                
#define TTL         BIT6                


#define NONE        0x00        
#define TX_VT1636   0x01
#define TX_SIL1162  0x02
#define TX_HW       0x03        
#define ENC_VT1625  0x04        
#define DSTN        0x05        
#define ENC_SAA7105 0x06        


#define Dev_SUPPORT BIT0        
#define TX_DIT      BIT1
#define TX_PS       BIT2
#define DSTN_COLOR  BIT3        
#define DSTN_MONO   0           
#define DSTN_6X4    BIT4        
#define DSTN_3X2    0           
#define TX_MSB      BIT5


#define DISP1       1
#define DISP2       2


#define NTSC                 0
#define PAL                  1
#define CVBS                 1
#define SVideo               2



#define MODE_640x480x8      0x101
#define MODE_800x600x8      0x103
#define MODE_1024x768x8     0x105
#define MODE_1280x1024x8    0x107
#define MODE_640x480x16     0x111
#define MODE_640x480x32     0x112
#define MODE_800x600x16     0x114
#define MODE_800x600x32     0x115
#define MODE_1024x768x16    0x117
#define MODE_1024x768x32    0x118
#define MODE_1280x1024x16   0x11A
#define MODE_1280x1024x32   0x11B

#define MODE_1600x1200x8    0x120
#define MODE_1600x1200x16   0x121
#define MODE_1600x1200x32   0x122

#define MODE_320x240x8      0x123
#define MODE_320x240x16     0x124
#define MODE_320x240x32     0x125

#define MODE_720x480x8      0x126
#define MODE_720x480x16     0x127
#define MODE_720x480x32     0x128

#define MODE_1360x768x8     0x129
#define MODE_1360x768x16    0x12A
#define MODE_1360x768x32    0x12B

#define MODE_848x480x8      0x12C
#define MODE_848x480x16     0x12D
#define MODE_848x480x32     0x12E

#define MODE_1440x900x8     0x12F
#define MODE_1440x900x16    0x130
#define MODE_1440x900x32    0x131

#define MODE_1152x864x8     0x132
#define MODE_1152x864x16    0x133
#define MODE_1152x864x32    0x134

#define MODE_1280x960x8     0x135
#define MODE_1280x960x16    0x136
#define MODE_1280x960x32    0x137

#define MODE_1400x1050x8    0x138
#define MODE_1400x1050x16   0x139
#define MODE_1400x1050x32   0x13A

#define MODE_800x480x8      0x13B
#define MODE_800x480x16     0x13C
#define MODE_800x480x32     0x13D

#define MODE_1024x600x8     0x13E
#define MODE_1024x600x16    0x13F
#define MODE_1024x600x32    0x140

#define MODE_USR_DEF1x8     0x1F1
#define MODE_USR_DEF1x16    0x1F2
#define MODE_USR_DEF1x32    0x1F3

#define MODE_USR_DEF2x8     0x1F4
#define MODE_USR_DEF2x16    0x1F5
#define MODE_USR_DEF2x32    0x1F6


typedef struct _RRATE_TABLE RRATE_TABLE;
typedef struct _MODE_INFO MODE_INFO;
typedef struct _PANEL_TABLE PANEL_TABLE;
typedef struct _PORT_CONFIG PORT_CONFIG;
typedef struct _VESA_TABLE VESA_TABLE;

struct _RRATE_TABLE {
    ULONG   Clock;          
    UCHAR   RRate_ID;
    USHORT  Attribute;
    USHORT  H_Blank_Time;
    USHORT  H_Sync_Start;
    USHORT  H_Sync_Time;
    USHORT  V_Blank_Time;
    USHORT  V_Sync_Start;
    USHORT  V_Sync_Time;
}__attribute__((packed));;

struct _MODE_INFO{
    USHORT  H_Size;
    USHORT  V_Size;
    USHORT  Mode_ID_8bpp;
    USHORT  Mode_ID_16bpp;
    USHORT  Mode_ID_32bpp;
    UCHAR   RRTableCount;
}__attribute__((packed));;

struct _PANEL_TABLE {
    USHORT    TD0;
    USHORT    TD1;
    USHORT    TD2;
    USHORT    TD3;
    USHORT    TD5;
    USHORT    TD6;
    USHORT    TD7;
    UCHAR   PWM_Clock;
    USHORT  Reserved1;
    USHORT  Reserved2;
    RRATE_TABLE Timing;
}__attribute__((packed));;

struct _VESA_TABLE {
    MODE_INFO   ModeInfo;
    RRATE_TABLE RRateTable;
}__attribute__((packed));;

#if 0
typedef struct _LCD_TABLE {
    MODE_INFO   ModeInfo;
    PANEL_TABLE *pPanelTable;
} LCD_TABLE;
#endif

typedef struct _REG_OP {
    UCHAR    RegGroup;
    UCHAR    RegMask;
    UCHAR    RegIndex;
    UCHAR    RegShiftBit;
} REG_OP;

#if 0
typedef struct _REG_PACKAGE {
    UCHAR    RegGroup;
    UCHAR    RegIndex;
    UCHAR    RegValue;
    UCHAR    RegMask;
} REG_PACKAGE;
#else

#define    CR       0x00
#define    SR       0x01
#define    GR       0x02
#define    I2C      0x03
#define    NR       0xFF
#define    BITS     BIT7
#endif


typedef struct _PLL_Info {
    UCHAR NS;
    UCHAR MS;
    UCHAR RS;
}PLL_Info;

struct _PORT_CONFIG
{
    UCHAR        DevID;
    UCHAR        PortID;
    UCHAR        TX_Enc_ID;
    UCHAR        TX_I2C_Port;
    UCHAR        TX_I2C_Addr;
    UCHAR        Attribute;
}__attribute__((packed));;


#define OFF_VID             0
#define OFF_DID             2
#define OFF_PTR_VPD         4
#define OFF_PCI_DS_LEN      6
#define OFF_PCI_DS_REV      8
#define OFF_CLASS_CODE1     9
#define OFF_CLASS_CODE2     10
#define OFF_CLASS_CODE3     11
#define OFF_SIZE_VBIOS      12
#define OFF_ROM_CODE_REV    14
#define OFF_PC_AT_COMP      16
#define OFF_LII             17
#define OFF_RESERVED        18




