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



Bool    bInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
Bool    bEnableCMDQ(RDCRecPtr pRDC);
Bool    bEnable2D(RDCRecPtr pRDC);
void    vDisable2D(RDCRecPtr pRDC);
void    vWaitEngIdle(RDCRecPtr pRDC);    
UCHAR   *pjRequestCMDQ(RDCRecPtr pRDC, ULONG   ulDataLen);

Bool    bCRInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
Bool    bCREnableCMDQ(RDCRecPtr pRDC);
Bool    bCREnable2D(RDCRecPtr pRDC);
void    vCRDisable2D(RDCRecPtr pRDC);
void    vCRWaitEngIdle(RDCRecPtr pRDC);    

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

    pRDC->CMDQInfo.bInitialized = TRUE;
    
    return (TRUE);    
}

Bool
bEnableCMDQ(RDCRecPtr pRDC)
{
    ULONG ulVMCmdQBasePort = 0;
    
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Entry Enable CMDQ== \n");

    vWaitEngIdle(pRDC);  

    
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
bEnable2D(RDCRecPtr pRDC)
{

    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Enable 2D== \n");
    
    SetIndexRegMask(COLOR_CRTC_INDEX, 0xA4, 0xFE, 0x01);        

    SetIndexRegMask(COLOR_CRTC_INDEX, 0xA3, ~0x20, 0x20);           
    *(ULONG *)MMIOREG_1ST_FLIP |=  0x80000000;

    return (TRUE);    
}

void
vDisable2D(RDCRecPtr pRDC)
{
   xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Engine Disable 2D== \n");

    vWaitEngIdle(pRDC);
    SetIndexRegMask(COLOR_CRTC_INDEX, 0xA4, 0xFE, 0x00);

    SetIndexRegMask(COLOR_CRTC_INDEX, 0xA3, ~0x20, 0x00);       
    *(ULONG *)MMIOREG_1ST_FLIP &=  ~0x80000000;
}


void
vWaitEngIdle(RDCRecPtr pRDC)
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
    
    pRDC->CMDQInfo.pjCmdQCtrlPort    = pRDC->MMIOVirtualAddr+ CR_CTRL; 
    pRDC->CMDQInfo.pjCmdQBasePort    = pRDC->MMIOVirtualAddr+ CR_BUFFER_START; 
    pRDC->CMDQInfo.pjWritePort       = pRDC->MMIOVirtualAddr+ CR_BUFFER_WRITEPORT;
    pRDC->CMDQInfo.pjReadPort        = pRDC->MMIOVirtualAddr+ CR_BUFFER_READPORT;
    pRDC->CMDQInfo.pjCmdQEndPort     = pRDC->MMIOVirtualAddr+ CR_BUFFER_END; 
    pRDC->CMDQInfo.pjEngStatePort    = pRDC->MMIOVirtualAddr+ CR_ENG_STATUS;

    pRDC->CMDQInfo.ulReadPointerMask = 0x000FFFFF;

    pScreen = screenInfo.screens[pScrn->scrnIndex];
    
    pRDC->CMDQInfo.pjCMDQVirtualAddr = pRDC->FBVirtualAddr + pRDC->CMDQInfo.ulCMDQOffsetAddr;
    pRDC->CMDQInfo.ulCurCMDQueueLen = pRDC->CMDQInfo.ulCMDQSize - CMD_QUEUE_GUARD_BAND;
    pRDC->CMDQInfo.ulCMDQMask = pRDC->CMDQInfo.ulCMDQSize - 1 ; 
    
    pRDC->CMDQInfo.bInitialized = TRUE;
    
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "<<== bCRInitCMDQInfo()\n");
    return (TRUE);    
}

Bool bCREnableCMDQ(RDCRecPtr pRDC)
{
    ULONG ulVMCmdQCtrlPort = 0;
    
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==bCREnableCMDQ()== \n");

    vCRWaitEngIdle(pRDC);  

    
    ulVMCmdQCtrlPort = CRCTRL_ENABLE | (CRCTRL_THRESHOLD << 8) | CRCTRL_DMAMMIO;

    if (pRDC->MMIOVPost)
        ulVMCmdQCtrlPort |= CRCTRL_VPOSTMMIO | CRCTRL_VDISPMMIO | CRCTRL_DMAMMIO;

    if (pRDC->MMIO2D)
        *(ULONG *)MMIOREG_CMDQ_SET = 0x02000000;
    else
        *(ULONG *)MMIOREG_CMDQ_SET = 0;
        
    *(ULONG *) (pRDC->CMDQInfo.pjCmdQBasePort) = pRDC->CMDQInfo.ulCMDQOffsetAddr;
    *(ULONG *) (pRDC->CMDQInfo.pjCmdQEndPort) = pRDC->CMDQInfo.ulCMDQOffsetAddr + DEFAULT_CMDQ_SIZE - 8;
    *(ULONG *) (pRDC->CMDQInfo.pjCmdQCtrlPort) = ulVMCmdQCtrlPort;

    pRDC->CMDQInfo.ulWritePointer = *(ULONG *) (pRDC->CMDQInfo.pjWritePort) << 3;                 

    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "<<== bCREnableCMDQ()\n");
    return (TRUE);    
}

Bool bCREnable2D(RDCRecPtr pRDC)
{

    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==bCREnable2D()== \n");
    
    SetIndexRegMask(COLOR_CRTC_INDEX, 0xA4, 0xFE, 0x01);        

    SetIndexRegMask(COLOR_CRTC_INDEX, 0xA3, ~0x20, 0x20);           
    *(ULONG *)MMIOREG_1ST_FLIP |=  0x80000000;

    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "<<== bCREnable2D()\n");
    return (TRUE);    
}

void vCRDisable2D(RDCRecPtr pRDC)
{
   xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==vCRDisable2D()== \n");

    vCRWaitEngIdle(pRDC);
    SetIndexRegMask(COLOR_CRTC_INDEX, 0xA4, 0xFE, 0x00);

    SetIndexRegMask(COLOR_CRTC_INDEX, 0xA3, ~0x20, 0x00);       
    *(ULONG *)MMIOREG_1ST_FLIP &=  ~0x80000000;

    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "<<== vCRDisable2D()\n");
}

void
vCRWaitEngIdle(RDCRecPtr pRDC)
{
    ULONG  ulReadPointor, ulWritePointor, ulEngState;

    xf86DrvMsgVerb(0, X_INFO, 10, "==vCRWaitEngIdle()== \n");

    ulWritePointor = *((volatile ULONG*)(pRDC->CMDQInfo.pjWritePort));
    do  
    {
        ulReadPointor = *((volatile ULONG*)(pRDC->CMDQInfo.pjReadPort));
        ulEngState = *((volatile ULONG*)(pRDC->CMDQInfo.pjEngStatePort));
    }
    while ((ulWritePointor != ulReadPointor) ||
              (ulEngState & (CR_2D_IDLE|CR_DMA_IDLE|CR_VIDEO_IDLE|CR_CR_IDLE)));
            
    xf86DrvMsgVerb(0, X_INFO, 10, "<<== vCRWaitEngIdle()\n");
}
