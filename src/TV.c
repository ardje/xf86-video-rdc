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
#include <stdlib.h>
#include "cbdbg.h"
#include "BiosDef.h"
#include "TVTbl.h"
#include "CInt10.h"
#include "CInt10FunProto.h"
#include "TV_proto.h"

RDC_EXPORT BYTE Get_TVEnc_TX_ID()
{
    return ReadScratch(IDX_TV1_ENCODER_ID);
}

RDC_EXPORT BYTE Get_TV_TYPE()
{
    return ReadScratch(IDX_TV1_TYPE);
}

RDC_EXPORT void Set_TV_TYPE(BYTE ucTVType)
{
    WriteScratch(IDX_TV1_TYPE, ucTVType);
}

RDC_EXPORT BYTE Get_TV_CONNECTION_TYPE()
{
    return ReadScratch(IDX_TV1_CONNECTION_TYPE);
}

RDC_EXPORT void Set_TV_CONNECTION_TYPE(BYTE ucTVConnectionType)
{
    WriteScratch(IDX_TV1_CONNECTION_TYPE, ucTVConnectionType);
}

RDC_EXPORT void WriteFS473I2CData(BYTE ucIndex, WORD wData)
{
    BYTE ucI2CPort, ucAddr;
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);
    CBIOSWriteI2C(ucI2CPort, ucAddr, ucIndex, (BYTE)wData);
    CBIOSWriteI2C(ucI2CPort, ucAddr, ucIndex+1, (BYTE)(wData>>8));
}

RDC_EXPORT BYTE bSetSAA7105TimingReg(BYTE bDisplayPath, WORD wModeNum)
{
    SAA7105_Mode_TABLE *pSAAModeTable;
    BYTE i, ucI2CPort, ucAddr;
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);

    if(Get_TV_TYPE()==NTSC)
        pSAAModeTable = (SAA7105_Mode_TABLE*)(&CBIOS_NTSC_SAA7105_TV_Mode_Table);
    else if(Get_TV_TYPE()==PAL)
        pSAAModeTable = (SAA7105_Mode_TABLE*)(&CBIOS_PAL_SAA7105_TV_Mode_Table);
    else
    {
        CBIOSDebugPrint((0, "==Unknown TV Type = %x==\n",Get_TV_TYPE()));
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
        CBIOSDebugPrint((0, "==Error!! Can not found any TV Mode Table = %x==\n"));
        return FALSE; 
    }
    
    for(i=0; i<13; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, i+0x5A, pSAAModeTable->Reg5Ato66[i]);
    for(i=0; i<17; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, i+0x6C, pSAAModeTable->Reg6Cto7C[i]);
    for(i=0; i<5; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, i+0x81, pSAAModeTable->Reg81to85[i]);
    for(i=0; i<16; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, i+0x90, pSAAModeTable->Reg90to9F[i]);
    
    
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

RDC_EXPORT BYTE bSetFS473TimingReg(BYTE bDisplayPath, WORD wModeNum)
{
    FS473_I2C_REG *pFS473FormatReg;
    FS473_Mode_TABLE *pFS473ModeTable;
    BYTE i, ucI2CPort, ucAddr;
    BYTE ucFSResetReg[4]={0x01, 0x00, 0x30, 0x04};
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);
    
    for(i=0; i<4; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, 0x0C+i, ucFSResetReg[i]);

    
    if(Get_TV_TYPE()==NTSC)
    {
        pFS473FormatReg = (FS473_I2C_REG*)(&FS473_NTSCReg);
        pFS473ModeTable = (FS473_Mode_TABLE*)(&CBIOS_NTSC_FS473_TV_Mode_Table);
    }else if(Get_TV_TYPE()==PAL)
    {
        pFS473FormatReg = (FS473_I2C_REG*)(&FS473_PALReg);
        pFS473ModeTable = (FS473_Mode_TABLE*)(&CBIOS_PAL_FS473_TV_Mode_Table);
    }else
    {
        CBIOSDebugPrint((0, "==Error!! Unknown TV Type = %x==\n",Get_TV_TYPE()));
        return FALSE;
    }
    
    
    
    do{
        CBIOSWriteI2C(ucI2CPort, ucAddr, pFS473FormatReg->ucIndex,pFS473FormatReg->ucDataLow);
        CBIOSWriteI2C(ucI2CPort, ucAddr, pFS473FormatReg->ucIndex+1,pFS473FormatReg->ucDataHigh);
        pFS473FormatReg = (FS473_I2C_REG*)((int)pFS473FormatReg + sizeof(FS473_I2C_REG));
    }while(pFS473FormatReg->ucIndex!=0xFF);
    
        
        
    for(i=0; i<TVSupportModeCnt; i++)
    {
        if(wModeNum == pFS473ModeTable->Mode_ID_8bpp ||
           wModeNum == pFS473ModeTable->Mode_ID_16bpp ||
           wModeNum == pFS473ModeTable->Mode_ID_32bpp)
           break;
        else
            pFS473ModeTable = (FS473_Mode_TABLE*)((int)pFS473ModeTable + sizeof(FS473_Mode_TABLE));
    }
    if(i==TVSupportModeCnt)
    {
        CBIOSDebugPrint((0, "==Error!! Can not found any TV Mode Table = %x==\n"));
        return FALSE; 
    }
        
    for(i=0; i<16; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, i, pFS473ModeTable->Reg00to0F[i]);
    for(i=0; i<8; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, i+0x12, pFS473ModeTable->Reg12to19[i]);
    for(i=0; i<12; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, i+0xc6, pFS473ModeTable->RegC6toD1[i]);
    CBIOSWriteI2C(ucI2CPort, ucAddr, 0x3C, pFS473ModeTable->Reg3Cto3D[0]);
    CBIOSWriteI2C(ucI2CPort, ucAddr, 0x3D, pFS473ModeTable->Reg3Cto3D[1]);

    
    SetHTotal(bDisplayPath, pFS473ModeTable->usHTotal);
    SetHDisplayEnd(bDisplayPath, pFS473ModeTable->usHDisplayEnd);
    SetHBlankingStart(bDisplayPath, pFS473ModeTable->usHBlankingStart);
    SetHBlankingEnd(bDisplayPath, pFS473ModeTable->usHBlankingEnd);
    SetHSyncStart(bDisplayPath, pFS473ModeTable->usHSyncStart);
    SetHSyncEnd(bDisplayPath, pFS473ModeTable->usHSyncEnd);
    SetVTotal(bDisplayPath, pFS473ModeTable->usVTotal);
    SetVDisplayEnd(bDisplayPath, pFS473ModeTable->usVDisplayEnd);
    SetVBlankingStart(bDisplayPath, pFS473ModeTable->usVBlankingStart);
    SetVBlankingEnd(bDisplayPath, pFS473ModeTable->usVBlankingEnd);
    SetVSyncStart(bDisplayPath, pFS473ModeTable->usVSyncStart);
    SetVSyncEnd(bDisplayPath, pFS473ModeTable->usVSyncEnd);
    return TRUE;
}

RDC_EXPORT void CBIOSSetFS473VScalingLevel(PCBIOSTVFun_Disp_Info pCBTVFun)
{
    FSVSRegdata *FSScalingTAble;
    BYTE i,ucI2CPort, ucAddr, bTVType, bStatus=0, bTmp=0, bDispPath=0;
    WORD wHres;
    bTVType = pCBTVFun->bTVType;
    if(pCBTVFun->bEnableHPanning)
        wHres = pCBTVFun->wTVOut_HSize;
    else
        wHres = pCBTVFun->wModeHres;
    bTmp = TVVESAModeCnt>>1;

    if(bTVType==NTSC)
    {
        for(i=0; i<bTmp; i++)
        {
            if(FocusVScaleringTable[i].wHRes == wHres)
            {
                FSScalingTAble = &(FocusVScaleringTable[i].ScalerTable[pCBTVFun->ucVScaler]);
                bStatus = TRUE;
                break;
            }
        }
    }else 
    {
        for(i=bTmp; i<TVVESAModeCnt; i++)
        {
            if(FocusVScaleringTable[i].wHRes == wHres)
            {
                FSScalingTAble = &(FocusVScaleringTable[i].ScalerTable[pCBTVFun->ucVScaler]);
                bStatus = TRUE;
                break;
            }
        }
    }
    
    if(bStatus)
    {
        BYTE ucColor[6]={0xA0,0,0xDB,0,0x7E,0};
        CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);
        
        
        
        for(i=0; i<6; i++)
            CBIOSWriteI2C(ucI2CPort, ucAddr, 0xA8+i, 0);
        
        
        if(TVIndex == Get_DEV_ID(DISP1))
            bDispPath = DISP1;
        else
            bDispPath = DISP2;
        UnLockTiming(bDispPath);
        SetVTotal(bDispPath, FSScalingTAble->VTotal);        
        SetVBlankingEnd(bDispPath, FSScalingTAble->VTotal);  
        LockTiming(bDispPath);
        
        
        for(i=0; i<10; i++)
        {
            CBIOSWriteI2C(ucI2CPort, ucAddr, i+0xC6, FSScalingTAble->ucData[i]);
        }
        
        for(i=10; i<18; i++)
        {
            CBIOSWriteI2C(ucI2CPort, ucAddr, i+8, FSScalingTAble->ucData[i]);
        }
        
        CBIOSWriteI2C(ucI2CPort, ucAddr, 0x6, FSScalingTAble->ucData[18]);
        CBIOSWriteI2C(ucI2CPort, ucAddr, 0x7, FSScalingTAble->ucData[19]);
        
        
        
        for(i=0; i<6; i++)
            CBIOSWriteI2C(ucI2CPort, ucAddr, 0xA8+i, ucColor[i]);
    }
}

RDC_EXPORT void LoadTVTiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum)
{
    BYTE bTVEncID = Get_TVEnc_TX_ID();
    CBIOSDebugPrint((0, "==Set TV mode = %X  at CBIOS !!==\n", ModeNum));
    
    if(bTVEncID == TVEnc_SAA7105)
        bSetSAA7105TimingReg(DisplayPath,ModeNum);
    else if(bTVEncID == TVEnc_FS473)
        bSetFS473TimingReg(DisplayPath,ModeNum);
}

RDC_EXPORT void SetSAA7105InitReg()
{
    BYTE ucI2CPort, ucAddr;
    SAA7105_I2C_REG *pInitSAA7105Reg = (SAA7105_I2C_REG*)(&SAA7105InitReg);
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);
    
    do{
        CBIOSWriteI2C(ucI2CPort, ucAddr, pInitSAA7105Reg->ucIndex,pInitSAA7105Reg->ucData);
        pInitSAA7105Reg = (SAA7105_I2C_REG*)((int)pInitSAA7105Reg + sizeof(SAA7105_I2C_REG));
    }while(pInitSAA7105Reg->ucIndex!=0xFF);

    CBIOSDebugPrint((0, "==SetSAA7105InitReg Completed !!==\n"));
}

RDC_EXPORT void SetFS473InitReg()
{
    BYTE ucI2CPort, ucAddr;
    FS473_I2C_REG *pInitFS473Reg = (FS473_I2C_REG*)(&FS473InitReg);
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);
    
    do{
        CBIOSWriteI2C(ucI2CPort, ucAddr, pInitFS473Reg->ucIndex,pInitFS473Reg->ucDataLow);
        CBIOSWriteI2C(ucI2CPort, ucAddr, pInitFS473Reg->ucIndex+1,pInitFS473Reg->ucDataHigh);
        pInitFS473Reg = (FS473_I2C_REG*)((int)pInitFS473Reg + sizeof(FS473_I2C_REG));
    }while(pInitFS473Reg->ucIndex!=0xFF);
    CBIOSDebugPrint((0, "==SetFS473InitReg Completed !!==\n"));
}

RDC_EXPORT void InitTVEncReg(BYTE ucTVEncID)
{
    if(ucTVEncID == TVEnc_SAA7105)
        SetSAA7105InitReg();
    else if(ucTVEncID == TVEnc_FS473)
        SetFS473InitReg();
}

RDC_EXPORT void CBIOSInitTV()
{
    InitTVEncReg(Get_TVEnc_TX_ID());
    Set_TV_TYPE(NTSC);
    Set_TV_CONNECTION_TYPE(CVBS|SVideo);
}

RDC_EXPORT void SetTVDACPower(BYTE bPowerstate)
{
    BYTE ucI2CPort, ucAddr;
    BYTE bTVEncID = Get_TVEnc_TX_ID();
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);

    if(bTVEncID == TVEnc_SAA7105)
    {
        if(bPowerstate == DeviceON) 
        {
            switch(Get_TV_CONNECTION_TYPE())
            {
            case CVBS:        
                CBIOSWriteI2C(ucI2CPort, ucAddr, SAA7105EncData,0x24);
                break;
            case SVideo:      
                CBIOSWriteI2C(ucI2CPort, ucAddr, SAA7105EncData,0x94);
                break;
            case CVBS+SVideo: 
                CBIOSWriteI2C(ucI2CPort, ucAddr, SAA7105EncData,0xB4);
                break;
            default:        
                break;
            }
            CBIOSDebugPrint((0, "==Set SAA7105 DAC Power to %x==\n",Get_TV_CONNECTION_TYPE()));
        }else
        {
            CBIOSWriteI2C(ucI2CPort, ucAddr, SAA7105EncData,0x08);
            CBIOSWriteI2C(ucI2CPort, ucAddr, SAA7105DACPowerDown,0x04);
            CBIOSDebugPrint((0, "==Set SAA7105 DAC Power Down ==\n"));
        }
        
    }else if(bTVEncID == TVEnc_FS473)
    {
        if(bPowerstate == DeviceON) 
        {
            switch(Get_TV_CONNECTION_TYPE())
            {
                case CVBS:        
                case SVideo:      
                case CVBS+SVideo: 
                    WriteFS473I2CData(FS473DACPower, 0x0);
                    break;
                default:
                    break;
            }
            CBIOSDebugPrint((0, "==Set FS473 DAC Power to %x==\n",Get_TV_CONNECTION_TYPE()));
        }else
        {
            WriteFS473I2CData(FS473DACPower, 0x3);
            CBIOSDebugPrint((0, "==Set FS473 DAC Power down ==\n"));
        }
        CBIOSWriteI2C(ucI2CPort, ucAddr, 0xD5,0x00);
        
    }
}


RDC_EXPORT BYTE SenseSAA7105TV(BYTE I2CPort, BYTE I2CSlave)
{
    BYTE EncoderType, PowerCtrl, bConnect, RData;

    EncoderType = PowerCtrl = bConnect = RData = 0;
    
    
    CBIOSReadI2C(I2CPort, I2CSlave, 0x2D, &EncoderType);
    
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x2D, 0xB4);
    
    CBIOSReadI2C(I2CPort, I2CSlave, 0x61, &PowerCtrl);
    
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x61, 0x3F);
    
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x1A, 0x46);
    
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x1B, 0x80);
    
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x1B, 0x00);
    
    CBIOSReadI2C(I2CPort, I2CSlave, 0x1B, &RData);
    RData = RData & 0x03;
    
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x61, PowerCtrl);
    
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x2D, EncoderType);

    if (RData & (BIT0 + BIT1 + BIT2))
        bConnect = 0;
    else
        bConnect = B_TV;

    return bConnect;
}


RDC_EXPORT BYTE SenseFS473TV(BYTE I2CPort, BYTE I2CSlave)
{
    BYTE Regd4Data, Regd9Data;
    BYTE bConnect, i;
    
    Regd4Data = Regd9Data = bConnect = 0;

    CBIOSReadI2C(I2CPort, I2CSlave, 0xD4, &Regd4Data);
    CBIOSWriteI2C(I2CPort, I2CSlave, 0xD4, 0x00);
    
    CBIOSWriteI2C(I2CPort, I2CSlave, 0xD5, 0x00);
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x9E, 0xE0);
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x9F, 0x01);
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x9C, 0x28);
    CBIOSWriteI2C(I2CPort, I2CSlave, 0x9D, 0x49);

    for (i=0; i<10; i++)
    {
        CBIOSReadI2C(I2CPort, I2CSlave, 0x9D, &Regd9Data);
        if (Regd9Data & BIT6)
            break;
    }

    CBIOSWriteI2C(I2CPort, Regd4Data, 0xd4, Regd4Data);
    CBIOSWriteI2C(I2CPort, Regd4Data, 0xd5, 0);

    if (Regd9Data & BIT3)
        bConnect = B_TV;
    else
        bConnect = 0;
        
    return bConnect;
}


RDC_EXPORT BYTE SenseTV()
{
    BYTE TV_Encoder;
    BYTE I2CPort, I2CSlave;
    BYTE bConnect = 0;

    CBIOSGetDeviceI2CInformation(TVIndex, &I2CPort, &I2CSlave);

    TV_Encoder = Get_TVEnc_TX_ID();

    if (TV_Encoder == TVEnc_SAA7105)
    {
        bConnect = SenseSAA7105TV(I2CPort, I2CSlave);
    }
    else if (TV_Encoder == TVEnc_FS473)
    {
        bConnect = SenseFS473TV(I2CPort, I2CSlave);
    }

    return bConnect;
}

RDC_EXPORT void EnableTVClock()
{
    SetCRReg(0xD0, BIT6, BIT6);
}

RDC_EXPORT void DisableTVClock()
{
    SetCRReg(0xD0, 0, BIT6);
    SetTVDACPower(DeviceOFF); 
}

RDC_EXPORT void UpdatePLLByTVEnc(DWORD dwPixelClock)
{
    FSPLLPatch  *pFSModePLLTable  = (FSPLLPatch*)(&FocusPLLPatch);
    SAAPLLPatch *pSAAModePLLTable = (SAAPLLPatch*)(&Saa7105PLLPatch);
    BYTE i=0,j=0;
    BYTE ucI2CPort, ucAddr, ucTVEncID=Get_TVEnc_TX_ID();
    if(ucTVEncID)
    {
        CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);

        if(ucTVEncID == TVEnc_FS473)
        {
            CBIOSDebugPrint((0, "CBIOS: Patch Focus PLL!!\n"));

            for(i=0; i<TMDSSupportModeCnt; i++)
            {
                if(dwPixelClock == FocusPLLPatch[i].dwPixelClock)
                {
                    break;
                }else
                    pFSModePLLTable = (FSPLLPatch*)((int)pFSModePLLTable + sizeof(FSPLLPatch));
            }
            if(i==TMDSSupportModeCnt)
			{
                CBIOSDebugPrint((0, "Error: Can not patch Focus PLL !!\n"));
			}
            for(j=0; j<10; j++)
			{
                CBIOSWriteI2C(ucI2CPort, ucAddr, 0xC6+j, FocusPLLPatch[i].ucData[j]);
			}
        }else if(ucTVEncID == TVEnc_SAA7105)
        {
            CBIOSDebugPrint((0, "CBIOS: Patch SAA7105 PLL!!\n"));

            for(i=0; i<TMDSSupportModeCnt; i++)
            {
                if(dwPixelClock == Saa7105PLLPatch[i].dwPixelClock)
                {
                    break;
                }else
                    pSAAModePLLTable = (SAAPLLPatch*)((int)pSAAModePLLTable + sizeof(SAAPLLPatch));
            }
            if(i==TMDSSupportModeCnt)
			{
                CBIOSDebugPrint((0, "Error: Can not patch SAA7105 PLL !!\n"));
			}
            for(j=0; j<4; j++)
			{
                CBIOSWriteI2C(ucI2CPort, ucAddr, 0x81+j, Saa7105PLLPatch[i].ucData[j]);
			}
        }
        EnableTVClock();
    }    
}

RDC_EXPORT void vSetFS473CCRSLevel(BYTE bCCRSLevel)
{
    BYTE bTmpdata[4],i;
    BYTE ucI2CPort, ucAddr;
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);

    bTmpdata[1]= 0;
    bTmpdata[3]= 0;
    switch(bCCRSLevel)
    {
        case 2:
            bTmpdata[0]= 0x8;
            bTmpdata[2]= 0xC;
        break;
        case 1:
            bTmpdata[0]= 0x8;
            bTmpdata[2]= 0x6;    
        break;
        case 0:
        default:
            bTmpdata[0]= 0x0;
            bTmpdata[2]= 0x0;
        break;
    }

    for(i=0; i<4; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, i+0x20,bTmpdata[i]);
}

RDC_EXPORT void SetTVCCRSLevel(BYTE bCCRSLevel)
{
    BYTE bTmpdata=0;
    BYTE ucI2CPort, ucAddr;
    BYTE bTVEncID = Get_TVEnc_TX_ID();
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);

    if(bTVEncID == TVEnc_SAA7105)
    {
        CBIOSReadI2C(ucI2CPort, ucAddr, 0x5F,&bTmpdata);
        bTmpdata = (bTmpdata & 0x3F)|(bCCRSLevel<<6);
        CBIOSWriteI2C(ucI2CPort, ucAddr, 0x5F,bTmpdata);
    }else if(bTVEncID == TVEnc_FS473)
    {
        vSetFS473CCRSLevel(bCCRSLevel);
    }
}

RDC_EXPORT void SetFS473HPosition(BYTE bHPosition)
{
    BYTE ucI2CPort, ucAddr;
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);
    CBIOSWriteI2C(ucI2CPort, ucAddr, Reg_FS473HPos,bHPosition);
    CBIOSWriteI2C(ucI2CPort, ucAddr, (Reg_FS473HPos+1),0);
}

RDC_EXPORT void SetFS473VPosition(BYTE bVPosition)
{
    BYTE ucI2CPort, ucAddr;
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);
    CBIOSWriteI2C(ucI2CPort, ucAddr, Reg_FS473VPos,bVPosition);
    CBIOSWriteI2C(ucI2CPort, ucAddr, (Reg_FS473VPos+1),0);
}

RDC_EXPORT void SetFS473HSCaler(PCBIOSTVFun_Disp_Info pCBTVFun)
{
    BYTE ucI2CPort, ucAddr;
    WORD wHres,wHScalerVector, wHScalerDiff;
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);
    
    if(pCBTVFun->bEnableHPanning)
        wHres = pCBTVFun->wTVOut_HSize;
    else
        wHres = pCBTVFun->wModeHres;

    switch(wHres)
    {
        case 640:
            wHScalerDiff     = 0x100;
            if(pCBTVFun->bTVType) 
                wHScalerVector   = 0xA00;
            else 
                wHScalerVector   = 0xC00; 
        break;
        case 720:
            wHScalerDiff     = 0x1 ;
            if(pCBTVFun->bTVType) 
                wHScalerVector   = 0xF5;
            else 
                wHScalerVector   = 0xF9; 
        break;
        case 800:
            wHScalerDiff     = 0x1 ;
            if(pCBTVFun->bTVType) 
                 wHScalerVector   = 0xE9;
            else 
                 wHScalerVector   = 0xEB;
         break;
        case 1024:
            wHScalerDiff     = 0x100; 
            if(pCBTVFun->bTVType) 
                wHScalerVector   = 0x2B00;
            else 
                wHScalerVector   = 0x2700; 
        break;
        default:
        break;
    }
    
    if(pCBTVFun->iHScaler>=0) 
    {
        wHScalerVector   += (WORD)(wHScalerDiff*pCBTVFun->iHScaler);
    }else 
    {
        pCBTVFun->iHScaler = -pCBTVFun->iHScaler;
        wHScalerVector   -= (WORD)(wHScalerDiff*pCBTVFun->iHScaler);
    }

    CBIOSWriteI2C(ucI2CPort, ucAddr, Reg_FS473HScaler,(BYTE)wHScalerVector);
    CBIOSWriteI2C(ucI2CPort, ucAddr, (Reg_FS473HScaler+1),(BYTE)(wHScalerVector>>8));
}

