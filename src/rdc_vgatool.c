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
 

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86cmap.h"
#include "compiler.h"
//#include "mibstore.h"
#include "vgaHW.h"
#include "mipointer.h"
#include "micmap.h"

#include "fb.h"
#include "regionstr.h"
#include "xf86xv.h"
#include <X11/extensions/Xv.h>
#include "vbe.h"

#include "xf86PciInfo.h"
#include "xf86Pci.h"


#include "xf86fbman.h"


//#include "xaa.h"
#include "xaarop.h"


#include "xf86Cursor.h"


#include "rdc.h"


void vRDCOpenKey(ScrnInfoPtr pScrn);
Bool bRDCRegInit(ScrnInfoPtr pScrn);
ULONG GetVRAMInfo(ScrnInfoPtr pScrn);
Bool RDCFilterModeByBandWidth(ScrnInfoPtr pScrn, DisplayModePtr mode);
void vSetStartAddressCRT1(RDCRecPtr pRDC, ULONG base);
void vRDCLoadPalette(ScrnInfoPtr pScrn, int numColors, int *indices, LOCO *colors, VisualPtr pVisual);
void RDCDisplayPowerManagementSet(ScrnInfoPtr pScrn, int PowerManagementMode, int flags);
ULONG RDCGetMemBandWidth(ScrnInfoPtr pScrn);


void
vRDCOpenKey(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);

    SetIndexReg(CRTC_PORT,0x80, 0xA8);     
}

Bool
bRDCRegInit(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);

    
    SetIndexRegMask(CRTC_PORT,0xA1, 0xFF, 0x04);

   return (TRUE);
}

ULONG
GetVRAMInfo(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    UCHAR jReg;

    vRDCOpenKey(pScrn);

    GetIndexRegMask(CRTC_PORT, 0xAA, 0xFF, jReg);  

    switch (jReg & 0x07)
    {
    case 0x00:
        return (VIDEOMEM_SIZE_08M);
    case 0x01:
        return (VIDEOMEM_SIZE_16M);
    case 0x02:
        return (VIDEOMEM_SIZE_32M);
    case 0x03:
        return (VIDEOMEM_SIZE_64M);
    case 0x04:
        return (VIDEOMEM_SIZE_128M);       
    }

    return (DEFAULT_VIDEOMEM_SIZE);
}

Bool 
RDCFilterModeByBandWidth(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    ULONG RequestMemBandWidth = 0;
    RDCRecPtr pRDC = RDCPTR(pScrn);
    Bool Flags = MODE_OK;
    
    RequestMemBandWidth = mode->Clock * ((pScrn->bitsPerPixel + 7) >> 3) / 1000;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, 
        "==Filter Mode() Memory bandwidth request %u MB==\n", RequestMemBandWidth);

    if (RequestMemBandWidth > pRDC->MemoryBandwidth)
        Flags = MODE_MEM;
    
    return(Flags);
}

void
vSetStartAddressCRT1(RDCRecPtr pRDC, ULONG base)
{
    ULONG uc1stFlippingCmdReg;

    
    
#ifdef Accel_2D
    uc1stFlippingCmdReg = (base & MASK_1ST_FLIP_BASE) | CMD_ENABLE_1STFLIP;
    *(ULONG *)MMIOREG_1ST_FLIP = uc1stFlippingCmdReg;
#else
    base >>= 2;
    SetIndexReg(CRTC_PORT, 0x0d, (UCHAR)base);
    SetIndexReg(CRTC_PORT, 0x0c, (UCHAR)(base >> 8));
    SetIndexRegMask(CRTC_PORT, 0xb0, 0xbf, (UCHAR)(base >> 18));
    SetIndexReg(CRTC_PORT, 0xaf, (UCHAR)(base >> 16));
#endif
}

ULONG
RDCGetMemBandWidth(ScrnInfoPtr pScrn)
{
    CBIOS_ARGUMENTS CBiosArguments;
    CBIOS_Extension CBiosExtension;
    RDCRecPtr pRDC = RDCPTR(pScrn);

    ULONG ulDRAMBusWidth, DRAMEfficiency; 
    ULONG ulMCLK, ulDRAMBandwidth, ActualDRAMBandwidth; 
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 7, 
        "==Device ID 0x%x==\n",DEVICE_ID(pRDC->PciInfo));

    switch(DEVICE_ID(pRDC->PciInfo))
    {
    case PCI_CHIP_M2010_A0:
        
        ulDRAMBusWidth = 16;
        DRAMEfficiency = 300;
        break;
    case PCI_CHIP_M2011:
        
        ulDRAMBusWidth = 32;
        DRAMEfficiency = 600;
        break;
    default:
        
        ulDRAMBusWidth = 16;
        DRAMEfficiency = 600;
    }
    
    CBiosExtension.pCBiosArguments = &CBiosArguments;
    CBiosExtension.IOAddress = (USHORT)(pRDC->RelocateIO);
    CBiosExtension.VideoVirtualAddress = (ULONG)(pRDC->FBVirtualAddr);
    
    vRDCOpenKey(pScrn);

    CBiosExtension.pCBiosArguments->reg.x.AX = OEMFunction;
    CBiosExtension.pCBiosArguments->reg.x.BX = QueryBiosInfo;
    CInt10(&CBiosExtension);

    if ((CBiosExtension.pCBiosArguments->reg.lh.CL & (0x07)) == 0x03)
    {
        ulMCLK = 266;
    }
    else
    {
        ulMCLK = 200;
    }

    
    
    
    ulDRAMBandwidth = ulMCLK * ulDRAMBusWidth * 2 / 8;

    ActualDRAMBandwidth = ulDRAMBandwidth * DRAMEfficiency / 1000;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, 
        "==Get actual memory bandwidth request %u MB==\n", ActualDRAMBandwidth);

    return(ActualDRAMBandwidth);
}

void
vRDCLoadPalette(ScrnInfoPtr pScrn, int numColors, int *indices, LOCO *colors,
               VisualPtr pVisual)
{
    RDCRecPtr  pRDC = RDCPTR(pScrn);
    int     i, j, index;
    UCHAR DACIndex, DACR, DACG, DACB;

    switch (pScrn->bitsPerPixel) 
    {
    case 15:
        for(i=0; i<numColors; i++)
        {
            index = indices[i];
            for(j=0; j<8; j++) 
            {
                DACIndex = (index * 8) + j;
                DACR = colors[index].red << (8- pScrn->rgbBits);
                DACG = colors[index].green << (8- pScrn->rgbBits);
                DACB = colors[index].blue << (8- pScrn->rgbBits);

                VGA_LOAD_PALETTE_INDEX (DACIndex, DACR, DACG, DACB);                         
            }
        }
        break;

    case 16:
        for(i=0; i<numColors; i++) 
        {
            index = indices[i];
            for(j=0; j<4; j++) 
            {
                DACIndex = (index * 4) + j;
                DACR = colors[index/2].red << (8- pScrn->rgbBits);
                DACG = colors[index].green << (8- pScrn->rgbBits);
                DACB = colors[index/2].blue << (8- pScrn->rgbBits);

                VGA_LOAD_PALETTE_INDEX (DACIndex, DACR, DACG, DACB);                         
            }
        }
        break;

    case 24:
        for(i=0; i<numColors; i++) 
        {
            index = indices[i];
            DACIndex = index;
            DACR = colors[index].red;
            DACG = colors[index].green;
            DACB = colors[index].blue;

            VGA_LOAD_PALETTE_INDEX (DACIndex, DACR, DACG, DACB);                         
        }    
        break;

    default:
        for(i=0; i<numColors; i++) 
        {
            index = indices[i];
            DACIndex = index;
            DACR = colors[index].red >> (8 - pScrn->rgbBits);
            DACG = colors[index].green >> (8 - pScrn->rgbBits);
            DACB = colors[index].blue >> (8 - pScrn->rgbBits);

            VGA_LOAD_PALETTE_INDEX (DACIndex, DACR, DACG, DACB);                         
        }    
    } 
} 

void
RDCDisplayPowerManagementSet(ScrnInfoPtr pScrn, int PowerManagementMode, int flags)
{
    RDCRecPtr pRDC;
    UCHAR SEQ01, CRB6;

    pRDC = RDCPTR(pScrn);
    SEQ01=CRB6=0;

    vRDCOpenKey(pScrn);
   
    switch (PowerManagementMode) 
    {
    case DPMSModeOn:
        
        SEQ01 = 0x00;
        CRB6 = 0x00;
        break;
        
    case DPMSModeStandby:
        
        SEQ01 = 0x20;
        CRB6  = 0x01;     
        break;
        
    case DPMSModeSuspend:
        
        SEQ01 = 0x20;
        CRB6  = 0x02;           
        break;
        
    case DPMSModeOff:
        
        SEQ01 = 0x20;
        CRB6  = 0x03;           
        break;
    }

    SetIndexRegMask(SEQ_PORT,0x01, 0xDF, SEQ01);
    SetIndexRegMask(CRTC_PORT,0xB6, 0xFC, CRB6);
}

