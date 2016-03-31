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



  




#include "BiosDef.h"

#define UT_FAIL                         0x0000
#define UT_SUCCESS                      0x0001
#define UT_MODESET                      0x0002
#define UT_INVALID                      0x1FFF
#define UT_NOT_IMPLEMENT                0xFFFF


#define UT_QUERY_SUPPORT_DEVICE         0x1001
#define UT_QUERY_CONNECT_DEVICE         0x1002
#define UT_SET_DEVICE_ACT               0x1003
#define UT_QUERY_PRIMARY_DEVICE         0x1004
#define UT_SET_PRIMARY_DEVICE           0x1005
#define UT_QUERY_ACT_DEVICE             0x1006

#define UT_SET_GAMMA                    0x1101
#define UT_QUERY_GAMMA                  0x1102
#define UT_SET_LCD_PWM                  0x1103
#define UT_QUERY_LCD_PWM                0x1104
#define UT_QUERY_LCD_SIZE               0X1105

#define UT_QUERY_HW_INFO                0x1201
#define UT_QUERY_BIOS_VERSION           0x1202
#define UT_QUERY_MEM_INFO               0x1203
#define UT_QUERY_CLK_SPEED              0x1204
#define UT_QUERY_SAMM                   0x1205

#define UT_SET_TV_INFO					0x1401
#define	UT_QUERY_TV_INFO				0x1402
#define	UT_SET_TV_CCRS_LEVEL			0x1403
#define	UT_QUERY_TV_CCRS_LEVEL			0x1404
#define UT_QUERY_TV_ENC_ID              0x1405
#define UT_QUERY_TV_H_POS               0x1406
#define UT_SET_TV_H_POS                 0x1407
#define UT_QUERY_TV_V_POS               0x1408
#define UT_SET_TV_V_POS                 0x1409
#define UT_QUERY_TV_H_SCALER            0x140A
#define UT_SET_TV_H_SCALER              0x140B
#define UT_SET_TV_REGISTRY              0x140C
#define UT_SET_TV_PANNING_ENABLE        0x140D
#define UT_QUERY_TV_PANNING_ENABLE      0x140E
#define UT_QUERY_TV_V_SCALER            0x140F
#define UT_SET_TV_V_SCALER              0x1410
#define UT_QUERY_TV_DEFAULT_VALUE       0x1411
#define UT_SET_TV_PANNING_RES           0x1412
#define UT_QUERY_TV_PANNING_RES         0x1413


#define UT_QUERY_VIDEO_CONTRAST         0X1501
#define UT_SET_VIDEO_CONTRAST           0X1502
#define UT_QUERY_VIDEO_BRIGHT           0X1503
#define UT_SET_VIDEO_BRIGHT             0X1504
#define UT_QUERY_VIDEO_HUE              0X1505
#define UT_SET_VIDEO_HUE                0X1506
#define UT_QUERY_VIDEO_SATURATION       0X1507
#define UT_SET_VIDEO_SATURATION         0X1508

#define UT_SET_HDMI_ACT_RANGE             0x1601
#define UT_QUERY_HDMI_ACT_RANGE           0x1602
#define UT_SET_HDMI_UNDERSCAN_PERSENT     0X1603
#define UT_QUERY_HDMI_UNDERSCAN_PERSENT   0x1604
#define UT_QUERY_HDMI_TYPE                0x1605

#define UT_QUERY_ROTATION_CAP             0x1F16


#define UT_DECODER_JPEG_ENTRY           0x2001


#define UT_ENCODER_JPEG_ENTRY           0x2101

#define UT_GET_USERMODE_DIRECTACCESS    0x2200


#define RDC_GFX_UT_EXTENSION_NAME       "RDCGFX_API"

#define RDCGFX_PARM_NUM         256

#define X_RDCGFXQueryVersion   0
#define X_RDCGFXCommand        1


#define VIDEO_MAX_COLOR_REGISTER  0xFF

#define SAA7105DACPower      0x2D
#define SAA7105DACCCRSLevel  0x5F

typedef struct {
    BYTE    type;           
    BOOL    pad1;
    WORD  sequenceNumber B16;
    ULONG  length B32;
    WORD  majorVersion B16;   
    WORD  minorVersion B16;   
    ULONG  pad2 B32;
    ULONG  pad3 B32;
    ULONG  pad4 B32;
    ULONG  pad5 B32;
    ULONG  pad6 B32;
} xRDCGFXQueryVersionReply;

typedef struct {
    BYTE   reqType;
    BYTE   RDCGFXReqType;
    WORD  length B16;
    ULONG  util_cmd_id;
    ULONG  *inBuffer;
    ULONG  inBufferSize;
    ULONG  *outBuffer;
    ULONG  outBufferSize;
    ULONG  result_header;
    ULONG  pad;
} xRDCGFXCommandReq;


typedef union
{
    struct
    {
        WORD LCD;
        WORD DVI;
        WORD CRT;
        WORD HDMI;
        WORD HDTV;
        WORD TV;
        WORD LCD2;
        WORD DVI2;
        WORD CRT2;
        WORD HDMI2;
        WORD HDTV2;
        WORD TV2;
        WORD RESERVE;
    } TABLE;
    WORD MERGE;
} UTDEVICELIST;

typedef struct _UTHWINFO
{
	ULONG ulVenderID;
    ULONG ulDeviceID;
	ULONG ulSubSystemID;
    ULONG ulSubVenderID;
    ULONG ulRevision;
}UTHWINFO;

typedef struct _UTBIOSDATA
{
		ULONG wData1;
		ULONG wData2;
		ULONG wData3;
		ULONG wDate;
}UTBIOSDATA;

typedef struct _CLKINFO
{
	ULONG MemCLK; 
	ULONG EngCLK; 
}CLKINFO;

typedef struct _VIDEO_CLUTDATA
{   
    UCHAR Red;
    UCHAR Green;
    UCHAR Blue;
    UCHAR Unused;
} __attribute__((packed)) VIDEO_CLUTDATA, *PVIDEO_CLUTDATA;

typedef struct
{   
    USHORT   NumEntries ;               
    USHORT   FirstEntry ;               
    union
    {
        VIDEO_CLUTDATA  RgbArray ;
        ULONG           RgbLong ;
    } LookupTable[1] ;
} __attribute__((packed)) VIDEO_CLUT, *PVIDEO_CLUT ;

typedef struct
{
        DWORD           device ;        
        DWORD           dwSize;         
        PVIDEO_CLUT     pClut ;         
} __attribute__((packed)) GammaTbl, *pGammaTbl ;

#define MAX_CLUT_SIZE (sizeof(VIDEO_CLUT) + (sizeof(ULONG) * 256))


typedef struct _LCDINFO
{
	WORD wLCDHeight; 
	WORD wLCDWidth; 
} LCDINFO, *PLCDINFO;


void vInitializeGammaRamp(
    ScrnInfoPtr pScrn
    );

int VgaSetColorLookup(
    ScrnInfoPtr pScrn,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    );

typedef struct _TVINFO
{
    WORD wType;             
    UCHAR bConnector;        
} __attribute__((packed)) TVINFO, *PTVINFO;

typedef struct _TV_Disp_Info
{
    unsigned char    ucCCRSLevel;        
    unsigned char    ucHPosition;        
    unsigned char    ucVPosition;        
    int              iHScaler;           
    unsigned char    ucVScaler;          
} TV_Disp_Info,*PTV_Disp_Info;

typedef struct _CBTV_Disp_Info
{
    BYTE    ucCCRSLevel;        
    BYTE    ucHPosition;        
    BYTE    ucVPosition;        
    int     iHScaler;           
    BYTE    ucVScaler;          
    BYTE    bTVType;            
    BYTE    bChange;            
    BYTE    bEnableHPanning;    
    WORD    wTVOut_HSize;       
    WORD    wModeHres;          
} CBTV_Disp_Info,*PCBTV_Disp_Info;


int EC_SetLCDPWM(RDCRecPtr pRDC, char *level);
int EC_QueryLCDPWM(RDCRecPtr pRDC, char *level);
void GetFS473PositionFromVBIOS(RDCRecPtr pRDC);
void CBIOS_SetTVEncDispRegModify(RDCRecPtr pRDC, PTV_Disp_Info pTVDispInfo, BYTE bChange);
void GetSAA7105CCRSLevel(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR *Level);
UCHAR ucGetTV_CVBS_CCRSLevel(UCHAR *Level);
