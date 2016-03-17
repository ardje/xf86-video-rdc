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
 


  




// user defined type.
typedef unsigned short          WORD;
typedef unsigned long           DWORD;

// Escape return value
#define UT_FAIL                         0x0000
#define UT_SUCCESS                      0x0001
#define UT_MODESET                      0x0002
#define UT_INVALID                      0x1FFF
#define UT_NOT_IMPLEMENT                0xFFFF

// Escape function define
#define UT_QUERY_SUPPORT_DEVICE      0x1001
#define UT_QUERY_CONNECT_DEVICE      0x1002
#define UT_SET_DEVICE_ACT            0x1003
#define UT_QUERY_PRIMARY_DEVICE      0x1004
#define UT_SET_PRIMARY_DEVICE        0x1005
#define UT_QUERY_ACT_DEVICE          0x1006

#define UT_QUERY_HW_INFO                0x1201
#define UT_QUERY_BIOS_VERSION           0x1202
#define UT_QUERY_MEM_INFO               0x1203
#define UT_QUERY_CLK_SPEED              0x1204
#define UT_QUERY_SAMM                   0x1205

#define UT_SET_GAMMA                    0x1101
#define UT_QUERY_GAMMA                  0x1102

#define UT_SET_LCD_PWM                  0x1103
#define UT_QUERY_LCD_PWM                0x1104
#define UT_QUERY_LCD_SIZE               0X1105


// Escape jpeg decoder function define
#define UT_DECODER_JPEG_ENTRY           0x2001

// Escape jpeg eecoder function define
#define UT_ENCODER_JPEG_ENTRY           0x2101

#define UT_GET_USERMODE_DIRECTACCESS    0x2200


#define RDC_GFX_UT_EXTENSION_NAME       "RDCGFX_API"

#define RDCGFX_PARM_NUM         256

#define X_RDCGFXQueryVersion   0
#define X_RDCGFXCommand        1

typedef struct {
    BYTE    type;           
    BOOL    pad1;
    CARD16  sequenceNumber B16;
    CARD32  length B32;
    CARD16  majorVersion B16;   
    CARD16  minorVersion B16;   
    CARD32  pad2 B32;
    CARD32  pad3 B32;
    CARD32  pad4 B32;
    CARD32  pad5 B32;
    CARD32  pad6 B32;
} xRDCGFXQueryVersionReply;

typedef struct {
    CARD8   reqType;
    CARD8   RDCGFXReqType;
    CARD16  length B16;
    CARD32  util_cmd_id;
    CARD32  *inBuffer;
    CARD32  inBufferSize;
    CARD32  *outBuffer;
    CARD32  outBufferSize;
    CARD32  result_header;
    CARD32  pad;
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

// LCD info structure.
typedef struct _LCDINFO
{
	WORD wLCDHeight; 
	WORD wLCDWidth; 
} LCDINFO, *PLCDINFO;

int EC_SetLCDPWM(ScrnInfoPtr pScrn, char *level);
int EC_QueryLCDPWM(ScrnInfoPtr pScrn, char *level);
int BIOS_SetLCDPWM(CBIOS_Extension *CBIOSExtension, char *level);
int BIOS_QueryLCDPWM(CBIOS_Extension *CBIOSExtension, char *level);

//Limitation Define
#define VIDEO_MAX_COLOR_REGISTER  0xFF

//Prototype Declaration
void vInitializeGammaRamp(
    ScrnInfoPtr pScrn
    );

int VgaSetColorLookup(
    ScrnInfoPtr pScrn,
    PVIDEO_CLUT ClutBuffer,
    ULONG ClutBufferSize
    );

