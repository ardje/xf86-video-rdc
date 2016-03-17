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


#include "exa.h"


#include "xf86Cursor.h"


#include "rdc.h"

#ifdef Accel_2D


Bool    bInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
Bool    bEnableCMDQ(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
Bool    bEnable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
void    vDisable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
void    vWaitEngIdle(ScrnInfoPtr pScrn, RDCRecPtr pRDC);    
UCHAR   *pjRequestCMDQ(RDCRecPtr pRDC, ULONG   ulDataLen);

Bool    bCRInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
Bool    bCREnableCMDQ(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
Bool    bCREnable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
void    vCRDisable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
void    vCRWaitEngIdle(ScrnInfoPtr pScrn, RDCRecPtr pRDC);    

Bool
bInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{

    ScreenPtr pScreen;
    
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Init CMDQ Info== \n");
    
    pRDC->CMDQInfo.pjCmdQBasePort    = pRDC->MMIOVirtualAddr+ 0x8044; 
    pRDC->CMDQInfo.pjWritePort       = pRDC->MMIOVirtualAddr+ 0x8048;
    pRDC->CMDQInfo.pjReadPort        = pRDC->MMIOVirtualAddr+ 0x804C;
    pRDC->CMDQInfo.pjEngStatePort    = pRDC->MMIOVirtualAddr+ 0x804C;

    pRDC->CMDQInfo.ulReadPointerMask = 0x0003FFFF;

    
    if (!pRDC->MMIO2D) 
    {
        pRDC->CMDQInfo.ulCMDQType = VM_CMD_QUEUE;    
       
        pScreen = screenInfo.screens[pScrn->scrnIndex];
        
        pRDC->CMDQInfo.pjCMDQVirtualAddr = pRDC->FBVirtualAddr + pRDC->CMDQInfo.ulCMDQOffsetAddr;
        pRDC->CMDQInfo.ulCurCMDQueueLen = pRDC->CMDQInfo.ulCMDQSize - CMD_QUEUE_GUARD_BAND;
        pRDC->CMDQInfo.ulCMDQMask = pRDC->CMDQInfo.ulCMDQSize - 1 ; 
    }
    
      
    if (pRDC->MMIO2D)
    {        
        pRDC->CMDQInfo.ulCMDQType = VM_CMD_MMIO;        
    }
       
    return (TRUE);    
}

Bool
bEnableCMDQ(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{
    ULONG ulVMCmdQBasePort = 0;
    
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Entry Enable CMDQ== \n");

    vWaitEngIdle(pScrn, pRDC);  

    
    switch (pRDC->CMDQInfo.ulCMDQType)
    {
    case VM_CMD_QUEUE:
        ulVMCmdQBasePort  = pRDC->CMDQInfo.ulCMDQOffsetAddr >> 3;
 
        
        ulVMCmdQBasePort |= 0xF0000000;               

        
        switch (pRDC->CMDQInfo.ulCMDQSize)
        {
        case CMD_QUEUE_SIZE_256K:
            ulVMCmdQBasePort |= 0x00000000;   
            break;

        case CMD_QUEUE_SIZE_512K:
            ulVMCmdQBasePort |= 0x04000000;   
            break;
      
        case CMD_QUEUE_SIZE_1M:
            ulVMCmdQBasePort |= 0x08000000;       
            break;
            
        case CMD_QUEUE_SIZE_2M:
            ulVMCmdQBasePort |= 0x0C000000;       
            break;        
            
        default:
            return(FALSE);
            break;
        }     
                                 
        *(ULONG *) (pRDC->CMDQInfo.pjCmdQBasePort) = ulVMCmdQBasePort;         
        pRDC->CMDQInfo.ulWritePointer = *(ULONG *) (pRDC->CMDQInfo.pjWritePort) << 3;                 
        break;
        
    case VM_CMD_MMIO:
        
        ulVMCmdQBasePort |= 0xF0000000;               
        ulVMCmdQBasePort |= 0x02000000;            
        *(ULONG *) (pRDC->CMDQInfo.pjCmdQBasePort) = ulVMCmdQBasePort;                                
        break;
        
    default:
        return (FALSE);
        break;
    }

    return (TRUE);    
}

Bool
bEnable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{

    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Enable 2D== \n");
    
    SetIndexRegMask(CRTC_PORT, 0xA4, 0xFE, 0x01);        

    SetIndexRegMask(CRTC_PORT, 0xA3, ~0x20, 0x20);           
    *(ULONG *)MMIOREG_1ST_FLIP |=  0x80000000;

    if (!bInitCMDQInfo(pScrn, pRDC))
    {
        vDisable2D(pScrn, pRDC);      
        return (FALSE);
    }
        
    if (!bEnableCMDQ(pScrn, pRDC))
    {
        vDisable2D(pScrn, pRDC);      
        return (FALSE);
    }
            
    return (TRUE);    
}

void
vDisable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{
   xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Engine Disable 2D== \n");

    vWaitEngIdle(pScrn, pRDC);
    SetIndexRegMask(CRTC_PORT, 0xA4, 0xFE, 0x00);

    SetIndexRegMask(CRTC_PORT, 0xA3, ~0x20, 0x00);       
    *(ULONG *)MMIOREG_1ST_FLIP &=  ~0x80000000;
}


void
vWaitEngIdle(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{
    ULONG  ulEngState, ulEngState2;

    xf86DrvMsgVerb(0, X_INFO, 10, "==Entry Wait Idle== \n");

    do  
    {
        ulEngState = *((volatile ULONG*)(pRDC->CMDQInfo.pjEngStatePort));    
        ulEngState2 = *((volatile ULONG*)(pRDC->CMDQInfo.pjWritePort));        
    } while ((ulEngState & 0x80000000) || 
            ((ulEngState&0x3ffff) != (ulEngState2&0x3ffff)));
            
    xf86DrvMsgVerb(0, X_INFO, 10, "==Exit Wait Idle== \n");
}    


__inline ULONG ulGetCMDQLength(RDCRecPtr pRDC, ULONG ulWritePointer, ULONG ulCMDQMask)
{
    ULONG ulReadPointer;

    ulReadPointer  = *((volatile ULONG *)(pRDC->CMDQInfo.pjReadPort)) & pRDC->CMDQInfo.ulReadPointerMask;        

    return ((ulReadPointer << 3) - ulWritePointer - CMD_QUEUE_GUARD_BAND) & ulCMDQMask;
}

UCHAR *pjRequestCMDQ(
RDCRecPtr pRDC, ULONG   ulDataLen)
{
    UCHAR   *pjBuffer;
    ULONG   i, ulWritePointer, ulCMDQMask, ulCurCMDQLen, ulContinueCMDQLen;

    ulWritePointer = pRDC->CMDQInfo.ulWritePointer;
    ulContinueCMDQLen = pRDC->CMDQInfo.ulCMDQSize - ulWritePointer;
    ulCMDQMask = pRDC->CMDQInfo.ulCMDQMask;        
    
    if (ulContinueCMDQLen >= ulDataLen)
    {
                        
        if (pRDC->CMDQInfo.ulCurCMDQueueLen < ulDataLen)
        {
            do
            {
                ulCurCMDQLen = ulGetCMDQLength(pRDC, ulWritePointer, ulCMDQMask);
            } while (ulCurCMDQLen < ulDataLen);
            
            pRDC->CMDQInfo.ulCurCMDQueueLen = ulCurCMDQLen;
        }
        
        pjBuffer = pRDC->CMDQInfo.pjCMDQVirtualAddr + ulWritePointer;
        pRDC->CMDQInfo.ulCurCMDQueueLen -= ulDataLen;            
        pRDC->CMDQInfo.ulWritePointer = (ulWritePointer + ulDataLen) & ulCMDQMask;
        return pjBuffer;            
    }
    else
    {   

        
        if (pRDC->CMDQInfo.ulCurCMDQueueLen < ulContinueCMDQLen)
        {
            do
            {
                ulCurCMDQLen = ulGetCMDQLength(pRDC, ulWritePointer, ulCMDQMask);
            } while (ulCurCMDQLen < ulContinueCMDQLen);
            
            pRDC->CMDQInfo.ulCurCMDQueueLen = ulCurCMDQLen;
        }
    
        pjBuffer = pRDC->CMDQInfo.pjCMDQVirtualAddr + ulWritePointer;
        
        for (i = 0; i<ulContinueCMDQLen/8; i++, pjBuffer+=8)
        {
            *(ULONG *)pjBuffer = (ULONG) PKT_NULL_CMD;
            *(ULONG *) (pjBuffer+4) = 0;
        }
        
        pRDC->CMDQInfo.ulCurCMDQueueLen -= ulContinueCMDQLen;
        pRDC->CMDQInfo.ulWritePointer = ulWritePointer = 0;
            
            
        if (pRDC->CMDQInfo.ulCurCMDQueueLen < ulDataLen)
        {
            do
            {
                ulCurCMDQLen = ulGetCMDQLength(pRDC, ulWritePointer, ulCMDQMask);
            } while (ulCurCMDQLen < ulDataLen);

            pRDC->CMDQInfo.ulCurCMDQueueLen = ulCurCMDQLen;
        }

        pRDC->CMDQInfo.ulCurCMDQueueLen -= ulDataLen;
        pjBuffer = pRDC->CMDQInfo.pjCMDQVirtualAddr + ulWritePointer;
        pRDC->CMDQInfo.ulWritePointer = (ulWritePointer + ulDataLen) & ulCMDQMask;
        return pjBuffer;            
    }
   
} 

Bool bCRInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{

    ScreenPtr pScreen;
    
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==bCRInitCMDQInfo()== \n");
    
    pRDC->CMDQInfo.pjCmdQCtrlPort    = pRDC->MMIOVirtualAddr+ 0x0400; 
    pRDC->CMDQInfo.pjCmdQBasePort    = pRDC->MMIOVirtualAddr+ 0x0404; 
    pRDC->CMDQInfo.pjWritePort       = pRDC->MMIOVirtualAddr+ 0x0408;
    pRDC->CMDQInfo.pjReadPort        = pRDC->MMIOVirtualAddr+ 0x040C;
    pRDC->CMDQInfo.pjCmdQEndPort     = pRDC->MMIOVirtualAddr+ 0x0410; 
    pRDC->CMDQInfo.pjEngStatePort    = pRDC->MMIOVirtualAddr+ 0x0414;

    pRDC->CMDQInfo.ulReadPointerMask = 0x000FFFFF;

    
    if (!pRDC->MMIO2D) 
    {
        pRDC->CMDQInfo.ulCMDQType = VM_CMD_QUEUE;    
       
        pScreen = screenInfo.screens[pScrn->scrnIndex];
        
        pRDC->CMDQInfo.pjCMDQVirtualAddr = pRDC->FBVirtualAddr + pRDC->CMDQInfo.ulCMDQOffsetAddr;
        pRDC->CMDQInfo.ulCurCMDQueueLen = pRDC->CMDQInfo.ulCMDQSize - CMD_QUEUE_GUARD_BAND;
        pRDC->CMDQInfo.ulCMDQMask = pRDC->CMDQInfo.ulCMDQSize - 1 ; 
    }
    
      
    if (pRDC->MMIO2D)
    {        
        pRDC->CMDQInfo.ulCMDQType = VM_CMD_MMIO;        
    }
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "<<== bCRInitCMDQInfo()\n");
    return (TRUE);    
}

Bool bCREnableCMDQ(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{
    ULONG ulVMCmdQCtrlPort = 0;
    
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==bCREnableCMDQ()== \n");

    vCRWaitEngIdle(pScrn, pRDC);  

    
    switch (pRDC->CMDQInfo.ulCMDQType)
    {
        case VM_CMD_QUEUE:
            
            ulVMCmdQCtrlPort = *(ULONG *) (pRDC->CMDQInfo.pjCmdQCtrlPort);
            ulVMCmdQCtrlPort |= 0x00000F01;

            *(ULONG *) (pRDC->CMDQInfo.pjCmdQBasePort) = pRDC->CMDQInfo.ulCMDQOffsetAddr;
            *(ULONG *) (pRDC->CMDQInfo.pjCmdQEndPort) = pRDC->CMDQInfo.ulCMDQOffsetAddr + DEFAULT_CMDQ_SIZE - 8;
            *(ULONG *) (pRDC->CMDQInfo.pjCmdQCtrlPort) = ulVMCmdQCtrlPort;

            pRDC->CMDQInfo.ulWritePointer = *(ULONG *) (pRDC->CMDQInfo.pjWritePort) << 3;                 
            break;
            
        case VM_CMD_MMIO:
            
            ulVMCmdQCtrlPort &= ~(0x00000001);

            *(ULONG *) (pRDC->CMDQInfo.pjCmdQCtrlPort) = ulVMCmdQCtrlPort;
            break;
            
        default:
            return (FALSE);
            break;
    }
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "<<== bCREnableCMDQ()\n");
    return (TRUE);    
}

Bool bCREnable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{

    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==bCREnable2D()== \n");
    
    SetIndexRegMask(CRTC_PORT, 0xA4, 0xFE, 0x01);        

    SetIndexRegMask(CRTC_PORT, 0xA3, ~0x20, 0x20);           
    *(ULONG *)MMIOREG_1ST_FLIP |=  0x80000000;

    if (!bCRInitCMDQInfo(pScrn, pRDC))
    {
        vCRDisable2D(pScrn, pRDC);      
        return (FALSE);
    }
        
    if (!bCREnableCMDQ(pScrn, pRDC))
    {
        vCRDisable2D(pScrn, pRDC);      
        return (FALSE);
    }
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "<<== bCREnable2D()\n");
    return (TRUE);    
}

void vCRDisable2D(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{
   xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==vCRDisable2D()== \n");

    vCRWaitEngIdle(pScrn, pRDC);
    SetIndexRegMask(CRTC_PORT, 0xA4, 0xFE, 0x00);

    SetIndexRegMask(CRTC_PORT, 0xA3, ~0x20, 0x00);       
    *(ULONG *)MMIOREG_1ST_FLIP &=  ~0x80000000;

    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "<<== vCRDisable2D()\n");
}

void
vCRWaitEngIdle(ScrnInfoPtr pScrn, RDCRecPtr pRDC)
{
    ULONG  ulReadPointor, ulWritePointor, ulEngState;

    xf86DrvMsgVerb(0, X_INFO, 10, "==vCRWaitEngIdle()== \n");

    ulWritePointor = *((volatile ULONG*)(pRDC->CMDQInfo.pjWritePort));
    ulWritePointor &= 0x3ffff;
    do  
    {
        ulReadPointor = *((volatile ULONG*)(pRDC->CMDQInfo.pjReadPort));
        ulEngState = *((volatile ULONG*)(pRDC->CMDQInfo.pjEngStatePort));
    }
    while ((ulEngState & BIT12) || (ulWritePointor != ulReadPointor));
            
    xf86DrvMsgVerb(0, X_INFO, 10, "<<== vCRWaitEngIdle()\n");
}
#endif    
