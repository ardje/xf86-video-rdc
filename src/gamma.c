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



RDC_EXPORT void EnableGamma(ScrnInfoPtr pScrn, BOOL Enable)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    BYTE CRA8 = 0;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Enter EnableGamma()== \n");

    
    SetReg(COLOR_CRTC_INDEX, 0xA8);
    CRA8 = GetReg(COLOR_CRTC_DATA);

    if (Enable)
        CRA8 |= BIT1;
    else
        CRA8 &= ~BIT1;

    
    while(*(volatile DWORD *)(pRDC->MMIOVirtualAddr) & BIT0);
    while(!(*(volatile DWORD *)(pRDC->MMIOVirtualAddr) & BIT0));

    
    SetReg(COLOR_CRTC_DATA, CRA8);
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Leave EnableGamma()== \n");
}


RDC_EXPORT void SaveGammaTable(ScrnInfoPtr pScrn, PVIDEO_CLUT ClutBuffer)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    ULONG ulStatus;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Enter SaveGammaTable()== \n");
    
    
}

 
RDC_EXPORT BOOL CompGamma(PVIDEO_CLUT ClutBuffer)
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


RDC_EXPORT int VgaSetColorLookup(ScrnInfoPtr pScrn, PVIDEO_CLUT ClutBuffer, ULONG ClutBufferSize)
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
        

        
        SaveGammaTable(pScrn, ClutBuffer);
    }
    
    return UT_SUCCESS;
} 
