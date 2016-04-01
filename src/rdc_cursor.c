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
#include "cursorstr.h"


#include "rdc.h"


static void RDCShowCursor(ScrnInfoPtr pScrn); 
static void RDCSetCursorPosition(ScrnInfoPtr pScrn, int x, int y);
static void RDCSetCursorColors(ScrnInfoPtr pScrn, int bg, int fg);
static void RDCLoadCursorImage(ScrnInfoPtr pScrn, UCHAR *src);
static Bool RDCUseHWCursor(ScreenPtr pScreen, CursorPtr pCurs);
static void RDCLoadCursorARGB(ScrnInfoPtr pScrn, CursorPtr pCurs);
static Bool RDCUseHWCursorARGB(ScreenPtr pScreen, CursorPtr pCurs);

static void RDCFireCursor(ScrnInfoPtr pScrn); 



static void RDCShowCursor_HQ(ScrnInfoPtr pScrn); 
static void RDCSetCursorColors_HQ(ScrnInfoPtr pScrn, int bg, int fg);
static void RDCLoadCursorImage_HQ(ScrnInfoPtr pScrn, UCHAR *src);
static void RDCLoadCursorARGB_HQ(ScrnInfoPtr pScrn, CursorPtr pCurs);
static void RDCSetCursorPosition_HQ(ScrnInfoPtr pScrn, int x, int y);


RDC_EXPORT Bool RDCCursorInit(ScreenPtr pScreen)
{
    ScrnInfoPtr    pScrn = xf86Screens[pScreen->myNum];
    RDCRecPtr     pRDC = RDCPTR(pScrn);
    xf86CursorInfoPtr infoPtr;

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCCursorInit()== \n");
#endif

    infoPtr = xf86CreateCursorInfoRec();
    if(!infoPtr) return FALSE;

    pRDC->HWCInfoPtr = infoPtr;

    infoPtr->Flags = HARDWARE_CURSOR_TRUECOLOR_AT_8BPP |
                     HARDWARE_CURSOR_INVERT_MASK |        
                     HARDWARE_CURSOR_BIT_ORDER_MSBFIRST;

    infoPtr->MaxWidth  = MAX_HWC_WIDTH;
    infoPtr->MaxHeight = MAX_HWC_HEIGHT;
    
    infoPtr->UseHWCursor = RDCUseHWCursor;
    infoPtr->UseHWCursorARGB = RDCUseHWCursorARGB;

    if (pRDC->ENGCaps & ENG_CAP_HWC_MMIO)
    {
        *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_CTRL)) = BIT31;
        infoPtr->SetCursorPosition = RDCSetCursorPosition_HQ;
        infoPtr->SetCursorColors = RDCSetCursorColors_HQ;
        infoPtr->LoadCursorImage = RDCLoadCursorImage_HQ;
        infoPtr->ShowCursor = RDCShowCursor_HQ;
        infoPtr->HideCursor = RDCHideCursor_HQ;
        infoPtr->LoadCursorARGB = RDCLoadCursorARGB_HQ;
    }
    else
    {
        infoPtr->SetCursorPosition = RDCSetCursorPosition;
        infoPtr->SetCursorColors = RDCSetCursorColors;
        infoPtr->LoadCursorImage = RDCLoadCursorImage;
        infoPtr->ShowCursor = RDCShowCursor;
        infoPtr->HideCursor = RDCHideCursor;
        infoPtr->LoadCursorARGB = RDCLoadCursorARGB;
    }

    if (pRDC->DeviceInfo.ScalerConfig.EnableDownScaling)
    {
        SetIndexRegMask(SEQ_INDEX, 0x78, ~BIT2, BIT2);
    }

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCCursorInit()== \n");
#endif
    return(xf86InitCursor(pScreen, infoPtr));    
}

RDC_EXPORT Bool bInitHWC(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{
    
     
    
    pRDC->HWCInfo.HWC_NUM_Next = 0;    

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter bInitHWC()== \n");
#endif

      
    pRDC->HWCInfo.pjHWCVirtualAddr = pRDC->FBVirtualAddr + pRDC->HWCInfo.ulHWCOffsetAddr; 
        
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==HWC Virtual Addr = 0x%8x==\n", (ULONG)pRDC->HWCInfo.pjHWCVirtualAddr);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==HWC Physical Offset = 0x%8x==\n", pRDC->HWCInfo.ulHWCOffsetAddr);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "==Exit2 bInitHWC(), return TRUE== \n");
#endif
    
    return (TRUE);    
}


static void
RDCShowCursor(ScrnInfoPtr pScrn)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);    
    UCHAR     jReg;
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCShowCursor()== \n");
#endif
    jReg= 0x02;
    if (pRDC->HWCInfo.cursortype ==HWC_COLOR)
        jReg |= 0x01;
    else
        jReg &= ~BIT0;
        
    SetIndexRegMask(COLOR_CRTC_INDEX, 0xCB, 0xFC, jReg);         
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCShowCursor()== \n");
#endif
}

RDC_EXPORT void RDCHideCursor(ScrnInfoPtr pScrn)
{
    RDCRecPtr  pRDC = RDCPTR(pScrn);
        
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCHideCursor()== \n");
#endif
    SetIndexRegMask(COLOR_CRTC_INDEX, 0xCB, 0xFC, 0x00);         
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCHideCursor()== \n");
#endif
}

static void
RDCSetCursorPosition(ScrnInfoPtr pScrn, int x, int y)
{
    RDCRecPtr    pRDC = RDCPTR(pScrn);
    DisplayModePtr mode = pRDC->ModePtr;    
    int        x_offset, y_offset, y_end;
    UCHAR     *pjSignature;    

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 7, "==Enter RDCSetCursorPosition(x = %d, y = %d)== \n", x, y);
#endif
        
    pjSignature = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + 
                  (HWC_SIZE+HWC_SIGNATURE_SIZE)*pRDC->HWCInfo.HWC_NUM_Next + 
                  HWC_SIZE;
                  
    *((ULONG *) (pjSignature + HWC_SIGNATURE_X)) = x;
    *((ULONG *) (pjSignature + HWC_SIGNATURE_Y)) = y;
               
    x_offset = pRDC->HWCInfo.offset_x;
    y_offset = pRDC->HWCInfo.offset_y;

    if(x < 0)
    {
        x_offset = (-x) + pRDC->HWCInfo.offset_x;
        x = 0;
    }

    if(y < 0)
    {
        y_offset = (-y) + pRDC->HWCInfo.offset_y;
        y = 0;
    }

    if ( pRDC->DeviceInfo.ScalerConfig.EnableHorScaler &&
       ((VENDOR_ID(pRDC->PciInfo) != PCI_VENDOR_RDC) || (DEVICE_ID(pRDC->PciInfo) != PCI_CHIP_M2010_A0)))
    {
        x = (x * pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor + 4095) >> 12;
    }

    if (pRDC->DeviceInfo.ScalerConfig.EnableVerScaler)
    {
        y = (y * pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor + 2048) >> 11;
        y_end = y + (((MAX_HWC_HEIGHT - y_offset) * pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor + 2048) >> 11) - 1;
    }

    if(mode->Flags & V_DBLSCAN)
        y *= 2;
 
    
    SetIndexReg(COLOR_CRTC_INDEX, 0xC2, (UCHAR) (x_offset));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xC3, (UCHAR) (y_offset));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xC4, (UCHAR) (x & 0xFF));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xC5, (UCHAR) ((x >> 8) & 0x0F));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xC6, (UCHAR) (y & 0xFF));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xC7, (UCHAR) ((y >> 8) & 0x07));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xCC, (UCHAR) (y_end & 0xFF));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xCD, (UCHAR) ((y_end >> 8) & 0x07));     
    
    
    RDCFireCursor(pScrn);
    
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 7, "==Exit RDCSetCursorPosition()== \n");
#endif
}  


static void
RDCSetCursorColors(ScrnInfoPtr pScrn, int bg, int fg)
{
    RDCRecPtr     pRDC = RDCPTR(pScrn);

#if HWC_DEBUG    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetCursorColors(bg = %x, fg = %x)== \n", bg, fg);
#endif    
    pRDC->HWCInfo.MonoHWC.fg = (fg & 0x0F) | (((fg>>8) & 0x0F) << 4) | (((fg>>16) & 0x0F) << 8);
    pRDC->HWCInfo.MonoHWC.bg = (bg & 0x0F) | (((bg>>8) & 0x0F) << 4) | (((bg>>16) & 0x0F) << 8);
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetCursorColors()== \n");
#endif
}

static void
RDCLoadCursorImage(ScrnInfoPtr pScrn, UCHAR *src)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);   
    int         i, j, k;
    UCHAR       *pjSrcAnd, *pjSrcXor, *pjDstData;
    ULONG       ulTempDstAnd32[2], ulTempDstData32[2];
    UCHAR       jTempSrcAnd32, jTempSrcXor32;
    ULONG       ulCheckSum = 0;                         
    ULONG       ulPatternAddr;
    
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCLoadCursorImage()== \n");
#endif 
    
    pRDC->HWCInfo.cursortype = HWC_MONO;
    pRDC->HWCInfo.width  = (USHORT) MAX_HWC_WIDTH;
    pRDC->HWCInfo.height = (USHORT) MAX_HWC_HEIGHT;
    pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
    pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;

    
    pjSrcXor = src;
    pjSrcAnd = src + (MAX_HWC_WIDTH*MAX_HWC_HEIGHT/8);
    pjDstData =  pRDC->HWCInfo.pjHWCVirtualAddr+(HWC_SIZE+HWC_SIGNATURE_SIZE)*pRDC->HWCInfo.HWC_NUM_Next;
      
    for (j = 0; j < MAX_HWC_HEIGHT; j++)
    {
        for (i = 0; i < (MAX_HWC_WIDTH/8); i++ )
        {
            for (k=7; k>0; k-=2)
            {
                jTempSrcAnd32 = *((UCHAR *) pjSrcAnd);
                jTempSrcXor32 = *((UCHAR *) pjSrcXor);
                ulTempDstAnd32[0] = ((jTempSrcAnd32 >> k) & 0x01) ? 0x00008000L:0x00L;
                ulTempDstData32[0] = ((jTempSrcXor32 >> k) & 0x01) ? pRDC->HWCInfo.MonoHWC.fg:pRDC->HWCInfo.MonoHWC.bg;
                ulTempDstAnd32[1] = ((jTempSrcAnd32 >> (k-1)) & 0x01) ? 0x80000000L:0x00L;
                ulTempDstData32[1] = ((jTempSrcXor32 >> (k-1)) & 0x01) ? (pRDC->HWCInfo.MonoHWC.fg << 16):(pRDC->HWCInfo.MonoHWC.bg << 16);
                *((ULONG *) pjDstData) = ulTempDstAnd32[0] | ulTempDstData32[0] | ulTempDstAnd32[1] | ulTempDstData32[1];
                ulCheckSum += *((ULONG *) pjDstData);                               
                pjDstData += 4;
            }
            pjSrcAnd ++;
            pjSrcXor ++;
        }        
    }                    

    
    pjDstData = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + (HWC_SIZE+HWC_SIGNATURE_SIZE)*pRDC->HWCInfo.HWC_NUM_Next + HWC_SIZE;   
    *((ULONG *) pjDstData) = ulCheckSum;
    *((ULONG *) (pjDstData + HWC_SIGNATURE_SizeX)) = pRDC->HWCInfo.width;
    *((ULONG *) (pjDstData + HWC_SIGNATURE_SizeY)) = pRDC->HWCInfo.height;
    *((ULONG *) (pjDstData + HWC_SIGNATURE_HOTSPOTX)) = 0;
    *((ULONG *) (pjDstData + HWC_SIGNATURE_HOTSPOTY)) = 0;
    
    
    ulPatternAddr = ((pRDC->HWCInfo.ulHWCOffsetAddr+(HWC_SIZE+HWC_SIGNATURE_SIZE)*pRDC->HWCInfo.HWC_NUM_Next) >> 3);
    
    SetIndexReg(COLOR_CRTC_INDEX, 0xC8, (UCHAR) (ulPatternAddr & 0xFF));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xC9, (UCHAR) ((ulPatternAddr >> 8) & 0xFF));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xCA, (UCHAR) ((ulPatternAddr >> 16) & 0xFF)); 
    
    
    pRDC->HWCInfo.HWC_NUM_Next = (pRDC->HWCInfo.HWC_NUM_Next+1) % pRDC->HWCInfo.HWC_NUM;
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCLoadCursorImage()== \n");
#endif
}

static Bool 
RDCUseHWCursor(ScreenPtr pScreen, CursorPtr pCurs)
{
    ScrnInfoPtr     pScrn = xf86Screens[pScreen->myNum];
    RDCRecPtr       pRDC = RDCPTR(pScrn);

#if HWC_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCUseHWCursor(), return TRUE== \n");
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "hot spot (x, y) = (%d, %d)\n", pCurs->bits->xhot, pCurs->bits->yhot);
#endif
    pRDC->HWCInfo.cursortype = HWC_MONO;
    

    
    if (pRDC->bRandRRotation)
    {
        switch (pRDC->rotate)
        {
            case RR_Rotate_0:
                pRDC->HWCInfo.width = MAX_HWC_WIDTH;
                pRDC->HWCInfo.height = MAX_HWC_WIDTH;
                pRDC->HWCInfo.xhot = pCurs->bits->xhot;
                pRDC->HWCInfo.yhot = pCurs->bits->yhot;
                pRDC->HWCInfo.offset_x = 0;
                pRDC->HWCInfo.offset_y = 0;
                break;

            case RR_Rotate_90:
                pRDC->HWCInfo.width = MAX_HWC_WIDTH;
                pRDC->HWCInfo.height = pCurs->bits->width;
                pRDC->HWCInfo.xhot = pCurs->bits->yhot;
                pRDC->HWCInfo.yhot = pCurs->bits->width - pCurs->bits->xhot - 1;
                pRDC->HWCInfo.offset_x = 0;
                pRDC->HWCInfo.offset_y = MAX_HWC_WIDTH - pCurs->bits->width;
                break;

            case RR_Rotate_180:
                pRDC->HWCInfo.width = pCurs->bits->width;
                pRDC->HWCInfo.height = pCurs->bits->height;
                pRDC->HWCInfo.xhot = pCurs->bits->width - pCurs->bits->xhot - 1;
                pRDC->HWCInfo.yhot = pCurs->bits->height - pCurs->bits->yhot - 1;
                pRDC->HWCInfo.offset_x = MAX_HWC_HEIGHT - pCurs->bits->height;
                pRDC->HWCInfo.offset_y = MAX_HWC_WIDTH - pCurs->bits->width;
                break;

            case RR_Rotate_270:
                pRDC->HWCInfo.width = pCurs->bits->height;
                pRDC->HWCInfo.height = MAX_HWC_HEIGHT;
                pRDC->HWCInfo.xhot = pCurs->bits->height - pCurs->bits->yhot - 1;
                pRDC->HWCInfo.yhot = pCurs->bits->xhot;
                pRDC->HWCInfo.offset_x = MAX_HWC_HEIGHT - pCurs->bits->height;
                pRDC->HWCInfo.offset_y = 0;
                break;
        }
    }
    else
    {
        pRDC->HWCInfo.width = MAX_HWC_WIDTH;
        pRDC->HWCInfo.height = MAX_HWC_WIDTH;
        pRDC->HWCInfo.xhot = pCurs->bits->xhot;
        pRDC->HWCInfo.yhot = pCurs->bits->yhot;
        pRDC->HWCInfo.offset_x = 0;
        pRDC->HWCInfo.offset_y = 0;
    }

    return TRUE;
}

static void
RDCLoadCursorARGB(ScrnInfoPtr pScrn, CursorPtr pCurs)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);   

    UCHAR       *pjDstXor, *pjSrcXor;
    ULONG       i, j, ulSrcWidth, ulSrcHeight;
    ULONG       ulPerPixelCopy, ulTwoPixelCopy;    
    LONG        lAlphaDstDelta, lLastAlphaDstDelta;
    
    union
    {
        ULONG   ul;
        UCHAR   b[4];
    } ulSrcData32[2], ulData32;
    union
    {
        USHORT  us;
        UCHAR   b[2];
    } usData16;
    
    ULONG       ulCheckSum = 0;                     
    ULONG       ulPatternAddr;

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCLoadCursorARGB()== \n");
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "hot spot (x, y) = (%d, %d)\n", pCurs->bits->xhot, pCurs->bits->yhot);
#endif
    
    pRDC->HWCInfo.cursortype = HWC_COLOR;
    pRDC->HWCInfo.width  = pCurs->bits->width;
    pRDC->HWCInfo.height = pCurs->bits->height;
    pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
    pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;
    
    
    ulSrcWidth  =  pRDC->HWCInfo.width;  
    ulSrcHeight =  pRDC->HWCInfo.height;  
        
    lAlphaDstDelta = MAX_HWC_WIDTH << 1;
    lLastAlphaDstDelta = lAlphaDstDelta - (ulSrcWidth << 1);

    pjSrcXor  = (UCHAR *) pCurs->bits->argb;;
    pjDstXor  = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + (HWC_SIZE+HWC_SIGNATURE_SIZE)*pRDC->HWCInfo.HWC_NUM_Next
                        + lLastAlphaDstDelta + (MAX_HWC_HEIGHT - ulSrcHeight) * lAlphaDstDelta;
    
    ulPerPixelCopy =  ulSrcWidth & 1;
    ulTwoPixelCopy =  ulSrcWidth >> 1;
                   
    for (j = 0; j < ulSrcHeight; j++)
    {
        for (i = 0; i < ulTwoPixelCopy; i++ )
        {
            ulSrcData32[0].ul = *((ULONG *) pjSrcXor) & 0xF0F0F0F0;
            ulSrcData32[1].ul = *((ULONG *) (pjSrcXor+4)) & 0xF0F0F0F0;
            ulData32.b[0] = ulSrcData32[0].b[1] | (ulSrcData32[0].b[0] >> 4);
            ulData32.b[1] = ulSrcData32[0].b[3] | (ulSrcData32[0].b[2] >> 4);
            ulData32.b[2] = ulSrcData32[1].b[1] | (ulSrcData32[1].b[0] >> 4);
            ulData32.b[3] = ulSrcData32[1].b[3] | (ulSrcData32[1].b[2] >> 4);                     
            *((ULONG *) pjDstXor) = ulData32.ul;
            ulCheckSum += (ULONG) ulData32.ul;                                     
            pjDstXor += 4;
            pjSrcXor += 8;
        } 
        
        for (i = 0; i < ulPerPixelCopy; i++ )
        {
            ulSrcData32[0].ul = *((ULONG *) pjSrcXor) & 0xF0F0F0F0;
            usData16.b[0] = ulSrcData32[0].b[1] | (ulSrcData32[0].b[0] >> 4);
            usData16.b[1] = ulSrcData32[0].b[3] | (ulSrcData32[0].b[2] >> 4);
            *((USHORT *) pjDstXor) = usData16.us;
            ulCheckSum += (ULONG) usData16.us;                                                 
            pjDstXor += 2;
            pjSrcXor += 4;
        } 

        
        pjDstXor += lLastAlphaDstDelta;
    } 

    
    pjDstXor = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + (HWC_SIZE+HWC_SIGNATURE_SIZE)*pRDC->HWCInfo.HWC_NUM_Next + HWC_SIZE;   
    *((ULONG *) pjDstXor) = ulCheckSum;
    *((ULONG *) (pjDstXor + HWC_SIGNATURE_SizeX)) = pRDC->HWCInfo.width;
    *((ULONG *) (pjDstXor + HWC_SIGNATURE_SizeY)) = pRDC->HWCInfo.height;
    *((ULONG *) (pjDstXor + HWC_SIGNATURE_HOTSPOTX)) = 0;
    *((ULONG *) (pjDstXor + HWC_SIGNATURE_HOTSPOTY)) = 0;
           
    
    ulPatternAddr = ((pRDC->HWCInfo.ulHWCOffsetAddr +(HWC_SIZE+HWC_SIGNATURE_SIZE)*pRDC->HWCInfo.HWC_NUM_Next) >> 3);
    SetIndexReg(COLOR_CRTC_INDEX, 0xC8, (UCHAR) (ulPatternAddr & 0xFF));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xC9, (UCHAR) ((ulPatternAddr >> 8) & 0xFF));     
    SetIndexReg(COLOR_CRTC_INDEX, 0xCA, (UCHAR) ((ulPatternAddr >> 16) & 0xFF)); 
    
    
    pRDC->HWCInfo.HWC_NUM_Next = (pRDC->HWCInfo.HWC_NUM_Next+1) % pRDC->HWCInfo.HWC_NUM;

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCLoadCursorARGB()== \n");
#endif
}

static Bool 
RDCUseHWCursorARGB(ScreenPtr pScreen, CursorPtr pCurs)
{
    ScrnInfoPtr     pScrn = xf86Screens[pScreen->myNum];
    RDCRecPtr       pRDC = RDCPTR(pScrn);

#if HWC_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCUseHWCursorARGB(), return TRUE== \n");
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "hot spot (x, y) = (%d, %d)\n", pCurs->bits->xhot, pCurs->bits->yhot);
#endif

    pRDC->HWCInfo.cursortype = HWC_COLOR;

    if (pRDC->bRandRRotation)
    {
        switch (pRDC->rotate)
        {
            case RR_Rotate_0:
                pRDC->HWCInfo.width  = pCurs->bits->width;
                pRDC->HWCInfo.height = pCurs->bits->height;
                pRDC->HWCInfo.xhot = pCurs->bits->xhot;
                pRDC->HWCInfo.yhot = pCurs->bits->yhot;
                pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
                pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;
                break;

            case RR_Rotate_90:
                pRDC->HWCInfo.width  = pCurs->bits->height;
                pRDC->HWCInfo.height = pCurs->bits->width;
                pRDC->HWCInfo.xhot = pCurs->bits->yhot;
                pRDC->HWCInfo.yhot = pCurs->bits->width - pCurs->bits->xhot - 1;
                pRDC->HWCInfo.offset_x = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;
                pRDC->HWCInfo.offset_y = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
                break;

            case RR_Rotate_180:
                pRDC->HWCInfo.width  = pCurs->bits->width;
                pRDC->HWCInfo.height = pCurs->bits->height;
                pRDC->HWCInfo.xhot = pCurs->bits->width - pCurs->bits->xhot - 1;
                pRDC->HWCInfo.yhot = pCurs->bits->height - pCurs->bits->yhot - 1;
                pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
                pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;
                break;

            case RR_Rotate_270:
                pRDC->HWCInfo.width  = pCurs->bits->height;
                pRDC->HWCInfo.height = pCurs->bits->width;
                pRDC->HWCInfo.xhot = pCurs->bits->height - pCurs->bits->yhot - 1;
                pRDC->HWCInfo.yhot = pCurs->bits->xhot;
                pRDC->HWCInfo.offset_x = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;
                pRDC->HWCInfo.offset_y = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
                break;
        }
    }
    else
    {
        pRDC->HWCInfo.width  = pCurs->bits->width;
        pRDC->HWCInfo.height = pCurs->bits->height;
        pRDC->HWCInfo.xhot = pCurs->bits->xhot;
        pRDC->HWCInfo.yhot = pCurs->bits->yhot;
        pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
        pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;
    }

    return TRUE;
}

static void
RDCFireCursor(ScrnInfoPtr pScrn)
{
    RDCRecPtr  pRDC = RDCPTR(pScrn);
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCFireCursor()== \n");
#endif
    SetIndexRegMask(COLOR_CRTC_INDEX, 0xCB, 0xFF, 0x00);         
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCFireCursor()== \n");
#endif
}



RDC_EXPORT void RDCHideCursor_HQ(ScrnInfoPtr pScrn)
{
    ULONG      ulCursorCTRL;
    RDCRecPtr  pRDC = RDCPTR(pScrn);
    
    ulCursorCTRL = *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_CTRL)); 
    *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_CTRL)) = (ulCursorCTRL& ~BIT1);
}

static void RDCShowCursor_HQ(ScrnInfoPtr pScrn)
{
    ULONG      ulCursorCTRL;
    RDCRecPtr  pRDC = RDCPTR(pScrn);
    
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Enter RDCShowCursor_HQ()== \n");
#endif

    ulCursorCTRL = *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_CTRL));

    if (pRDC->HWCInfo.cursortype ==HWC_COLOR)
        ulCursorCTRL |= BIT0;
    else
        ulCursorCTRL &= ~BIT0;
        
    *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_CTRL)) = (ulCursorCTRL | BIT1);

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Cursor MMIO regs = %08x %08x %08x %08x== \n", 
                                *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_CTRL)),
                                *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_OFFSET)),
                                *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_POSITION)),
                                *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_ADDRESS)));
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Exit RDCShowCursor_HQ()== \n");
#endif
}

static void RDCSetCursorColors_HQ(ScrnInfoPtr pScrn, int bg, int fg)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);

    int         i, j, k;
    UCHAR       *pjSrcAnd, *pjSrcXor;
    UCHAR       jTempSrcAnd, jTempSrcXor;
    ULONG       ulTempDstAnd32, ulTempDstData32;
    ULONG       *pulDstData;
    ULONG       ulROTAP_CTL1 = 0;
    uint64 dwdwROTAP_SRC_PITCH_RECI=0;
    DWORD  dwROTAP_SRC_PITCH_RECI=0; 
    
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetCursorColors_HQ(bg = %x, fg = %x)== \n", bg, fg);
#endif

    pRDC->HWCInfo.MonoHWC.fg = fg;
    pRDC->HWCInfo.MonoHWC.bg = bg;

    pjSrcXor = pRDC->HWCInfo.MonoHWC.ucXorBitmap;
    pjSrcAnd = pRDC->HWCInfo.MonoHWC.ucAndBitmap;
    pulDstData = (ULONG*)(pRDC->HWCInfo.pjHWCVirtualAddr + HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next);


    if (pRDC->bRandRRotation)
    {
        ulROTAP_CTL1 |= ROTAPS_32BPP;

        if (pRDC->rotate == RR_Rotate_0)
        {
                ulROTAP_CTL1 &= ~(ROTAPS_ENABLE);
        }
        else
        {
            ulROTAP_CTL1 |= ROTAPS_ENABLE;
            *ROTAP_PITCH1 = (MAX_HWC_HEIGHT << 16) | (MAX_HWC_WIDTH);

            if(pRDC->ENGCaps & ENG_CAPS_ADV_ROT)
            {
                dwdwROTAP_SRC_PITCH_RECI = ROTAPS_SRC_RECI_FACTOR;
                dwdwROTAP_SRC_PITCH_RECI = ((dwdwROTAP_SRC_PITCH_RECI<<2)<<1)/MAX_HWC_WIDTH;
                dwdwROTAP_SRC_PITCH_RECI = (dwdwROTAP_SRC_PITCH_RECI+1)>>1;
                dwROTAP_SRC_PITCH_RECI = (DWORD)dwdwROTAP_SRC_PITCH_RECI; 
                *ROTAP_SRC_PITCH_RECI1 = dwROTAP_SRC_PITCH_RECI;
            }
            else
            {
                *ROTAP_SRC_PITCH_RECI1 = (1 << 30) / (MAX_HWC_WIDTH);
            }
            *ROTAP_START_ADDR1  = pRDC->HWCInfo.ulHWCOffsetAddr +
                                  HQ_HWC_SIZE * pRDC->HWCInfo.HWC_NUM_Next;
            *ROTAP_END_ADDR1    = pRDC->HWCInfo.ulHWCOffsetAddr +
                                  HQ_HWC_SIZE * (pRDC->HWCInfo.HWC_NUM_Next + 1);
            *ROTAP_WH1 = ((MAX_HWC_HEIGHT-1) << 16) | (MAX_HWC_WIDTH- 1);
        }
        *ROTAP_CTL1 = ulROTAP_CTL1;
    }

    for (j = 0; j < MAX_HWC_HEIGHT; j++)
    {
        for (i = 0; i < (MAX_HWC_WIDTH/8); i++)
        {
            jTempSrcXor = *pjSrcXor;
            jTempSrcAnd = *pjSrcAnd;

            for (k=7; k>=0; k--)
            {
                ulTempDstAnd32 = ((jTempSrcAnd >> k) & 0x01) ? 0x80000000L:0x00L;
                ulTempDstData32 = ((jTempSrcXor >> k) & 0x01) ? fg:bg;
                *pulDstData = ulTempDstAnd32|ulTempDstData32;
                pulDstData++;
            }

            pjSrcXor++;
            pjSrcAnd++;
        }        
    }                    

    ulROTAP_CTL1 = 0;
    
    
    pRDC->HWCInfo.HWC_NUM_Next = (pRDC->HWCInfo.HWC_NUM_Next+1) % pRDC->HWCInfo.HWC_NUM;

}

static void RDCLoadCursorImage_HQ(ScrnInfoPtr pScrn, UCHAR *src)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);   
    ULONG       ulPatternOffset;
    
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCLoadCursorImage_HQ()== \n");
#endif 
    
    memcpy(&(pRDC->HWCInfo.MonoHWC.ucXorBitmap), src, sizeof(pRDC->HWCInfo.MonoHWC.ucXorBitmap));
    memcpy(&(pRDC->HWCInfo.MonoHWC.ucAndBitmap), src + (MAX_HWC_WIDTH*MAX_HWC_HEIGHT/8), sizeof(pRDC->HWCInfo.MonoHWC.ucAndBitmap));

    
    ulPatternOffset = pRDC->HWCInfo.ulHWCOffsetAddr + HQ_HWC_SIZE * pRDC->HWCInfo.HWC_NUM_Next;
    *((ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_ADDRESS)) = ulPatternOffset >> 3;

    
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCLoadCursorImage_HQ()== \n");
#endif
}

static void RDCLoadCursorARGB_HQ(ScrnInfoPtr pScrn, CursorPtr pCurs)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);   
    UCHAR       *pjDstXor, *pjSrcXor;
    ULONG       j, ulSrcWidthInByte, ulSrcHeight;
    LONG        ulDstCursorPitch;
    ULONG       ulPatternAddr;
    ULONG       ulROTAP_CTL1 = 0;
    uint64      dwdwROTAP_SRC_PITCH_RECI=0;
    DWORD       dwROTAP_SRC_PITCH_RECI=0; 
    
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCLoadCursorARGB_HQ()== \n");
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "hot spot (x, y) = (%d, %d)\n", pCurs->bits->xhot, pCurs->bits->yhot);
#endif
    
    ulSrcWidthInByte    = pCurs->bits->width << 2;  
    ulSrcHeight         = pCurs->bits->height;  
    ulDstCursorPitch    = MAX_HWC_WIDTH << 2;
    pjSrcXor            = (UCHAR *) pCurs->bits->argb;

    if (pRDC->bRandRRotation)
    {
        ulROTAP_CTL1 |= ROTAPS_32BPP;

        switch (pRDC->rotate)
        {
            case RR_Rotate_0:
                pRDC->HWCInfo.width  = pCurs->bits->width;
                pRDC->HWCInfo.height = pCurs->bits->height;
                pRDC->HWCInfo.xhot = pCurs->bits->xhot;
                pRDC->HWCInfo.yhot = pCurs->bits->yhot;
                pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
                pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;

                pjDstXor = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + 
                           HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next +
                           (MAX_HWC_HEIGHT - ulSrcHeight) * ulDstCursorPitch +
                           (ulDstCursorPitch - ulSrcWidthInByte);
                ulROTAP_CTL1 &= ~(ROTAPS_ENABLE);
                break;

            case RR_Rotate_90:
                pRDC->HWCInfo.width  = pCurs->bits->height;
                pRDC->HWCInfo.height = pCurs->bits->width;
                pRDC->HWCInfo.xhot = pCurs->bits->yhot;
                pRDC->HWCInfo.yhot = pCurs->bits->width - pCurs->bits->xhot - 1;
                pRDC->HWCInfo.offset_x = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;
                pRDC->HWCInfo.offset_y = MAX_HWC_WIDTH - pRDC->HWCInfo.width;

                *ROTAP_PITCH1 = (MAX_HWC_HEIGHT << 16) | (MAX_HWC_WIDTH);
                ulROTAP_CTL1 |= ROTAPS_ENABLE;

                pjDstXor = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + 
                           HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next +
                           (MAX_HWC_HEIGHT - ulSrcHeight) * ulDstCursorPitch;
                break;

            case RR_Rotate_180:
                pRDC->HWCInfo.width  = pCurs->bits->width;
                pRDC->HWCInfo.height = pCurs->bits->height;
                pRDC->HWCInfo.xhot = pCurs->bits->width - pCurs->bits->xhot - 1;
                pRDC->HWCInfo.yhot = pCurs->bits->height - pCurs->bits->yhot - 1;
                pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
                pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;

                *ROTAP_PITCH1 = (MAX_HWC_WIDTH << 16) | (MAX_HWC_WIDTH);
                ulROTAP_CTL1 |= ROTAPS_ENABLE;

            
                pjDstXor = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + 
                           HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next;
                break;

            case RR_Rotate_270:
                pRDC->HWCInfo.width  = pCurs->bits->height;
                pRDC->HWCInfo.height = pCurs->bits->width;
                pRDC->HWCInfo.xhot = pCurs->bits->height - pCurs->bits->yhot - 1;
                pRDC->HWCInfo.yhot = pCurs->bits->xhot;
                pRDC->HWCInfo.offset_x = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;
                pRDC->HWCInfo.offset_y = MAX_HWC_WIDTH - pRDC->HWCInfo.width;

                *ROTAP_PITCH1 = (MAX_HWC_HEIGHT << 16) | (MAX_HWC_WIDTH);
                ulROTAP_CTL1 |= ROTAPS_ENABLE;
            
                pjDstXor = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + 
                           HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next +
                           (ulDstCursorPitch - ulSrcWidthInByte);
                break;
        }
        
        if(pRDC->ENGCaps & ENG_CAPS_ADV_ROT)
        {
            dwdwROTAP_SRC_PITCH_RECI = ROTAPS_SRC_RECI_FACTOR;
            dwdwROTAP_SRC_PITCH_RECI = ((dwdwROTAP_SRC_PITCH_RECI<<2)<<1)/MAX_HWC_WIDTH;
            dwdwROTAP_SRC_PITCH_RECI = (dwdwROTAP_SRC_PITCH_RECI+1)>>1;
            dwROTAP_SRC_PITCH_RECI = (DWORD)dwdwROTAP_SRC_PITCH_RECI; 
            *ROTAP_SRC_PITCH_RECI1 = dwROTAP_SRC_PITCH_RECI;
        }
        else
        {            
            *ROTAP_SRC_PITCH_RECI1 = (1 << 30) / (MAX_HWC_WIDTH);
        }
        *ROTAP_START_ADDR1  = pRDC->HWCInfo.ulHWCOffsetAddr +
                              HQ_HWC_SIZE * pRDC->HWCInfo.HWC_NUM_Next;
        *ROTAP_END_ADDR1    = pRDC->HWCInfo.ulHWCOffsetAddr +
                              HQ_HWC_SIZE * (pRDC->HWCInfo.HWC_NUM_Next + 1);
        *ROTAP_WH1 = ((MAX_HWC_HEIGHT-1) << 16) | (MAX_HWC_WIDTH- 1);

        *ROTAP_CTL1 = ulROTAP_CTL1;
    }
    else
    {
        pRDC->HWCInfo.width  = pCurs->bits->width;
        pRDC->HWCInfo.height = pCurs->bits->height;
        pRDC->HWCInfo.xhot = pCurs->bits->xhot;
        pRDC->HWCInfo.yhot = pCurs->bits->yhot;
        pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
        pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;

        pjDstXor  = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + 
                    HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next +
                    (MAX_HWC_HEIGHT - ulSrcHeight) * ulDstCursorPitch +
                    (ulDstCursorPitch - ulSrcWidthInByte);
    }
    
    
    for (j = 0; j < ulSrcHeight; j++)
    {
        memcpy(pjDstXor,pjSrcXor, ulSrcWidthInByte);
        
        
        pjSrcXor += ulSrcWidthInByte;
        pjDstXor += ulDstCursorPitch;
    } 

    
    *ROTAP_CTL1 = 0;

    
    ulPatternAddr = 
        ((pRDC->HWCInfo.ulHWCOffsetAddr +HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next) >> 3);

    *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_ADDRESS)) = ulPatternAddr;
    
    
    pRDC->HWCInfo.HWC_NUM_Next = (pRDC->HWCInfo.HWC_NUM_Next+1) % pRDC->HWCInfo.HWC_NUM;

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCLoadCursorARGB()== \n");
#endif
}

static void RDCSetCursorPosition_HQ(ScrnInfoPtr pScrn, int x, int y)
{
    RDCRecPtr    pRDC = RDCPTR(pScrn);
    DisplayModePtr mode = pRDC->ModePtr;
    HWCINFO     *pHWCInfo = &pRDC->HWCInfo;
    int         x_offset, y_offset, y_end;
    int         xhot, yhot;
    int         iOverlayStartX, iOverlayStartY;
    int         iOverlayEndX,   iOverlayEndY;
    int         iOverlayDispCntX, iOverlayDispCntY;
    int         iOverlayStartOffsetX, iOverlayStartOffsetY;
    int         iOverlaySrcWidth, iOverlaySrcHeight;
    int         iOverlayDstWidth, iOverlayDstHeight;
    int         iPanningVDDstWdith;
    int         iOverlayFetchCnt;
    ULONG       ulCursorCtrl;
    Bool        bNeedAdjustFrame = FALSE;
    ULONG       ulHorScalingFactor = pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor;
    ULONG       ulVerScalingFactor = pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor;
    int         iMaxHorCoor, iMaxVerCoor;
    int         iBpp = pRDC->VideoModeInfo.Bpp;
    PKT_SC      *pSingleCMD;
    
    

    int         ori_x = x, ori_y = y;
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Enter RDCSetCursorPosition_HQ(x = %d, y = %d)== \n", x, y);
#endif
    if (pRDC->bRandRRotation)
    {
        switch (pRDC->rotate)
        {
            case RR_Rotate_0:
                break;
            case RR_Rotate_90:
                x = ori_y;
                y = pRDC->VideoModeInfo.ScreenHeight - ori_x - pRDC->HWCInfo.height;
                break;
            case RR_Rotate_180:
                x = pRDC->VideoModeInfo.ScreenWidth - ori_x - pRDC->HWCInfo.width;
                y = pRDC->VideoModeInfo.ScreenHeight - ori_y - pRDC->HWCInfo.height;
                break;
            case RR_Rotate_270:
                x = pRDC->VideoModeInfo.ScreenWidth - ori_y - pRDC->HWCInfo.width;
                y = ori_x;
                break;
        }
    }
    xhot = pRDC->HWCInfo.xhot;
    yhot = pRDC->HWCInfo.yhot;
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "hot spot (x, y) = (%d, %d)\n", xhot, yhot);
#endif

    x_offset = pRDC->HWCInfo.offset_x;
    y_offset = pRDC->HWCInfo.offset_y;

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "offset (x, y) = (%d, %d)\n", x_offset, y_offset);
#endif

    
    if ((x+xhot) >= mode->HDisplay)
    {
        x = mode->HDisplay - xhot -1;
    }

    if ((y+yhot) >= mode->VDisplay)
    {
        y = mode->VDisplay - yhot -1;
    }
    
    if ((x+xhot) < 0)
    {
        x = (-xhot);
    }

    if ((y+yhot) < 0)
    {
        y = (-yhot);
    }

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "iScreenOffset_(x, y) = (%d, %d)\n",
                   pHWCInfo->iScreenOffset_x, pHWCInfo->iScreenOffset_y);
#endif

    if ((pRDC->DeviceInfo.ScalerConfig.EnableDownScaling) && (pRDC->DeviceInfo.ScalerConfig.EnableHorDownScaler))
    {
        if (mode->HDisplay > 1024)
        {
            iMaxHorCoor = 1024 - 1;
        }
        else
        {
            iMaxHorCoor = (int)mode->HDisplay - 1;
        }
    }
    else
    {
        iMaxHorCoor = (int)pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution - 1;
    }

    if ((pRDC->DeviceInfo.ScalerConfig.EnableDownScaling) && (pRDC->DeviceInfo.ScalerConfig.EnableVerDownScaler))
    {
        if (mode->VDisplay > 768)
        {
            iMaxVerCoor = 768 - 1;
        }
        else
        {
            iMaxVerCoor = (int)mode->VDisplay - 1;
        }
    }
    else
    {
        iMaxVerCoor = (int)pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution - 1;
    }

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "(int)mode->HDisplay = %d\n", (int)mode->HDisplay);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "(int)mode->VDisplay = %d\n", (int)mode->VDisplay);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "iMaxHorCoor = %d\n", iMaxHorCoor);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "iMaxVerCoor = %d\n", iMaxVerCoor);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "ulHorMaxResolution = %d\n", pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "ulVerMaxResolution = %d\n", pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution);
#endif

    
    if (pRDC->DeviceInfo.ucDeviceID == LCDIndex ||
        pRDC->DeviceInfo.ucDeviceID == HDMIIndex ||
        (pRDC->DeviceInfo.ucDeviceID == CRTIndex && pRDC->pCBIOSExtension->bEDIDValid) ||
        (pRDC->DeviceInfo.ucDeviceID == TVIndex && pRDC->bEnableTVPanning))
    {
        if (x+xhot > pHWCInfo->iScreenOffset_x + iMaxHorCoor)
        {
            pHWCInfo->iScreenOffset_x = x + xhot - iMaxHorCoor;
            if (pRDC->DeviceInfo.ScalerConfig.EnableDownScaling && pRDC->DeviceInfo.ScalerConfig.EnableHorDownScaler)
            {
                
                if ((iBpp == 4) || (iBpp == 3))
                {
                    pHWCInfo->iScreenOffset_x = ALIGN_TO_UB_16(pHWCInfo->iScreenOffset_x);
                }
                else if (iBpp == 2)
                {
                    pHWCInfo->iScreenOffset_x = ALIGN_TO_UB_32(pHWCInfo->iScreenOffset_x);
                }
            }
            else
            {
                
                if ((iBpp == 4) || (iBpp == 3))
                    pHWCInfo->iScreenOffset_x = ALIGN_TO_UB_2(pHWCInfo->iScreenOffset_x);
                else if (iBpp == 2)
                    pHWCInfo->iScreenOffset_x = ALIGN_TO_UB_4(pHWCInfo->iScreenOffset_x);
            }
            bNeedAdjustFrame = TRUE;
        }
        else if (x+xhot < pHWCInfo->iScreenOffset_x)
        {
            pHWCInfo->iScreenOffset_x = x + xhot;
            if (pRDC->DeviceInfo.ScalerConfig.EnableDownScaling && pRDC->DeviceInfo.ScalerConfig.EnableHorDownScaler)
            {
                
                if ((iBpp == 4) || (iBpp == 3))
                {
                    pHWCInfo->iScreenOffset_x = ALIGN_TO_LB_16(pHWCInfo->iScreenOffset_x);
                }
                else if (iBpp == 2)
                {
                    pHWCInfo->iScreenOffset_x = ALIGN_TO_LB_32(pHWCInfo->iScreenOffset_x);
                }
            }
            else
            {
                
                if ((iBpp == 4) || (iBpp == 3))
                    pHWCInfo->iScreenOffset_x = ALIGN_TO_LB_2(pHWCInfo->iScreenOffset_x);
                else if (iBpp == 2)
                    pHWCInfo->iScreenOffset_x = ALIGN_TO_LB_4(pHWCInfo->iScreenOffset_x);
            }
            bNeedAdjustFrame = TRUE;
        }

        x -= pHWCInfo->iScreenOffset_x;


        if (y+yhot > pHWCInfo->iScreenOffset_y + iMaxVerCoor)
        {
           pHWCInfo->iScreenOffset_y = y + yhot - iMaxVerCoor;
           bNeedAdjustFrame = TRUE;
        }
        else if (y+yhot < pHWCInfo->iScreenOffset_y)
        {
            pHWCInfo->iScreenOffset_y = y + yhot;
            bNeedAdjustFrame = TRUE;
        }

        y -= pHWCInfo->iScreenOffset_y;

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "pHWCInfo->iScreenOffset (x,y) = (%d, %d)\n", pHWCInfo->iScreenOffset_x, pHWCInfo->iScreenOffset_y);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "bNeedAdjustFrame = %d\n", bNeedAdjustFrame);

#endif

        if (bNeedAdjustFrame)
        {
            
            if (pRDC->bRandRRotation)
            {
                switch (pRDC->rotate)
                {
                    case RR_Rotate_0:
                        vSetStartAddressCRT1(pRDC, pHWCInfo->iScreenOffset_x * pRDC->VideoModeInfo.Bpp + pHWCInfo->iScreenOffset_y * pRDC->VideoModeInfo.ScreenPitch);
                        break;
                    case RR_Rotate_90:
                        vSetStartAddressCRT1(pRDC,   pHWCInfo->iScreenOffset_x * pRDC->VideoModeInfo.Bpp + (pHWCInfo->iScreenOffset_y +pScrn->displayWidth - pRDC->VideoModeInfo.ScreenHeight) * pRDC->VideoModeInfo.ScreenPitch);
                        break;
                    case RR_Rotate_180:
                        vSetStartAddressCRT1(pRDC, (pScrn->displayWidth - pRDC->VideoModeInfo.ScreenWidth + pHWCInfo->iScreenOffset_x) * pRDC->VideoModeInfo.Bpp + pHWCInfo->iScreenOffset_y * pRDC->VideoModeInfo.ScreenPitch);
                        break;
                    case RR_Rotate_270:
                        vSetStartAddressCRT1(pRDC,   (pScrn->displayWidth - pRDC->VideoModeInfo.ScreenWidth + pHWCInfo->iScreenOffset_x) * pRDC->VideoModeInfo.Bpp + pHWCInfo->iScreenOffset_y * pRDC->VideoModeInfo.ScreenPitch);
                        break;
                    default:
                        xf86DrvMsgVerb(pScrn->scrnIndex, X_ERROR, ErrorLevel, "  Unexpected rotation in RDCAdjustFrame\n");
                        return;
                }
            }
            else
            {
                vSetStartAddressCRT1(pRDC, pHWCInfo->iScreenOffset_x * pRDC->VideoModeInfo.Bpp + pHWCInfo->iScreenOffset_y * pRDC->VideoModeInfo.ScreenPitch);
            }
            
            
            if (pRDC->OverlayStatus.bOverlayEnable)
            {
                iOverlaySrcWidth = pRDC->OverlayStatus.iSrcWidth;
                iOverlaySrcHeight = pRDC->OverlayStatus.iSrcHeight;

                iOverlayDstWidth = pRDC->OverlayStatus.iDstWidth;
                iOverlayDstHeight = pRDC->OverlayStatus.iDstHeight;
                
                iOverlayStartX = pRDC->OverlayStatus.iDstLeft - pHWCInfo->iScreenOffset_x;
                iOverlayStartY = pRDC->OverlayStatus.iDstTop - pHWCInfo->iScreenOffset_y;
                
                iOverlayEndX = iOverlayStartX + iOverlayDstWidth - 1;
                iOverlayEndY = iOverlayStartY + iOverlayDstHeight - 1;

                
                if (pRDC->ENGCaps & ENG_CAP_VIDEO_POST)
                {
                    if (iOverlayDstWidth < iOverlaySrcWidth)
                    {
                        iOverlayEndX = iOverlayStartX + iOverlayDstWidth;
                        iOverlaySrcWidth = iOverlayDstWidth;
                    }

                    if (iOverlayDstHeight < iOverlaySrcHeight)
                    {
                        iOverlayEndY = iOverlayStartY + iOverlayDstHeight;
                        iOverlaySrcHeight = iOverlayDstHeight;
                    }
                }
                
                
                iOverlayStartOffsetX = 0;
                iOverlayStartOffsetY = 0;
                
                if ((iOverlayEndX > 0) && 
                    (iOverlayEndX >= pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution))
                {
                    iOverlayEndX = 
                        pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution - 1;
                }
                
                if (iOverlayEndY >= pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution)
                {
                    iOverlayEndY = 
                        pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution - 1;
                }
                
                iOverlayDispCntX = 
                    (iOverlaySrcWidth * (iOverlayEndX - iOverlayStartX + 1) -1) / 
                    iOverlayDstWidth;

                iOverlayDispCntY = 
                    (iOverlaySrcHeight * (iOverlayEndY - iOverlayStartY + 1) -1) / 
                    iOverlayDstHeight;

                if (iOverlayStartX < 0)
                {
                    iOverlayStartOffsetX = 
                        (iOverlaySrcWidth * (-iOverlayStartX)) / iOverlayDstWidth;
                    iOverlayStartX = 0;
                }
                
                if (iOverlayStartY < 0)
                {
                    iOverlayStartOffsetY = 
                        (iOverlaySrcHeight * (-iOverlayStartY)) / iOverlayDstHeight;
                    iOverlayStartY = 0;
                }

                iPanningVDDstWdith = (iOverlayEndX - iOverlayStartX + 1);
                if (iOverlaySrcWidth > iPanningVDDstWdith)
                {
                    
                    
                    iOverlayFetchCnt = 
                        ((iPanningVDDstWdith << 1)  + 7) >> 3;
                }
                else
                {
                    
                    iOverlayFetchCnt = 
                        ((iOverlaySrcWidth << 1) + 7) >> 3;
                }

                if ((pRDC->ModePtr->HDisplay > pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution)&&
                    (iOverlaySrcWidth < iOverlayDstWidth))
                {
                    
                    
                    iOverlayFetchCnt = 
                        (((iOverlayDispCntX - iOverlayStartOffsetX) << 1) + 7) >> 3;
                }
                
                if (iOverlayFetchCnt < 4)
                    iOverlayFetchCnt = 4;

                if ((iOverlayEndX < 0) || 
                    (iOverlayStartX > pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution) ||
                    (iOverlayStartY > pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution))
                {
                    pRDC->OverlayStatus.ulVidDispCtrl &= ~VDPS_ENABLE;
                    pRDC->OverlayStatus.bPanningOverlayEnable = FALSE;
                    
                    while (!(GetReg(COLOR_INPUT_STATUS1_READ) & 0x08))
                    {
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "wait v sync\n");
                    }
                    
                    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_CTL) = pRDC->OverlayStatus.ulVidDispCtrl; 
                }
                else
                {
                    pRDC->OverlayStatus.ulVidDispCtrl |= VDPS_ENABLE;
                    pRDC->OverlayStatus.bPanningOverlayEnable = TRUE;
                    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_CTL) = pRDC->OverlayStatus.ulVidDispCtrl; 
                }

                if (pRDC->MMIOVPost)
                {
                    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_START_LOCATION) = (iOverlayStartX << 16) | iOverlayStartY;
                    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_END_LOCATION) = (iOverlayEndX << 16) | iOverlayEndY;
                    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_START_OFFSET) = (iOverlayStartOffsetX << 16) | iOverlayStartOffsetY;
                    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_SRC_DISP_COUNT_ON_SCR) = (iOverlayDispCntY << 16) | iOverlayDispCntX;
                    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FETCH_COUNT) = iOverlayFetchCnt;
                    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_REG_UPDATE_MOD_SELECT) = VDPS_FIRE;
                }
                else
                {
                    PKT_SC *pCMDQ;
                    if (pRDC->IoctlCR)
                    {
                        pSingleCMD = (PKT_SC *) malloc(PKT_SINGLE_LENGTH*6);
                        pCMDQ = pSingleCMD;
                    }
                    else
                        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*6);
                    pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_START_LOCATION_I;
                    pSingleCMD->PKT_SC_dwData[0] = (iOverlayStartX << 16) | iOverlayStartY;
                    pSingleCMD++;
                    pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_END_LOCATION_I;
                    pSingleCMD->PKT_SC_dwData[0] = (iOverlayEndX << 16) | iOverlayEndY;
                    pSingleCMD++;
                    pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_START_OFFSET_I;
                    pSingleCMD->PKT_SC_dwData[0] = (iOverlayStartOffsetX << 16) | iOverlayStartOffsetY;
                    pSingleCMD++;
                    pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_SRC_DISP_COUNT_ON_SCR_I;
                    pSingleCMD->PKT_SC_dwData[0] = (iOverlayDispCntY << 16) | iOverlayDispCntX;
                    pSingleCMD++;
                    pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_FETCH_COUNT_I;
                    pSingleCMD->PKT_SC_dwData[0] = iOverlayFetchCnt;
                    pSingleCMD++;
                    pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_REG_UPDATE_MOD_SELECT_I;
                    pSingleCMD->PKT_SC_dwData[0] = VDPS_FIRE;

                    
                    if (pRDC->IoctlCR)
                        FireCRCMDQ(pRDC->iFBDev, pCMDQ, 6);
                    else
                        mUpdateWritePointer;
                }
            }
            else
            {
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "overlay disabled\n");
            }
        }
    }

    if(x < 0)
    {
        x_offset = (-x) + pRDC->HWCInfo.offset_x;
        x = 0;
    }

    if(y < 0)
    {
        y_offset = (-y) + pRDC->HWCInfo.offset_y;
        y = 0;
    }
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "x =%d\n", x);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "xhot = %d\n", xhot);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "x_offset =%d\n", x_offset);
#endif

    if ( pRDC->DeviceInfo.ScalerConfig.EnableHorScaler)
    {
        if ((pRDC->DeviceInfo.ScalerConfig.EnableDownScaling) && (pRDC->DeviceInfo.ScalerConfig.EnableHorDownScaler))
        {
            x += xhot;
            x = (((x << 8) + ulHorScalingFactor - 1) / ulHorScalingFactor);
            x -= xhot;
            if (x < 0)
            {
                x_offset -= x;
                x = 0;
            }
        }
        else
            x = (((x << 12) + ulHorScalingFactor - 1) / ulHorScalingFactor);
#if HWC_DEBUG
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "H scaling factor = %x\n", ulHorScalingFactor);
#endif
    }

    y_end = y + (MAX_HWC_HEIGHT - y_offset);

    if (pRDC->DeviceInfo.ScalerConfig.EnableVerScaler)
    {
#if HWC_DEBUG
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "V scaling factor = %x\n", ulVerScalingFactor);
#endif
        if ((pRDC->DeviceInfo.ScalerConfig.EnableDownScaling) && (pRDC->DeviceInfo.ScalerConfig.EnableVerDownScaler))
        {
            y += yhot;
            y = (((y << 8) + ulVerScalingFactor - 1) / ulVerScalingFactor);
            y -= yhot;
            y_end += yhot;
            y_end =  (((y_end << 8) + ulVerScalingFactor - 1) / ulVerScalingFactor) - 1;
            y_end -= yhot;
            if (y < 0)
            {
                y_offset -= y;
                y = 0;
            }

        }
        else
        {
            y = (((y << 11) + ulVerScalingFactor - 2) / ulVerScalingFactor);
            y_end =  (((y_end << 11) + ulVerScalingFactor - 2) / ulVerScalingFactor) - 1;
        }
    }
    else
    {
        y_end--;
    }

    if (x_offset > 63)
        x_offset = 63;

    if (y_offset > 63)
        y_offset = 63;
       
    if (y_end < 0)
        y_end = 0;
        
    if(mode->Flags & V_DBLSCAN)
        y *= 2;

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "(x, y) = (%d, %d)\n", x, y);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "(x, y)_offset = (%d, %d)\n", x_offset, y_offset);
#endif
    
    *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_OFFSET)) = 
        (x_offset & 0x3F) | ((y_offset & 0x3F) << 8) | ((y_end & 0x7FF) << 16);
    *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_POSITION)) =
         (x & 0xFFF) | ((y & 0x7FF)<< 16);
    
    
    ulCursorCtrl = *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_CTRL)); 
    *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_CTRL)) = ulCursorCtrl;
    
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Exit RDCSetCursorPosition()== \n");
#endif
}
