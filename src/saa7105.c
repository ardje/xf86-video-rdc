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
     
#ifndef XFree86Module
#include "compiler.h"
#else
#include "xf86_ansic.h"
#endif

#include "typedef.h"
#include "saa7105.h"

void SAATest()
{


}

void SetSAA7105InitReg(UCHAR ucI2Cport, UCHAR ucDevAddress)
{
    SAA7105_Init_Reg *pInitSAA7105Reg = (SAA7105_Init_Reg*)(&SAA7105InitReg);
    do{
        
        WriteI2C(ucI2Cport,ucDevAddress,pInitSAA7105Reg->ucIndex,pInitSAA7105Reg->ucData);
        pInitSAA7105Reg = (SAA7105_Init_Reg*)((int)pInitSAA7105Reg + sizeof(SAA7105_Init_Reg));
    }while(pInitSAA7105Reg->ucIndex!=0xFF);
    xf86DrvMsgVerb(0, 0, 4, "==SetSAA7105InitReg Completed !!==\n");
}

void SetSAA7105DACPower(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR ucTVConnectorType)
{
    switch(ucTVConnectorType)
    {
    case CVBS:        
        WriteI2C(ucI2Cport,ucDevAddress,SAA7105DACPower,0x24);
        break;
    case SVideo:      
        WriteI2C(ucI2Cport,ucDevAddress,SAA7105DACPower,0x94);
        break;
    case CVBS+SVideo: 
        WriteI2C(ucI2Cport,ucDevAddress,SAA7105DACPower,0xB4);
        break;
    case 0:           
    default:          
        WriteI2C(ucI2Cport,ucDevAddress,SAA7105DACPower,0x08);
        break;
    }
    xf86DrvMsgVerb(0, 0, 4, "==SetSAA7105DACPower to %x==\n",ucTVConnectorType);
}

UCHAR bSetSAA7105Reg(UCHAR bDisplayPath, USHORT wModeNum, UCHAR ucTVType, UCHAR ucI2Cport, UCHAR ucDevAddress)
{
    UCHAR i;
    SAA7105_Mode_TABLE *pSAAModeTable;

    if(ucTVType==NTSC)
        pSAAModeTable = (SAA7105_Mode_TABLE*)(&CBIOS_NTSC_TV_Mode_Table);
    else if(ucTVType==PAL)
        pSAAModeTable = (SAA7105_Mode_TABLE*)(&CBIOS_PAL_TV_Mode_Table);
    else
    {
        xf86DrvMsgVerb(0, 0, 4, "==Unknown TV Type = %x==\n",ucTVType);
        return FALSE;
    }
    for(i=0; i<TVSupportModeCnt; i++)
    {
        if(wModeNum == pSAAModeTable->Mode_ID_8bpp ||
           wModeNum == pSAAModeTable->Mode_ID_16bpp ||
           wModeNum == pSAAModeTable->Mode_ID_32bpp)
           break;
        else
            pSAAModeTable = (SAA7105_Mode_TABLE*)((int)pSAAModeTable + sizeof(SAA7105_Mode_TABLE));
    }
    if(i==TVSupportModeCnt)
    {
        xf86DrvMsgVerb(0, 0, 4, "==Can not found any TV Mode Table = %x==\n");
        return FALSE; 
    }
    
    for(i=0; i<13; i++)
        WriteI2C(ucI2Cport,ucDevAddress, i+0x5A, pSAAModeTable->Reg5Ato66[i]);
    for(i=0; i<17; i++)
        WriteI2C(ucI2Cport,ucDevAddress, i+0x6C, pSAAModeTable->Reg6Cto7C[i]);
    for(i=0; i<5; i++)
        WriteI2C(ucI2Cport,ucDevAddress, i+0x81, pSAAModeTable->Reg81to85[i]);
    for(i=0; i<16; i++)
        WriteI2C(ucI2Cport,ucDevAddress, i+0x90, pSAAModeTable->Reg90to9F[i]);
    
    
    SetHTotal(bDisplayPath, pSAAModeTable->usHTotal);
    SetHDisplayEnd(bDisplayPath, pSAAModeTable->usHDisplayEnd);
    SetHBlankingStart(bDisplayPath, pSAAModeTable->usHBlankingStart);
    SetHBlankingEnd(bDisplayPath, pSAAModeTable->usHBlankingEnd);
    SetHSyncStart(bDisplayPath, pSAAModeTable->usHSyncStart);
    SetHSyncEnd(bDisplayPath, pSAAModeTable->usHSyncEnd);
    SetVTotal(bDisplayPath, pSAAModeTable->usVTotal);
    SetVDisplayEnd(bDisplayPath, pSAAModeTable->usVDisplayEnd);
    SetVBlankingStart(bDisplayPath, pSAAModeTable->usVBlankingStart);
    SetVBlankingEnd(bDisplayPath, pSAAModeTable->usVBlankingEnd);
    SetVSyncStart(bDisplayPath, pSAAModeTable->usVSyncStart);
    SetVSyncEnd(bDisplayPath, pSAAModeTable->usVSyncEnd);
    
    
    return TRUE;
}

void SetSAA7105CCRSLevel(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR Level)
{
    UCHAR bTmpData;
    ReadI2C(ucI2Cport,ucDevAddress,SAA7105DACCCRSLevel, &bTmpData);
    bTmpData = (bTmpData & 0x3F)|(Level<<6);
    WriteI2C(ucI2Cport,ucDevAddress,SAA7105DACCCRSLevel,bTmpData);
    xf86DrvMsgVerb(0, 0, 4, "==CCRS Level Set Data =%x==\n",bTmpData);
}

void GetSAA7105CCRSLevel(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR *Level)
{
    UCHAR bTmpData;
    ReadI2C(ucI2Cport,ucDevAddress,SAA7105DACCCRSLevel, &bTmpData);
    *Level = (bTmpData & 0x3F)>>6;
    xf86DrvMsgVerb(0, 0, 4, "==CCRS Level Get Data =%x==\n",*Level);
}

