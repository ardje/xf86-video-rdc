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


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86cmap.h"
#include "compiler.h"
#include "vgaHW.h"
#include "mipointer.h"
#include "micmap.h"

#include "fb.h"
#include "regionstr.h"
#include "xf86xv.h"
#include <X11/extensions/Xv.h>
#include "vbe.h"

#include "xf86Pci.h"


#include "xf86fbman.h"


#ifdef HAVE_XAA
#include "xaa.h"
#endif
#include "xaarop.h"


#include "xf86Cursor.h"


#include "rdc.h"


void vRDCOpenKey(ScrnInfoPtr pScrn);
Bool bRDCRegInit(ScrnInfoPtr pScrn);
ULONG GetVRAMInfo(ScrnInfoPtr pScrn);
Bool RDCCheckCapture(ScrnInfoPtr pScrn);
Bool RDCFilterModeByBandWidth(ScrnInfoPtr pScrn, DisplayModePtr mode);
void vSetStartAddressCRT1(RDCRecPtr pRDC, ULONG base);
void vSetDispalyStartAddress(xf86CrtcPtr crtc, int x, int y);
void vRDCLoadPalette(ScrnInfoPtr pScrn, int numColors, int *indices, LOCO *colors, VisualPtr pVisual);
void RDCDisplayPowerManagementSet(ScrnInfoPtr pScrn, int PowerManagementMode, int flags);
ULONG RDCGetMemBandWidth(ScrnInfoPtr pScrn);


BYTE GetReg(WORD base)
{
    return InPort(base);
}

void SetReg(WORD base, BYTE val)
{
    OutPort(base,val);
}

void GetIndexReg(WORD base, BYTE index, BYTE* val)
{
    OutPort(base,index);
    *val = InPort(base+1);
}
                                        
void SetIndexReg(WORD base, BYTE index, BYTE val)
{
    OutPort(base,index);
    OutPort(base+1,val);
}
                                        
void GetIndexRegMask(WORD base, BYTE index, BYTE mask, BYTE* val)
{
    OutPort(base,index);
    *val = (InPort(base+1) & mask);
}
    
void SetIndexRegMask(WORD base, BYTE index, BYTE mask, BYTE val)
{
    UCHAR ucTemp;
    OutPort(base,index);
    ucTemp = (InPort((base)+1)&(mask))|(val & ~(mask));
    SetIndexReg(base,index,ucTemp);
}

void VGA_LOAD_PALETTE_INDEX(BYTE index, BYTE red, BYTE green, BYTE blue)
{
    UCHAR ucTemp;
    SetReg(DAC_INDEX_WRITE, index);
    ucTemp = GetReg(SEQ_INDEX);
    SetReg(DAC_DATA, red);
    ucTemp = GetReg(SEQ_INDEX);
    SetReg(DAC_DATA, green);
    ucTemp = GetReg(SEQ_INDEX);
    SetReg(DAC_DATA, blue);
    ucTemp = GetReg(SEQ_INDEX);
}

void
vRDCOpenKey(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    SetIndexReg(COLOR_CRTC_INDEX,0x80, 0xA8);     
}

Bool
bRDCRegInit(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);

    
    SetIndexRegMask(COLOR_CRTC_INDEX,0xA1, 0xFF, 0x04);

   return (TRUE);
}

Bool
RDCCheckCapture(ScrnInfoPtr pScrn)
{
    UCHAR jReg;
    vRDCOpenKey(pScrn);
    
    GetIndexRegMask(COLOR_CRTC_INDEX, 0xAB, 0xFF, &jReg);

    if (jReg & BIT7)
        return TRUE;
    else
        return FALSE;
}

ULONG
GetVRAMInfo(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    UCHAR jReg;

    vRDCOpenKey(pScrn);
    GetIndexRegMask(COLOR_CRTC_INDEX, 0xAA, 0xFF, &jReg);  

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
    case 0x05:
        return (VIDEOMEM_SIZE_256M);
    case 0x06:
        return (VIDEOMEM_SIZE_512M);
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


void vSetDispalyStartAddress(xf86CrtcPtr crtc, int x, int y)
{
    BYTE         IndexData;
    unsigned long Offset;
    ScrnInfoPtr pScrn = crtc->scrn;
    RDCRecPtr pRDC = RDCPTR(pScrn);
    RDCCrtcPrivatePtr	rdc_crtc = crtc->driver_private;
    int plane = rdc_crtc->plane;
    
    
    
    SetReg(COLOR_CRTC_INDEX, 0xa3);
    IndexData = GetReg(COLOR_CRTC_DATA);
    IndexData|=0x20;
    SetReg(COLOR_CRTC_DATA, IndexData);
    Offset = ((y * pScrn->displayWidth + x) * 4);
    if(plane == 0)
    {        
        *(ULONG *)MMIOREG_1ST_FLIP = pScrn->fbOffset|CMD_ENABLE_1STFLIP|Offset;
    }
    else
    {        
        *(ULONG *)MMIOREG_2ST_FLIP = pScrn->fbOffset|CMD_ENABLE_1STFLIP|Offset;
    }
    
}

void
vSetStartAddressCRT1(RDCRecPtr pRDC, ULONG base)
{
    ULONG uc1stFlippingCmdReg;

    
    

    if (!pRDC->noAccel)
    {
        uc1stFlippingCmdReg = (base & MASK_1ST_FLIP_BASE) | CMD_ENABLE_1STFLIP;
        *(ULONG *)MMIOREG_1ST_FLIP = uc1stFlippingCmdReg;
    }

    base >>= 2;
    SetIndexReg(COLOR_CRTC_INDEX, 0x0d, (UCHAR)base); 
    SetIndexReg(COLOR_CRTC_INDEX, 0x0c, (UCHAR)(base >> 8)); 

    
    if (pRDC->ENGCaps & ENG_CAP_EXTENDFLIPADDRS)
    {
        SetIndexRegMask(COLOR_CRTC_INDEX, 0xb0, 0x3f, (UCHAR)(base >> 18)); 
        SetIndexRegMask(COLOR_CRTC_INDEX, 0xad, 0x7f, (UCHAR)(base >> 19)); 
    }
    else
        SetIndexRegMask(COLOR_CRTC_INDEX, 0xb0, 0xbf, (UCHAR)(base >> 18));
    SetIndexReg(COLOR_CRTC_INDEX, 0xaf, (UCHAR)(base >> 16)); 

}

ULONG
RDCGetMemBandWidth(ScrnInfoPtr pScrn)
{
    CBIOS_ARGUMENTS *pCBiosArguments;
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
    
    pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
    
    vRDCOpenKey(pScrn);

    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->AX = OEMFunction;
    pCBiosArguments->BX = QueryBiosInfo;
    
    
    CInt10(pRDC->pCBIOSExtension);
    

    if ((pCBiosArguments->CL & (0x07)) == 0x03)
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

    SetIndexRegMask(SEQ_INDEX,0x01, 0xDF, SEQ01);
    SetIndexRegMask(COLOR_CRTC_INDEX,0xB6, 0xFC, CRB6);
}

CBStatus CBIOS_SetEDIDToModeTable(ScrnInfoPtr pScrn, EDID_DETAILED_TIMING *pEDIDDetailedTiming)
{
    
    RDCRecPtr pRDC = RDCPTR(pScrn);
    CBIOS_ARGUMENTS *pCBiosArguments;
    ULONG Status = 0;

    pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;

    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->Eax = OEMFunction;
    pCBiosArguments->Ebx = SetEDIDInModeTable;
    pCBiosArguments->Ecx = (DWORD)pEDIDDetailedTiming;

    
    return CInt10(pRDC->pCBIOSExtension);
}

void ParseEDIDDetailedTiming(UCHAR *pucDetailedTimingBlock, EDID_DETAILED_TIMING *pEDIDDetailedTiming)
{
    ULONG ulRefreshRate, ulPixelClock;
    ULONG ulHorTotal, ulHorDispEnd, ulHorBlankingTime, ulHorBorder, ulHorSyncStart, ulHorSyncTime;
    ULONG ulVerTotal, ulVerDispEnd, ulVerBlankingTime, ulVerBorder, ulVerSyncStart, ulVerSyncTime;
    UCHAR ucFlags;
        
    
    ulPixelClock = (ULONG)(*(USHORT*)(pucDetailedTimingBlock+0));
    
    ulHorDispEnd = (ULONG)(((*(USHORT*)(pucDetailedTimingBlock+2)) & 0x00FF) | (((*(USHORT*)(pucDetailedTimingBlock+4)) & 0x00F0) << 4));
    ulHorBlankingTime = (ULONG)(((*(USHORT*)(pucDetailedTimingBlock+3)) & 0x00FF) | (((*(USHORT*)(pucDetailedTimingBlock+4)) & 0x000F) << 8));
    ulHorSyncStart = (ULONG)(((*(USHORT*)(pucDetailedTimingBlock+8)) & 0x00FF) | (((*(USHORT*)(pucDetailedTimingBlock+11)) & 0x00C0) << 4));
    ulHorSyncTime = (ULONG)(((*(USHORT*)(pucDetailedTimingBlock+9)) & 0x00FF) | (((*(USHORT*)(pucDetailedTimingBlock+11)) & 0x0030) << 8));
    ulHorBorder = (ULONG)((*(pucDetailedTimingBlock+15)) & 0xFF);
    ulHorTotal = ulHorDispEnd + ulHorBlankingTime + ulHorBorder * 2;
            
    ulVerDispEnd = (ULONG)(((*(USHORT*)(pucDetailedTimingBlock+5)) & 0x00FF) | (((*(USHORT*)(pucDetailedTimingBlock+7)) & 0x00F0) << 4));
    ulVerBlankingTime = (ULONG)(((*(USHORT*)(pucDetailedTimingBlock+6)) & 0x00FF) | (((*(USHORT*)(pucDetailedTimingBlock+7)) & 0x000F) << 8));
    ulVerSyncStart = (ULONG)((((*(USHORT*)(pucDetailedTimingBlock+10)) & 0x00F0) >> 4) | (((*(USHORT*)(pucDetailedTimingBlock+11)) & 0x000C) << 2));
    ulVerSyncTime = (ULONG)(((*(USHORT*)(pucDetailedTimingBlock+10)) & 0x000F) | (((*(USHORT*)(pucDetailedTimingBlock+11)) & 0x0003) << 4));
    ulVerBorder = (ULONG)((*(pucDetailedTimingBlock+16)) & 0xFF);
    ulVerTotal = ulVerDispEnd + ulVerBlankingTime + ulVerBorder * 2;
      
    ulRefreshRate = ((ulPixelClock*10000)+(ulHorTotal * ulVerTotal)/2) / (ulHorTotal * ulVerTotal);
    
    ucFlags = *(pucDetailedTimingBlock+17);
 
    pEDIDDetailedTiming->usPixelClock = (USHORT)ulPixelClock;
 
    pEDIDDetailedTiming->usHorDispEnd = (USHORT)ulHorDispEnd;
    pEDIDDetailedTiming->usHorBlankingTime = (USHORT)ulHorBlankingTime;
    pEDIDDetailedTiming->usHorSyncStart = (USHORT)ulHorSyncStart;
    pEDIDDetailedTiming->usHorSyncTime = (USHORT)ulHorSyncTime;
    pEDIDDetailedTiming->ucHorBorder = (UCHAR)ulHorBorder;

    pEDIDDetailedTiming->usVerDispEnd = (USHORT)ulVerDispEnd;
    pEDIDDetailedTiming->usVerBlankingTime = (USHORT)ulVerBlankingTime;
    pEDIDDetailedTiming->usVerSyncStart = (USHORT)ulVerSyncStart;
    pEDIDDetailedTiming->usVerSyncTime = (USHORT)ulVerSyncTime;
    pEDIDDetailedTiming->ucVerBorder = (UCHAR)ulVerBorder;
        
    pEDIDDetailedTiming->ucFlags = ucFlags;
    
}   

void CreateEDIDDetailedTimingList(UCHAR *ucEdidBuffer, ULONG ulEdidBufferSize, EDID_DETAILED_TIMING *pEDIDDetailedTiming)
{
    UCHAR *pucDetailedTimingBlock = ucEdidBuffer + 0x36;
    EDID_DETAILED_TIMING *pCurrent;
    
    pCurrent = pEDIDDetailedTiming;
    if ((*pucDetailedTimingBlock != 0) || (*(pucDetailedTimingBlock+1) != 0))
    {
        pCurrent->bValid = TRUE;
        ParseEDIDDetailedTiming(pucDetailedTimingBlock, pCurrent);
    }    
    else
    {
        pCurrent->bValid = FALSE;
    }
}  

