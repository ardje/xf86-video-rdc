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


#define _rdc_mode_c_
#include "rdc.h"
#include "rdc_mode.h"

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



RDC_EXPORT Bool RDCSetMode(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    RDCRecPtr pRDC;
    MODE_PRIVATE *pModePrivate;
    CBIOS_ARGUMENTS *pCBiosArguments;
    USHORT usVESAMode;
    
    pRDC = RDCPTR(pScrn);
    pModePrivate = MODE_PRIVATE_PTR(mode);
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Enter RDCSetMode()== \n");

    vRDCOpenKey(pScrn);
    bRDCRegInit(pScrn);
    
    pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
    
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " Set Display1 Refresh Rate \n");

    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->AX = OEMFunction;
    pCBiosArguments->BX = SetDisplay1RefreshRate;
    pCBiosArguments->CL = pModePrivate->ucRRate_ID;
    
    CInt10(pRDC->pCBIOSExtension);

    usVESAMode = usGetVbeModeNum(pScrn, mode);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " RDCSetMode() Set VESA Mode 0x%x== \n",usVESAMode);
    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->AX = VBEFunction02;
    pCBiosArguments->BX = (0x4000 | usVESAMode);
    
    CInt10(pRDC->pCBIOSExtension);
    
    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->AX = VBEFunction06;
    
    pCBiosArguments->BL = 0x02;
    
    
    pCBiosArguments->CX = (USHORT)((ALIGN_TO_UB_32(pScrn->displayWidth*pScrn->bitsPerPixel)) >> 3);

    
    CInt10(pRDC->pCBIOSExtension);

    

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "==Exit RDCSetMode(), return true== \n");    
    return (TRUE);    
}

RDC_EXPORT USHORT usGetVbeModeNum(ScrnInfoPtr pScrn, DisplayModePtr mode)
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

    
    if (pRDC->DeviceInfo.ucNewDeviceID == TVIndex && pRDC->bEnableTVPanning)
    {
        WORD wHSize = pRDC->TVEncoderInfo[0].TVOut_HSize;
        switch (ucColorDepth)
        {
        case 8:
            if(wHSize == 640)
                usVESAModeNum = 0x101;
            else if(wHSize == 800)
                usVESAModeNum = 0x103;
            else
                usVESAModeNum = 0x105;
            break;
        case 16:
            if(wHSize == 640)
                usVESAModeNum = 0x111;
            else if(wHSize == 800)
                usVESAModeNum = 0x114;
            else
                usVESAModeNum = 0x117;
            break;
        case 32:
            if(wHSize == 640)
                usVESAModeNum = 0x112;
            else if(wHSize == 800)
                usVESAModeNum = 0x115;
            else
                usVESAModeNum = 0x118;
            break;
        }

    }else
    {
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
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit usGetVbeModeNum() return VESA Mode = 0x%x==\n", usVESAModeNum);
    return usVESAModeNum;

}

RDC_EXPORT DisplayModePtr RDCBuildModePool(ScrnInfoPtr pScrn)
{
    DisplayModePtr pMode = NULL, pModePoolHead = NULL, pModePoolTail = NULL;
    
    CBIOS_ARGUMENTS *pCBiosArguments;
    RDCRecPtr pRDC = RDCPTR(pScrn);
    MODE_PRIVATE *pModePrivate;
    USHORT usSerialNum = 0;
    USHORT wLCDHorSize, wLCDVerSize;
    USHORT wVESAModeHorSize, wVESAModeVerSize;
    BYTE bColorDepth; 
    BYTE bEnoughMem;
    ULONG   ulModeMemSize;
    
    pRDC->ulMaxPitch = pRDC->ulMaxHeight = 0;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCBuildModePool()== \n");

    
    pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;

    do {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "Mode serial Num 0x%x\n",usSerialNum);

        
        memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
        pCBiosArguments->AX = OEMFunction;
        pCBiosArguments->BX = QuerySupportedMode;
        pCBiosArguments->CX = usSerialNum++;
        
        CInt10(pRDC->pCBIOSExtension);

        wVESAModeHorSize = (USHORT)(pCBiosArguments->Edx & 0x0000FFFF);
        wVESAModeVerSize = (USHORT)(pCBiosArguments->Edx >> 16);

        
        bEnoughMem = FALSE;
        
        
        if (pScrn->bitsPerPixel == pCBiosArguments->CL)
        {
            
            
            if (wVESAModeHorSize > pRDC->ulMaxPitch)
                pRDC->ulMaxPitch = wVESAModeHorSize;
                
            if (wVESAModeVerSize > pRDC->ulMaxHeight)
                pRDC->ulMaxHeight = wVESAModeVerSize;

            ulModeMemSize = ALIGN_TO_UB_32(pRDC->ulMaxPitch * pCBiosArguments->CL >> 3);
            ulModeMemSize = ulModeMemSize * pRDC->ulMaxHeight;
            
            if (pRDC->AvailableFBsize > ulModeMemSize)
                bEnoughMem = TRUE;
        }

        
        if (pCBiosArguments->AX == VBEFunctionCallSuccessful)
        {
            pMode = SearchDisplayModeRecPtr(pModePoolHead, pCBiosArguments);

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

                pModePoolTail->name = pcConvertResolutionToString(pCBiosArguments->Edx);

                pModePoolTail->status = MODE_OK;
                pModePoolTail->type = M_T_BUILTIN;
                pModePoolTail->Flags = 0;

                
                pModePoolTail->PrivSize = sizeof(MODE_PRIVATE);
                pModePoolTail->Private  = xnfcalloc(1, pModePoolTail->PrivSize);
                pModePrivate = MODE_PRIVATE_PTR(pModePoolTail);

                
                pModePoolTail->Clock = pModePoolTail->SynthClock = pCBiosArguments->Edi;
                pModePoolTail->HDisplay = pModePoolTail->CrtcHDisplay = wVESAModeHorSize;
                pModePoolTail->VDisplay = pModePoolTail->CrtcVDisplay = wVESAModeVerSize;
                pModePoolTail->HTotal = (int)(pCBiosArguments->Esi & 0x0000FFFF);
                pModePoolTail->VTotal = (int)(pCBiosArguments->Esi >> 16);
 
                BTranslateIndexToRefreshRate(pCBiosArguments->CH, &(pModePoolTail->VRefresh));
                
                pModePoolTail->PrivFlags = (int)pCBiosArguments->SI;
                pModePrivate->ucRRate_ID = pCBiosArguments->CH;
            }
            else
            {
                pModePrivate = MODE_PRIVATE_PTR(pMode);
            }
            
            switch (pCBiosArguments->CL)
            {
                case 8:
                    pModePrivate->Mode_ID_8bpp = pCBiosArguments->BX;
                    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "pModePrivate->Mode_ID_8bpp = 0x%x\n",pModePrivate->Mode_ID_8bpp);
                    break;
                case 16:
                    pModePrivate->Mode_ID_16bpp = pCBiosArguments->BX;
                    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "pModePrivate->Mode_ID_16bpp = 0x%x\n",pModePrivate->Mode_ID_16bpp);
                    break;
                case 32:
                    pModePrivate->Mode_ID_32bpp = pCBiosArguments->BX;
                    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "pModePrivate->Mode_ID_32bpp = 0x%x\n",pModePrivate->Mode_ID_32bpp);
                    break;
            }
        }
    } while (pCBiosArguments->AX == VBEFunctionCallSuccessful);

    
        for (bColorDepth = 0; bColorDepth < 3; bColorDepth++)
        {
            
            memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
            pCBiosArguments->AX = OEMFunction;
            pCBiosArguments->BX = QueryLCDPanelSizeMode;
            pCBiosArguments->CX = bColorDepth;
            
            CInt10(pRDC->pCBIOSExtension);
            if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
            {
                pMode = SearchDisplayModeRecPtr(pModePoolHead, pCBiosArguments);

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

                    pModePoolTail->name = pcConvertResolutionToString(pCBiosArguments->Edx);

                    pModePoolTail->status = MODE_OK;
                    pModePoolTail->type = M_T_BUILTIN;
                    pModePoolTail->Flags = 0;

                    
                    pModePoolTail->PrivSize = sizeof(MODE_PRIVATE);
                    pModePoolTail->Private  = xnfcalloc(1, pModePoolTail->PrivSize);
                    pModePrivate = MODE_PRIVATE_PTR(pModePoolTail);

                    
                    pModePoolTail->Clock = pModePoolTail->SynthClock = pCBiosArguments->Edi;
                    pModePoolTail->HDisplay = pModePoolTail->CrtcHDisplay = (pCBiosArguments->Edx & 0x0000FFFF);
                    pModePoolTail->VDisplay = pModePoolTail->CrtcVDisplay = (pCBiosArguments->Edx >> 16);
                    BTranslateIndexToRefreshRate(pCBiosArguments->CH, &(pModePoolTail->VRefresh));
                    
                    pModePoolTail->PrivFlags = (int)pCBiosArguments->SI | LCD_TIMING;
                    pModePrivate->ucRRate_ID = pCBiosArguments->CH;
                }
                else
                {
                    pModePrivate = MODE_PRIVATE_PTR(pMode);
                }

                if (pModePoolTail->PrivFlags & LCD_TIMING)
                {
                    switch (pCBiosArguments->CL)
                    {
                        case 8:
                            pModePrivate->Mode_ID_8bpp  = pCBiosArguments->BX;
                            break;
                        case 16:
                            pModePrivate->Mode_ID_16bpp = pCBiosArguments->BX;
                            break;
                        case 32:
                            pModePrivate->Mode_ID_32bpp = pCBiosArguments->BX;
                            break;
                    }
                }
                
            }
        }

    
    
    
    
    
       
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCBuildModePool()== pModePoolHead = 0x%x\n", pModePoolHead);         
    return pModePoolHead;
}

RDC_EXPORT Bool BTranslateIndexToRefreshRate(UCHAR ucRRateIndex, float *fRefreshRate)
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

RDC_EXPORT char* pcConvertResolutionToString(ULONG ulResolution)
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

RDC_EXPORT DisplayModePtr SearchDisplayModeRecPtr(DisplayModePtr pModePoolHead, CBIOS_ARGUMENTS *pCBiosArguments)
{
    DisplayModePtr pMode = pModePoolHead;
    MODE_PRIVATE *pModePrivate;
    
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Enter SearchDisplayModeRecPtr(CH = 0x%02X, EDX = 0x%08X, SI = 0x%X, EDI = %d)== \n",
        pCBiosArguments->CH, pCBiosArguments->Edx, pCBiosArguments->SI, pCBiosArguments->Edi);
    
    while(pMode != NULL)
    {
        pModePrivate = MODE_PRIVATE_PTR(pMode);
        
        if ((pModePrivate->ucRRate_ID == pCBiosArguments->CH) &&
            (pMode->HDisplay == (int)(pCBiosArguments->Edx & 0x0000FFFF)) &&
            (pMode->VDisplay == (int)(pCBiosArguments->Edx >>16)) &&
            ((pMode->PrivFlags & 0xFFFF) == (int)pCBiosArguments->SI) &&
            (pMode->Clock == pCBiosArguments->Edi))
        {
            xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Exit1 SearchDisplayModeRecPtr()== \n");
            return pMode;
        }

        pMode = pMode->next;
    }
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "==Exit2 SearchDisplayModeRecPtr()== \n");
    return NULL;
}
