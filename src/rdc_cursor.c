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
#include "xf86Resources.h"
#include "xf86RAC.h"
#include "xf86cmap.h"
#include "compiler.h"
#include "mibstore.h"
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


#include "xaa.h"
#include "xaarop.h"


#include "xf86Cursor.h"
#include "cursorstr.h"


#include "rdc.h"

#ifdef HWC

Bool RDCCursorInit(ScreenPtr pScreen);
Bool bInitHWC(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
static void RDCShowCursor(ScrnInfoPtr pScrn); 
void RDCHideCursor(ScrnInfoPtr pScrn);
static void RDCSetCursorPosition(ScrnInfoPtr pScrn, int x, int y);
static void RDCSetCursorColors(ScrnInfoPtr pScrn, int bg, int fg);
static void RDCLoadCursorImage(ScrnInfoPtr pScrn, UCHAR *src);
static Bool RDCUseHWCursor(ScreenPtr pScreen, CursorPtr pCurs);
static void RDCLoadCursorARGB(ScrnInfoPtr pScrn, CursorPtr pCurs);
static Bool RDCUseHWCursorARGB(ScreenPtr pScreen, CursorPtr pCurs);

static void RDCFireCursor(ScrnInfoPtr pScrn); 

//
// For 32bit data and MMIO control Hardware Cursor
void RDCHideCursor_HQ(ScrnInfoPtr pScrn);
static void RDCShowCursor_HQ(ScrnInfoPtr pScrn); 
static void RDCSetCursorColors_HQ(ScrnInfoPtr pScrn, int bg, int fg);
static void RDCLoadCursorImage_HQ(ScrnInfoPtr pScrn, UCHAR *src);
static void RDCLoadCursorARGB_HQ(ScrnInfoPtr pScrn, CursorPtr pCurs);
static void RDCSetCursorPosition_HQ(ScrnInfoPtr pScrn, int x, int y);


Bool
RDCCursorInit(ScreenPtr pScreen)
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
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCCursorInit()== \n");
#endif
    return(xf86InitCursor(pScreen, infoPtr));    
}

Bool bInitHWC(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
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
               
    SetIndexRegMask(CRTC_PORT, 0xCB, 0xFC, jReg);         
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCShowCursor()== \n");
#endif
}

void
RDCHideCursor(ScrnInfoPtr pScrn)
{
    RDCRecPtr  pRDC = RDCPTR(pScrn);
        
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCHideCursor()== \n");
#endif
    SetIndexRegMask(CRTC_PORT, 0xCB, 0xFC, 0x00);         
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
        y = (y * pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor + 2047) >> 11;
        y_end = y + (((MAX_HWC_HEIGHT - y_offset) * pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor + 2047) >> 11) - 1;
    }

    if(mode->Flags & V_DBLSCAN)
        y *= 2;
 
    
    SetIndexReg(CRTC_PORT, 0xC2, (UCHAR) (x_offset));     
    SetIndexReg(CRTC_PORT, 0xC3, (UCHAR) (y_offset));     
    SetIndexReg(CRTC_PORT, 0xC4, (UCHAR) (x & 0xFF));     
    SetIndexReg(CRTC_PORT, 0xC5, (UCHAR) ((x >> 8) & 0x0F));     
    SetIndexReg(CRTC_PORT, 0xC6, (UCHAR) (y & 0xFF));     
    SetIndexReg(CRTC_PORT, 0xC7, (UCHAR) ((y >> 8) & 0x07));     
    SetIndexReg(CRTC_PORT, 0xCC, (UCHAR) (y_end & 0xFF));     
    SetIndexReg(CRTC_PORT, 0xCD, (UCHAR) ((y_end >> 8) & 0x07));     
    
    
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
    ULONG       ulTempDstAnd32[2], ulTempDstXor32[2], ulTempDstData32[2];
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
                ulTempDstXor32[0] = ((jTempSrcXor32 >> k) & 0x01) ? 0x00004000L:0x00L;
                ulTempDstData32[0] = ((jTempSrcXor32 >> k) & 0x01) ? pRDC->HWCInfo.MonoHWC.fg:pRDC->HWCInfo.MonoHWC.bg;
                ulTempDstAnd32[1] = ((jTempSrcAnd32 >> (k-1)) & 0x01) ? 0x80000000L:0x00L;
                ulTempDstXor32[1] = ((jTempSrcXor32 >> (k-1)) & 0x01) ? 0x40000000L:0x00L;
                ulTempDstData32[1] = ((jTempSrcXor32 >> (k-1)) & 0x01) ? (pRDC->HWCInfo.MonoHWC.fg << 16):(pRDC->HWCInfo.MonoHWC.bg << 16);
                *((ULONG *) pjDstData) = ulTempDstAnd32[0] | ulTempDstXor32[0] | ulTempDstData32[0] | ulTempDstAnd32[1] | ulTempDstXor32[1] | ulTempDstData32[1];
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
    
    SetIndexReg(CRTC_PORT, 0xC8, (UCHAR) (ulPatternAddr & 0xFF));     
    SetIndexReg(CRTC_PORT, 0xC9, (UCHAR) ((ulPatternAddr >> 8) & 0xFF));     
    SetIndexReg(CRTC_PORT, 0xCA, (UCHAR) ((ulPatternAddr >> 16) & 0xFF)); 
    
    
    pRDC->HWCInfo.HWC_NUM_Next = (pRDC->HWCInfo.HWC_NUM_Next+1) % pRDC->HWCInfo.HWC_NUM;
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCLoadCursorImage()== \n");
#endif
}

static Bool 
RDCUseHWCursor(ScreenPtr pScreen, CursorPtr pCurs)
{
#if HWC_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCUseHWCursor(), return TRUE== \n");
#endif
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
    SetIndexReg(CRTC_PORT, 0xC8, (UCHAR) (ulPatternAddr & 0xFF));     
    SetIndexReg(CRTC_PORT, 0xC9, (UCHAR) ((ulPatternAddr >> 8) & 0xFF));     
    SetIndexReg(CRTC_PORT, 0xCA, (UCHAR) ((ulPatternAddr >> 16) & 0xFF)); 
    
    
    pRDC->HWCInfo.HWC_NUM_Next = (pRDC->HWCInfo.HWC_NUM_Next+1) % pRDC->HWCInfo.HWC_NUM;

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCLoadCursorARGB()== \n");
#endif
}

static Bool 
RDCUseHWCursorARGB(ScreenPtr pScreen, CursorPtr pCurs)
{
#if HWC_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCUseHWCursorARGB(), return TRUE== \n");
#endif
    return TRUE;
}

static void
RDCFireCursor(ScrnInfoPtr pScrn)
{
    RDCRecPtr  pRDC = RDCPTR(pScrn);
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCFireCursor()== \n");
#endif
    SetIndexRegMask(CRTC_PORT, 0xCB, 0xFF, 0x00);         
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCFireCursor()== \n");
#endif
}

//
// For 32bit data and MMIO control Hardware Cursor
void RDCHideCursor_HQ(ScrnInfoPtr pScrn)
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
    ULONG       ulTempDstXor32, ulTempDstAnd32, ulTempDstData32;
    ULONG       *pulDstData;
  
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetCursorColors_HQ(bg = %x, fg = %x)== \n", bg, fg);
#endif

    pRDC->HWCInfo.MonoHWC.fg = fg;
    pRDC->HWCInfo.MonoHWC.bg = bg;

    pjSrcXor = pRDC->HWCInfo.MonoHWC.ucXorBitmap;
    pjSrcAnd = pRDC->HWCInfo.MonoHWC.ucAndBitmap;
    pulDstData = (ULONG*)(pRDC->HWCInfo.MonoHWC.pjPatternVirtualAddr);

    for (j = 0; j < MAX_HWC_HEIGHT; j++)
    {
        for (i = 0; i < (MAX_HWC_WIDTH/8); i++)
        {
            jTempSrcXor = *pjSrcXor;
            jTempSrcAnd = *pjSrcAnd;

            for (k=7; k>=0; k--)
            {
                ulTempDstAnd32 = ((jTempSrcAnd >> k) & 0x01) ? 0x80000000L:0x00L;
                ulTempDstXor32 = ((jTempSrcXor >> k) & 0x01) ? 0x40000000L:0x00L;
                ulTempDstData32 = ((jTempSrcXor >> k) & 0x01) ? fg:bg;
                *pulDstData = ulTempDstAnd32|ulTempDstXor32|ulTempDstData32;
                pulDstData++;
            }

            pjSrcXor++;
            pjSrcAnd++;
        }        
    }                    
}

static void RDCLoadCursorImage_HQ(ScrnInfoPtr pScrn, UCHAR *src)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);   
    UCHAR       *pjSrcAnd, *pjSrcXor;
    ULONG       ulPatternOffset;
    
#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCLoadCursorImage_HQ()== \n");
#endif 
    
    pRDC->HWCInfo.cursortype = HWC_MONO;
    pRDC->HWCInfo.width  = (USHORT) MAX_HWC_WIDTH;
    pRDC->HWCInfo.height = (USHORT) MAX_HWC_HEIGHT;
    pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
    pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;

    
    pjSrcXor = src;
    pjSrcAnd = src + (MAX_HWC_WIDTH*MAX_HWC_HEIGHT/8);

    memcpy(&(pRDC->HWCInfo.MonoHWC.ucXorBitmap),pjSrcXor, sizeof(pRDC->HWCInfo.MonoHWC.ucXorBitmap));
    memcpy(&(pRDC->HWCInfo.MonoHWC.ucAndBitmap),pjSrcAnd, sizeof(pRDC->HWCInfo.MonoHWC.ucAndBitmap));

    
    ulPatternOffset = pRDC->HWCInfo.ulHWCOffsetAddr+HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next;
    pRDC->HWCInfo.MonoHWC.pjPatternVirtualAddr = pRDC->FBVirtualAddr + ulPatternOffset;
    *((ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_ADDRESS)) = ((ulPatternOffset >> 3) & 0xFFFFFF);

    
    pRDC->HWCInfo.HWC_NUM_Next = (pRDC->HWCInfo.HWC_NUM_Next+1) % pRDC->HWCInfo.HWC_NUM;
    
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

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCLoadCursorARGB_HQ()== \n");
#endif
    
    pRDC->HWCInfo.cursortype = HWC_COLOR;
    pRDC->HWCInfo.width  = pCurs->bits->width;
    pRDC->HWCInfo.height = pCurs->bits->height;
    pRDC->HWCInfo.xhot = pCurs->bits->xhot;
    pRDC->HWCInfo.yhot = pCurs->bits->yhot;
    pRDC->HWCInfo.offset_x = MAX_HWC_WIDTH - pRDC->HWCInfo.width;
    pRDC->HWCInfo.offset_y = MAX_HWC_HEIGHT - pRDC->HWCInfo.height;
    
    
    ulSrcWidthInByte = pRDC->HWCInfo.width << 2;  
    ulSrcHeight = pRDC->HWCInfo.height;  
        
    ulDstCursorPitch = MAX_HWC_WIDTH << 2;

    pjSrcXor  = (UCHAR *) pCurs->bits->argb;
    pjDstXor  = (UCHAR *) pRDC->HWCInfo.pjHWCVirtualAddr + 
                HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next +
                (MAX_HWC_HEIGHT - ulSrcHeight) * ulDstCursorPitch +
                (ulDstCursorPitch - ulSrcWidthInByte);
    
    for (j = 0; j < ulSrcHeight; j++)
    {
        memcpy(pjDstXor,pjSrcXor, ulSrcWidthInByte);
        
        
        pjSrcXor += ulSrcWidthInByte;
        pjDstXor += ulDstCursorPitch;
    } 

    
    ulPatternAddr = 
        ((pRDC->HWCInfo.ulHWCOffsetAddr +HQ_HWC_SIZE*pRDC->HWCInfo.HWC_NUM_Next) >> 3);

    *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_ADDRESS)) = (ulPatternAddr & 0xFFFFFF);
    
    
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
    Bool        bNeedAdjustFrame = false;

#if HWC_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Enter RDCSetCursorPosition_HQ(x = %d, y = %d)== \n", x, y);
#endif

    xhot = pRDC->HWCInfo.xhot;
    yhot = pRDC->HWCInfo.yhot;

    x_offset = pRDC->HWCInfo.offset_x;
    y_offset = pRDC->HWCInfo.offset_y;

    
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

    
    if ((pRDC->DeviceInfo.ucDeviceID == LCDINDEX) &&
        (DEVICE_ID(pRDC->PciInfo) == PCI_CHIP_M2010))
    {
        if (x+xhot >= pHWCInfo->iScreenOffset_x + (int)pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution)
        {
           pHWCInfo->iScreenOffset_x = x + xhot - ((int)(pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution) - 1);
           x = (int)(pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution) - 1 - xhot;
           bNeedAdjustFrame = true;
        }
        else if (x+xhot < pHWCInfo->iScreenOffset_x)
        {
            pHWCInfo->iScreenOffset_x = x + xhot;
            x = -xhot;
            bNeedAdjustFrame = true;
        }
        else
        {
            x -= pHWCInfo->iScreenOffset_x;
        }

        if (y+yhot >= pHWCInfo->iScreenOffset_y + (int)pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution)
        {
           pHWCInfo->iScreenOffset_y = y + yhot - ((int)pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution - 1);
           y = (int)pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution - 1 - yhot;
           bNeedAdjustFrame = true;
        }
        else if (y+yhot < pHWCInfo->iScreenOffset_y)
        {
            pHWCInfo->iScreenOffset_y = y + yhot;
            y = -yhot;
            bNeedAdjustFrame = true;
        }
        else
        {
            y -= pHWCInfo->iScreenOffset_y;
        }

        if (bNeedAdjustFrame)
        {
            RDCAdjustFrame(pScrn->scrnIndex, pHWCInfo->iScreenOffset_x, pHWCInfo->iScreenOffset_y, 0);
            
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
                    
                    while (!(GetReg(INPUT_STATUS1_READ) & 0x08))
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

                *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_START_LOCATION) = (iOverlayStartX << 16) | iOverlayStartY;
                *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_END_LOCATION) = (iOverlayEndX << 16) | iOverlayEndY;
                *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_START_OFFSET) = (iOverlayStartOffsetX << 16) | iOverlayStartOffsetY;
                *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_SRC_DISP_COUNT_ON_SCR) = (iOverlayDispCntY << 16) | iOverlayDispCntX;
                *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FETCH_COUNT) = iOverlayFetchCnt;
                *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_REG_UPDATE_MOD_SELECT) = VDPS_FIRE;
            }
            else
            {
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "overlay disabled\n");
            }
        }
    }

    
    y++;

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

    if ( pRDC->DeviceInfo.ScalerConfig.EnableHorScaler)
    {
        x = (x * pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor + 4095) >> 12;
#if HWC_DEBUG
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "H scaling factor = %x\n", pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor);
#endif

    }

    y_end = y + (MAX_HWC_HEIGHT - y_offset);

    if (pRDC->DeviceInfo.ScalerConfig.EnableVerScaler)
    {
        y = (y * pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor + 2047) >>11;
        y_end =  ((y_end * pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor + 2047) >> 11) - 1;
    }
    else
    {
        y_end--;
    }

    if(mode->Flags & V_DBLSCAN)
        y *= 2;
 
    
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
#endif    
