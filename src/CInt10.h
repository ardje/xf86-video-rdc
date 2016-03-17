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
 


#define     CBIOS_DEBUG             0


#define VBEFunctionCallSuccessful   0x004F
#define VBEFunctionCallFail         0x014F
#define VBEFunctionCallNotSupported 0x024F
#define VBEFunctionCallInvalid      0x034F



#define VBESetMode                  0x4F02 
#define VBESetGetScanLineLength     0x4F06
#define OEMFunction                 0x4F14

#define QueryBiosInfo               0x0000 
#define QueryBiosCaps               0x0001
#define QueryExternalDeviceInfo     0x0100
#define QueryDisplayPathInfo        0x0200
#define QueryDeviceConnectStatus    0x0201
#define QuerySupportedMode          0x0202
#define QueryLCDPanelSizeMode       0x0203
#define QueryLCDPWMLevel            0x0301
#define QueryTVConfiguration        0x0400
#define QueryTV2Configuration       0x0480
#define QueryHDTVConfiguration      0x0500
#define QueryHDTV2Configuration     0x0580
#define QueryHDMIConfiguration      0x0600
#define QueryHDMI2Configuration     0x0680
#define QueryDisplay2Pitch          0x0801
#define GetDisplay2MaxPitch         0x0803


#define SetActiveDisplayDevice      0x8200
#define SetVESAModeForDisplay2      0x8202
#define SetDevicePowerState         0x8203
#define SetDisplay1RefreshRate      0x8301
#define SetLCDPWMLevel              0x8302
#define SetDisplay2RefreshRate      0x8381
#define SetTVConfiguration          0x8400
#define SetTV2Configuration         0x8480
#define SetHDTVConnectType          0x8501
#define SetHDTV2ConnectType         0x8581
#define SetHDMIType                 0x8600
#define SetHDMI2Type                0x8680
#define SetHDMIOutputSignal         0x8601
#define SetHDMI2OutputSignal        0x8681
#define SetDisplay2PitchInPixels    0x8700
#define SetDisplay2PitchInBytes     0x8782

#define SetVideoPOST                0xF100
#define CINT10DataInit              0xF101


#define DISABLE              0x0001             
#define NHS                  0x0002             
#define PHS                  0
#define NVS                  0x0004             
#define PVS                  0
#define HB                   0x0008             
#define VB                   0x0010             
#define RBK                  0x0020             
#define ROT                  0x0040             
#define EN_DIT               0x0080             
#define MSB                  0x0100             
#define _2_CH                0x0200             
#define SW_PS                0x0400             


#define BIOS_ROM_SIZE           32*1024


typedef enum _CINT10STATUS {
    false = 0,
    true = 1
} CI_STATUS;

typedef struct _CBIOS_ARGUMENTS {
    union
    {
        struct EX
        {
            unsigned long   EAX;
            unsigned long   EBX;
            unsigned long   ECX;
            unsigned long   EDX;
            unsigned long   ESI;
            unsigned long   EDI;
        }ex;

        struct X
        {
            unsigned short  AX;
            unsigned short  dummy1;
            unsigned short  BX;
            unsigned short  dummy2;
            unsigned short  CX;
            unsigned short  dummy3;
            unsigned short  DX;
            unsigned short  dummy4;
            unsigned short  SI;
            unsigned short  dummy5;
            unsigned short  DI;
        }x;

        struct LH
        {
            unsigned char   AL;
            unsigned char   AH;
            unsigned short  dummy6;
            unsigned char   BL;
            unsigned char   BH;
            unsigned short  dummy7;
            unsigned char   CL;
            unsigned char   CH;
            unsigned short  dummy8;
            unsigned char   DL;
            unsigned char   DH;
            unsigned short  dummy9;
        }lh;
    }reg;

} CBIOS_ARGUMENTS;

typedef struct _CBIOS_Extension {

    
    CBIOS_ARGUMENTS *pCBiosArguments;

    
    unsigned long   VideoVirtualAddress;
    unsigned short  IOAddress;
}CBIOS_Extension;

extern CI_STATUS CInt10(CBIOS_Extension *pCBIOSExtension);

