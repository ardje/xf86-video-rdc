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

#ifndef XFree86Module
#include "compiler.h"
#else
#include "xf86_ansic.h"
#endif

#include "xf86str.h"
#include "vbe.h"


#include "xf86Cursor.h"


#include "rdc.h"
#include "rdc_extension.h"





void EnableGamma(ScrnInfoPtr pScrn, BOOL Enable)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    BYTE CRA8 = 0;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Enter EnableGamma()== \n");

    
    SetReg(CRTC_PORT, 0xA8);
    CRA8 = GetReg(CRTC_DATA);

    if (Enable)
        CRA8 |= BIT1;
    else
        CRA8 &= ~BIT1;

    
    while(*(volatile DWORD *)(pRDC->MMIOVirtualAddr) & BIT0);
    while(!(*(volatile DWORD *)(pRDC->MMIOVirtualAddr) & BIT0));

    
    SetReg(CRTC_DATA, CRA8);
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Leave EnableGamma()== \n");
}


void SaveGammaTable(ScrnInfoPtr pScrn, PVIDEO_CLUT ClutBuffer)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    ULONG ulStatus;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Enter SaveGammaTable()== \n");
    
    
}

 
BOOL CompGamma(PVIDEO_CLUT ClutBuffer)
{
    DWORD i;

    for (i=0; i < ClutBuffer->NumEntries; i++)
    {
        if ((ClutBuffer->LookupTable[i].RgbArray.Red != i) ||
            (ClutBuffer->LookupTable[i].RgbArray.Green != i) ||
            (ClutBuffer->LookupTable[i].RgbArray.Blue != i) )
        {
            return FALSE;
        }
    }
    
    return TRUE;
}


int VgaSetColorLookup(ScrnInfoPtr pScrn, PVIDEO_CLUT ClutBuffer, ULONG ClutBufferSize)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    USHORT i, j;
    PVIDEO_CLUT LUTCachePtr = NULL;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Enter VgaSetColorLookup()== \n");

    
    
    
    if ((ClutBufferSize < (sizeof(VIDEO_CLUT) - sizeof(ULONG))) ||
        (ClutBufferSize < (sizeof(VIDEO_CLUT) +
        (sizeof(ULONG) * (ClutBuffer->NumEntries - 1))))) 
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "VgaSetColorLookup - insufficient buffer\n"); 
        
        return UT_FAIL;
    }

    
    
    
    if ( (ClutBuffer->NumEntries == 0) ||
         (ClutBuffer->FirstEntry > VIDEO_MAX_COLOR_REGISTER) ||
         (ClutBuffer->FirstEntry + ClutBuffer->NumEntries >
                                     VIDEO_MAX_COLOR_REGISTER + 1) ) 
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "VgaSetColorLookup - invalid parameter\n");
        return UT_FAIL;
    }

    
    
    
    if (pScrn->depth == 8)
    {
        
        for (i = 0, j = ClutBuffer->FirstEntry; 
             i < ClutBuffer->NumEntries; 
             i++, j++)
        {
           VGA_LOAD_PALETTE_INDEX (j, \
                                   ClutBuffer->LookupTable[i].RgbArray.Red   >> 2, \
                                   ClutBuffer->LookupTable[i].RgbArray.Green >> 2, \
                                   ClutBuffer->LookupTable[i].RgbArray.Blue  >> 2);
        }                           
    }
    else
    {
        

        
        if (CompGamma(ClutBuffer))
        {
            pRDC->bEnableGamma = FALSE;
            EnableGamma(pScrn, FALSE);
        }
        else
        {
            pRDC->bEnableGamma = TRUE;
            
            EnableGamma(pScrn,TRUE);    

            
            for (i = 0, j = ClutBuffer->FirstEntry; 
                 i < ClutBuffer->NumEntries; 
                 i++, j++)
            {
               VGA_LOAD_PALETTE_INDEX (j, \
                                       ClutBuffer->LookupTable[i].RgbArray.Red  , \
                                       ClutBuffer->LookupTable[i].RgbArray.Green, \
                                       ClutBuffer->LookupTable[i].RgbArray.Blue );
            }
        }
        

        
        SaveGammaTable(pRDC, ClutBuffer);
    }
    
    return UT_SUCCESS;
} 
