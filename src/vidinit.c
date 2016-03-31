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

#include "rdc.h"

void QDec2Hex( Cofe *RGB )
{
	int iIt[12],i;
	double dTmp[12];

    dTmp[0]  = fabs(RGB->A1);
    dTmp[1]  = fabs(RGB->B1);
    dTmp[2]  = fabs(RGB->C1);
    dTmp[3]  = fabs(RGB->D1);
    dTmp[4]  = fabs(RGB->A2);
    dTmp[5]  = fabs(RGB->B2);
    dTmp[6]  = fabs(RGB->C2);
    dTmp[7]  = fabs(RGB->D2);
    dTmp[8]  = fabs(RGB->A3);
    dTmp[9]  = fabs(RGB->B3);
    dTmp[10] = fabs(RGB->C3);
    dTmp[11] = fabs(RGB->D3);
    
    iIt[0]  = (int)dTmp[0];
    iIt[1]  = (int)dTmp[1];
    iIt[2]  = (int)dTmp[2];
    iIt[3]  = (int)dTmp[3];
    iIt[4]  = (int)dTmp[4];
    iIt[5]  = (int)dTmp[5];
    iIt[6]  = (int)dTmp[6];
    iIt[7]  = (int)dTmp[7];
    iIt[8]  = (int)dTmp[8];
    iIt[9]  = (int)dTmp[9];
    iIt[10] = (int)dTmp[10];
    iIt[11] = (int)dTmp[11];
    
    RGB->dwIA1 = iIt[0];
    RGB->dwIB1 = iIt[1];
    RGB->dwIC1 = iIt[2];
    RGB->dwID1 = iIt[3];
    RGB->dwIA2 = iIt[4];
    RGB->dwIB2 = iIt[5];
    RGB->dwIC2 = iIt[6];
    RGB->dwID2 = iIt[7];
    RGB->dwIA3 = iIt[8];
    RGB->dwIB3 = iIt[9];
    RGB->dwIC3 = iIt[10];
    RGB->dwID3 = iIt[11];

	for( i=0 ; i< 12 ; i++)
		dTmp[i] -= iIt[i];

	RGB->dwPA1 = ( (unsigned long)(dTmp[0] * (float)0x100)) & 0xff ;
	RGB->dwPB1 = ( (unsigned long)(dTmp[1] * (float)0x100)) & 0xff ;
	RGB->dwPC1 = ( (unsigned long)(dTmp[2] * (float)0x100)) & 0xff ;
	RGB->dwPD1 = ( (unsigned long)(dTmp[3] * (float)0x100)) & 0xff ;
	RGB->dwPA2 = ( (unsigned long)(dTmp[4] * (float)0x100)) & 0xff ;
	RGB->dwPB2 = ( (unsigned long)(dTmp[5] * (float)0x100)) & 0xff ;
	RGB->dwPC2 = ( (unsigned long)(dTmp[6] * (float)0x100)) & 0xff ;
	RGB->dwPD2 = ( (unsigned long)(dTmp[7] * (float)0x100)) & 0xff ;
	RGB->dwPA3 = ( (unsigned long)(dTmp[8] * (float)0x100)) & 0xff ;
	RGB->dwPB3 = ( (unsigned long)(dTmp[9] * (float)0x100)) & 0xff ;
	RGB->dwPC3 = ( (unsigned long)(dTmp[10] * (float)0x100)) & 0xff ;
	RGB->dwPD3 = ( (unsigned long)(dTmp[11] * (float)0x100)) & 0xff ;
}

unsigned long CMD(Cofe *RGB, int op)
{
	int tmpA,tmpB,tmpC;
	unsigned long dwtmpA=0,dwtmpB=0,dwtmpC=0,Reg=0;

	switch(op)
	{
	case 1:
		if( RGB->A1<0 )
		{
			Reg |= BIT11;
			RGB->dwIA1 = (~RGB->dwIA1) & 0x3;
			RGB->dwPA1 = (~RGB->dwPA1);
		}
		if( RGB->B1<0 )
		{
			Reg |= BIT21;
			RGB->dwIB1 = (~RGB->dwIB1)  & 0x3;
			RGB->dwPB1 = (~RGB->dwPB1);
		}
		if( RGB->C1<0 )
		{
			Reg |= BIT31;
			RGB->dwIC1 = (~RGB->dwIC1)  & 0x3;
			RGB->dwPC1 = (~RGB->dwPC1);
		}

		if( RGB->dwIA1 > 0xf )
			RGB->dwIA1 = 0xf;
		if( RGB->dwIB1 > 0xf )
			RGB->dwIB1 = 0xf;
		if( RGB->dwIC1 > 0xf )
			RGB->dwIC1 = 0xf;

		Reg |= ( (RGB->dwIC1 << 29) | (( RGB->dwPC1 >> 1) & 0x7F ) << 22 )  | 
		       ( (RGB->dwIB1 << 19) | (( RGB->dwPB1 >>1 )& 0x7F) << 12 ) | 
		       ( (RGB->dwIA1 << 9) | ( (RGB->dwPA1 >>1 )& 0x7F ) << 2);
		break;
	
	case 2:
		if( RGB->A2<0 )
		{
			Reg |= BIT11;
			RGB->dwIA2 = (~RGB->dwIA2)  & 0x3;
			RGB->dwPA2 = (~RGB->dwPA2);
		}
		if( RGB->B2<0 )
		{
			Reg |= BIT21;
			RGB->dwIB2 = (~RGB->dwIB2)  & 0x3;
			RGB->dwPB2 = (~RGB->dwPB2);
		}
		if( RGB->C2<0 )
		{
			Reg |= BIT31;
			RGB->dwIC2 = (~RGB->dwIC2)  & 0x3;
			RGB->dwPC2 = (~RGB->dwPC2);
		}

		if( RGB->dwIA2 > 0xf )
			RGB->dwIA2 = 0xf;
		if( RGB->dwIB2 > 0xf )
			RGB->dwIB2 = 0xf;
		if( RGB->dwIC2 > 0xf )
			RGB->dwIC2 = 0xf;

		Reg |= ( (RGB->dwIC2 << 29) | (( RGB->dwPC2 >> 1) & 0x7F ) << 22 )  | 
		       ( (RGB->dwIB2 << 19) | ((( RGB->dwPB2 >>1 )& 0x7F) << 12 ) );
		break;

	case 3:
		if( RGB->A3<0 )
		{
			Reg |= BIT11;
			RGB->dwIA3 = (~RGB->dwIA3)  & 0x3;
			RGB->dwPA3 = (~RGB->dwPA3);
		}
		if( RGB->B3<0 )
		{
			Reg |= BIT21;
			RGB->dwIB3 = (~RGB->dwIB3) & 0x3 ;
			RGB->dwPB3 = (~RGB->dwPB3);
		}
		if( RGB->C3<0 )
		{
			Reg |= BIT31;
			RGB->dwIC3 = (~RGB->dwIC3) & 0x3;
			RGB->dwPC3 = (~RGB->dwPC3);
		}

		if( RGB->dwIA3 > 0xf )
			RGB->dwIA3 = 0xf;
		if( RGB->dwIB3 > 0xf )
			RGB->dwIB3 = 0xf;
		if( RGB->dwIC3 > 0xf )
			RGB->dwIC3 = 0xf;

		Reg |= ( (RGB->dwIC3 << 29) | (( RGB->dwPC3 >> 1) & 0x7F ) << 22 )  | 
		       ( (RGB->dwIB3 << 19) | ((( RGB->dwPB3 >>1 )& 0x7F) << 12 ));
		break;

	case 4:
		if( RGB->D1<0 )
		{
			Reg |= BIT9;
			RGB->dwID1 = (~RGB->dwID1) &0xFFF ;
			RGB->dwPD1 = (~RGB->dwPD1);
		}
		if( RGB->D2<0 )
		{
			Reg |= BIT20;
			RGB->dwID2 = (~RGB->dwID2)  &0xFFF;
			RGB->dwPD2 = (~RGB->dwPD2);
		}
		if( RGB->D3<0 )
		{
			Reg |= BIT31;
			RGB->dwID3 = (~RGB->dwID3)  &0xFFF;
			RGB->dwPD3 = (~RGB->dwPD3);
		}

		if(RGB->dwPD1)
			Reg |= BIT0;
		if(RGB->dwPD2)
			Reg |= BIT10;
		if(RGB->dwPD3)
			Reg |= BIT21;

		Reg |= ( (RGB->dwID3 << 22) | (RGB->dwID2 << 11) | ( RGB->dwID1 << 1) );
		break;
	}
	return Reg;
}

void CheckBoundary(float *pfValue, float fMin, float fMax)
{
    if (*pfValue > fMax)
        *pfValue = fMax;
    else if (*pfValue < fMin)
        *pfValue = fMin;
};

void SetVIDColor(RDCRecPtr pRDC)
{
    POVERLAY_STATUS lpOverlayStatus;
    LPVIDCOLORENHANCE lpVidColor;
    Cofe    VidColCofe;
    float   fPI, fContrast, fSaturation, fHue, fBrightness;
    float   fCIncrement, fSIncrement, fHIncrement, fBIncrement;
    PKT_SC  *pSingleCMD;

    lpOverlayStatus = &(pRDC->OverlayStatus);
    lpVidColor = &(lpOverlayStatus->VidColorEnhance);

    fPI = (float)(PI/180);

    
    
    fBrightness = (float) lpVidColor->ulScaleBrightness;
    if (fBrightness == BRIGHTNESS_DEFAULT)
    {
        fBrightness = 0;
    }
    else if (fBrightness > BRIGHTNESS_DEFAULT)
    {
        fBIncrement = (float)((127.) / (255.-128.));
        fBrightness = 0 + fBIncrement * (fBrightness-10000)/78;
    }
    else
    {
        fBIncrement = (float)((128.) / (128. - 0.));
        fBrightness = 0 - fBIncrement * (10000-fBrightness)/78;
    }

    
    
    fContrast = (float) lpVidColor->ulScaleContrast;
    if (fContrast == CONTRAST_DEFAULT)
    {
        fContrast = 1;
    }
    else if (fContrast > CONTRAST_DEFAULT)
    {
        fCIncrement = (float)((2.-1.) / (255.-128.));
        fContrast = 1 + fCIncrement * (fContrast-10000)/78;
    }
    else
    {
        fCIncrement =  (float)((1.-0.) / (128.-0.));
        fContrast = 1 - fCIncrement * (10000-fContrast)/78;
    }

    
    if(fContrast < 0)
        fContrast = 0;

    
    
    if (lpVidColor->ulScaleHue & 0xFFFFFF00)
    {
        fHue = (float)(0-lpVidColor->ulScaleHue);
        fHue = -fHue;
    }
    else
    {
        fHue = (float) lpVidColor->ulScaleHue;
    }
    
    if (fHue == HUE_DEFAULT)
    {
        fHue = 0;
    }
    else if (fHue > HUE_DEFAULT)
    {
        fHIncrement = (float)((180.-0.) / (255.-128.));
        fHue = (float)(0 + fHIncrement * (fHue-0)/1.4);
    }
    else
    {
        fHIncrement = (float)((0.-(-180.)) / (128.-0.));
        fHue = (float)(0 - fHIncrement * (0-fHue)/1.4);
    } 
    
    
    
    fSaturation = (float)lpVidColor->ulScaleSaturation;
    if (fSaturation == SATURATION_DEFAULT)
    {
        fSaturation = 1;
    }
    else if (fSaturation > SATURATION_DEFAULT)
    {
        fSIncrement = (float)((2.-1.) / (255.-128.));
        fSaturation = 1 + fSIncrement * (fSaturation-10000)/78;
    }
    else
    {
        fSIncrement = (float)((1.-0.) / (128.-0.));
        fSaturation = 1 - fSIncrement * (10000-fSaturation)/78;
    }

    
    CheckBoundary(&fBrightness, -128, 127);
    CheckBoundary(&fHue, -180, 180);
    CheckBoundary(&fContrast, 0, 255);
    CheckBoundary(&fSaturation, 0, 255);
    
    
    VidColCofe.A1 = fContrast;
    VidColCofe.B1 = VidColCofe.C1 = 0;
    VidColCofe.D1 = (float) fBrightness;

    VidColCofe.A2 = fContrast;
    VidColCofe.B2 = (float)(fSaturation * cos(fHue*fPI));
    VidColCofe.C2 = (float)(fSaturation * (-sin(fHue*fPI)));
    VidColCofe.D2 = (float)(-128)*VidColCofe.B2+(-128)*VidColCofe.C2+128;

    VidColCofe.A3 = fContrast;
    VidColCofe.B3 = (float)(fSaturation * sin(fHue*fPI));
    VidColCofe.C3 = (float)(fSaturation * cos(fHue*fPI));
    VidColCofe.D3 = (float)(-128)*VidColCofe.B3+(-128)*VidColCofe.C3+128;

    
    QDec2Hex(&VidColCofe);

    lpVidColor->ulFact1 = CMD(&VidColCofe, 1);
    lpVidColor->ulFact2 = CMD(&VidColCofe, 2);
    lpVidColor->ulFact3 = CMD(&VidColCofe, 3);
    lpVidColor->ulFact4 = CMD(&VidColCofe, 4);

    if((lpVidColor->ulFact1 == 0x200)&&
       (lpVidColor->ulFact2 == 0x80000)&&
       (lpVidColor->ulFact3 == 0x20000000)&&
       (lpVidColor->ulFact4 == 0))
    {
        pRDC->bColorEnhanceOn = FALSE;       

        lpOverlayStatus->ulVidDispCtrl &= ~VDPS_COLOR_HNHANCE ;
    }
    else
    {
        pRDC->bColorEnhanceOn = TRUE;       

        if (DEVICE_ID(pRDC->PciInfo) == PCI_CHIP_M2010)
        {
            lpOverlayStatus->ulVidDispCtrl |= VDPS_COLOR_HNHANCE ;
            pRDC->bVPColorEnhance = FALSE;
        }
        else
            pRDC->bVPColorEnhance = TRUE;
    };

    if(pRDC->MMIOVPost)
    {
        if (pRDC->bVPColorEnhance) 
        {
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VP_COLOR_ENHANCE_CTL1) = lpVidColor->ulFact1;
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VP_COLOR_ENHANCE_CTL2) = lpVidColor->ulFact2;
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VP_COLOR_ENHANCE_CTL3) = lpVidColor->ulFact3;
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VP_COLOR_ENHANCE_CTL4) = lpVidColor->ulFact4;
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VP_CSC_CTL) |= BIT28;
        }
        else 
        {
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VDP_COLOR_ENHANCE_CTL1) = lpVidColor->ulFact1;
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VDP_COLOR_ENHANCE_CTL2) = lpVidColor->ulFact2;
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VDP_COLOR_ENHANCE_CTL3) = lpVidColor->ulFact3;
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VDP_COLOR_ENHANCE_CTL4) = lpVidColor->ulFact4;
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VDP_CTL) |= lpOverlayStatus->ulVidDispCtrl;
            *(volatile unsigned long *)(pRDC->MMIOVirtualAddr + VDP_REG_UPDATE_MOD_SELECT) |= VDPS_FIRE ;
        };
    }
    else
    {
        if (pRDC->bVPColorEnhance) 
        {
            PKT_SC *pCMDQ;
            if (pRDC->IoctlCR)
            {
                pSingleCMD = (PKT_SC *) malloc(PKT_SINGLE_LENGTH*6);
                pCMDQ = pSingleCMD;
            }
            else
                pSingleCMD = (PPKT_SC)pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*6);
            
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) CR_LOCK_ID; 
            pSingleCMD->PKT_SC_dwData[0] = CR_LOCK_VIDEO;  
            pSingleCMD++;

            
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_COLOR_ENHANCE_CTL1_I;
            pSingleCMD->PKT_SC_dwData[0] = lpVidColor->ulFact1;
            pSingleCMD++;
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_COLOR_ENHANCE_CTL2_I;
            pSingleCMD->PKT_SC_dwData[0] = lpVidColor->ulFact2;
            pSingleCMD++;
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_COLOR_ENHANCE_CTL3_I;
            pSingleCMD->PKT_SC_dwData[0] = lpVidColor->ulFact3;
            pSingleCMD++;
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_COLOR_ENHANCE_CTL4_I;
            pSingleCMD->PKT_SC_dwData[0] = lpVidColor->ulFact4;
            pSingleCMD++;

            
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_CSC_CTL_I;
            pSingleCMD->PKT_SC_dwData[0] = BIT28;
            
            
            if (pRDC->IoctlCR)
                FireCRCMDQ(pRDC->iFBDev, pCMDQ, 6);
            else
                mUpdateWritePointer;
        }
        else 
        {
            PKT_SC *pCMDQ;
            if (pRDC->IoctlCR)
            {
                pSingleCMD = (PKT_SC *) malloc(PKT_SINGLE_LENGTH*7);
                pCMDQ = pSingleCMD;
            }
            else
                pSingleCMD = (PPKT_SC)pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*7);
            
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) CR_LOCK_ID; 
            pSingleCMD->PKT_SC_dwData[0] = CR_LOCK_VIDEO;  
            pSingleCMD++;

            
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_COLOR_ENHANCE_CTL1_I;
            pSingleCMD->PKT_SC_dwData[0] = lpVidColor->ulFact1;
            pSingleCMD++;
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_COLOR_ENHANCE_CTL2_I;
            pSingleCMD->PKT_SC_dwData[0] = lpVidColor->ulFact2;
            pSingleCMD++;
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_COLOR_ENHANCE_CTL3_I;
            pSingleCMD->PKT_SC_dwData[0] = lpVidColor->ulFact3;
            pSingleCMD++;
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_COLOR_ENHANCE_CTL4_I;
            pSingleCMD->PKT_SC_dwData[0] = lpVidColor->ulFact4;
            pSingleCMD++;

            
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_CTL_I;
            pSingleCMD->PKT_SC_dwData[0] = lpOverlayStatus->ulVidDispCtrl;
            pSingleCMD++;
            pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_REG_UPDATE_MOD_SELECT_I;
            pSingleCMD->PKT_SC_dwData[0] = VDPS_FIRE;
            
            
            if (pRDC->IoctlCR)
                FireCRCMDQ(pRDC->iFBDev, pCMDQ, 7);
            else
                mUpdateWritePointer;;
        };
    }
}

