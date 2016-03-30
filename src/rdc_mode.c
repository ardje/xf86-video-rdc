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

RRateInfo RefreshRateMap[] = { {60.0f,  FALSE, 0},
                               {50.0f,  TRUE,  1},
                               {50.0f,  FALSE, 3},
                               {56.0f,  FALSE, 4},
                               {24.0f,  FALSE, 6},
                               {70.0f,  FALSE, 7},
                               {75.0f,  FALSE, 8},
                               {80.0f,  FALSE, 9},
                               {85.0f,  FALSE, 10},
                               {90.0f,  FALSE, 11},
                               {100.0f, FALSE, 12},
                               {120.0f, FALSE, 13},
                               {72.0f,  FALSE, 14},
                               {65.0f,  FALSE, 15}};



extern void vRDCOpenKey(ScrnInfoPtr pScrn);
extern Bool bRDCRegInit(ScrnInfoPtr pScrn);

#ifdef HWC
extern Bool bInitHWC(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
#endif


Bool RDCSetMode(ScrnInfoPtr pScrn, DisplayModePtr mode);
USHORT usGetVbeModeNum(ScrnInfoPtr pScrn, DisplayModePtr mode);
float fDifference(float Value1, float Value2);
DisplayModePtr RDCBuildModePool(ScrnInfoPtr pScrn);
Bool BTranslateIndexToRefreshRate(UCHAR ucRRateIndex, float *fRefreshRate);
char* pcConvertResolutionToString(ULONG ulResolution);
DisplayModePtr SearchDisplayModeRecPtr(DisplayModePtr pModePoolHead, CBIOS_ARGUMENTS CBiosArguments);

Bool
RDCSetMode(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    RDCRecPtr pRDC;
    MODE_PRIVATE *pModePrivate;
    CBIOS_ARGUMENTS CBiosArguments;
    CBIOS_Extension CBiosExtension;
    USHORT usVESAMode;
    
    pRDC = RDCPTR(pScrn);
    pModePrivate = MODE_PRIVATE_PTR(mode);
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Enter RDCSetMode()== \n");

    vRDCOpenKey(pScrn);
    bRDCRegInit(pScrn);

    CBiosExtension.pCBiosArguments = &CBiosArguments;
    CBiosExtension.IOAddress = (USHORT)(pRDC->RelocateIO);
    CBiosExtension.VideoVirtualAddress = (ULONG)(pRDC->FBVirtualAddr);

    
    CBiosArguments.reg.x.AX = OEMFunction;
    CBiosArguments.reg.x.BX = SetDisplay1RefreshRate;
    CBiosArguments.reg.lh.CL = pModePrivate->ucRRate_ID;
    CInt10(&CBiosExtension);

    usVESAMode = usGetVbeModeNum(pScrn, mode);
    
    
    CBiosArguments.reg.x.AX = VBESetMode;
    CBiosArguments.reg.x.BX = (0x4000 | usVESAMode);
    CInt10(&CBiosExtension);

    
    CBiosArguments.reg.x.AX = VBESetGetScanLineLength;
    CBiosArguments.reg.lh.BL = 0x00;
    CBiosArguments.reg.x.CX = (USHORT)(pScrn->displayWidth);
    CInt10(&CBiosExtension);

    

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Exit RDCSetMode(), return true== \n");    
    return (TRUE);    
}

USHORT usGetVbeModeNum(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    RDCRecPtr pRDC;
    MODE_PRIVATE *pModePrivate;
    USHORT usVESAModeNum;
    UCHAR  ucColorDepth = (UCHAR)(pScrn->bitsPerPixel);

    pRDC = RDCPTR(pScrn);
    pModePrivate = MODE_PRIVATE_PTR(mode);
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter usGetVbeModeNum()== \n");
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "==Display Width=0x%x, Height=0x%x, Color Depth=0x%x==\n",
               mode->HDisplay,mode->VDisplay,pScrn->bitsPerPixel);


    switch (ucColorDepth)
    {
        case 8:
            usVESAModeNum = pModePrivate->Mode_ID_8bpp;
            break;

        case 16:
            usVESAModeNum = pModePrivate->Mode_ID_16bpp;
            break;

        case 32:
            usVESAModeNum = pModePrivate->Mode_ID_32bpp;
            break;
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit usGetVbeModeNum() return VESA Mode = 0x%x==\n", usVESAModeNum);
    return usVESAModeNum;

}

DisplayModePtr RDCBuildModePool(ScrnInfoPtr pScrn)
{
    DisplayModePtr pMode = NULL, pModePoolHead = NULL, pModePoolTail = NULL;
    
    CBIOS_ARGUMENTS CBiosArguments;
    CBIOS_Extension CBiosExtension;
    RDCRecPtr pRDC = RDCPTR(pScrn);
    MODE_PRIVATE *pModePrivate;
    USHORT usSerialNum = 0;
    USHORT wLCDHorSize, wLCDVerSize;
    USHORT wVESAModeHorSize, wVESAModeVerSize;
    BYTE bColorDepth; 

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCBuildModePool()== \n");

    CBiosExtension.pCBiosArguments = &CBiosArguments;
    CBiosExtension.IOAddress = (USHORT)(pRDC->RelocateIO);
    CBiosExtension.VideoVirtualAddress = (ULONG)(pRDC->FBVirtualAddr);

    do {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "Mode serial Num 0x%x\n",usSerialNum);

        CBiosArguments.reg.x.AX = OEMFunction;
        CBiosArguments.reg.x.BX = QuerySupportedMode;
        CBiosArguments.reg.x.CX = usSerialNum++;
        CInt10(&CBiosExtension);

        wVESAModeHorSize = (USHORT)(CBiosArguments.reg.ex.EDX & 0x0000FFFF);
        wVESAModeVerSize = (USHORT)(CBiosArguments.reg.ex.EDX >> 16);

        
        if (CBiosArguments.reg.x.AX == VBEFunctionCallSuccessful)
        {
            pMode = SearchDisplayModeRecPtr(pModePoolHead, CBiosArguments);

            if (pMode == NULL)
            {
                if (pModePoolHead != NULL)
                {
                    pModePoolTail->next = xnfcalloc(1, sizeof(DisplayModeRec));
                    pModePoolTail->next->prev = pModePoolTail;
                    pModePoolTail = pModePoolTail->next;
                }
                else
                {
                    pModePoolHead = xnfcalloc(1, sizeof(DisplayModeRec));
                    pModePoolHead->prev = NULL;
                    pModePoolTail = pModePoolHead;
                }
                
                pModePoolTail->next = NULL;

                pModePoolTail->name = pcConvertResolutionToString(CBiosArguments.reg.ex.EDX);

                pModePoolTail->status = MODE_OK;
                pModePoolTail->type = M_T_BUILTIN;
                pModePoolTail->Flags = 0;

                
                pModePoolTail->PrivSize = sizeof(MODE_PRIVATE);
                pModePoolTail->Private  = xnfcalloc(1, pModePoolTail->PrivSize);
                pModePrivate = MODE_PRIVATE_PTR(pModePoolTail);

                
                pModePoolTail->Clock = pModePoolTail->SynthClock = CBiosArguments.reg.ex.EDI;
                pModePoolTail->HDisplay = pModePoolTail->CrtcHDisplay = wVESAModeHorSize;
                pModePoolTail->VDisplay = pModePoolTail->CrtcVDisplay = wVESAModeVerSize;
                BTranslateIndexToRefreshRate(CBiosArguments.reg.lh.CH, &(pModePoolTail->VRefresh));
                
                pModePoolTail->PrivFlags = (int)CBiosArguments.reg.x.SI;
                pModePrivate->ucRRate_ID = CBiosArguments.reg.lh.CH;
            }
            else
            {
                pModePrivate = MODE_PRIVATE_PTR(pMode);
            }
            
            switch (CBiosArguments.reg.lh.CL)
            {
                case 8:
                    pModePrivate->Mode_ID_8bpp = CBiosArguments.reg.x.BX;
                    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "pModePrivate->Mode_ID_8bpp = 0x%x\n",pModePrivate->Mode_ID_8bpp);
                    break;
                case 16:
                    pModePrivate->Mode_ID_16bpp = CBiosArguments.reg.x.BX;
                    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "pModePrivate->Mode_ID_16bpp = 0x%x\n",pModePrivate->Mode_ID_16bpp);
                    break;
                case 32:
                    pModePrivate->Mode_ID_32bpp = CBiosArguments.reg.x.BX;
                    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "pModePrivate->Mode_ID_32bpp = 0x%x\n",pModePrivate->Mode_ID_32bpp);
                    break;
            }
        }
    } while (CBiosArguments.reg.x.AX == VBEFunctionCallSuccessful);

    
        for (bColorDepth = 0; bColorDepth < 3; bColorDepth++)
        {
            CBiosArguments.reg.x.AX = OEMFunction;
            CBiosArguments.reg.x.BX = QueryLCDPanelSizeMode;
            CBiosArguments.reg.x.CX = bColorDepth;
            CInt10(&CBiosExtension);

            if (CBiosArguments.reg.x.AX == VBEFunctionCallSuccessful)
            {
                pMode = SearchDisplayModeRecPtr(pModePoolHead, CBiosArguments);

                if (pMode == NULL)
                {
                    if (pModePoolHead != NULL)
                    {
                        pModePoolTail->next = xnfcalloc(1, sizeof(DisplayModeRec));
                        pModePoolTail->next->prev = pModePoolTail;
                        pModePoolTail = pModePoolTail->next;
                    }
                    else
                    {
                        pModePoolHead = xnfcalloc(1, sizeof(DisplayModeRec));
                        pModePoolHead->prev = NULL;
                        pModePoolTail = pModePoolHead;
                    }
                    
                    pModePoolTail->next = NULL;

                    pModePoolTail->name = pcConvertResolutionToString(CBiosArguments.reg.ex.EDX);

                    pModePoolTail->status = MODE_OK;
                    pModePoolTail->type = M_T_BUILTIN;
                    pModePoolTail->Flags = 0;

                    
                    pModePoolTail->PrivSize = sizeof(MODE_PRIVATE);
                    pModePoolTail->Private  = xnfcalloc(1, pModePoolTail->PrivSize);
                    pModePrivate = MODE_PRIVATE_PTR(pModePoolTail);

                    
                    pModePoolTail->Clock = pModePoolTail->SynthClock = CBiosArguments.reg.ex.EDI;
                    pModePoolTail->HDisplay = pModePoolTail->CrtcHDisplay = (CBiosArguments.reg.ex.EDX & 0x0000FFFF);
                    pModePoolTail->VDisplay = pModePoolTail->CrtcVDisplay = (CBiosArguments.reg.ex.EDX >> 16);
                    BTranslateIndexToRefreshRate(CBiosArguments.reg.lh.CH, &(pModePoolTail->VRefresh));
                    
                    pModePoolTail->PrivFlags = (int)CBiosArguments.reg.x.SI | LCD_TIMING;
                    pModePrivate->ucRRate_ID = CBiosArguments.reg.lh.CH;
                }
                else
                {
                    pModePrivate = MODE_PRIVATE_PTR(pMode);
                }
                
                switch (CBiosArguments.reg.lh.CL)
                {
                    case 8:
                        pModePrivate->Mode_ID_8bpp = CBiosArguments.reg.x.BX;
                        break;
                    case 16:
                        pModePrivate->Mode_ID_16bpp = CBiosArguments.reg.x.BX;
                        break;
                    case 32:
                        pModePrivate->Mode_ID_32bpp = CBiosArguments.reg.x.BX;
                        break;
                }
                
            }
        }

    
    
    
    
    
       
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCBuildModePool()== pModePoolHead = 0x%x\n", pModePoolHead);         
    return pModePoolHead;
}

Bool BTranslateIndexToRefreshRate(UCHAR ucRRateIndex, float *fRefreshRate)
{
    int i;

    for (i = 0; i < sizeof(RefreshRateMap)/sizeof(RRateInfo); i++)
    {
        if (RefreshRateMap[i].ucRRateIndex == ucRRateIndex)
        {
            *fRefreshRate = RefreshRateMap[i].fRefreshRate;
            return (TRUE);
        }
    }
    return (FALSE);
}

char* pcConvertResolutionToString(ULONG ulResolution)
{
    USHORT usHorResolution = (USHORT)(ulResolution & 0x0000FFFF);
    USHORT usVerResolution = (USHORT)(ulResolution >> 16);
    USHORT usTemp;
    int iIndex, iStringSize, i;
    char *pcResolution;
    
    pcResolution = xnfcalloc(1, 10);
    
    
    iIndex = 0;

    iStringSize = 1;
    usTemp = usHorResolution;
    while ((usTemp/10) > 0)
    {
        iStringSize++;
        usTemp /= 10;
    }

    usTemp = usHorResolution;
    for ( i = 1 ; i <= iStringSize; i++)
    {
        pcResolution[iIndex+ iStringSize - i] = (usTemp%10) + 0x30;
        usTemp /= 10;
    }
    iIndex += iStringSize;

    pcResolution[iIndex] = 'x';
    iIndex++;
    
    iStringSize = 1;
    usTemp = usVerResolution;
    while ((usTemp/10) > 0)
    {
        iStringSize++;
        usTemp /= 10;
    }

    usTemp = usVerResolution;
    for ( i = 1 ; i <= iStringSize; i++)
    {
        pcResolution[iIndex+ iStringSize - i] = (usTemp%10) + 0x30;
        usTemp /= 10;
    }
    iIndex += iStringSize;
    
    pcResolution[iIndex] = '\0';

    return pcResolution;
}

DisplayModePtr SearchDisplayModeRecPtr(DisplayModePtr pModePoolHead, CBIOS_ARGUMENTS CBiosArguments)
{
    DisplayModePtr pMode = pModePoolHead;
    MODE_PRIVATE *pModePrivate;
    
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Enter SearchDisplayModeRecPtr(CH = 0x%02X, EDX = 0x%08X, SI = 0x%X, EDI = %d)== \n", CBiosArguments.reg.lh.CH, CBiosArguments.reg.ex.EDX, CBiosArguments.reg.x.SI, CBiosArguments.reg.ex.EDI);
    
    while(pMode != NULL)
    {
        pModePrivate = MODE_PRIVATE_PTR(pMode);
        
        if ((pModePrivate->ucRRate_ID == CBiosArguments.reg.lh.CH) &&
            (pMode->HDisplay == (int)(CBiosArguments.reg.ex.EDX & 0x0000FFFF)) &&
            (pMode->VDisplay == (int)(CBiosArguments.reg.ex.EDX >>16)) &&
            ((pMode->PrivFlags & 0xFFFF) == (int)CBiosArguments.reg.x.SI) &&
            (pMode->Clock == CBiosArguments.reg.ex.EDI))
        {
            xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Exit1 SearchDisplayModeRecPtr()== \n");
            return pMode;
        }

        pMode = pMode->next;
    }
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Exit2 SearchDisplayModeRecPtr()== \n");
    return NULL;
}
