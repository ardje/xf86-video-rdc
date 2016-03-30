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


#ifdef USE_XAA
#include "xaa.h"
#include "xaarop.h"
#endif


#include "xf86Cursor.h"


#include "rdc.h"

#ifdef Accel_2D

int RDCXAACopyROP[16] =
{
    ROP_0,               
    ROP_DSa,             
    ROP_SDna,            
    ROP_S,               
    ROP_DSna,            
    ROP_D,               
    ROP_DSx,             
    ROP_DSo,             
    ROP_DSon,            
    ROP_DSxn,            
    ROP_Dn,              
    ROP_SDno,            
    ROP_Sn,              
    ROP_DSno,            
    ROP_DSan,            
    ROP_1                
};

int RDCXAAPatternROP[16]=
{
    ROP_0,
    ROP_DPa,
    ROP_PDna,
    ROP_P,
    ROP_DPna,
    ROP_D,
    ROP_DPx,
    ROP_DPo,
    ROP_DPon,
    ROP_PDxn,
    ROP_Dn,
    ROP_PDno,
    ROP_Pn,
    ROP_DPno,
    ROP_DPan,
    ROP_1
};


extern Bool bCREnable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern void vCRDisable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern void vCRWaitEngIdle(ScrnInfoPtr pScrn, RDCRecPtr pRDC);

extern Bool bEnable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern void vDisable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern void vWaitEngIdle(ScrnInfoPtr pScrn, RDCRecPtr pRDC);

extern UCHAR *pjRequestCMDQ(RDCRecPtr pRDC, ULONG ulDataLen);
extern Bool bGetLineTerm(_LINEInfo *LineInfo, LINEPARAM *dsLineParam);


Bool RDCAccelInit(ScreenPtr pScreen);
static void RDCSync(ScrnInfoPtr pScrn);
static void RDCSetupForScreenToScreenCopy(ScrnInfoPtr pScrn, 
                                          int xdir, int ydir, int rop,
                                          unsigned int planemask, int trans_color);
static void RDCSubsequentScreenToScreenCopy(ScrnInfoPtr pScrn, int x1, int y1, int x2,
                                            int y2, int w, int h);                                
static void RDCSetupForSolidFill(ScrnInfoPtr pScrn,
                                 int color, int rop, unsigned int planemask);
static void RDCSubsequentSolidFillRect(ScrnInfoPtr pScrn,
                                       int dst_x, int dst_y, int width, int height);  
static void RDCSetupForSolidLine(ScrnInfoPtr pScrn,
                                 int color, int rop, unsigned int planemask);
static void RDCSubsequentSolidHorVertLine(ScrnInfoPtr pScrn,
                                          int x, int y, int len, int dir);
static void RDCSetupForDashedLine(ScrnInfoPtr pScrn,
                                  int fg, int bg, int rop, unsigned int planemask,
                                  int length, UCHAR *pattern);
static void RDCSetupForMonoPatternFill(ScrnInfoPtr pScrn,
                                       int patx, int paty, int fg, int bg,
                                       int rop, unsigned int planemask);
static void RDCSubsequentMonoPatternFill(ScrnInfoPtr pScrn,
                                         int patx, int paty,
                                         int x, int y, int w, int h);
static void RDCSetupForColor8x8PatternFill(ScrnInfoPtr pScrn, int patx, int paty,
                               int rop, unsigned int planemask, int trans_col);
static void RDCSubsequentColor8x8PatternFillRect(ScrnInfoPtr pScrn, int patx, int paty,
                                                 int x, int y, int w, int h);
static void RDCSetupForCPUToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                  int fg, int bg,
                                                  int rop, unsigned int planemask);
static void RDCSubsequentCPUToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                    int x, int y,
                                                    int width, int height, int skipleft);
static void RDCSetupForScreenToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                     int fg, int bg,
                                                     int rop, unsigned int planemask);
static void RDCSubsequentScreenToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                                       int x, int y, int width, int height,
                                                       int src_x, int src_y, int offset);
static void RDCSetClippingRectangle(ScrnInfoPtr pScrn,
                                    int left, int top, int right, int bottom);
static void RDCDisableClipping(ScrnInfoPtr pScrn); 

static void RDCSubsequentSolidTwoPointLine(ScrnInfoPtr pScrn,
                                           int x1, int y1, int x2, int y2, int flags);                                       
static void RDCSubsequentDashedTwoPointLine(ScrnInfoPtr pScrn,
                                            int x1, int y1, int x2, int y2,
                                            int flags, int phase);


static ExaDriverPtr RDCInitExa(ScreenPtr pScreen);

Bool
RDCAccelInit(ScreenPtr pScreen)
{
    XAAInfoRecPtr  infoPtr;
    ScrnInfoPtr    pScrn = xf86Screens[pScreen->myNum];
    RDCRecPtr      pRDC = RDCPTR(pScrn);
    
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCAccelInit()== \n");
#endif

    if (pRDC->ENGCaps & ENG_CAP_CR_SUPPORT)
    {
        pRDC->CMDQInfo.Disable2D = vCRDisable2D;
        pRDC->CMDQInfo.Enable2D =bCREnable2D ;
        pRDC->CMDQInfo.WaitEngIdle = vCRWaitEngIdle;
    }
    else
    {
        pRDC->CMDQInfo.Disable2D = vDisable2D;
        pRDC->CMDQInfo.Enable2D = bEnable2D;
        pRDC->CMDQInfo.WaitEngIdle = vWaitEngIdle;
    }

    if (pRDC->useEXA) 
    {
	    pRDC->exaDriverPtr = RDCInitExa(pScreen);
	    if (!pRDC->exaDriverPtr) 
	    {
    	    
            xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Init EXA Fail== \n");
    	    pRDC->noAccel = TRUE;
    	    return FALSE;
	    }

	    xf86DrvMsg(pScrn->scrnIndex, X_INFO,"[EXA] Enabled EXA acceleration.\n");
	    return TRUE;
    }



    pRDC->AccelInfoPtr = infoPtr = XAACreateInfoRec();
    if (!infoPtr)
    {
#if Accel_2D_DEBUG
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Exit1 RDCAccelInit(), return FALSE== \n");
#endif
        return FALSE;
    }
    infoPtr->Flags = LINEAR_FRAMEBUFFER |
                     OFFSCREEN_PIXMAPS |
                     PIXMAP_CACHE;

    
    if (pRDC->ENGCaps & ENG_CAP_Sync)
        infoPtr->Sync = RDCSync;

    
    if (pRDC->ENGCaps & ENG_CAP_ScreenToScreenCopy)
    {    
        infoPtr->SetupForScreenToScreenCopy =  RDCSetupForScreenToScreenCopy;
        infoPtr->SubsequentScreenToScreenCopy = RDCSubsequentScreenToScreenCopy;
        infoPtr->ScreenToScreenCopyFlags = NO_TRANSPARENCY | NO_PLANEMASK;
    }

    
    if (pRDC->ENGCaps & ENG_CAP_SolidFill)
    {    
        infoPtr->SetupForSolidFill = RDCSetupForSolidFill;
        infoPtr->SubsequentSolidFillRect = RDCSubsequentSolidFillRect;
        infoPtr->SolidFillFlags = NO_PLANEMASK;
    } 
        
    
    if (pRDC->ENGCaps & ENG_CAP_SolidLine)
    {    
        infoPtr->SetupForSolidLine = RDCSetupForSolidLine;            
        infoPtr->SubsequentSolidTwoPointLine = RDCSubsequentSolidTwoPointLine;
        infoPtr->SubsequentSolidHorVertLine = RDCSubsequentSolidHorVertLine;    
        infoPtr->SolidLineFlags = NO_PLANEMASK;
    }

    
    if (pRDC->ENGCaps & ENG_CAP_DashedLine)
    {
        infoPtr->SetupForDashedLine = RDCSetupForDashedLine;        
        infoPtr->SubsequentDashedTwoPointLine = RDCSubsequentDashedTwoPointLine;
        infoPtr->DashPatternMaxLength = 64;
        infoPtr->DashedLineFlags = NO_PLANEMASK |
                       LINE_PATTERN_MSBFIRST_LSBJUSTIFIED;
    }                                              

    
    if (pRDC->ENGCaps & ENG_CAP_Mono8x8PatternFill)
    {    
        infoPtr->SetupForMono8x8PatternFill = RDCSetupForMonoPatternFill;
        infoPtr->SubsequentMono8x8PatternFillRect = RDCSubsequentMonoPatternFill;
        infoPtr->Mono8x8PatternFillFlags = NO_PLANEMASK |
                                           NO_TRANSPARENCY |
                                           HARDWARE_PATTERN_SCREEN_ORIGIN |
                                           HARDWARE_PATTERN_PROGRAMMED_BITS |
                                           BIT_ORDER_IN_BYTE_MSBFIRST;
    }                           
                           
    
    if (pRDC->ENGCaps & ENG_CAP_Color8x8PatternFill)
    {    
        infoPtr->SetupForColor8x8PatternFill = RDCSetupForColor8x8PatternFill;
        infoPtr->SubsequentColor8x8PatternFillRect = RDCSubsequentColor8x8PatternFillRect;
        infoPtr->Color8x8PatternFillFlags = NO_PLANEMASK |
                                            NO_TRANSPARENCY |    
                                            HARDWARE_PATTERN_SCREEN_ORIGIN;
    }                         
                     
    
    if (pRDC->ENGCaps & ENG_CAP_CPUToScreenColorExpand)
    {    
        infoPtr->SetupForCPUToScreenColorExpandFill = RDCSetupForCPUToScreenColorExpandFill;
        infoPtr->SubsequentCPUToScreenColorExpandFill = RDCSubsequentCPUToScreenColorExpandFill;
        infoPtr->ColorExpandRange = MAX_PATReg_Size;
        infoPtr->ColorExpandBase = MMIOREG_PAT;
        infoPtr->CPUToScreenColorExpandFillFlags = NO_PLANEMASK |
                                                   BIT_ORDER_IN_BYTE_MSBFIRST;
    }                                  

    
    if (pRDC->ENGCaps & ENG_CAP_ScreenToScreenColorExpand)
    {    
        infoPtr->SetupForScreenToScreenColorExpandFill = RDCSetupForScreenToScreenColorExpandFill;
        infoPtr->SubsequentScreenToScreenColorExpandFill = RDCSubsequentScreenToScreenColorExpandFill;
        infoPtr->ScreenToScreenColorExpandFillFlags = NO_PLANEMASK |
                                                      BIT_ORDER_IN_BYTE_MSBFIRST;
    }                                                  
                                              
    
    if (pRDC->ENGCaps & ENG_CAP_Clipping)
    {    
        infoPtr->SetClippingRectangle = RDCSetClippingRectangle;
        infoPtr->DisableClipping = RDCDisableClipping;
        infoPtr->ClippingFlags = HARDWARE_CLIP_SCREEN_TO_SCREEN_COPY     |
                                 HARDWARE_CLIP_MONO_8x8_FILL |
                                 HARDWARE_CLIP_COLOR_8x8_FILL |
                                 HARDWARE_CLIP_SOLID_LINE | 
                                 HARDWARE_CLIP_DASHED_LINE | 
                                 HARDWARE_CLIP_SOLID_LINE; 
    }                         

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit2 RDCAccelInit()== \n");
#endif

    return(XAAInit(pScreen, infoPtr));
} 


static void
RDCSync(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCSync()== \n");
#endif
    
    pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit RDCSync()== \n");
#endif
} 


static void RDCSetupForScreenToScreenCopy(ScrnInfoPtr pScrn,
                                          int xdir, int ydir, int rop,
                                          unsigned int planemask, int trans_color)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG  cmdreg = 0;
    
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForScreenToScreenCopy(xdir = %d, ydir = %d, rop = 0x%x, planemask = 0x%x, trans_color = 0x%x)== \n", xdir, ydir, rop, planemask, trans_color);
#endif    
    
    cmdreg = CMD_BITBLT;
    switch (pRDC->VideoModeInfo.bitsPerPixel)
    {
        case 8:
            cmdreg |= CMD_COLOR_08;
            break;
        case 15:
        case 16:
            cmdreg |= CMD_COLOR_16;
            break;    
        case 24:
        case 32:
            cmdreg |= CMD_COLOR_32;
            break;        
    }
    
    cmdreg |= (RDCXAACopyROP[rop] << 8);
    
    if (xdir == -1)
    {
        cmdreg |= CMD_X_DEC;
    }
    if (ydir == -1)
    {
        cmdreg |= CMD_Y_DEC;
    }
    
    pRDC->ulCMDReg = cmdreg;
     
    if (!pRDC->MMIO2D)
    {   
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*2);
                
        RDCSetupSRCPitch(pSingleCMD, pRDC->VideoModeInfo.ScreenPitch);  
        pSingleCMD++;
        RDCSetupDSTPitchHeight(pSingleCMD, pRDC->VideoModeInfo.ScreenPitch, -1);
    }
    else
    {
                        
        RDCSetupSRCPitch_MMIO(pRDC->VideoModeInfo.ScreenPitch);  
        RDCSetupDSTPitchHeight_MMIO(pRDC->VideoModeInfo.ScreenPitch, -1);    
    }

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetupForScreenToScreenCopy()== \n");
#endif
} 

static void
RDCSubsequentScreenToScreenCopy(ScrnInfoPtr pScrn, int x1, int y1, int x2,
                                int y2, int width, int height)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    int src_x, src_y, dst_x, dst_y;
    ULONG srcbase, dstbase, cmdreg;

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSubsequentScreenToScreenCopy(x1 = %d, y1 = %d, x2 = %d, y2 = %d, width = %d, height = %d)== \n", x1, y1, x2, y2, width, height);
#endif        
    
    cmdreg = pRDC->ulCMDReg;

    if (pRDC->EnableClip)
        cmdreg |= CMD_ENABLE_CLIP;
    else
        cmdreg &= ~CMD_ENABLE_CLIP;        
        
    srcbase = pRDC->VideoModeInfo.ScreenPitch*y1;
    dstbase = pRDC->VideoModeInfo.ScreenPitch*y2;
          
    if (cmdreg & CMD_X_DEC)
    {
        src_x = x1 + width - 1;
        dst_x = x2 + width - 1;
    }
    else
    {
        src_x = x1;
        dst_x = x2;    
    }

    if (cmdreg & CMD_Y_DEC)
    {
        src_y = height - 1;
        dst_y = height - 1;
    }
    else
    {
        src_y = 0;
        dst_y = 0;    
    }
        
    if (!pRDC->MMIO2D)        
    {
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*6);
      
        RDCSetupSRCBase(pSingleCMD, srcbase);
        pSingleCMD++;
        RDCSetupDSTBase(pSingleCMD, dstbase);
        pSingleCMD++;
        RDCSetupDSTXY(pSingleCMD, dst_x, dst_y);
        pSingleCMD++;
        RDCSetupSRCXY(pSingleCMD, src_x, src_y);
        pSingleCMD++;
        RDCSetupRECTXY(pSingleCMD, width, height);
        pSingleCMD++;
        RDCSetupCMDReg(pSingleCMD, cmdreg);

        
        mUpdateWritePointer;
    
    }
    else
    {
        RDCSetupSRCBase_MMIO(srcbase);
        RDCSetupDSTBase_MMIO(dstbase);
        RDCSetupDSTXY_MMIO(dst_x, dst_y);    
        RDCSetupSRCXY_MMIO(src_x, src_y);    
        RDCSetupRECTXY_MMIO(width, height);    
        RDCSetupCMDReg_MMIO(cmdreg);       
                
        pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);
    }
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSubsequentScreenToScreenCopy()== \n");
#endif
} 

static void
RDCSetupForSolidFill(ScrnInfoPtr pScrn,
                     int color, int rop, unsigned int planemask)
{
    
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG cmdreg;

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForSolidFill(color = 0x%x, rop = 0x%x, planemake = 0x%x)== \n", color, rop, planemask);
#endif
    
    cmdreg = CMD_BITBLT | CMD_PAT_FGCOLOR;
    switch (pRDC->VideoModeInfo.bitsPerPixel)
    {
    case 8:
        cmdreg |= CMD_COLOR_08;
        break;
    case 15:
    case 16:
        cmdreg |= CMD_COLOR_16;
        break;    
    case 24:
    case 32:
        cmdreg |= CMD_COLOR_32;
        break;        
    }
    cmdreg |= (RDCXAAPatternROP[rop] << 8);
    pRDC->ulCMDReg = cmdreg;
            
    if (!pRDC->MMIO2D)                    
    {
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*2);

        RDCSetupDSTPitchHeight(pSingleCMD, pRDC->VideoModeInfo.ScreenPitch, -1);
        pSingleCMD++;
        RDCSetupFG(pSingleCMD, color);        
    }
    else
    {
        RDCSetupDSTPitchHeight_MMIO(pRDC->VideoModeInfo.ScreenPitch, -1);
        RDCSetupFG_MMIO(color);                
    }

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetupForSolidFill()== \n");           
#endif
} 


static void
RDCSubsequentSolidFillRect(ScrnInfoPtr pScrn,
                           int dst_x, int dst_y, int width, int height)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG dstbase, cmdreg;        

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSubsequentSolidFillRect(dst_x = %d, dst_y = %d, width = %d, height = %d)== \n", dst_x, dst_y, width, height);           
#endif
    
    cmdreg = pRDC->ulCMDReg;
    if (pRDC->EnableClip)
        cmdreg |= CMD_ENABLE_CLIP;
    else
        cmdreg &= ~CMD_ENABLE_CLIP;            
    dstbase = 0;

    if (dst_y >= pScrn->virtualY) 
    {   
        dstbase=pRDC->VideoModeInfo.ScreenPitch*dst_y;
        dst_y=0;
    }
                  
    if (!pRDC->MMIO2D)                    
    {                  
            
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*4);

        RDCSetupDSTBase(pSingleCMD, dstbase);
        pSingleCMD++;    
        RDCSetupDSTXY(pSingleCMD, dst_x, dst_y);
        pSingleCMD++;    
        RDCSetupRECTXY(pSingleCMD, width, height);
        pSingleCMD++;    
        RDCSetupCMDReg(pSingleCMD, cmdreg);        
      
        
        mUpdateWritePointer;
                
    }
    else
    {                  
        RDCSetupDSTBase_MMIO(dstbase);
        RDCSetupDSTXY_MMIO(dst_x, dst_y);
        RDCSetupRECTXY_MMIO(width, height);
        RDCSetupCMDReg_MMIO(cmdreg);        
 
        pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);
      
    }

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSubsequentSolidFillRect()== \n");           
#endif
} 


static void RDCSetupForSolidLine(ScrnInfoPtr pScrn, 
                                 int color, int rop, unsigned int planemask)
{

    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG  cmdreg;
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==RDCSetupForSolidLine(color = 0x%x, rop = 0x%x, planemask = 0x%x)\n==", color, rop, planemask);
#endif
    
    cmdreg = CMD_BITBLT;
    
    switch (pRDC->VideoModeInfo.bitsPerPixel)
    {
    case 8:
        cmdreg |= CMD_COLOR_08;
        break;
    case 15:
    case 16:
        cmdreg |= CMD_COLOR_16;
        break;    
    case 24:
    case 32:
        cmdreg |= CMD_COLOR_32;
        break;        
    }
    cmdreg |= (RDCXAAPatternROP[rop] << 8);  
    
    pRDC->ulCMDReg = cmdreg;
     
    if (!pRDC->MMIO2D)
    {   
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*3);

        RDCSetupDSTPitchHeight(pSingleCMD, pRDC->VideoModeInfo.ScreenPitch, -1);
        pSingleCMD++;
        RDCSetupFG(pSingleCMD, color);  
        pSingleCMD++;
        RDCSetupBG(pSingleCMD, 0);
        
    }
    else
    {
           
        RDCSetupDSTPitchHeight_MMIO(pRDC->VideoModeInfo.ScreenPitch, -1);                     
        RDCSetupFG_MMIO(color);  
        RDCSetupBG_MMIO(0);            
    }
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetupForSolidLine()== \n");           
#endif
} 


static void RDCSubsequentSolidHorVertLine(ScrnInfoPtr pScrn,
                                          int x, int y, int len, int dir)
{

    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG dstbase, cmdreg;   
    int width, height;
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSubsequentSolidHorVertLine(x = %d, y = %d, len = %d, dir = %d)\n==", x, y, len, dir);
#endif
            
    
    cmdreg = (pRDC->ulCMDReg & (~CMD_MASK)) | CMD_BITBLT;
    if (pRDC->EnableClip)
        cmdreg |= CMD_ENABLE_CLIP;
    else
        cmdreg &= ~CMD_ENABLE_CLIP;            
    dstbase = 0;
    
    if(dir == DEGREES_0)
    {            
        width  = len;
        height = 1;    
    } 
    else 
    {                    
        width  = 1;
        height = len;            
    }
              
    if ((y + height) >= pScrn->virtualY) 
    {   
        dstbase=pRDC->VideoModeInfo.ScreenPitch*y;
        y=0;
    }
              
    
    if (!pRDC->MMIO2D)                    
    {                  
            
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*4);

        RDCSetupDSTBase(pSingleCMD, dstbase);
        pSingleCMD++;    
        RDCSetupDSTXY(pSingleCMD, x, y);
        pSingleCMD++;    
        RDCSetupRECTXY(pSingleCMD, width, height);
        pSingleCMD++;    
        RDCSetupCMDReg(pSingleCMD, cmdreg);        
      
        
        mUpdateWritePointer;
               
    }
    else
    {                  
        RDCSetupDSTBase_MMIO(dstbase);
        RDCSetupDSTXY_MMIO(x, y);
        RDCSetupRECTXY_MMIO(width, height);
        RDCSetupCMDReg_MMIO(cmdreg);        
 
        pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);
    }

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSubsequentSolidHorVertLine()== \n");           
#endif
} 


static void
RDCSetupForDashedLine(ScrnInfoPtr pScrn,
                      int fg, int bg, int rop, unsigned int planemask,
                      int length, UCHAR *pattern)
{

    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG  cmdreg;
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForDashedLine(fg = 0x%x, bg = 0x%x, rop = 0x%x, planemask = 0x%x, length = %d)\n==", fg, bg, rop, planemask, length);
#endif
    
    cmdreg = CMD_LINEDRAW | CMD_RESET_STYLE_COUNTER | CMD_ENABLE_LINE_STYLE;
    
    switch (pRDC->VideoModeInfo.bitsPerPixel)
    {
    case 8:
        cmdreg |= CMD_COLOR_08;
        break;
    case 15:
    case 16:
        cmdreg |= CMD_COLOR_16;
        break;    
    case 24:
    case 32:
        cmdreg |= CMD_COLOR_32;
        break;        
    }
    
    cmdreg |= (RDCXAAPatternROP[rop] << 8);  

    if(bg == -1) 
    {
        cmdreg |= CMD_TRANSPARENT;    
        bg = 0;
    }

    cmdreg |= (((length-1) & 0x3F) << 24);        
    pRDC->ulCMDReg = cmdreg;
     
    if (!pRDC->MMIO2D)
    {   
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*5);

        RDCSetupDSTPitchHeight(pSingleCMD, pRDC->VideoModeInfo.ScreenPitch, -1);
        pSingleCMD++;
        RDCSetupFG(pSingleCMD, fg);  
        pSingleCMD++;
        RDCSetupBG(pSingleCMD, bg); 
        pSingleCMD++;
        RDCSetupLineStyle1(pSingleCMD, *pattern);
        pSingleCMD++;
        RDCSetupLineStyle2(pSingleCMD, *(pattern+4));                   
                       
    }
    else
    {
           
        RDCSetupDSTPitchHeight_MMIO(pRDC->VideoModeInfo.ScreenPitch, -1);
        RDCSetupFG_MMIO(fg);  
        RDCSetupBG_MMIO(bg); 
        RDCSetupLineStyle1_MMIO(*pattern);
        RDCSetupLineStyle2_MMIO(*(pattern+4));                
    }
                                     
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetupForDashedLine()== \n");           
#endif
}


static void
RDCSetupForMonoPatternFill(ScrnInfoPtr pScrn,
                           int patx, int paty, int fg, int bg,
                           int rop, unsigned int planemask)
{
    
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG cmdreg;

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFill(patx = %d, paty = %d, fg = 0x%x, bg = 0x%x, rop = 0x%x, planemask = 0x%x)\n==", patx, paty, fg, bg, rop, planemask);
#endif
    
    cmdreg = CMD_BITBLT | CMD_PAT_MONOMASK;
    switch (pRDC->VideoModeInfo.bitsPerPixel)
    {
    case 8:
        cmdreg |= CMD_COLOR_08;
        break;
    case 15:
    case 16:
        cmdreg |= CMD_COLOR_16;
        break;    
    case 24:
    case 32:
        cmdreg |= CMD_COLOR_32;
        break;        
    }
    
    cmdreg |= (RDCXAAPatternROP[rop] << 8);
    pRDC->ulCMDReg = cmdreg;
            
    if (!pRDC->MMIO2D)                    
    {
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*5);

        RDCSetupDSTPitchHeight(pSingleCMD, pRDC->VideoModeInfo.ScreenPitch, -1);
        pSingleCMD++;
        RDCSetupFG(pSingleCMD, fg);  
        pSingleCMD++;
        RDCSetupBG(pSingleCMD, bg);
        pSingleCMD++;
        RDCSetupMONO1(pSingleCMD, patx);  
        pSingleCMD++;
        RDCSetupMONO2(pSingleCMD, paty);                           
    }
    else
    {
        RDCSetupDSTPitchHeight_MMIO(pRDC->VideoModeInfo.ScreenPitch, -1);
        RDCSetupFG_MMIO(fg);   
        RDCSetupBG_MMIO(bg);
        RDCSetupMONO1_MMIO(patx);  
        RDCSetupMONO2_MMIO(paty);                         
    }
               
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetupForMonoPatternFill()== \n");           
#endif
} 

                      
static void
RDCSubsequentMonoPatternFill(ScrnInfoPtr pScrn,
                             int patx, int paty,
                             int dst_x, int dst_y, int width, int height)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG dstbase, cmdreg;        
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSubsequentMonoPatternFill(patx = %d, paty = %d, dst_x = %d, dst_y = %d, width = %d, height = %d,)\n==", patx, paty, dst_x, dst_y, width, height);
#endif
            
    
    cmdreg = pRDC->ulCMDReg;
    if (pRDC->EnableClip)
        cmdreg |= CMD_ENABLE_CLIP;
    else
        cmdreg &= ~CMD_ENABLE_CLIP;            
    dstbase = 0;

    if (dst_y >= pScrn->virtualY) 
    {   
        dstbase=pRDC->VideoModeInfo.ScreenPitch*dst_y;
        dst_y=0;
    }
                  
    if (!pRDC->MMIO2D)                    
    {                  
            
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*4);

        RDCSetupDSTBase(pSingleCMD, dstbase);
        pSingleCMD++;    
        RDCSetupDSTXY(pSingleCMD, dst_x, dst_y);
        pSingleCMD++;    
        RDCSetupRECTXY(pSingleCMD, width, height);
        pSingleCMD++;    
        RDCSetupCMDReg(pSingleCMD, cmdreg);        
      
        
        mUpdateWritePointer;
              
    }
    else
    {                  
        RDCSetupDSTBase_MMIO(dstbase);
        RDCSetupDSTXY_MMIO(dst_x, dst_y);
        RDCSetupRECTXY_MMIO(width, height);
        RDCSetupCMDReg_MMIO(cmdreg);        
 
        pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);      
    }    

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSubsequentMonoPatternFill()== \n");           
#endif
} 

static void
RDCSetupForColor8x8PatternFill(ScrnInfoPtr pScrn, int patx, int paty,
                   int rop, unsigned int planemask, int trans_col)
{
    
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG cmdreg;
    CARD32 *pataddr;
    ULONG ulPatSize;
    int i, j, cpp;
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSetupForColor8x8PatternFill(patx = %d, paty = %d, rop = 0x%x, planemask = 0x%x, trans_col = 0x%x)\n==", patx, paty, rop, planemask, trans_col);
#endif
    
    cmdreg = CMD_BITBLT | CMD_PAT_PATREG;

    switch (pRDC->VideoModeInfo.bitsPerPixel)
    {
    case 8:
        cmdreg |= CMD_COLOR_08;
        break;
    case 15:
    case 16:
        cmdreg |= CMD_COLOR_16;
        break;    
    case 24:
    case 32:
        cmdreg |= CMD_COLOR_32;
        break;        
    }
    
    cmdreg |= (RDCXAAPatternROP[rop] << 8);
    pRDC->ulCMDReg = cmdreg;
    cpp = (pScrn->bitsPerPixel + 1) / 8;
    pataddr = (CARD32 *)(pRDC->FBVirtualAddr +
                        (paty * pRDC->VideoModeInfo.ScreenWidth) + (patx * cpp));   
    ulPatSize = 8*8*cpp;
                
    if (!pRDC->MMIO2D)                    
    {
                
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*(1 + ulPatSize/4));
        RDCSetupDSTPitchHeight(pSingleCMD, pRDC->VideoModeInfo.ScreenPitch, -1);
        pSingleCMD++;
        for (i=0; i<8; i++)
        {
            for (j=0; j<8*cpp/4; j++)
            {
                RDCSetupPatReg(pSingleCMD, (i*j + j) , (*(CARD32 *) (pataddr++)));
                pSingleCMD++;                    
            }    
        }                
    }
    else
    {        
        RDCSetupDSTPitchHeight_MMIO(pRDC->VideoModeInfo.ScreenPitch, -1);
        for (i=0; i<8; i++)
        {
            for (j=0; j<8*cpp/4; j++)
            {
                RDCSetupPatReg_MMIO((i*j + j) , (*(CARD32 *) (pataddr++)));
            }    
        }                
                 
    }

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetupForColor8x8PatternFill()== \n");           
#endif
} 
               
static void
RDCSubsequentColor8x8PatternFillRect(ScrnInfoPtr pScrn, int patx, int paty,
                                     int dst_x, int dst_y, int width, int height)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG dstbase, cmdreg;        
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSubsequentColor8x8PatternFillRect(patx = %d, paty = %d, dst_x = %d, dst_y = %d, width = %d, height)\n==", patx, paty, dst_x, dst_y, width, height);
#endif
            
    
    cmdreg = pRDC->ulCMDReg;
    if (pRDC->EnableClip)
        cmdreg |= CMD_ENABLE_CLIP;
    else
        cmdreg &= ~CMD_ENABLE_CLIP;            

    dstbase = 0;

    if (dst_y >= pScrn->virtualY) 
    {   
        dstbase=pRDC->VideoModeInfo.ScreenPitch*dst_y;
        dst_y=0;
    }
                  
    if (!pRDC->MMIO2D)                    
    {                  
            
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*4);

        RDCSetupDSTBase(pSingleCMD, dstbase);
        pSingleCMD++;    
        RDCSetupDSTXY(pSingleCMD, dst_x, dst_y);
        pSingleCMD++;    
        RDCSetupRECTXY(pSingleCMD, width, height);
        pSingleCMD++;    
        RDCSetupCMDReg(pSingleCMD, cmdreg);        
      
        
        mUpdateWritePointer;
                
    }
    else
    {                  
        RDCSetupDSTBase_MMIO(dstbase);
        RDCSetupDSTXY_MMIO(dst_x, dst_y);
        RDCSetupRECTXY_MMIO(width, height);
        RDCSetupCMDReg_MMIO(cmdreg);        
 
        pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);      
    }    
    
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSubsequentColor8x8PatternFillRect()== \n");           
#endif
} 


static void
RDCSetupForCPUToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                      int fg, int bg,
                                      int rop, unsigned int planemask)
{

    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG cmdreg;

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSetupForCPUToScreenColorExpandFill(fg = 0x%x, bg = ox%x, rop = ox%x, planemask = ox%x)\n==", fg, bg, rop, planemask);
#endif
    
    cmdreg = CMD_COLOREXP;

    switch (pRDC->VideoModeInfo.bitsPerPixel)
    {
    case 8:
        cmdreg |= CMD_COLOR_08;
        break;
    case 15:
    case 16:
        cmdreg |= CMD_COLOR_16;
        break;    
    case 24:
    case 32:
        cmdreg |= CMD_COLOR_32;
        break;        
    }
    
    cmdreg |= (RDCXAAPatternROP[rop] << 8);

    if(bg == -1) 
    {
        cmdreg |= CMD_FONT_TRANSPARENT;    
        bg = 0;
    }
    
    pRDC->ulCMDReg = cmdreg;
            
    if (!pRDC->MMIO2D)                    
    {
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*3);

        RDCSetupDSTPitchHeight(pSingleCMD, pRDC->VideoModeInfo.ScreenPitch, -1);
        pSingleCMD++;
        RDCSetupFG(pSingleCMD, fg);  
        pSingleCMD++;
        RDCSetupBG(pSingleCMD, bg); 
       
    }
    else
    {
        RDCSetupDSTPitchHeight_MMIO(pRDC->VideoModeInfo.ScreenPitch, -1);
        RDCSetupFG_MMIO(fg); 
        RDCSetupBG_MMIO(bg); 
                       
    }
               
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetupForCPUToScreenColorExpandFill()== \n");           
#endif
}
                               
static void
RDCSubsequentCPUToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                        int dst_x, int dst_y,
                                        int width, int height, int offset)
{

    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG dstbase, cmdreg;

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSetupForCPUToScreenColorExpandFill(dst_x = %d, dst_y = %d, width = %d, height, offset = %d)\n==", dst_x, dst_y, width, height, offset);
#endif
            
    
    cmdreg = pRDC->ulCMDReg;
    if (pRDC->EnableClip)
        cmdreg |= CMD_ENABLE_CLIP;
    else
        cmdreg &= ~CMD_ENABLE_CLIP;            
    dstbase = 0;

    if (dst_y >= pScrn->virtualY) 
    {   
        dstbase=pRDC->VideoModeInfo.ScreenPitch*dst_y;
        dst_y=0;
    }
                  
    if (!pRDC->MMIO2D)                    
    {                  
            
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*5);

        RDCSetupSRCPitch(pSingleCMD, ((width+7)/8));  
        pSingleCMD++;    
        RDCSetupDSTBase(pSingleCMD, dstbase);
        pSingleCMD++;    
        RDCSetupDSTXY(pSingleCMD, dst_x, dst_y);
        pSingleCMD++;    
        RDCSetupRECTXY(pSingleCMD, width, height);
        pSingleCMD++;    
        RDCSetupCMDReg(pSingleCMD, cmdreg);        
      
        
        mUpdateWritePointer;
               
    }
    else
    {  
        RDCSetupSRCPitch_MMIO((width+7)/8);                          
        RDCSetupDSTBase_MMIO(dstbase);
        RDCSetupDSTXY_MMIO(dst_x, dst_y);
        RDCSetupSRCXY_MMIO(0, 0);
        
        RDCSetupRECTXY_MMIO(width, height);
        RDCSetupCMDReg_MMIO(cmdreg);        
 
        pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);      
    }
        
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSubsequentCPUToScreenColorExpandFill()== \n");           
#endif
}



static void
RDCSetupForScreenToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                         int fg, int bg,
                                         int rop, unsigned int planemask)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG cmdreg;

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSetupForScreenToScreenColorExpandFill(fg = 0x%x, bg = ox%x, rop = ox%x, planemask = ox%x)\n==", fg, bg, rop, planemask);
#endif
           
    
    cmdreg = CMD_ENHCOLOREXP;
    switch (pRDC->VideoModeInfo.bitsPerPixel)
    {
    case 8:
        cmdreg |= CMD_COLOR_08;
        break;
    case 15:
    case 16:
        cmdreg |= CMD_COLOR_16;
        break;    
    case 24:
    case 32:
        cmdreg |= CMD_COLOR_32;
        break;        
    }
    
    cmdreg |= (RDCXAAPatternROP[rop] << 8);
    
    if(bg == -1)
    {
        cmdreg |= CMD_FONT_TRANSPARENT;    
        bg = 0;
    }    
    pRDC->ulCMDReg = cmdreg;
            
    if (!pRDC->MMIO2D)                    
    {
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*3);

        RDCSetupDSTPitchHeight(pSingleCMD, pRDC->VideoModeInfo.ScreenPitch, -1);
        pSingleCMD++;
        RDCSetupFG(pSingleCMD, fg);  
        pSingleCMD++;
        RDCSetupBG(pSingleCMD, bg); 
       
    }
    else
    {
        RDCSetupDSTPitchHeight_MMIO(pRDC->VideoModeInfo.ScreenPitch, -1);
        RDCSetupFG_MMIO(fg); 
        RDCSetupBG_MMIO(bg); 
                       
    }
               
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetupForScreenToScreenColorExpandFill()== \n");           
#endif
}



static void
RDCSubsequentScreenToScreenColorExpandFill(ScrnInfoPtr pScrn,
                                           int dst_x, int dst_y, int width, int height,
                                           int src_x, int src_y, int offset)
{
   RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    ULONG srcbase, dstbase, cmdreg;
    USHORT srcpitch;

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSubsequentScreenToScreenColorExpandFill(dst_x = %d, dst_y = %d, width = %d, height, src_x = %d, src_y = %d, offset = %d)\n==", dst_x, dst_y, width, height, src_x, src_y, offset);
#endif
            
    
    cmdreg = pRDC->ulCMDReg;
    
    if (pRDC->EnableClip)
        cmdreg |= CMD_ENABLE_CLIP;
    else
        cmdreg &= ~CMD_ENABLE_CLIP;   
        
    dstbase = 0;
    
    if (dst_y >= pScrn->virtualY) 
    {   
        dstbase=pRDC->VideoModeInfo.ScreenPitch*dst_y;
        dst_y=0;
    }
    
    srcbase = pRDC->VideoModeInfo.ScreenPitch*src_y + ((pScrn->bitsPerPixel+1)/8)*src_x;            
    srcpitch = (pScrn->displayWidth+7)/8;
    
    if (!pRDC->MMIO2D)                    
    {                  
            
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*6);

        RDCSetupSRCBase(pSingleCMD, srcbase);
        pSingleCMD++;  
        RDCSetupSRCPitch(pSingleCMD,srcpitch);  
        pSingleCMD++;    
        RDCSetupDSTBase(pSingleCMD, dstbase);
        pSingleCMD++;    
        RDCSetupDSTXY(pSingleCMD, dst_x, dst_y);
        pSingleCMD++;    
        RDCSetupRECTXY(pSingleCMD, width, height);
        pSingleCMD++;    
        RDCSetupCMDReg(pSingleCMD, cmdreg);        
      
        
        mUpdateWritePointer;
       
    }
    else
    { 
        RDCSetupSRCBase_MMIO(srcbase);         
        RDCSetupSRCPitch_MMIO(srcpitch);                          
        RDCSetupDSTBase_MMIO(dstbase);
        RDCSetupDSTXY_MMIO(dst_x, dst_y);
        RDCSetupRECTXY_MMIO(width, height);
        RDCSetupCMDReg_MMIO(cmdreg);        
 
        pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);
      
    }
        
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSubsequentScreenToScreenColorExpandFill()== \n");           
#endif
}

      

static void
RDCSetClippingRectangle(ScrnInfoPtr pScrn,
                        int left, int top, int right, int bottom)
{
    
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSetClippingRectangle(left = %d, top = %d, right = %d, bottom = %d)\n==", left, top, right, bottom);
#endif
    pRDC->EnableClip = TRUE;
            
    if (!pRDC->MMIO2D)                    
    {
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*2);

        RDCSetupCLIP1(pSingleCMD, left, top);
        pSingleCMD++;
        RDCSetupCLIP2(pSingleCMD, right, bottom);                 
    }
    else
    {
        RDCSetupCLIP1_MMIO(left, top);
        RDCSetupCLIP2_MMIO(right, bottom);                       
    }
    
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSetClippingRectangle()== \n");           
#endif
}

static void
RDCDisableClipping(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCDisableClipping()\n==");
#endif
    pRDC->EnableClip = FALSE;
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCDisableClipping()== \n");           
#endif
}

static void RDCSubsequentSolidTwoPointLine(ScrnInfoPtr pScrn,
                                           int x1, int y1, int x2, int y2, int flags)
{
 
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC    *pSingleCMD;
    ULONG     dstbase, ulCommand;
    ULONG     miny, maxy;         
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSubsequentSolidTwoPointLine(x1 = %d, y1 = %d, x2 = %d, y2 = %d, flags = 0x%x)\n==", x1, y1, x2, y2, flags);
#endif

    
    ulCommand = (pRDC->ulCMDReg & (~CMD_MASK)) | CMD_LINEDRAW | CMD_NORMAL_LINE;
    
    if(flags & OMIT_LAST)
        ulCommand |= CMD_NOT_DRAW_LAST_PIXEL;
    else
        ulCommand &= ~CMD_NOT_DRAW_LAST_PIXEL;
        
    if (pRDC->EnableClip)
        ulCommand |= CMD_ENABLE_CLIP;
    else
        ulCommand &= ~CMD_ENABLE_CLIP;        
    dstbase = 0;
    miny = (y1 > y2) ? y2 : y1;
    maxy = (y1 > y2) ? y1 : y2;
    
    if(maxy >= pScrn->virtualY) 
    {
        dstbase = pRDC->VideoModeInfo.ScreenPitch * miny;
        y1 -= miny;
        y2 -= miny;
    }

    if (!pRDC->MMIO2D)                    
    {                  
            
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*5);

        RDCSetupDSTBase(pSingleCMD, dstbase);
        pSingleCMD++;    
        RDCSetupLineXY(pSingleCMD, x1, y1);
        pSingleCMD++;
        RDCSetupLineXY2(pSingleCMD, x2, y2);
        pSingleCMD++;
        RDCSetupLineNumber(pSingleCMD, 0);
        pSingleCMD++;                     
        RDCSetupCMDReg(pSingleCMD, ulCommand);        
              
        
        mUpdateWritePointer;                

        
        
        
    }
    else
    {                  
        RDCSetupDSTBase_MMIO(dstbase);
        RDCSetupLineXY_MMIO(x1, y1);
        RDCSetupLineXY2_MMIO(x2, y2);
        RDCSetupLineNumber_MMIO(0);
        RDCSetupCMDReg_MMIO(ulCommand);        
             
        pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);
    }

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSubsequentSolidTwoPointLine()== \n");           
#endif
} 


static void
RDCSubsequentDashedTwoPointLine(ScrnInfoPtr pScrn,
                                int x1, int y1, int x2, int y2,
                                int flags, int phase)
{
 
    RDCRecPtr  pRDC = RDCPTR(pScrn);
    PKT_SC     *pSingleCMD;
    ULONG      dstbase, ulCommand; 
    ULONG      miny, maxy;  
#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSetupForMonoPatternFillRDCSubsequentDashedTwoPointLine(x1 = %d, y1 = %d, x2 = %d, y2 = %d, flags = 0x%x, phase = %d(0x%x)\n==", x1, y1, x2, y2, phase, phase);
#endif

    
    ulCommand = pRDC->ulCMDReg | CMD_NORMAL_LINE;
    
    if(flags & OMIT_LAST)
        ulCommand |= CMD_NOT_DRAW_LAST_PIXEL;
    else
        ulCommand &= ~CMD_NOT_DRAW_LAST_PIXEL;
        
    if (pRDC->EnableClip)
        ulCommand |= CMD_ENABLE_CLIP;
    else
        ulCommand &= ~CMD_ENABLE_CLIP;        
    dstbase = 0;        
    miny = (y1 > y2) ? y2 : y1;
    maxy = (y1 > y2) ? y1 : y2;
    
    if(maxy >= pScrn->virtualY) 
    {
        dstbase = pRDC->VideoModeInfo.ScreenPitch * miny;
        y1 -= miny;
        y2 -= miny;
    }

    if (!pRDC->MMIO2D)                    
    {                  
            
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*5);

        RDCSetupDSTBase(pSingleCMD, dstbase);
        pSingleCMD++;    
        RDCSetupLineXY(pSingleCMD, x1, y1);
        pSingleCMD++;
        RDCSetupLineXY2(pSingleCMD, x2, y2);
        pSingleCMD++;
        RDCSetupLineNumber(pSingleCMD, 0);
        pSingleCMD++;                     
        RDCSetupCMDReg(pSingleCMD, ulCommand);        
              
        
        mUpdateWritePointer;

        
        
              
    }
    else
    {                  
        RDCSetupDSTBase_MMIO(dstbase);
        RDCSetupLineXY_MMIO(x1, y1);
        RDCSetupLineXY2_MMIO(x2, y2);
        RDCSetupLineNumber_MMIO(0);
        RDCSetupCMDReg_MMIO(ulCommand);
       
        pRDC->CMDQInfo.WaitEngIdle(pScrn, pRDC);
    }

#if Accel_2D_DEBUG
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSubsequentDashedTwoPointLine()== \n");           
#endif
}



void
RDCAccelWaitMarker(ScreenPtr pScreen, int marker)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    CARD32 uMarker = marker;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry RDCAccelWaitMarker(marker = %d)== \n",marker);

    if (!pRDC->MMIO2D) 
    {
	    while ((pRDC->lastMaker - uMarker) > (1 << 24))
	    {
	        pRDC->lastMaker = *(ULONG*)MMIOREG_2D_FENCE;
	        pRDC->lastMaker = pRDC->lastMaker >> 16;
	    }
    } 
    else 
    {
	    RDCSync(pScrn);
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit RDCAccelWaitMarker== \n");
}


int
RDCAccelMarkSync(ScreenPtr pScreen)
{
    PKT_SC *pSingleCMD;
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry RDCAccelMarkSync== \n");

     ++pRDC->curMaker;

    

    pRDC->curMaker &= 0xFFFF;

    if (!pRDC->MMIO2D) 
    {
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*2);
        RDCSetup2DFenceID(pSingleCMD, pRDC->curMaker);
        pSingleCMD++;
        RDCSetupCMDReg(pSingleCMD, 0x7);        
        mUpdateWritePointer;
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit RDCAccelMarkSync== \n");
    return pRDC->curMaker;
}


static Bool
RDCExaPrepareSolid(PixmapPtr pPixmap, int alu, Pixel planeMask, Pixel fg)
{
    ScrnInfoPtr pScrn = xf86Screens[pPixmap->drawable.pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    ULONG   cmdreg;
    unsigned ModeMaskShift = 0;
    CARD32 ModeMask;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, 
                   "==Entry RDCExaPrepareSolid(alu = 0x%x, planeMask = 0x%x, fg = 0x%x)== \n",
                   alu, planeMask, fg);

    if (exaGetPixmapPitch(pPixmap) & 7)
	    return FALSE;
	
    cmdreg = CMD_BITBLT | CMD_PAT_FGCOLOR;
    switch (pPixmap->drawable.depth)
    {
    case 8:
        cmdreg |= CMD_COLOR_08;
        ModeMaskShift = 0;
        break;
    case 15:
    case 16:
        cmdreg |= CMD_COLOR_16;
        ModeMaskShift = 1;
        break;    
    case 24:
    case 32:
        cmdreg |= CMD_COLOR_32;
        ModeMaskShift = 2;
        break;        
    }

    ModeMask = (1 << ((1 << ModeMaskShift) << 3)) - 1;
    
    if (planeMask != ModeMask)
	    return FALSE;

    cmdreg |= (RDCXAAPatternROP[alu] << 8);
    pRDC->Hw2Dinfo.ulFireCMD = cmdreg;        
    pRDC->Hw2Dinfo.ulForeColorPat = fg;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit RDCExaPrepareSolid== \n");
    return TRUE;
}


static void
RDCExaSolid(PixmapPtr pPixmap, int x1, int y1, int x2, int y2)
{
    ScrnInfoPtr pScrn = xf86Screens[pPixmap->drawable.pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, 
                   "==Entry RDCExaSolid(x1 = 0x%x, y1 = 0x%x, x2 = 0x%x, y2 = 0x%x)== \n",
                   x1, y1, x2, y2);

    pRDC->Hw2Dinfo.ulDrawWidth = (ULONG)(x2-x1);
    pRDC->Hw2Dinfo.ulDrawHeight = (ULONG)(y2-y1); 
    pRDC->Hw2Dinfo.ulDstX = (ULONG)x1;
    pRDC->Hw2Dinfo.ulDstY = (ULONG)y1;
    pRDC->Hw2Dinfo.ulDstPitch = exaGetPixmapPitch(pPixmap);
    pRDC->Hw2Dinfo.ulDstBase = exaGetPixmapOffset(pPixmap);

    pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*6);
    RDCSetupDSTPitchHeight(pSingleCMD, pRDC->Hw2Dinfo.ulDstPitch, -1);
    pSingleCMD++;
    RDCSetupFG(pSingleCMD, pRDC->Hw2Dinfo.ulForeColorPat);
    pSingleCMD++;
    RDCSetupDSTBase(pSingleCMD, pRDC->Hw2Dinfo.ulDstBase);
    pSingleCMD++;
    RDCSetupDSTXY(pSingleCMD, pRDC->Hw2Dinfo.ulDstX, pRDC->Hw2Dinfo.ulDstY);
    pSingleCMD++;
    RDCSetupRECTXY(pSingleCMD, pRDC->Hw2Dinfo.ulDrawWidth, pRDC->Hw2Dinfo.ulDrawHeight);
    pSingleCMD++;
    RDCSetupCMDReg(pSingleCMD, pRDC->Hw2Dinfo.ulFireCMD);
    
    mUpdateWritePointer;
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel,"==Exit RDCExaSolid== \n");
}


static void
RDCExaDoneSolidCopy(PixmapPtr pPixmap)
{
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==RDCExaDoneSolidCopy==\n");
}


static Bool
RDCExaPrepareCopy( PixmapPtr pSrcPixmap, 
                   PixmapPtr pDstPixmap, 
                   int xdir,
                   int ydir, 
                   int alu, 
                   Pixel planeMask)
{
    ScrnInfoPtr pScrn = xf86Screens[pDstPixmap->drawable.pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    unsigned ModeMaskShift = 0;
    CARD32 ModeMask;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry RDCExaPrepareCopy==\n");

    if (pSrcPixmap->drawable.bitsPerPixel != pDstPixmap->drawable.bitsPerPixel)
    {
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Exit RDCExaPrepareCopy Color depth mismatch==\n");
	    return FALSE;
    }
    
    if ((pRDC->Hw2Dinfo.ulSrcPitch = exaGetPixmapPitch(pSrcPixmap)) & 7)
	    return FALSE;

    if ((pRDC->Hw2Dinfo.ulDstPitch = exaGetPixmapPitch(pDstPixmap)) & 7)
	    return FALSE;

    pRDC->Hw2Dinfo.ulSrcBase = exaGetPixmapOffset(pSrcPixmap);
    pRDC->Hw2Dinfo.ulDstBase = exaGetPixmapOffset(pDstPixmap);
    
    pRDC->Hw2Dinfo.ulFireCMD = CMD_BITBLT | (RDCXAACopyROP[alu] << 8);

    switch (pDstPixmap->drawable.depth)
    {
    case 8:
        pRDC->Hw2Dinfo.ulFireCMD |= CMD_COLOR_08;
        ModeMaskShift = 0;
        break;
    case 15:
    case 16:
        pRDC->Hw2Dinfo.ulFireCMD |= CMD_COLOR_16;
        ModeMaskShift = 1;
        break;    
    case 24:
    case 32:
        pRDC->Hw2Dinfo.ulFireCMD |= CMD_COLOR_32;
        ModeMaskShift = 2;
        break;        
    }
    
    ModeMask = (1 << ((1 << ModeMaskShift) << 3)) - 1;
    if (planeMask != ModeMask)
    {
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Exit RDCExaPrepareCopy Mask mismatch==\n");
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==planeMask = 0x%x; ModeMask = 0x%x==\n",planeMask, ModeMask);
	    return FALSE;
    }
    
    if (xdir < 0)
	    pRDC->Hw2Dinfo.ulFireCMD |= CMD_X_DEC;
    if (ydir < 0)
	    pRDC->Hw2Dinfo.ulFireCMD |= CMD_Y_DEC;
	    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit RDCExaPrepareCopy==\n");
    return TRUE;
}


static void
RDCExaCopy( PixmapPtr pDstPixmap, 
            int srcX, 
            int srcY, 
            int dstX, 
            int dstY,
            int width, 
            int height)
{
    ScrnInfoPtr pScrn = xf86Screens[pDstPixmap->drawable.pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    PKT_SC *pSingleCMD;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry RDCExaCopy==\n");
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, 
                   "==RDCExaCopy(srcX=0x%x,srcY=0x%x,dstX=0x%x,dstY=0x%x,width=0x%x,height=0x%x==\n",
                   srcX,srcY,dstX,dstY,width,height);
    
    pRDC->Hw2Dinfo.ulDrawWidth = width;
    pRDC->Hw2Dinfo.ulDrawHeight = height;
    pRDC->Hw2Dinfo.ulDstX = (ULONG)dstX;
    pRDC->Hw2Dinfo.ulDstY = (ULONG)dstY;
    pRDC->Hw2Dinfo.ulSrcX = (ULONG)srcX;
    pRDC->Hw2Dinfo.ulSrcY = (ULONG)srcY;
    
    if (pRDC->Hw2Dinfo.ulFireCMD & CMD_X_DEC)
    {
        pRDC->Hw2Dinfo.ulDstX += width - 1;
        pRDC->Hw2Dinfo.ulSrcX += width - 1;
    }

    if (pRDC->Hw2Dinfo.ulFireCMD & CMD_Y_DEC)
    {
        pRDC->Hw2Dinfo.ulDstY += height - 1;
        pRDC->Hw2Dinfo.ulSrcY += height - 1;
    }

    pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*8);
    RDCSetupDSTBase(pSingleCMD, pRDC->Hw2Dinfo.ulDstBase);
    pSingleCMD++;
    RDCSetupDSTPitchHeight(pSingleCMD, pRDC->Hw2Dinfo.ulDstPitch, -1);
    pSingleCMD++;
    RDCSetupDSTXY(pSingleCMD, pRDC->Hw2Dinfo.ulDstX, pRDC->Hw2Dinfo.ulDstY);
    pSingleCMD++;
    RDCSetupSRCBase(pSingleCMD, pRDC->Hw2Dinfo.ulSrcBase);
    pSingleCMD++;
    RDCSetupSRCPitch(pSingleCMD, pRDC->Hw2Dinfo.ulSrcPitch);
    pSingleCMD++;
    RDCSetupSRCXY(pSingleCMD, pRDC->Hw2Dinfo.ulSrcX, pRDC->Hw2Dinfo.ulSrcY);
    pSingleCMD++;
    RDCSetupRECTXY(pSingleCMD, pRDC->Hw2Dinfo.ulDrawWidth, pRDC->Hw2Dinfo.ulDrawHeight);
    pSingleCMD++;
    RDCSetupCMDReg(pSingleCMD, pRDC->Hw2Dinfo.ulFireCMD);
    mUpdateWritePointer;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit RDCExaCopy==\n");
}


static void
RDCExaDoneCopy(PixmapPtr pPixmap)
{
    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==RDCExaDoneCopy==\n");
}



Bool RDCExaUploadToScreen ( PixmapPtr   pDst,
				            int x, int y,
				            int w, int h,
                            char *src, int src_pitch)
{
    ScrnInfoPtr pScrn = xf86Screens[pDst->drawable.pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    unsigned  DstPitch = exaGetPixmapPitch(pDst);
    unsigned  wBytes = (pDst->drawable.bitsPerPixel * w + 7) >> 3;
    unsigned  DstOffset;
    char *dst = NULL;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry RDCExaUploadToScreen==\n");
    
    if (!w || !h)
    {
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Exit RDCExaUploadToScreen width or height = 0==\n");
	    return TRUE;
	}
	
    DstOffset = x * pDst->drawable.bitsPerPixel;
    DstOffset = exaGetPixmapOffset(pDst) + y * DstPitch + (DstOffset >> 3);    
	dst = (char *)pRDC->FBVirtualAddr + DstOffset;

	while (h--)
	{
	    memcpy(dst, src, wBytes);
	    src += src_pitch;
	    dst += DstPitch;
	}
	
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit RDCExaUploadToScreen==\n");
    return TRUE;
}


Bool RDCExaDownloadFromScreen (PixmapPtr pSrc,
                               int x,  int y,
                               int w,  int h,
                               char *dst,  int dst_pitch)
{
    ScrnInfoPtr pScrn = xf86Screens[pSrc->drawable.pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    unsigned srcPitch = exaGetPixmapPitch(pSrc);
    unsigned wBytes = (pSrc->drawable.bitsPerPixel * w + 7) >> 3;
    unsigned srcOffset;
    char *src = NULL;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry RDCExaDownloadFromScreen==\n");
    
    if (!w || !h)
    {
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Exit RDCExaDownloadFromScreen width or height = 0==\n");
	    return TRUE;
	}
	
    srcOffset = x * pSrc->drawable.bitsPerPixel;
    srcOffset = exaGetPixmapOffset(pSrc) + y * srcPitch + (srcOffset >> 3);    
	src = (char *)pRDC->FBVirtualAddr + srcOffset;

	while (h--)
	{
	    memcpy(dst, src, wBytes);
	    dst += dst_pitch;
	    src += srcPitch;
	}

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit RDCExaDownloadFromScreen==\n");
    return TRUE;
}



static ExaDriverPtr RDCInitExa(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    ExaDriverPtr pExa = exaDriverAlloc();

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry RDCInitExa Func== \n");

    memset(pExa, 0, sizeof(*pExa));

    if (!pExa)
	    return NULL;

    pExa->exa_major = 2;
    pExa->exa_minor = 0;
    pExa->memoryBase = pRDC->FBVirtualAddr;
    pExa->memorySize = pRDC->AvailableFBsize;
    pExa->offScreenBase = pScrn->virtualY * pRDC->VideoModeInfo.ScreenPitch;
    pExa->pixmapOffsetAlign = 32;
    pExa->pixmapPitchAlign = 16;
    pExa->flags = EXA_OFFSCREEN_PIXMAPS | EXA_OFFSCREEN_ALIGN_POT;
    pExa->maxX = 2047;
    pExa->maxY = 2047;

    pExa->WaitMarker = RDCAccelWaitMarker;
    pExa->MarkSync = RDCAccelMarkSync;

    
    pExa->PrepareSolid = RDCExaPrepareSolid;
    pExa->Solid = RDCExaSolid;
    pExa->DoneSolid = RDCExaDoneSolidCopy;

    
    pExa->PrepareCopy = RDCExaPrepareCopy;
    pExa->Copy = RDCExaCopy;
    pExa->DoneCopy = RDCExaDoneCopy;
    
	pExa->UploadToScreen = RDCExaUploadToScreen;
    pExa->DownloadFromScreen = RDCExaDownloadFromScreen;

    
    if (!exaDriverInit(pScreen, pExa)) 
    {
	    xfree(pExa);
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Init EXA fail== \n");
	    return NULL;
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Leave RDCInitExa Func== \n");

    return pExa;
}
                                         
#endif    
