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
#include <string.h>
#include "cbdbg.h"
#include "CInt10.h"
#include "CInt10Tbl.h"
#include "CInt10FunProto.h"


BYTE* pRelated_IOAddress;


__inline void OutPort(WORD Index,BYTE Value)     
{              
    *((volatile BYTE*)pRelated_IOAddress + Index) = Value;
    return;                          
}                                                                       

__inline BYTE InPort(WORD Index)                        
{                                                                      
    BYTE temp = 0x0;    

    temp = *((volatile BYTE*)pRelated_IOAddress + Index);
    
    return temp;                                                 
}

__inline void OutIOPort(WORD Index,BYTE Value)     
{
    
}

__inline BYTE InIOPort(WORD Index)     
{
    BYTE temp = 0x0;    

    

    return temp;                                                 
}

RDC_EXPORT void ResetATTR()
{
    InPort(COLOR_INPUT_STATUS1_READ);
    InPort(MONO_INPUT_STATUS1_READ);
}

RDC_EXPORT void EnableATTR()
{
    ResetATTR();
    OutPort(ATTR_DATA_WRITE, 0x20);
    ResetATTR();
}


RDC_EXPORT void SetCRReg(BYTE bRegIndex, BYTE bRegValue, BYTE bMask)
{
    BYTE btemp = 0x0,btemp1 = 0x0;
    
    if(bMask != 0xFF)
    {
        OutPort(COLOR_CRTC_INDEX,bRegIndex);
        btemp = (BYTE)InPort(COLOR_CRTC_DATA);
        bRegValue &= bMask;
        btemp &=~(bMask);
        btemp |= bRegValue;
        OutPort(COLOR_CRTC_DATA,btemp);
    }
    else
    {
        OutPort(COLOR_CRTC_INDEX,bRegIndex);
        OutPort(COLOR_CRTC_DATA,bRegValue);
    }

    return;
}


RDC_EXPORT BYTE GetCRReg(BYTE bRegIndex)
{
    BYTE btemp = 0x0;
    
    OutPort(COLOR_CRTC_INDEX,bRegIndex);
    btemp = (BYTE)InPort(COLOR_CRTC_DATA);
        
    return btemp;
}


RDC_EXPORT void SetSRReg(BYTE bRegIndex, BYTE bRegValue, BYTE bMask)
{
    BYTE btemp = 0x0,btemp1 = 0x0;
    
    if(bMask != 0xFF)
    {
        OutPort(SEQ_INDEX,bRegIndex);
        btemp = (BYTE)InPort(SEQ_DATA);
        bRegValue &= bMask;
        btemp &=~(bMask);
        btemp |= bRegValue;
        OutPort(SEQ_DATA,btemp);
    }
    else
    {
        OutPort(SEQ_INDEX,bRegIndex);
        OutPort(SEQ_DATA,bRegValue);
    }

    return;
}


RDC_EXPORT BYTE GetSRReg(BYTE bRegIndex)
{
    BYTE btemp = 0x0;
    
    OutPort(SEQ_INDEX,bRegIndex);
    btemp = (BYTE)InPort(SEQ_DATA);
        
    return btemp;
}

RDC_EXPORT void SetARReg(BYTE index,BYTE value)
{
    ResetATTR();

    OutPort(ATTR_DATA_WRITE,index);
    OutPort(ATTR_DATA_WRITE,value);

    InPort(COLOR_INPUT_STATUS1_READ);
    OutPort(ATTR_DATA_WRITE,BIT5);
}

RDC_EXPORT BYTE GetARReg(BYTE index)
{
    BYTE bTmp;
    InPort(COLOR_INPUT_STATUS1_READ);
    OutPort(ATTR_DATA_WRITE,index);
    bTmp = (BYTE)InPort(0x3C1);
    InPort(COLOR_INPUT_STATUS1_READ);
    OutPort(ATTR_DATA_WRITE,BIT5);
    return bTmp;
}


RDC_EXPORT void SetGRReg(BYTE bRegIndex, BYTE bRegValue, BYTE bMask)
{
    BYTE btemp = 0x0,btemp1 = 0x0;
    
    if(bMask != 0xFF)
    {
        OutPort(GRAPH_INDEX,bRegIndex);
        btemp = (BYTE)InPort(GRAPH_DATA);
        bRegValue &= bMask;
        btemp &=~(bMask);
        btemp |= bRegValue;
        OutPort(GRAPH_DATA,btemp);
    }
    else
    {
        OutPort(GRAPH_INDEX,bRegIndex);
        OutPort(GRAPH_DATA,bRegValue);
    }

    return;
}

RDC_EXPORT void SetMSReg(BYTE bRegValue)
{
    OutPort(MISC_WRITE,bRegValue);
}


RDC_EXPORT WORD ReadRegFromHW(REG_OP *pRegOp)
{
    WORD wValue = 0x0;
    BYTE    btemp = 0x0, bMasktemp = 0x0;

    while(pRegOp!=NULL)
    {        
        if(pRegOp->RegGroup == CR)
        {
            
            OutPort(COLOR_CRTC_INDEX,(pRegOp->RegIndex));
            btemp = (BYTE)InPort(COLOR_CRTC_DATA);
        }
        else
        {
            
            OutPort(SEQ_INDEX,(pRegOp->RegIndex));
            btemp = (BYTE)InPort(SEQ_DATA);
        }
        
        bMasktemp = (pRegOp->RegMask);

        
        btemp &= bMasktemp;

        
        while(!(bMasktemp & BIT0))
        {
            bMasktemp = bMasktemp >> 1;
            btemp = btemp >> 1;
        }

        
        btemp = btemp << (pRegOp->RegShiftBit);
        wValue |= btemp;    
        
        
        pRegOp++;

    }

    return wValue;
}


RDC_EXPORT void WriteRegToHW(REG_OP *pRegOp, WORD value)
{
    BYTE btemp, btemp1;
    BYTE bCount;
    BYTE bMasktemp;

    while((pRegOp->RegGroup)!= NR)
    {
        btemp = 0x0; btemp1 = 0x0;
        bCount = 0x0; bMasktemp = 0x0;
    
        bMasktemp = (pRegOp->RegMask);

        
        while(!(bMasktemp & BIT0))
        {
            bMasktemp = bMasktemp >> 1;
            bCount ++;
        }
    
        
        btemp = value >> (pRegOp->RegShiftBit);
        btemp &= (bMasktemp);

        
        if(!(pRegOp->RegMask & BIT0))
        {
            btemp = btemp << bCount;
        }
            
        if(pRegOp->RegGroup == CR)
        {
            
            OutPort(COLOR_CRTC_INDEX,(pRegOp->RegIndex));
            btemp1 = (BYTE)InPort(COLOR_CRTC_DATA);
            btemp1 &= ~(pRegOp->RegMask);
            btemp1 |= btemp;
            OutPort(COLOR_CRTC_DATA,btemp1);
        }
        else
        {
            
            OutPort(SEQ_INDEX,(pRegOp->RegIndex));
            btemp1 = (BYTE)InPort(SEQ_DATA);
            btemp1 &= ~(pRegOp->RegMask);
            btemp1 |= btemp;
            OutPort(SEQ_DATA,btemp1);            
        }

        pRegOp++;
        
    }
 
}


RDC_EXPORT BYTE AlignDataToLSB(BYTE Data, BYTE Mask)
{
    Data &= Mask;
    
    while ((Mask & BIT0) == 0)
    {
        Data >>= 1;
        Mask >>= 1;
    }
    
    return Data;
}


RDC_EXPORT BYTE AlignDataToMask(BYTE Data, BYTE Mask)
{
    
    
    while ((Mask & BIT0) == 0)
    {
        Data <<= 1;
        Mask >>= 1;
    }
    
    return Data;
}


RDC_EXPORT BYTE ReadScratch(WORD IndexMask)
{
    BYTE Index = (BYTE)(IndexMask >> 8);
    BYTE Mask = (BYTE)IndexMask;
    BYTE RetValue;

    RetValue = GetCRReg(Index);
    RetValue &= Mask;
    
    RetValue = AlignDataToLSB(RetValue, Mask);

    return RetValue;
}


RDC_EXPORT void WriteScratch(WORD IndexMask, BYTE Data)
{
    BYTE Index = (BYTE)(IndexMask >> 8);
    BYTE Mask = (BYTE)IndexMask;

    Data = AlignDataToMask(Data, Mask);
    Data &= Mask;
    
    SetCRReg(Index, Data, Mask);
}

RDC_EXPORT BYTE Get_DEV_ID(BYTE DisplayPath)
{
    return ((DisplayPath == DISP1) ? ReadScratch(IDX_IGA1_DEV_ID) : ReadScratch(IDX_IGA2_DEV_ID));
}

RDC_EXPORT void Set_DEV_ID(BYTE DeviceID, BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
        WriteScratch(IDX_IGA1_DEV_ID, DeviceID);
    else
        WriteScratch(IDX_IGA2_DEV_ID, DeviceID);
}

RDC_EXPORT BYTE Get_NEW_DEV_ID(BYTE DisplayPath)
{
    return ((DisplayPath == DISP1) ? ReadScratch(IDX_NEW_IGA1_DEV_ID) : ReadScratch(IDX_NEW_IGA2_DEV_ID));
}

RDC_EXPORT void Set_NEW_DEV_ID(BYTE DeviceID, BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
        WriteScratch(IDX_NEW_IGA1_DEV_ID, DeviceID);
    else
        WriteScratch(IDX_NEW_IGA2_DEV_ID, DeviceID);
}

RDC_EXPORT DWORD Get_LCD_Panel_Size()
{
    DWORD dwLCDPanelSize=0;
    BYTE  ucHigh,ucLow;
    ucLow = ReadScratch(IDX_LCD_V_SIZE);
    ucHigh= ReadScratch(IDX_LCD_V_SIZE_OVERFLOW);
    dwLCDPanelSize = ((ucHigh << 8) | ucLow) << 16;
    ucLow = ReadScratch(IDX_LCD_H_SIZE);
    ucHigh= ReadScratch(IDX_LCD_H_SIZE_OVERFLOW);
    dwLCDPanelSize |= ((ucHigh << 8) | ucLow);
    return dwLCDPanelSize;
}

RDC_EXPORT void  Set_LCD_Panel_Size(DWORD dwHres,DWORD dwVres)
{
    BYTE  ucHigh,ucLow;
    ucLow = (BYTE)dwHres;
    ucHigh= (BYTE)(dwHres >> 8);
    WriteScratch(IDX_LCD_H_SIZE,ucLow);
    WriteScratch(IDX_LCD_H_SIZE_OVERFLOW,ucHigh);
    ucLow = (BYTE)dwVres;
    ucHigh= (BYTE)(dwVres >> 8);
    WriteScratch(IDX_LCD_V_SIZE,ucLow);
    WriteScratch(IDX_LCD_V_SIZE_OVERFLOW,ucHigh);
}

RDC_EXPORT DWORD Get_LCD2_Panel_Size()
{
    DWORD dwLCD2PanelSize=0;
    BYTE  ucHigh,ucLow;
    ucLow = ReadScratch(IDX_LCD2_V_SIZE);
    ucHigh= ReadScratch(IDX_LCD2_V_SIZE_OVERFLOW);
    dwLCD2PanelSize = ((ucHigh << 8) + ucLow) << 16;
    ucLow = ReadScratch(IDX_LCD2_H_SIZE);
    ucHigh= ReadScratch(IDX_LCD2_H_SIZE_OVERFLOW);
    dwLCD2PanelSize = dwLCD2PanelSize + (ucHigh << 8) + ucLow;
    return dwLCD2PanelSize;
}

RDC_EXPORT void  Set_LCD2_Panel_Size(DWORD dwHres,DWORD dwVres)
{
    BYTE  ucHigh,ucLow;
    ucLow = (BYTE)dwHres;
    ucHigh= (BYTE)(dwHres >> 8);
    WriteScratch(IDX_LCD2_H_SIZE,ucLow);
    WriteScratch(IDX_LCD2_H_SIZE_OVERFLOW,ucHigh);
    ucLow = (BYTE)dwVres;
    ucHigh= (BYTE)(dwVres >> 8);
    WriteScratch(IDX_LCD2_V_SIZE,ucLow);
    WriteScratch(IDX_LCD2_V_SIZE_OVERFLOW,ucHigh);
}

RDC_EXPORT BYTE Get_RRATE_ID(BYTE DisplayPath)
{
    return ((DisplayPath == DISP1) ? ReadScratch(IDX_IGA1_RRATE_ID) : ReadScratch(IDX_IGA2_RRATE_ID));
}

RDC_EXPORT void Set_RRATE_ID(BYTE RRateID, BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
        WriteScratch(IDX_IGA1_RRATE_ID, RRateID);
    else
        WriteScratch(IDX_IGA2_RRATE_ID, RRateID);
}

RDC_EXPORT WORD Get_VESA_MODE(BYTE DisplayPath)
{
    BYTE VESAMode, VESAModeOver;

    if (DisplayPath == DISP1)
    {
        VESAMode = ReadScratch(IDX_IGA1_VESA_MODE);
        VESAModeOver = ReadScratch(IDX_IGA1_VESA_MODE_OVERFLOW);
    }
    else
    {
        VESAMode = ReadScratch(IDX_IGA2_VESA_MODE);
        VESAModeOver = ReadScratch(IDX_IGA2_VESA_MODE_OVERFLOW);
    }
    
    return  ((WORD)VESAModeOver) << 8 | (WORD)VESAMode;
}

RDC_EXPORT void Set_VESA_MODE(WORD ModeNum, BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
    {
        WriteScratch(IDX_IGA1_VESA_MODE, (BYTE)ModeNum);
        WriteScratch(IDX_IGA1_VESA_MODE_OVERFLOW, (BYTE)(ModeNum >> 8));
    }
    else
    {
        WriteScratch(IDX_IGA2_VESA_MODE, (BYTE)ModeNum);
        WriteScratch(IDX_IGA2_VESA_MODE_OVERFLOW, (BYTE)(ModeNum >> 8));
    }
}

RDC_EXPORT BYTE Get_LVDS_TX_ID()
{
    return ReadScratch(IDX_LVDS1_TX_ID);
}

RDC_EXPORT void Set12BitDVP()
{
    SetSRReg(0x1E, 0x0, BIT3);
}

RDC_EXPORT void Set24BitDVP()
{
    SetSRReg(0x1E, BIT3, BIT3);
}

RDC_EXPORT void TurnOnCRTPad()
{
    SetCRReg(0xA8, 0x00, BIT7);
}

RDC_EXPORT void TurnOffCRTPad()
{
    SetCRReg(0xA8, 0x80, BIT7);
}

RDC_EXPORT void TurnOnDVP1Pad()
{
    SetCRReg(0xA3, 0x80, BIT7);
}

RDC_EXPORT void TurnOffDVP1Pad()
{
    SetCRReg(0xA3, 0x00, BIT7);
}

RDC_EXPORT void TurnOnDVP2Pad()
{
    SetCRReg(0xA3, 0x40, BIT6);
}

RDC_EXPORT void TurnOffDVP2Pad()
{
    SetCRReg(0xA3, 0x00, BIT6);
}

RDC_EXPORT void TurnOnDVP12Pad()
{
    TurnOnDVP1Pad();
    TurnOnDVP2Pad();
}

RDC_EXPORT void TurnOffDVP12Pad()
{
    TurnOffDVP1Pad();
    TurnOffDVP2Pad();
}

RDC_EXPORT void SequencerOn(BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
        SetSRReg(0x01, 0, BIT5);
    else
        SetCRReg(0x33, 0, BIT0);
}

RDC_EXPORT void SequencerOff(BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
        SetSRReg(0x01, BIT5, BIT5);
    else
        SetCRReg(0x33, BIT0, BIT0);
}


RDC_EXPORT void WaitPowerSequenceDone()
{
    BYTE SR32;
     
    SR32 = GetSRReg(0x32);

    
    while(SR32 == GetSRReg(0x32));
}

RDC_EXPORT void PowerSequenceOn()
{
    
    if (!(GetSRReg(0x11) & BIT0))
    {
        
        SetSRReg(0x32, BIT1, BIT1);
        
        
        SetSRReg(0x11, BIT0, BIT0);
        
        
        WaitPowerSequenceDone();
    }
}

RDC_EXPORT void PowerSequenceOff()
{
    
    if (GetSRReg(0x11) & BIT0)
    {
        
        SetSRReg(0x11, 0, BIT0);
        
        
        WaitPowerSequenceDone();
        
        
        SetSRReg(0x32, 0, BIT1);
    }
}

RDC_EXPORT void TurnOnCRTMask()
{
    SetSRReg(0x31, BIT4, BIT4);
}

RDC_EXPORT void TurnOffCRTMask()
{
    SetSRReg(0x31, 0, BIT4);
}

RDC_EXPORT void TurnOnDVP1Mask()
{
    SetSRReg(0x31, 3, 3);
}

RDC_EXPORT void TurnOffDVP1Mask()
{
    SetSRReg(0x31, 0, 3);
}

RDC_EXPORT void TurnOnDVP2Mask()
{
    SetSRReg(0x31, 0xc, 0xc);
}

RDC_EXPORT void TurnOffDVP2Mask()
{
    SetSRReg(0x31, 0, 0xc);
}

RDC_EXPORT void TurnOnDVPMask(BYTE bDigitalDevice)
{    
    switch (bDigitalDevice)
    {
        case CRT_PORT:
            TurnOnCRTMask();
            break;
        
        case DVP1:
            TurnOnDVP1Mask();
            break;

        case DVP2:
            TurnOnDVP2Mask();
            break;
            
        case DVP12:
            TurnOnDVP1Mask();
            TurnOnDVP2Mask();
            break;
    }
}

RDC_EXPORT void TurnOffDVPMask(BYTE bDigitalDevice)
{
    switch (bDigitalDevice)
    {
        case CRT_PORT:
            TurnOffCRTMask();
            break;
        
        case DVP1:
            TurnOffDVP1Mask();
            break;

        case DVP2:
            TurnOffDVP2Mask();
            break;
            
        case DVP12:
            TurnOffDVP1Mask();
            TurnOffDVP2Mask();
            break;
    }

}

RDC_EXPORT void UnLockTiming(BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
        SetSRReg(0x65, 0, 0xFF);
    else
        SetSRReg(0x68, 0, 0xFF);
}

RDC_EXPORT void LockTiming(BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
        SetSRReg(0x65, 0xFF, 0xFF);
    else
        SetSRReg(0x68, 0xFF, 0xFF);
}


RDC_EXPORT void SetHTotal(BYTE DisplayPath, WORD Value)
{
    
    Value -= 40;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(HTotal1, Value);
    }
    else
    {
        WriteRegToHW(HTotal2, Value);
    }

    return;
}


RDC_EXPORT void SetHDisplayEnd(BYTE DisplayPath, WORD Value)
{
    
    Value -= 8;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(HDispEnd1, Value);
    }
    else
    {
        WriteRegToHW(HDispEnd2, Value);
    }

    return;
    
}


RDC_EXPORT void SetHBlankingStart(BYTE DisplayPath, WORD Value)
{
    
    Value -= 8;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(HBnkSt1, Value);
    }
    else
    {
        WriteRegToHW(HBnkSt2, Value);
    }

    return;
    
}


RDC_EXPORT void SetHBlankingEnd(BYTE DisplayPath, WORD Value)
{
    
    Value -= 8;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(HBnkEnd1, Value);
    }
    else
    {
        WriteRegToHW(HBnkEnd2, Value);
    }

    return;
    
}


RDC_EXPORT void SetHSyncStart(BYTE DisplayPath, WORD Value)
{
    
    Value -= 0;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(HSyncSt1, Value);
    }
    else
    {
        WriteRegToHW(HSyncSt2, Value);
    }

    return;
    
}


RDC_EXPORT void SetHSyncEnd(BYTE DisplayPath, WORD Value)
{
    
    Value -= 0;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(HSyncEnd1, Value);
    }
    else
    {
        WriteRegToHW(HSyncEnd2, Value);
    }

    return;
    
}


RDC_EXPORT void SetVTotal(BYTE DisplayPath, WORD Value)
{
    
    Value -= 2;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(VTotal1, Value);
    }
    else
    {
        WriteRegToHW(VTotal2, Value);
    }

    return;
    
}


RDC_EXPORT void SetVDisplayEnd(BYTE DisplayPath, WORD Value)
{
    
    Value -= 1;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(VDispEnd1, Value);
    }
    else
    {
        WriteRegToHW(VDispEnd2, Value);
    }

    return;
    
}


RDC_EXPORT void SetVBlankingStart(BYTE DisplayPath, WORD Value)
{
    
    Value -= 1;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(VBnkSt1, Value);
    }
    else
    {
        WriteRegToHW(VBnkSt2, Value);
    }

    return;
    
}


RDC_EXPORT void SetVBlankingEnd(BYTE DisplayPath, WORD Value)
{
    
    Value -= 1;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(VBnkEnd1, Value);
    }
    else
    {
        WriteRegToHW(VBnkEnd2, Value);
    }

    return;
    
}


RDC_EXPORT void SetVSyncStart(BYTE DisplayPath, WORD Value)
{
    
    Value -= 1;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(VSyncSt1, Value);
    }
    else
    {
        WriteRegToHW(VSyncSt2, Value);
    }

    return;
    
}


RDC_EXPORT void SetVSyncEnd(BYTE DisplayPath, WORD Value)
{
    
    Value -= 1;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(VSyncEnd1, Value);
    }
    else
    {
        WriteRegToHW(VSyncEnd2, Value);
    }

    return;
    
}


RDC_EXPORT void SetPixelClock(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, DWORD Clock)
{
    PLL_Info PLLInfo;
    BYTE bPLLDiv=0;
    
    
    if(Clock<25000)
        bPLLDiv|=2;
        
        
        
        
    Clock <<= bPLLDiv;
    PLLInfo = pCBIOSExtension->pfnCBIOS_TransVGAPLL(Clock);
    pCBIOSExtension->pfnCBIOS_SetVGAPLLReg(DisplayPath, PLLInfo);
}

RDC_EXPORT DWORD Difference(DWORD Value1, DWORD Value2)
{
    if (Value1 > Value2)
        return (Value1 - Value2);
    else
        return (Value2 - Value1);
}


RDC_EXPORT PLL_Info ClockToPLLF4002A(DWORD Clock)
{
    DWORD MSCount, NSCount, RSCount, FCKVCO, FCKOUT;    
    DWORD NearestClock = 300000000; 
    PLL_Info PLLInfo = {0, 0, 0};

    for (MSCount = 3; MSCount < 6; MSCount++)
    {
        for (NSCount = 1; NSCount < 256; NSCount++)
        {
            FCKVCO = PLLReferenceClock * NSCount / MSCount;
            
            if ( (MaxFCKVCO4002A >= FCKVCO) && (FCKVCO >= MinFCKVCO4002A) )
            {
                for (RSCount = 1; RSCount <= 7; RSCount++)
                {
                    FCKOUT = FCKVCO >> RSCount;
                    if ( Difference(FCKOUT, Clock) < Difference(NearestClock, Clock) )
                    {
                        NearestClock = FCKOUT;
                        PLLInfo.MS = (BYTE)MSCount;
                        PLLInfo.NS = (BYTE)NSCount;
                        PLLInfo.RS = (BYTE)RSCount;
                    }
                }
            }
        }

    }

    
    PLLInfo.MS <<=3 ;

    return PLLInfo;

}


RDC_EXPORT PLL_Info ClockToPLLF9003A(DWORD Clock)
{
    DWORD MSCount, NSCount, RSCount, FCKVCO, FCKOUT;    
    DWORD NearestClock = 300000000; 
    PLL_Info PLLInfo = {0, 0, 0};

    for (MSCount = 3; MSCount < 6; MSCount++)
    {
        for (NSCount = 1; NSCount < 256; NSCount++)
        {
            FCKVCO = PLLReferenceClock * NSCount / MSCount;
            
            if ( (MaxFCKVCO9003A >= FCKVCO) && (FCKVCO >= MinFCKVCO9003A) )
            {
                for (RSCount = 1; RSCount < 6; RSCount++)
                {
                    FCKOUT = FCKVCO >> RSCount;
                    if ( Difference(FCKOUT, Clock) < Difference(NearestClock, Clock) )
                    {
                        NearestClock = FCKOUT;
                        if (MSCount == 3)
                            PLLInfo.MS = 0x00;
                        if (MSCount == 4)
                            PLLInfo.MS = BIT3;
                        if (MSCount == 5)
                            PLLInfo.MS = BIT4+BIT3;
                        PLLInfo.NS = (BYTE)NSCount;
                        PLLInfo.RS = (BYTE)RSCount-1;
                    }
                }
            }
        }

    }

    return PLLInfo;

}


RDC_EXPORT void SetF9003APLLReg(BYTE DisplayPath, PLL_Info PLLInfo)
{
    BYTE RetValue; 

    if (DisplayPath == DISP1)
    {
        SetCRReg(0xC1, PLLInfo.MS, BIT4+BIT3);
        SetCRReg(0xC0, PLLInfo.NS, 0xFF);
        SetCRReg(0xCF, PLLInfo.RS, BIT2+BIT1+BIT0);
    }
    else
    {
        SetCRReg(0xBF, PLLInfo.MS, BIT4+BIT3);
        SetCRReg(0xBE, PLLInfo.NS, 0xFF);
        SetCRReg(0xCE, PLLInfo.RS, BIT2+BIT1+BIT0);
    }

    
    RetValue = GetCRReg(0xBB);
    SetCRReg(0xBB, RetValue, 0xFF);

}


RDC_EXPORT void SetF4002APLLReg(BYTE DisplayPath, PLL_Info PLLInfo)
{
    BYTE RetValue; 

    if (DisplayPath == DISP1)
    {
        SetCRReg(0xC1, PLLInfo.MS, BIT5+BIT4+BIT3);
        SetCRReg(0xC0, PLLInfo.NS, 0xFF);
        SetCRReg(0xCF, PLLInfo.RS, BIT2+BIT1+BIT0);
    }
    else
    {
        SetCRReg(0xBF, PLLInfo.MS, BIT5+BIT4+BIT3);
        SetCRReg(0xBE, PLLInfo.NS, 0xFF);
        SetCRReg(0xCE, PLLInfo.RS, BIT2+BIT1+BIT0);
    }

    
    RetValue = GetCRReg(0xBB);
    SetCRReg(0xBB, RetValue, 0xFF);

}


RDC_EXPORT void SetPolarity(BYTE DevicePort, WORD wValue)
{
    BYTE ucTmpData = 0;
    wValue &= NCLK+NVS+NHS; 
    ucTmpData = (BYTE)wValue;
    ucTmpData |= (BYTE)(wValue>>8); 
    switch (DevicePort)
    {
        case CRT_PORT:
            OutPort(MISC_WRITE, ((wValue<<5) & 0xC0) | (InPort(MISC_READ) & 0x3F));
            break;
        
        case DVP1:
        case DVP12:
            SetSRReg(0x20, wValue>>1, BIT2+BIT1+BIT0);
            break;

        case DVP2:
            SetSRReg(0x20, wValue<<2, BIT5+BIT4+BIT3);
            break;
    }
}


RDC_EXPORT void SetFIFO(BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
    {
        SetCRReg(0xA7, 0xBF, 0xFF);
        SetCRReg(0xA6, 0x9F, 0xFF);
    }
    else if (DisplayPath == DISP2)
    {
        SetCRReg(0x35, 0x5F, 0xFF);
        SetCRReg(0x34, 0x3F, 0xFF);
    }
}


RDC_EXPORT void SetTimingRegs(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, MODE_INFO *pModeInfo, RRATE_TABLE *pRRateTable)
{
    WORD  Attribute    = pRRateTable->Attribute;
    BYTE ucDeviceID   = Get_DEV_ID(DisplayPath);
    BYTE ucDevicePort = GetDevicePort(ucDeviceID);
    
    SetHTotal(DisplayPath, pRRateTable->H_Total);

    SetHBlankingEnd(DisplayPath, (Attribute & HB) ? pRRateTable->H_Total-8 : pRRateTable->H_Total);

    SetHDisplayEnd(DisplayPath, pModeInfo->H_Size);

    SetHBlankingStart(DisplayPath, (Attribute & HB) ? pModeInfo->H_Size+8 : pModeInfo->H_Size);

    SetHSyncStart(DisplayPath, pRRateTable->H_Sync_Start);

    SetHSyncEnd(DisplayPath, pRRateTable->H_Sync_End);

    SetVTotal(DisplayPath, pRRateTable->V_Total);

    SetVBlankingEnd(DisplayPath, (Attribute & VB) ? pRRateTable->V_Total-8 : pRRateTable->V_Total);

    SetVDisplayEnd(DisplayPath, pModeInfo->V_Size);

    SetVBlankingStart(DisplayPath, (Attribute & VB) ? pModeInfo->V_Size+8 : pModeInfo->V_Size);

    SetVSyncStart(DisplayPath, pRRateTable->V_Sync_Start);

    SetVSyncEnd(DisplayPath, pRRateTable->V_Sync_End);
    
    SetPixelClock(pCBIOSExtension, DisplayPath, pRRateTable->Clock);

    if(ucDeviceID != CRTIndex && pCBIOSExtension->bPLLFromTVEnc)  
        UpdatePLLByTVEnc(pRRateTable->Clock);
    if(ucDeviceID == DVIIndex || ucDeviceID == HDMIIndex)
        SetTMDSDPAReg(pRRateTable->Clock, ucDeviceID);
    SetPolarity(ucDevicePort, pRRateTable->Attribute);

    
}

RDC_EXPORT void SetTimingCentering(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, MODE_INFO *pModeInfo, RRATE_TABLE *pRRateTable)
{
    WORD  Attribute    = pRRateTable->Attribute;
    WORD  wHDMITypeHSize =1280, wHDMITypeVSize =720;
    WORD  wHDiff=0, wVDiff=0;
    BYTE ucDeviceID   = Get_DEV_ID(DisplayPath);
    BYTE ucDevicePort = GetDevicePort(ucDeviceID);
    
    if(ucDeviceID == HDMIIndex)
    {
        if(Get_HDMI_TYPE()==HDMI1080P)
        {
            wHDMITypeHSize = 1920;
            wHDMITypeVSize = 1080;
        }
        wHDiff = (wHDMITypeHSize - pModeInfo->H_Size)>>1;
        wVDiff = (wHDMITypeVSize - pModeInfo->V_Size)>>1;
        SetHTotal(DisplayPath, pRRateTable->H_Total);

        SetHBlankingEnd(DisplayPath, pRRateTable->H_Total-wHDiff);

        SetHDisplayEnd(DisplayPath, pModeInfo->H_Size);

        SetHBlankingStart(DisplayPath, pModeInfo->H_Size+wHDiff);

        SetHSyncStart(DisplayPath, pRRateTable->H_Sync_Start-wHDiff);

        SetHSyncEnd(DisplayPath, pRRateTable->H_Sync_End-wHDiff);

        SetVTotal(DisplayPath, pRRateTable->V_Total);

        SetVBlankingEnd(DisplayPath, pRRateTable->V_Total-wVDiff);

        SetVDisplayEnd(DisplayPath, pModeInfo->V_Size);

        SetVBlankingStart(DisplayPath, pModeInfo->V_Size+wVDiff);

        SetVSyncStart(DisplayPath, pRRateTable->V_Sync_Start-wVDiff);

        SetVSyncEnd(DisplayPath, pRRateTable->V_Sync_End-wVDiff);
        
        SetPixelClock(pCBIOSExtension, DisplayPath, pRRateTable->Clock);
        
        if(ucDeviceID != CRTIndex)  
            UpdatePLLByTVEnc(pRRateTable->Clock);

        SetPolarity(ucDevicePort, pRRateTable->Attribute);
    }
    
}


RDC_EXPORT void SetPitch(BYTE DisplayPath, WORD Value)
{
    
    Value += 7;
    Value >>= 3;

    if(DisplayPath == DISP1)
    {
        WriteRegToHW(Pitch1, Value);
    }
    else
    {
        WriteRegToHW(Pitch2, Value);
    }

    return;
    
}


RDC_EXPORT WORD GetPitch(BYTE DisplayPath)
{
    WORD wPitch;
    
    if(DisplayPath == DISP1)
    {
        wPitch = ReadRegFromHW(Pitch1);
    }
    else
    {
        wPitch = ReadRegFromHW(Pitch2);
    }

    wPitch <<= 3;

    return wPitch;
}


RDC_EXPORT WORD GetVDisplayEnd(BYTE DisplayPath)
{
    WORD  wDisplayEnd = 0x0;
    
    if(DisplayPath == DISP1)
    {
        wDisplayEnd = ReadRegFromHW(VDispEnd1);
    }
    else
    {
        wDisplayEnd = ReadRegFromHW(VDispEnd2);
    }
    
    
    wDisplayEnd += 1;

    return  wDisplayEnd;
}


RDC_EXPORT void SetColorDepth(BYTE DisplayPath, BYTE Value)
{
    BYTE bSetBit = 0x0;

    switch(Value)
    {
        case 8:
            bSetBit = (BYTE)BIT0;
            break;
        case 16:
            bSetBit = (BYTE)BIT2;
            break;
        case 32:
            bSetBit = (BYTE)BIT3;
            break;
        default:
            return;
    }

    if(DisplayPath == DISP1)
    {
        SetCRReg(0xA3, 0x0, BIT0+BIT1+BIT2+BIT3); 
        SetCRReg(0xA3, bSetBit, bSetBit);
    }
    else
    {
        
        if(Value == 8)
            return;
        SetCRReg(0x33, 0x0, BIT1+BIT2+BIT3); 
        SetCRReg(0x33, bSetBit, bSetBit);
    }
    
}

RDC_EXPORT void HandleShareConnector(BYTE *pDeviceIndex)
{
    
    if(*pDeviceIndex == DVIIndex)
    {
        if(TMDS_ID_EP932M==Get_TMDS_TX_ID()) 
            *pDeviceIndex = HDMIIndex;
    }
    
    
}


RDC_EXPORT BYTE GetVBIOSBootUpDevice(WORD* ucDeiceBit)
{
    BYTE i;
    *ucDeiceBit = 0;
    
    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        if(PortConfig[i].Attribute & Dev_Boot)
        {
            *ucDeiceBit |= TransDevIDtoBit(PortConfig[i].DeviceIndex);
            return TRUE;
        }
    }
    
    i = ReadScratch(IDX_BOOT_DEV_ID);
    if(i)
    {
        *ucDeiceBit |= TransDevIDtoBit(i);     
        return TRUE;
    }
    return FALSE;
}


RDC_EXPORT BYTE GetDevicePort(BYTE DeviceIndex)
{
    BYTE i;
    HandleShareConnector(&DeviceIndex);
    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        if(PortConfig[i].DeviceIndex==DeviceIndex)
            return PortConfig[i].PortID;
    }
    return FALSE;
}


RDC_EXPORT void ConfigDigitalPort(BYTE DisplayPath)
{
    BYTE bDigitalDevice = 0x0, bSetValue = 0x0, DeviceIndex = 0x0;
    
    
    if(DisplayPath == DISP1)
    {
        DeviceIndex    = Get_DEV_ID(DISP1);
        bSetValue      = BIT0+BIT1;  
    }
    else
    {
        DeviceIndex    = Get_DEV_ID(DISP2);
        bSetValue      = BIT2;       
    }

    
    bDigitalDevice = GetDevicePort(DeviceIndex);
    switch(bDigitalDevice)
    {          
        case DVP1:
            Set12BitDVP();
            SetSRReg(0x1F, bSetValue, BIT0);
            break;
        case DVP2:
            Set12BitDVP();
            SetSRReg(0x1F, bSetValue, BIT1);
            break;
        case DVP12:
            Set24BitDVP();
            SetSRReg(0x1F, bSetValue, BIT0);
            break;
        case CRT_PORT:
            SetSRReg(0x1F, bSetValue, BIT2);
            break;
        default:
            return;
    }
    return;
    
}


RDC_EXPORT void CheckLCDSyncStartValue(BYTE DisplayPath) 
{
    WORD wCheckData;
    wCheckData = LCDTable[0].pPanelTable.Timing.H_Sync_Start + 64;
    if(wCheckData >= LCDTable[0].pPanelTable.Timing.H_Total) 
    {
        SetHSyncStart(DisplayPath, LCDTable[0].pPanelTable.Timing.H_Total-64); 
    }
}

RDC_EXPORT BYTE bHDMIUnscan(BYTE ucHDMIType)
{
    WORD wHSize, wVSize;
    if(ucHDMIType==HDMI1080P)
    {
        wHSize = 1920;
        wVSize = 1080;
    }else
    {
        wHSize = 1280;
        wVSize = 720;
    }
    if(wHSize>HDMITable[ucHDMIType].H_Size || 
       wVSize>HDMITable[ucHDMIType].V_Size)
        return TRUE;
    else
        return FALSE;
}


RDC_EXPORT void SetDisplay1UpScalingFactor(MODE_INFO *pModeInfo, MODE_INFO *pPanelInfo)
{
    WORD  wScalingVector=0;
    BYTE  ucScalingCTLReg = (GetSRReg(0x58)&0xF8);
    
    if((pModeInfo->H_Size <  pPanelInfo->H_Size) || 
       (pModeInfo->V_Size <  pPanelInfo->V_Size))
    {
        
        SetSRReg(0x4B,(BYTE)(pModeInfo->H_Size-1) ,0xFF);
        SetSRReg(0x4C,(BYTE)(pModeInfo->H_Size>>8),0x0F);
        
        
        if((pModeInfo->H_Size < pPanelInfo->H_Size))
        {
            wScalingVector = (WORD)(((DWORD)(pModeInfo->H_Size << 12)) / pPanelInfo->H_Size);
            SetSRReg(0x59,(BYTE)wScalingVector ,0xFF);
            SetSRReg(0x5B,(BYTE)(wScalingVector>>8) ,0x0F);
            ucScalingCTLReg |= BIT2; 
        }
        
        
        if((pModeInfo->V_Size < pPanelInfo->V_Size))
        {
            wScalingVector = (WORD)(((DWORD)(pModeInfo->V_Size << 11)) / pPanelInfo->V_Size);
            SetSRReg(0x5A,(BYTE)wScalingVector ,0xFF);
            SetSRReg(0x5B,(BYTE)(wScalingVector>>4) ,0x70); 
            ucScalingCTLReg |= BIT1; 
        }
        
        if(ucScalingCTLReg&0x6) 
            ucScalingCTLReg |= BIT0; 
        SetSRReg(0x58, ucScalingCTLReg, 0xFF); 
    }
}


RDC_EXPORT void SetDisplay2UpScalingFactor(MODE_INFO *pModeInfo, MODE_INFO *pPanelInfo)
{
    WORD  wScalingVector=0;
    BYTE  ucScalingCTLReg = (GetSRReg(0x50)&0xF8);
    
    if((pModeInfo->H_Size <  pPanelInfo->H_Size) || 
       (pModeInfo->V_Size <  pPanelInfo->V_Size))
    {
        
        SetSRReg(0x4D,(BYTE)(pModeInfo->H_Size-1) ,0xFF);
        SetSRReg(0x4E,(BYTE)(pModeInfo->H_Size>>8),0x0F);
        
        
        if((pModeInfo->H_Size < pPanelInfo->H_Size))
        {
            wScalingVector = (WORD)(((DWORD)(pModeInfo->H_Size << 12)) / pPanelInfo->H_Size);
            SetSRReg(0x51,(BYTE)wScalingVector ,0xFF);
            SetSRReg(0x53,(BYTE)(wScalingVector>>8) ,0x0F);
            ucScalingCTLReg |= BIT2; 
        }
        
        
        if((pModeInfo->V_Size < pPanelInfo->V_Size))
        {
            wScalingVector = (WORD)(((DWORD)(pModeInfo->V_Size << 11)) / pPanelInfo->V_Size);
            SetSRReg(0x52,(BYTE)wScalingVector ,0xFF);
            SetSRReg(0x53,(BYTE)(wScalingVector>>4) ,0x70); 
            ucScalingCTLReg |= BIT1; 
        }
        
        if(ucScalingCTLReg&0x6) 
            ucScalingCTLReg |= BIT0; 
        SetSRReg(0x50, ucScalingCTLReg, 0xFF); 
    }
}


RDC_EXPORT void SetDisplay1DownScalingFactor(MODE_INFO *pModeInfo, MODE_INFO *pPanelInfo)
{
    WORD  wScalingVector=0;
    
    if((pModeInfo->H_Size > pPanelInfo->H_Size) || 
       (pModeInfo->V_Size > pPanelInfo->V_Size))
    {
        
        if(pModeInfo->H_Size > pPanelInfo->H_Size)
        {
            
            SetSRReg(0x77, (BYTE)pModeInfo->H_Size, 0xFF);
            SetSRReg(0x76, (BYTE)(pModeInfo->H_Size>>8),0x07);
            
            
            wScalingVector = (WORD)(((DWORD)(pModeInfo->H_Size << 8)) / pPanelInfo->H_Size);
            SetSRReg(0x75, (BYTE)wScalingVector ,0xFF);
            SetSRReg(0x74, (BYTE)(wScalingVector>>8), 0x03);
            SetSRReg(0x74, BIT7, BIT7); 
        }
        
        if(pModeInfo->V_Size > pPanelInfo->V_Size)
        {
            
            SetSRReg(0x73, (BYTE)pModeInfo->V_Size, 0xFF);
            SetSRReg(0x72, (BYTE)(pModeInfo->V_Size>>8),0x07);
            
            
            wScalingVector = (WORD)(((DWORD)(pModeInfo->V_Size << 8)) / pPanelInfo->V_Size);
            SetSRReg(0x71, (BYTE)wScalingVector ,0xFF);
            SetSRReg(0x70, (BYTE)(wScalingVector>>8), 0x03);
            SetSRReg(0x70, BIT7, BIT7); 
        }
    }
}


RDC_EXPORT CBStatus Get_MODE_INFO_From_VESA_Table(WORD ModeNum, MODE_INFO **ppModeInfo)
{
    MODE_INFO *pModeTemp = VESATable;
    CBStatus    bCheck = FALSE;
    
    do
    {
        if(((pModeTemp->Mode_ID_8bpp) == ModeNum)||
           ((pModeTemp->Mode_ID_16bpp) == ModeNum)||
           ((pModeTemp->Mode_ID_32bpp) == ModeNum)
          )
        {
            bCheck = TRUE;
            (*ppModeInfo) = pModeTemp;
        }
        
        pModeTemp++;
        
    }while((pModeTemp->pRRTable) !=NULL);
    
    return bCheck;
}


RDC_EXPORT CBStatus Get_MODE_INFO_From_HDMI_Table(WORD ModeNum, MODE_INFO **ppModeInfo)
{
    MODE_INFO *pModeTemp = HDMITable;
    CBStatus    bCheck = FALSE;
    
    do
    {
        if(((pModeTemp->Mode_ID_8bpp) == ModeNum)||
           ((pModeTemp->Mode_ID_16bpp) == ModeNum)||
           ((pModeTemp->Mode_ID_32bpp) == ModeNum)
          )
        {
            bCheck = TRUE;
            (*ppModeInfo) = pModeTemp;
        }
        
        pModeTemp++;
        
    }while((pModeTemp->pRRTable) !=NULL);
    
    return bCheck;
}


RDC_EXPORT CBStatus Get_MODE_INFO_From_LCD_Table(WORD ModeNum, MODE_INFO **ppModeInfo)
{
    *ppModeInfo = (MODE_INFO *)LCDTable;   
    if((((*ppModeInfo)->Mode_ID_8bpp) == ModeNum)||
       (((*ppModeInfo)->Mode_ID_16bpp) == ModeNum)||
       (((*ppModeInfo)->Mode_ID_32bpp) == ModeNum))
        return TRUE;
    else
        return FALSE;
}


RDC_EXPORT CBStatus GetModePointerFromVESATable(WORD ModeNum, BYTE RRIndex, MODE_INFO **ppModeInfo, RRATE_TABLE **ppRRateTable)
{
    RRATE_TABLE* pRRateTable_temp = NULL;
    MODE_INFO *pModeInfo_temp = NULL;
    
    
    if(!Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo_temp))
    {
        return FALSE;
    }
    else
    {
        

        pRRateTable_temp = pModeInfo_temp->pRRTable;
         
        do
        {   
            if(pRRateTable_temp->RRate_ID == RRIndex)
            {
                (*ppRRateTable) = pRRateTable_temp;
                (*ppModeInfo) = pModeInfo_temp;
                return TRUE;
            }
            else
            {
                pRRateTable_temp++;
            }

        }while(pRRateTable_temp!=NULL);

    }

    
    return FALSE;
}


RDC_EXPORT CBStatus GetModePointerFromHDMITable(WORD ModeNum, BYTE RRIndex, MODE_INFO **ppModeInfo, RRATE_TABLE **ppRRateTable)
{
    RRATE_TABLE* pRRateTable_temp = NULL;
    MODE_INFO *pModeInfo_temp = NULL;
    
    
    if(!Get_MODE_INFO_From_HDMI_Table(ModeNum,&pModeInfo_temp))
    {
        return FALSE;
    }
    else
    {
        

        pRRateTable_temp = pModeInfo_temp->pRRTable;
         
        do
        {   
            if(pRRateTable_temp->RRate_ID == RRIndex)
            {
                (*ppRRateTable) = pRRateTable_temp;
                (*ppModeInfo) = pModeInfo_temp;
                return TRUE;
            }
            else
            {
                pRRateTable_temp++;
            }

        }while(pRRateTable_temp!=NULL);

    }

    
    return FALSE;
}


RDC_EXPORT void LoadVESATiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum)
{
    BYTE bR_Rate_value = 0x0;
    MODE_INFO *pModeInfo = NULL, *pRefMode = NULL;
    MODE_INFO RefMode;
    RRATE_TABLE *pRRateTable = NULL;
    CBStatus    bUserSpecifiedTiming = ((pCBIOSExtension->pCBiosArguments->BX & BIT11) ? TRUE : FALSE);

    if(DisplayPath == DISP1)
    {
        
        bR_Rate_value = Get_RRATE_ID(DISP1);
    }
    else
    {
        
        bR_Rate_value = Get_RRATE_ID(DISP2);
    }

    
    if(pCBIOSExtension->bEDIDValid)
    {
        pModeInfo = VESAEDIDTable;
        pRRateTable = VESAEDIDTable[0].pRRTable;
    }
    else
    {
        
        
        GetModePointerFromVESATable(ModeNum,bR_Rate_value,&pModeInfo,&pRRateTable);
    }

    pCBIOSExtension->wCRTDefaultH = pModeInfo->H_Size;
    pCBIOSExtension->wCRTDefaultV = pModeInfo->V_Size;

    
    CBIOSDebugPrint((0, "CBIOS: Setting %d x %d resolution\n", pModeInfo->H_Size, pModeInfo->V_Size));
    SetTimingRegs(pCBIOSExtension, DisplayPath,pModeInfo,pRRateTable);
    
    
    if((pCBIOSExtension->bDuoView)&&(DisplayPath != DISP1))
    {
        RefMode.H_Size = pCBIOSExtension->DisplayOneModeH;
        RefMode.V_Size = pCBIOSExtension->DisplayOneModeV;

        
        
        
        SetDisplay2UpScalingFactor(&RefMode, pModeInfo);
    }
    else
    {
        
        if(!bUserSpecifiedTiming)
        {
            if(GetModePointerFromVESATable(ModeNum,bR_Rate_value,&pRefMode,&pRRateTable))
            {
                CBIOSDebugPrint((0, "CBIOS: Original Setting %d x %d resolution\n", pRefMode->H_Size, pRefMode->V_Size));
                
                
                
                
                SetDisplay1UpScalingFactor(pRefMode, pModeInfo);
            }
            else
            {
                CBIOSDebugPrint((0, "CBIOS: Setting Mode Fail, could not find Mode Num=%Xh\n", ModeNum));
            }
        }

        
        if(pCBIOSExtension->bEDIDValid)
        {
            pCBIOSExtension->DisplayOneModeH = VESAEDIDTable[0].H_Size;
            pCBIOSExtension->DisplayOneModeV = VESAEDIDTable[0].V_Size;
        }
        else
        {
            
            pCBIOSExtension->DisplayOneModeH = pModeInfo->H_Size;
            pCBIOSExtension->DisplayOneModeV = pModeInfo->V_Size;
        }
        
    }
    
    return;
}


RDC_EXPORT void LoadLCDTiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum)
{
    MODE_INFO pModeInfo;
    MODE_INFO *pMode = NULL;
    
    pModeInfo.H_Size = LCDTable[0].H_Size;
    pModeInfo.V_Size = LCDTable[0].V_Size;
    SetTimingRegs(pCBIOSExtension, DisplayPath, &pModeInfo, &(LCDTable[0].pPanelTable.Timing));
    CheckLCDSyncStartValue(DisplayPath);

    
    if((!Get_MODE_INFO_From_LCD_Table(ModeNum, &pMode)) &&
        Get_MODE_INFO_From_VESA_Table(ModeNum, &pMode))
    {
        
        if(DisplayPath==DISP1)
        {
            if(ReadScratch(IDX_DS_ENABLE)) 
                SetDisplay1DownScalingFactor(pMode, &pModeInfo);
            else
                SetDisplay1UpScalingFactor(pMode, &pModeInfo);
        }else
            SetDisplay2UpScalingFactor(pMode, &pModeInfo);
        
        
        pCBIOSExtension->DisplayOneModeH = pMode->H_Size;
        pCBIOSExtension->DisplayOneModeV = pMode->V_Size;
    }
}


RDC_EXPORT void LoadHDMITiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum)
{
    BYTE bR_Rate_value = 0x0, bType= 0;
    MODE_INFO *pModeInfo = NULL, *pMode = NULL;
    RRATE_TABLE *pRRateTable = NULL;
        
    if(DisplayPath == DISP1)
    {
        
        bR_Rate_value = Get_RRATE_ID(DISP1);
    }
    else
    {
        
        bR_Rate_value = Get_RRATE_ID(DISP2);
    }
    
    bType = Get_HDMI_TYPE();
    if(GetModePointerFromHDMITable(HDMITable[bType].Mode_ID_32bpp,bR_Rate_value,&pModeInfo,&pRRateTable)) 
    {
        if(bHDMIUnscan(bType))
        {
            SetTimingCentering(pCBIOSExtension, DisplayPath,pModeInfo,pRRateTable);
            CBIOSDebugPrint((0, "HDMI: Runs Unscan mode\n"));
        }
        else
            SetTimingRegs(pCBIOSExtension, DisplayPath,pModeInfo,pRRateTable);
        if(Get_MODE_INFO_From_VESA_Table(ModeNum, &pMode))
        {
            if(DisplayPath == DISP1)
                SetDisplay1UpScalingFactor(pMode, pModeInfo);
            else
                SetDisplay2UpScalingFactor(pMode, pModeInfo);
        }
        else
            CBIOSDebugPrint((0, "HDMI: Get VESA mode error\n"));

        pCBIOSExtension->DisplayOneModeH = pMode->H_Size;
        pCBIOSExtension->DisplayOneModeV = pMode->V_Size;
    }else
        CBIOSDebugPrint((0, "HDMI: Get native mode error\n"));

    return;
}


RDC_EXPORT void LoadTiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum)
{
    BYTE ucDevice;
    ucDevice = Get_DEV_ID(DisplayPath);
    UnLockTiming(DisplayPath); 
    

    switch(ucDevice)
    {
        case CRTIndex:
        case CRT2Index:
        case DVIIndex:
        case DVI2Index:
             LoadVESATiming(pCBIOSExtension, DisplayPath,ModeNum);
             break;

        case LCDIndex:
        case LCD2Index:                    
             LoadLCDTiming(pCBIOSExtension, DisplayPath,ModeNum);
             LockTiming(DisplayPath);
             break;
        case TVIndex:
        case TV2Index:
             LoadTVTiming(pCBIOSExtension, DisplayPath,ModeNum);
             LockTiming(DisplayPath);
             break;
        case HDMIIndex:
        case HDMI2Index:
             LoadHDMITiming(pCBIOSExtension, DisplayPath,ModeNum);
             break;
       default:
            break;
    }
}


RDC_EXPORT CBStatus GetModeColorDepth(WORD ModeNum, MODE_INFO *pModeInfo, BYTE *pColorDepth)
{
    if(pModeInfo->Mode_ID_8bpp == ModeNum)
    {
        *pColorDepth = 8;
        return TRUE;

    }
    else if(pModeInfo->Mode_ID_16bpp == ModeNum)
    {
        *pColorDepth = 16;
        return TRUE;
    }
    else if(pModeInfo->Mode_ID_32bpp == ModeNum)
    {
        *pColorDepth = 32;
        return TRUE;
    }
    CBIOSDebugPrint((0, "CBIOS GetModeColorDepth Fail !!==\n"));
    
    *pColorDepth = 0;
    
    return FALSE;
    
}

RDC_EXPORT CBStatus GetModePitch(BYTE DisplayPath, WORD ModeNum, WORD *pPitch)
{
    MODE_INFO* pModeInfo = NULL;
    BYTE ColorDepth = 0, ucDevice;
    CBStatus bStatus;
    ucDevice = Get_DEV_ID(DisplayPath);
    
    switch(ucDevice)
    {
        case LCDIndex:
        case LCD2Index:
            bStatus = Get_MODE_INFO_From_LCD_Table(ModeNum,&pModeInfo);
            if(!bStatus) 
                bStatus = Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo);
            break;
        case HDMIIndex:
        case HDMI2Index:
            bStatus = Get_MODE_INFO_From_HDMI_Table(ModeNum,&pModeInfo);
            if(!bStatus) 
                bStatus = Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo);
            break;
       default: 
            bStatus = Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo);
            break;
    }
    
    
    if(!bStatus)
    {
        CBIOSDebugPrint((0, "CBIOS GetModePitch Fail !!==\n"));
        return FALSE;
    }
    else
    {
        if(!GetModeColorDepth(ModeNum, pModeInfo, &ColorDepth))
        {
            return FALSE;
        }
        else
        {
            ColorDepth = ColorDepth >> 4;
            *pPitch = ((((pModeInfo ->H_Size) << ColorDepth)+0x7)&0xFFF8);
        }
    }

    return TRUE;
}


RDC_EXPORT void UnLockCR0ToCR7()
{
    
    SetCRReg(0x11, 0x00, BIT7);
}


RDC_EXPORT void LockCR0ToCR7()
{
    
    SetCRReg(0x11, 0x80, BIT7);
}


RDC_EXPORT CBStatus CheckForModeAvailable(WORD ModeNum)
{
    CBStatus bStatus;
    MODE_INFO *pModeInfo = NULL;
    bStatus = Get_MODE_INFO_From_VESA_Table(ModeNum, &pModeInfo);
    if(!bStatus)
        bStatus = Get_MODE_INFO_From_LCD_Table(ModeNum, &pModeInfo);
    if(!bStatus)
        bStatus = Get_MODE_INFO_From_HDMI_Table(ModeNum, &pModeInfo);
    return bStatus;
}


RDC_EXPORT void TurnOnDigitalPort(BYTE DisplayPath)
{
    BYTE DeviceIndex, DevicePort;
    
    DeviceIndex = Get_DEV_ID(DisplayPath);

    
    if(DeviceIndex)
    {
        DevicePort = GetDevicePort(DeviceIndex);
        switch(DevicePort)
        {
        case CRT_PORT:
            TurnOnCRTPad();
            break;
        case DVP1:
            TurnOnDVP1Pad();
            break;
        case DVP2:
            TurnOnDVP2Pad();
            break;
        case DVP12:        
            TurnOnDVP12Pad();
            break;
        default:
            break;
        }    
    }
}


RDC_EXPORT void TurnOffDigitalPort(BYTE DisplayPath)
{
    BYTE DeviceIndex, DevicePort;
    
    DeviceIndex = Get_DEV_ID(DisplayPath); 

    
    if(DeviceIndex)
    {
        DevicePort = GetDevicePort(DeviceIndex);
        switch(DevicePort)
        {
        case CRT_PORT:
            TurnOffCRTPad();
            break;
        case DVP1:
            TurnOffDVP1Pad();
            break;
        case DVP2:
            TurnOffDVP2Pad();
            break;
        case DVP12:        
            TurnOffDVP12Pad();
            break;
        default:
            break;
        }
    }    
}

RDC_EXPORT BYTE GetDevicePortConfig(BYTE DeviceIndex)
{
    BYTE i;
    BYTE Result = 0;
    
    HandleShareConnector(&DeviceIndex);

    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        if(PortConfig[i].DeviceIndex==DeviceIndex)
        {
            Result = i;
            break;
        }
        else
        {
            Result = BIT7;
        }
    }

    return Result;
}


RDC_EXPORT BYTE GetPortConnectPath(BYTE PortType)
{
    BYTE SR1F, PortMask;

    SR1F = GetSRReg(0x1F);
    SR1F ^= 0x03;
    
    switch(PortType)
    {
        case CRT_PORT:
            PortMask = BIT2;
            break;
            
        case DVP1:
        case DVP12:
            PortMask = BIT0;
            break;
            
        case DVP2:
            PortMask = BIT1;
            break;
    }
    
    return ((SR1F & PortMask) ? 1 : 0);

}


RDC_EXPORT WORD TransDevIDtoBit(BYTE DeviceIndex)
{
    if(DeviceIndex == LCDIndex)
        return B_LCD;
    else
    return (1 << (DeviceIndex - 1));
}

RDC_EXPORT void TurnOnScaler(BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
        SetSRReg(0x58, BIT0, BIT0);
    else
        SetSRReg(0x50, BIT0, BIT0);
}

RDC_EXPORT void TurnOffScaler(BYTE DisplayPath)
{
    if (DisplayPath == DISP1)
        SetSRReg(0x58, 0, BIT0);
    else
        SetSRReg(0x50, 0, BIT0);
    SetSRReg(0x70, 0, BIT7);
    SetSRReg(0x74, 0, BIT7);
}

RDC_EXPORT void SetDPAReg(BYTE ucDPADelay, BYTE ucDeviceID)
{
    BYTE bDigitalDevice = GetDevicePort(ucDeviceID);
    if(bDigitalDevice & DVP1)
    {          
        SetSRReg(0x21, ucDPADelay, 0x07);
    }else 
        SetSRReg(0x21, ucDPADelay, 0x38);
}

RDC_EXPORT void SetDrivingReg(BYTE ucDrivingStatus)
{
    SetSRReg(SR4A, ucDrivingStatus, BIT0+BIT1);
}


RDC_EXPORT void SerialLoadTable(REG_PACKAGE *pREG_PACKAGE_Table)
{
    REG_PACKAGE *pTemp_Reg = pREG_PACKAGE_Table;
    
    do{
        
         switch(pTemp_Reg->RegGroup)
        {
            case SR:
                SetSRReg(pTemp_Reg->RegIndex, pTemp_Reg->RegValue, pTemp_Reg->RegMask);
                break;
            case CR:
                SetCRReg(pTemp_Reg->RegIndex, pTemp_Reg->RegValue, pTemp_Reg->RegMask);
                break;
            case GR:
                SetGRReg(pTemp_Reg->RegIndex, pTemp_Reg->RegValue, pTemp_Reg->RegMask);                
                break;
            case MR:
                SetMSReg(pTemp_Reg->RegValue);
                break;
            case AR:
                SetARReg(pTemp_Reg->RegIndex, pTemp_Reg->RegValue);                
                break;
            default:
                break;
        }
        pTemp_Reg++;
    }while(pTemp_Reg->RegGroup!=NR);

}

RDC_EXPORT void LoadDisplay1VESAModeInitRegs()
{
    OutPort(MISC_WRITE, 0x2F);

    UnLockCR0ToCR7();
    
    SerialLoadTable(Display1VESAModeInitRegs);

    ResetATTR();

    SetARReg(0x10, 0x01);
    SetARReg(0x11, 0x00);
    SetARReg(0x12, 0x00);
    SetARReg(0x13, 0x00);
    SetARReg(0x14, 0x00);

    EnableATTR();
}

RDC_EXPORT void EnableSWPSByPass()
{
    SetSRReg(0x32, BIT0, BIT0);
}

RDC_EXPORT void DisableSWPSByPass()
{
    SetSRReg(0x32, 0, BIT0);
}

RDC_EXPORT void ResetLVDSChannelOutPut()
{
    SetSRReg(0x1E, 0, BIT0+BIT2);
}

RDC_EXPORT void DisableLCDDither()
{
    SetSRReg(0x22, 0, BIT0);
}

RDC_EXPORT void TurnOffTxEncReg(BYTE DisplayPath, BYTE bDigitalDevice)
{
    BYTE ucDeviceID, bDVP1 = (bDigitalDevice & DVP1);
    ucDeviceID = Get_DEV_ID(DisplayPath);
    
    switch(ucDeviceID)
    {
        case LCDIndex:
        case LCD2Index:
                if(bDVP1)
                PowerSequenceOff();
            break;
                        
        case TVIndex:
        case TV2Index:
            
            DisableTVClock();
            break;

        case HDMIIndex:
        case DVIIndex:    
        case HDMI2Index:
        case DVI2Index:    
            
            
            DisableTMDSReg();
            if(bDVP1)
                DisableSWPSByPass();
            break;
    }
    
}

RDC_EXPORT void TurnOnTxEncReg(BYTE DisplayPath ,BYTE bDigitalDevice)
{
    BYTE ucDeviceID, bDVP1 = (bDigitalDevice & DVP1);
    ucDeviceID = Get_DEV_ID(DisplayPath);

    switch(ucDeviceID)
    {
        case LCDIndex:
        case LCD2Index:
            PowerSequenceOn();
            break;
        
        case TVIndex:
        case TV2Index:
            EnableSWPSByPass();
            EnableTVClock();
            SetTVDACPower(DeviceON); 
            SetDPAReg(6, ucDeviceID);
            break;
        
        case HDMIIndex:
        case DVIIndex:    
        case HDMI2Index:
        case DVI2Index:    
            if(bDVP1)
            {
                ResetLVDSChannelOutPut();
                DisableLCDDither();        
                EnableSWPSByPass();
            }
            
            EnableTMDSReg(ucDeviceID, Get_HDMI_TYPE());
            break;
    }
}


RDC_EXPORT void DisableDisplayPathAndDevice(BYTE DisplayPath)
{
    BYTE DeviceIndex = Get_DEV_ID(DisplayPath);
    BYTE DevicePort = GetDevicePort(DeviceIndex);

    TurnOnDVPMask(DevicePort);
    TurnOffScaler(DisplayPath);
    SequencerOff(DisplayPath);
    TurnOffTxEncReg(DisplayPath,DevicePort);
    TurnOffDigitalPort(DisplayPath);    
}


RDC_EXPORT void ClearFrameBuffer(BYTE DisplayPath, DWORD *pFrameBufferBase, DWORD ulWidth, DWORD ulHeight, BYTE bColorDepth)
{
    DWORD dwFactor  = 0;
    DWORD i = 0;
    
    
    switch(bColorDepth)
    {
        case 8:
        case 16:
        case 32:    
          dwFactor = 32 / bColorDepth;
          break;
          
        default:
            return;
    }

    
    for(i = 0;i<((ulWidth*ulHeight)/dwFactor);i++)
    {
        *(pFrameBufferBase+i) = 0x00000000;
    }
    
}


RDC_EXPORT void SetDPMS(BYTE DPMSState, BYTE DisplayPath)
{
    BYTE RegValue=0; 

    
    if(DPMSState > DeviceON)
        RegValue = BIT1+BIT0; 
        
    if(DisplayPath == DISP1)
        SetCRReg(0xB6, RegValue, BIT1+BIT0);
    else
        SetCRReg(0x3E, RegValue, BIT1+BIT0);
        
    
    if(DPMSState > DeviceON) 
    {
        RegValue =0; 
        if(DisplayPath == DISP1)
            SetCRReg(0xB6, RegValue, BIT3);
        else
            SetCRReg(0xB4, RegValue, BIT0);
    }else
    {   
        if(DisplayPath == DISP1)
            SetCRReg(0xB6, BIT3, BIT3);
        else
            SetCRReg(0xB4, BIT0, BIT0);
    }
}


RDC_EXPORT void WaitDisplayPeriod()
{
     while ((InPort(COLOR_INPUT_STATUS1_READ)&BIT0) == 1);
     
     while ((InPort(COLOR_INPUT_STATUS1_READ)&BIT0) == 0);
}

RDC_EXPORT void CBIOSI2CWriteClock(BYTE I2CPort, BYTE data)
{
    ULONG i;
    BYTE ucMaskData=1, ucTmpData;
    if(data)            
        ucMaskData &= 0;
    for (i=0;i<0x1000; i++)
    {
        SetCRReg(I2CPort, ucMaskData, 0x1);
        ucTmpData = GetCRReg(I2CPort) & 0x01;
        if (ucMaskData == ucTmpData) 
            break;
    }
}

RDC_EXPORT void CBIOSI2CDelay(BYTE I2CPort)
{
    BYTE     i,jtemp;
    
    for (i=0;i<100;i++)
        jtemp = GetCRReg(I2CPort);
}

RDC_EXPORT void CBIOSI2CWriteData(BYTE I2CPort, BYTE data)
{

    ULONG i;
    BYTE ucMaskData=0x4, ucTmpData;
    if(data)            
        ucMaskData &= 0;
    for (i=0;i<0x1000; i++)
    {
        SetCRReg(I2CPort, ucMaskData, 0x4);
        ucTmpData = GetCRReg(I2CPort) & 0x04;
        if (ucMaskData == ucTmpData) 
            break;
    }
}

RDC_EXPORT void CBIOSI2CStart(BYTE I2CPort)
{
    CBIOSI2CWriteClock(I2CPort, 0x00);               
    CBIOSI2CDelay(I2CPort);
    CBIOSI2CWriteData(I2CPort, 0x01);                
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteClock(I2CPort, 0x01);               
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteData(I2CPort, 0x00);                
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteClock(I2CPort, 0x01);                  
    CBIOSI2CDelay(I2CPort);                    
}

RDC_EXPORT void CBIOSSendI2CDataByte(BYTE I2CPort, BYTE Data)
{
    char i;

    for (i=7;i>=0;i--)
    {
        CBIOSI2CWriteClock(I2CPort, 0x00);           
        CBIOSI2CDelay(I2CPort);         
        if((Data>>i)&0x1)
            CBIOSI2CWriteData(I2CPort, 0x1);           
        else
            CBIOSI2CWriteData(I2CPort, 0x0);           
        
        CBIOSI2CDelay(I2CPort);         
        
        CBIOSI2CWriteClock(I2CPort, 0x01);           
        CBIOSI2CDelay(I2CPort);                           
    }                
}

RDC_EXPORT BYTE CBIOSCheckACK(BYTE I2CPort)
{    
    CBIOSI2CWriteClock(I2CPort, 0x00);               
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteData(I2CPort, 0x01);                
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteClock(I2CPort, 0x01);               
    CBIOSI2CDelay(I2CPort);             
    return ((GetCRReg(I2CPort) & 0x20) ? 0:1);                
}

RDC_EXPORT BYTE CBIOSReceiveI2CDataByte(BYTE I2CPort, BYTE I2CSlave)
{
    BYTE jData=0, jTempData;
    char i;
    DWORD j;

    for (i=7;i>=0;i--)
    {
        CBIOSI2CWriteClock(I2CPort, 0x00);                
        CBIOSI2CDelay(I2CPort);     
            
        CBIOSI2CWriteData(I2CPort, 0x01);                 
        CBIOSI2CDelay(I2CPort);         
        
        CBIOSI2CWriteClock(I2CPort, 0x01);                
        CBIOSI2CDelay(I2CPort);           
        
        for (j=0; j<0x1000; j++)
        {   
            if (((GetCRReg(I2CPort) & 0x10) >> 4))
                break;
        }    
                    
        jTempData =  (GetCRReg(I2CPort) & 0x20) >> 5;
        jData |= ((jTempData & 0x01) << i); 
        
        CBIOSI2CWriteClock(I2CPort, 0x01);                
        CBIOSI2CDelay(I2CPort);                           
    }    
    
    return (jData);                              
}

RDC_EXPORT void CBIOSSendNACK(BYTE I2CPort)
{
    CBIOSI2CWriteClock(I2CPort, 0x00);               
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteData(I2CPort, 0x01);                
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteClock(I2CPort, 0x01);               
    CBIOSI2CDelay(I2CPort);    
}

RDC_EXPORT void CBIOSI2CStop(BYTE I2CPort)
{
    CBIOSI2CWriteClock(I2CPort, 0x00);               
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteData(I2CPort, 0x00);                
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteClock(I2CPort, 0x01);               
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteData(I2CPort, 0x01);                
    CBIOSI2CDelay(I2CPort);    
    CBIOSI2CWriteClock(I2CPort, 0x01);                
    CBIOSI2CDelay(I2CPort);                      
}

RDC_EXPORT BYTE CBIOSReadI2C(BYTE I2CPort, BYTE I2CSlave, BYTE RegIdx, BYTE* RegData)
{
    CBIOSI2CStart(I2CPort);

    
    CBIOSSendI2CDataByte(I2CPort, I2CSlave);
    
    if (!CBIOSCheckACK(I2CPort))
    {
        return CBIOSI2C_ERROR;
    }    

    
    CBIOSSendI2CDataByte(I2CPort, RegIdx);

    if (!CBIOSCheckACK(I2CPort))
    {
        return CBIOSI2C_ERROR;
    }    
    
    CBIOSI2CStart(I2CPort);
   
    
    CBIOSSendI2CDataByte(I2CPort, I2CSlave+1);
    
    if (!CBIOSCheckACK(I2CPort))
    {
        return CBIOSI2C_ERROR;
    }    
    
    *RegData = CBIOSReceiveI2CDataByte(I2CPort, I2CSlave);

    CBIOSSendNACK(I2CPort);

    CBIOSI2CStop(I2CPort);

    return CBIOSI2C_OK;       
}

RDC_EXPORT BYTE CBIOSWriteI2C(BYTE I2CPort, BYTE I2CSlave, BYTE RegIdx, BYTE RegData)
{
    CBIOSI2CStart(I2CPort);

    
    CBIOSSendI2CDataByte(I2CPort, I2CSlave);
    if (!CBIOSCheckACK(I2CPort))
    {
        return CBIOSI2C_ERROR;
    }    

    
    CBIOSSendI2CDataByte(I2CPort, RegIdx);
    if (!CBIOSCheckACK(I2CPort))
    {
        return CBIOSI2C_ERROR;
    }    

    
    CBIOSSendI2CDataByte(I2CPort, RegData);
    if (!CBIOSCheckACK(I2CPort))
    {
        return CBIOSI2C_ERROR;
    }
    
    CBIOSSendNACK(I2CPort);
         
    CBIOSI2CStop(I2CPort);
    return CBIOSI2C_OK;       
}

RDC_EXPORT void CBIOSGetDeviceI2CInformation(BYTE ucDevice, BYTE* ucI2Cport, BYTE* ucI2CAddress)
{
    BYTE i;
    WORD wI2C_Info=0;

    HandleShareConnector(&ucDevice);

    *ucI2Cport    = 0;
    *ucI2CAddress = 0;
    
    
    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        if(PortConfig[i].DeviceIndex == ucDevice)
        {
            *ucI2Cport    = (BYTE)(PortConfig[i].TX_I2C_Port_Addr);
            *ucI2CAddress = (BYTE)(PortConfig[i].TX_I2C_Port_Addr>>8);        
            break;
        }
    }
    
    
    if(ucDevice == HDMIIndex || 
       ucDevice == DVIIndex)
    {
        switch(Get_TMDS_TX_ID())
        {
        case TMDS_ID_ITE6610:
            *ucI2CAddress = 0x98;
            break;
        case TMDS_ID_SII162:
        case TMDS_ID_EP932M:
            *ucI2CAddress = 0x70;
            break;
        case TX_None:
            default:
            break;
        }
    }
}

RDC_EXPORT DWORD CBIOSGetPortI2CInfo(void)
{
    BYTE i,bCRT = 0x0, bDVP1 = 0x0, bDVP2 = 0x0;
    DWORD dwPortI2C = 0x0;

    
    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        
        if(PortConfig[i].DeviceIndex!=0)
        {
            switch(PortConfig[i].PortID)
            {
                case CRT_PORT:
                    bCRT = (BYTE)((PortConfig[i].TX_I2C_Port_Addr) & 0xFF);
                    break;
                case DVP1:
                case DVP12:
                    bDVP1 = (BYTE)((PortConfig[i].TX_I2C_Port_Addr) & 0xFF);
                    break;
                case DVP2:
                    bDVP2 = (BYTE)((PortConfig[i].TX_I2C_Port_Addr) & 0xFF);
                    break;
                default:
                    break;
            }    
        }
    }

    dwPortI2C = ((bDVP2 << 16)|(bDVP1 << 8)|bCRT);

    return dwPortI2C;
}

RDC_EXPORT void CBIOSSetOutFormat(WORD wAttribute)
{
    if(wAttribute & BIT6) 
    {
        SetSRReg(0x1E,BIT0,BIT0); 
    }else
    {
        SetSRReg(0x1E,0,BIT0);   
    }
}

RDC_EXPORT void CBIOSSetDither()
{
    BYTE i,ucLCDIndex;
    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        if(PortConfig[i].DeviceIndex==LCDIndex)
        {
            ucLCDIndex=i;
            break;
        }
    }
    if(PortConfig[ucLCDIndex].Attribute & TX_DIT)
    {
        SetSRReg(0x22,0,BIT0);
    }else
    {
        SetSRReg(0x22,BIT0,BIT0);
    }
}

RDC_EXPORT void CBIOSSetOpenLDI(WORD wAttribute)
{
    if(wAttribute & OpenLDI)
    {
        SetSRReg(0x1E,BIT2,BIT2);   
    }
    else
    {
        SetSRReg(0x1E,0,BIT2);       
    }
}

RDC_EXPORT WORD ConvertMStoCount(WORD wTDX)
{
    WORD  wTmp1;
    DWORD dwTmp2;
    wTmp1= wTDX>>6;
    wTDX<<=10;
    dwTmp2= (wTmp1<<16) + wTDX;
    return (WORD)(dwTmp2/293);
}

RDC_EXPORT void CBIOSSetTD0(WORD wTD0)
{
    WORD wSetReg;
    BYTE ucHighByte,ucLowByte;
    wSetReg=ConvertMStoCount(wTD0);
    ucHighByte=wSetReg>>8;
    ucLowByte =(BYTE)wSetReg;
    SetSRReg(0x12,ucLowByte ,0xFF);
    SetSRReg(0x1A,ucHighByte,0x0F);
}

RDC_EXPORT void CBIOSSetTD1(WORD wTD1)
{
    WORD wSetReg;
    BYTE ucHighByte,ucLowByte;
    wSetReg=ConvertMStoCount(wTD1);
    ucHighByte=wSetReg>>4;
    ucLowByte =(BYTE)wSetReg;
    SetSRReg(0x13,ucLowByte ,0xFF);
    SetSRReg(0x1A,ucHighByte,0xF0);
}

RDC_EXPORT void CBIOSSetTD2(WORD wTD2)
{
    WORD wSetReg;
    BYTE ucHighByte,ucLowByte;
    wSetReg=ConvertMStoCount(wTD2);
    ucHighByte=wSetReg>>8;
    ucLowByte =(BYTE)wSetReg;
    SetSRReg(0x14,ucLowByte ,0xFF);
    SetSRReg(0x1B,ucHighByte,0x0F);
}

RDC_EXPORT void CBIOSSetTD3(WORD wTD3)
{
    WORD wSetReg;
    BYTE ucHighByte,ucLowByte;
    wSetReg=ConvertMStoCount(wTD3);
    ucHighByte=wSetReg>>4;
    ucLowByte =(BYTE)wSetReg;
    SetSRReg(0x15,ucLowByte ,0xFF);
    SetSRReg(0x1B,ucHighByte,0xF0);
}

RDC_EXPORT void CBIOSSetTD5(WORD wTD5)
{
    WORD wSetReg;
    BYTE ucHighByte,ucLowByte;
    wSetReg=ConvertMStoCount(wTD5);
    ucHighByte=wSetReg>>4;
    ucLowByte =(BYTE)wSetReg;
    SetSRReg(0x17,ucLowByte ,0xFF);
    SetSRReg(0x1C,ucHighByte,0xF0);
}

RDC_EXPORT void CBIOSSetTD6(WORD wTD6)
{
    WORD wSetReg;
    BYTE ucHighByte,ucLowByte;
    wSetReg=ConvertMStoCount(wTD6);
    ucHighByte=wSetReg>>8;
    ucLowByte =(BYTE)wSetReg;
    SetSRReg(0x18,ucLowByte ,0xFF);
    SetSRReg(0x1D,ucHighByte,0x0F);
}

RDC_EXPORT void CBIOSSetTD7(WORD wTD7)
{
    WORD wSetReg;
    BYTE ucHighByte,ucLowByte;
    wSetReg=ConvertMStoCount(wTD7);
    ucHighByte=wSetReg>>8;
    ucLowByte =(BYTE)wSetReg;
    SetSRReg(0x19,ucLowByte ,0xFF);
    SetSRReg(0x1D,ucHighByte,0xF0);
}

RDC_EXPORT void LoadPowerSequenceTimer(void)
{
    BYTE i,ucLCDIndex;
    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        if(PortConfig[i].DeviceIndex==LCDIndex)
        {
            ucLCDIndex=i;
            break;
        }
    }
    if(!(PortConfig[ucLCDIndex].Attribute & BIT2)) 
    {
        if(PortConfig[ucLCDIndex].PortID==DVP1 || PortConfig[ucLCDIndex].PortID==DVP12)
        {
            CBIOSSetTD0(LCDTable[0].pPanelTable.TD0);
            CBIOSSetTD1(LCDTable[0].pPanelTable.TD1);
            CBIOSSetTD2(LCDTable[0].pPanelTable.TD2);
            CBIOSSetTD3(LCDTable[0].pPanelTable.TD3);
            CBIOSSetTD5(LCDTable[0].pPanelTable.TD5);
            CBIOSSetTD6(LCDTable[0].pPanelTable.TD6);
            CBIOSSetTD7(LCDTable[0].pPanelTable.TD7);
            if(LCDTable[0].pPanelTable.Timing.Attribute & SW_PS)
            {
                SetSRReg(0x11,BIT1,BIT1);
            }else
            {
                SetSRReg(0x11,0,BIT1);
            }
        }
    }
    SetSRReg(0x32,0,BIT0);  
}

RDC_EXPORT void CBIOSInitLCD()
{
    
    Set_LCD_Panel_Size(LCDTable[0].H_Size, LCDTable[0].V_Size);
    
    LoadPowerSequenceTimer();
    
    CBIOSSetOutFormat(LCDTable[0].pPanelTable.Timing.Attribute);
    
    CBIOSSetDither();
    
    CBIOSSetOpenLDI(LCDTable[0].pPanelTable.Timing.Attribute);
}

RDC_EXPORT void SetGPIOResetReg(BYTE bGPIOMask)
{
    BYTE ucTmpData;
    DWORD i;
    SetCRReg(0xDA,bGPIOMask,bGPIOMask);
    SetCRReg(0xDB,0,bGPIOMask);
    
    for(i=0; i<300000; i++)
        ucTmpData = GetCRReg(0);
    SetCRReg(0xDB,bGPIOMask,bGPIOMask);
}


RDC_EXPORT void CBIOSGetTMDSTxType(BYTE ucI2CPort)
{
    TxEncDetect *pTmp = pTMDSTxTable;
    BYTE ucTmpData = 0, ucTxID = 0;
    
    SetGPIOResetReg(GPIO2);

    do{
        
        
        CBIOSReadI2C(ucI2CPort, pTmp->ucTxAddr, pTmp->ucD_IDIndex, &ucTmpData);

        
        
        if(ucTmpData == pTmp->ucD_IDData)
        {
            ucTxID = pTmp->ucTxIndex;
            break;
        }

        pTmp++;
        
        
        
        if(pTmp->ucTxIndex == 0xFF)
            ucTxID = TMDS_ID_EP932M;
            
    }while(!ucTxID);

    
    
    WriteScratch(IDX_TMDS1_TX_ID, ucTxID);
}



RDC_EXPORT void CBIOSSetTXType(PCBIOS_Extension pCBIOSExtension)
{
    BYTE i;
    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        switch(PortConfig[i].DeviceIndex)
        {
        case LCDIndex:
            if (pCBIOSExtension->dwSupportDevices & B_LCD)
                WriteScratch(IDX_LVDS1_TX_ID,PortConfig[i].TX_ENC_ID);
            break;
        case LCD2Index:
            if (pCBIOSExtension->dwSupportDevices & B_LCD2)
                WriteScratch(IDX_LVDS2_TX_ID,PortConfig[i].TX_ENC_ID);
            break;
        case TVIndex:
            if (pCBIOSExtension->dwSupportDevices & B_TV)
                WriteScratch(IDX_TV1_ENCODER_ID,PortConfig[i].TX_ENC_ID);
            break;
        case TV2Index:
            if (pCBIOSExtension->dwSupportDevices & B_TV2)
                WriteScratch(IDX_TV2_ENCODER_ID,PortConfig[i].TX_ENC_ID);
            break;
        case HDMIIndex:
        case DVIIndex:
            if (pCBIOSExtension->dwSupportDevices & (B_HDMI | B_DVI))
                CBIOSGetTMDSTxType((BYTE)(PortConfig[i].TX_I2C_Port_Addr));
            break;
            
        
        
        
        default:
            break;
        }
    }
}


RDC_EXPORT void CBIOSGetSupportDevice(PCBIOS_Extension pCBIOSExtension)
{
    BYTE i;
    pCBIOSExtension->dwSupportDevices = 0;

    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        if (PortConfig[i].Attribute & Dev_Support)
            pCBIOSExtension->dwSupportDevices |=  TransDevIDtoBit(PortConfig[i].DeviceIndex);
    }
    
    
    if(pCBIOSExtension->dwSupportDevices & B_HDMI)
        pCBIOSExtension->dwSupportDevices |= B_DVI;
}


RDC_EXPORT void CBIOSUpdateDevSupFlag(PCBIOS_Extension pCBIOSExtension)
{
    
    if(pCBIOSExtension->dwSupportDevices & B_LCD)
        pCBIOSExtension->bLCDSupport = TRUE;
    if(pCBIOSExtension->dwSupportDevices & B_CRT)
        pCBIOSExtension->bCRTSupport = TRUE;
    if(pCBIOSExtension->dwSupportDevices & B_HDMI)
        pCBIOSExtension->bHDMISupport = TRUE;
    if(pCBIOSExtension->dwSupportDevices & B_DVI)
        pCBIOSExtension->bDVISupport = TRUE;
    if(pCBIOSExtension->dwSupportDevices & B_TV)
        pCBIOSExtension->bTVSupport = TRUE;
}


RDC_EXPORT void CBIOSInitPortConfig(BYTE* pjROMLinearAddr)
{
    BYTE i;
    BYTE *ucTmpData = (BYTE*)(PortConfig);
    WORD wVGAROMSIZE = 0;

    
    if (pjROMLinearAddr != NULL) 
    {
        for(wVGAROMSIZE=0; wVGAROMSIZE<0x8000; wVGAROMSIZE++)
        {
            if(*(pjROMLinearAddr + wVGAROMSIZE) =='P' &&
               *(pjROMLinearAddr + wVGAROMSIZE + 1) =='C' &&
               *(pjROMLinearAddr + wVGAROMSIZE + 2) =='F' &&
               *(pjROMLinearAddr + wVGAROMSIZE + 3) =='G')
                break;
        }
        if(wVGAROMSIZE==0x8000)
            wVGAROMSIZE &=0;         
    }
    if(wVGAROMSIZE) 
    {
        pjROMLinearAddr = (BYTE*)((int)(pjROMLinearAddr) + wVGAROMSIZE + 4); 
                                           
        for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
        {
            if(*(pjROMLinearAddr + i*sizeof(PORT_CONFIG)) == 0xFF)
                break;
        }
        
        memcpy(ucTmpData, pjROMLinearAddr, i*sizeof(PORT_CONFIG));
    }
}


RDC_EXPORT BYTE CBIOSCheckDeviceAvailable(BYTE NewDisplay1DeviceID)
{
    int i;
    BYTE status = FALSE;
    if(!NewDisplay1DeviceID)
        return TRUE;
    if(NewDisplay1DeviceID==DVIIndex)
        NewDisplay1DeviceID = HDMIIndex; 
    for(i=0;i<(sizeof(PortConfig)/sizeof(PORT_CONFIG));i++)
    {
        if((PortConfig[i].DeviceIndex == NewDisplay1DeviceID) &&
           (PortConfig[i].Attribute & Dev_Support))
        {
            status = TRUE;
            break;
        }
    }
    return status;
}



RDC_EXPORT void CBIOSInitLCDTable(BYTE* pjROMLinearAddr)
{
    WORD  wVGAROMSIZE=0, wDevAddress=0;
    BYTE *ucTmpData = (BYTE*)(LCDTable);
    for(wVGAROMSIZE=0; wVGAROMSIZE<0x8000; wVGAROMSIZE++)
    {
        if(*(pjROMLinearAddr + wVGAROMSIZE)     =='L' &&
           *(pjROMLinearAddr + wVGAROMSIZE + 1) =='C' &&
           *(pjROMLinearAddr + wVGAROMSIZE + 2) =='D' &&
           *(pjROMLinearAddr + wVGAROMSIZE + 3) =='T' &&
           *(pjROMLinearAddr + wVGAROMSIZE + 4) =='B' &&
           *(pjROMLinearAddr + wVGAROMSIZE + 5) =='L')
        {
            break;
        }
    }
    if(wVGAROMSIZE!=0x8000)
    {
        wDevAddress = ReadScratch(IDX_LCD1_TABLE_INDEX);
        if(!wDevAddress) 
        {
            wDevAddress = *(pjROMLinearAddr + 
                                wVGAROMSIZE + 
                                          6 + 
                       5*sizeof(PANEL_INFO) + 
                                          2 + 
                                          5); 
        }
        
        
        pjROMLinearAddr = (BYTE*)((int)(pjROMLinearAddr) +
                                           wVGAROMSIZE +  
                                                     6 +  
                      (wDevAddress-1)*sizeof(PANEL_INFO));
        
        memcpy(ucTmpData, pjROMLinearAddr, sizeof(PANEL_INFO));
        
        LCDTable[0].pPanelTable.Timing.H_Total+=LCDTable[0].H_Size;
        LCDTable[0].pPanelTable.Timing.H_Sync_End+=LCDTable[0].pPanelTable.Timing.H_Sync_Start;
        LCDTable[0].pPanelTable.Timing.V_Total+=LCDTable[0].V_Size;
        LCDTable[0].pPanelTable.Timing.V_Sync_End+=LCDTable[0].pPanelTable.Timing.V_Sync_Start;
    }
}


RDC_EXPORT void CBIOSInitHDMITable(BYTE* pjROMLinearAddr)
{
    WORD  wVGAROMSIZE;
    
    for(wVGAROMSIZE=0; wVGAROMSIZE<0x8000; wVGAROMSIZE++)
    {
        if(*(pjROMLinearAddr + wVGAROMSIZE)     =='T' &&
           *(pjROMLinearAddr + wVGAROMSIZE + 1) =='X' &&
           *(pjROMLinearAddr + wVGAROMSIZE + 2) =='T' &&
           *(pjROMLinearAddr + wVGAROMSIZE + 3) =='B' &&
           *(pjROMLinearAddr + wVGAROMSIZE + 4) =='L')
        {
            break;
        }
    }
    
    if(wVGAROMSIZE!=0x8000)
    {
        
        
        pTMDSTxTable = (TxEncDetect*)((int)(pjROMLinearAddr) +
                                           wVGAROMSIZE +       
                                                     5);       
    }
}

RDC_EXPORT void cbGetHDMIModeInfo(WORD* wHSize, WORD* wVSize, BYTE* bRRCnt, BYTE ucHDMIType)
{
    *wHSize = HDMITable[ucHDMIType].H_Size;
    *wVSize = HDMITable[ucHDMIType].V_Size;
    *bRRCnt = HDMITable[ucHDMIType].RRTableCount;
}

RDC_EXPORT void cbSetHDMIModeInfo(WORD wHSize, WORD wVSize, BYTE ucHDMIType)
{
    HDMITable[ucHDMIType].H_Size = wHSize;
    HDMITable[ucHDMIType].V_Size = wVSize;
}

RDC_EXPORT void CBIOSGetHDMIPanelSize(BYTE ucHDMIType, WORD* wHSize, WORD* wVSize)
{
    *wHSize = HDMITable[ucHDMIType].H_Size;
    *wVSize = HDMITable[ucHDMIType].V_Size;
}

RDC_EXPORT void CBIOSGetVBIOSBuildDate(PCBIOS_Extension pCBIOSExtension)
{
    BYTE* pROMDevIDAddr;
    BYTE ucTmpData[2], ucYear[4];

    
    pROMDevIDAddr = (BYTE*)(pCBIOSExtension->pjROMLinearAddr+ROM_Offset_Build_Year);
    ucYear[0]= *pROMDevIDAddr;
    ucYear[1]= *(pROMDevIDAddr+1);
    ucYear[2]= *(pROMDevIDAddr+2);
    ucYear[3]= *(pROMDevIDAddr+3);
    pCBIOSExtension->wVBIOSBuildYear = (WORD)(strtoul((char *)ucYear,NULL,10));

    
    pROMDevIDAddr = (BYTE*)(pCBIOSExtension->pjROMLinearAddr+ROM_Offset_Build_Month);
    ucTmpData[0] = *pROMDevIDAddr;
    ucTmpData[1] = *(pROMDevIDAddr+1);
    pCBIOSExtension->ucVBIOSBuildMonth = (BYTE)(strtoul((char *)ucTmpData,NULL,10));
    
    
    pROMDevIDAddr = (BYTE*)(pCBIOSExtension->pjROMLinearAddr+ROM_Offset_Build_Date);
    ucTmpData[0] = *pROMDevIDAddr;
    ucTmpData[1] = *(pROMDevIDAddr+1);
    pCBIOSExtension->ucVBIOSBuildDate = (BYTE)(strtoul((char *)ucTmpData,NULL,10));
}

RDC_EXPORT void CBIOSGetVBIOSVersion(PCBIOS_Extension pCBIOSExtension)
{
    BYTE* pROMDevIDAddr = (BYTE*)(pCBIOSExtension->pjROMLinearAddr+ROM_Offset_Rev_Num);
    BYTE ucTmpData[2];
    ucTmpData[0] = *pROMDevIDAddr;
    ucTmpData[1] = *(pROMDevIDAddr+1);
    pCBIOSExtension->ulVBIOS_Version = strtoul((char *)ucTmpData,NULL, 10);
}

RDC_EXPORT void CBIOSGetVBIOSRomDeviceID(PCBIOS_Extension pCBIOSExtension)
{
    WORD* pROMDevIDAddr = (WORD*)(pCBIOSExtension->pjROMLinearAddr+ROM_Offset_Dev_ID);
    pCBIOSExtension->wVenderID = *pROMDevIDAddr;
    pCBIOSExtension->wDeviceID = *(pROMDevIDAddr+1);
}


RDC_EXPORT void CBIOSInitialDataFromVBIOS(PCBIOS_Extension pCBIOSExtension)
{
    pRelated_IOAddress = pCBIOSExtension->pjIOAddress;
    
    
    CBIOSGetVBIOSVersion(pCBIOSExtension);
    
    CBIOSGetVBIOSBuildDate(pCBIOSExtension);
    
    CBIOSGetVBIOSRomDeviceID(pCBIOSExtension);
    
    CBIOSInitPortConfig(pCBIOSExtension->pjROMLinearAddr);
    
    CBIOSGetSupportDevice(pCBIOSExtension);
    
    CBIOSUpdateDevSupFlag(pCBIOSExtension);
    
    if(pCBIOSExtension->dwSupportDevices & B_LCD)
    {
        CBIOSInitLCDTable(pCBIOSExtension->pjROMLinearAddr);
    }

    if(pCBIOSExtension->dwSupportDevices & B_HDMI)
    {
        CBIOSInitHDMITable(pCBIOSExtension->pjROMLinearAddr);
    }

    CBIOSSetTXType(pCBIOSExtension);
}

RDC_EXPORT void CBIOSInitialI2CReg(void)
{
    SetCRReg(0xB7, 0x00, 0xFF);
    SetCRReg(0xB5, 0x00, 0xFF);
    SetCRReg(0x39, 0x00, 0xFF);
}


RDC_EXPORT BYTE DetectMonitor(BYTE I2CPort)
{
    BYTE RegData;
    BYTE Status = 0;
    
    if (CBIOSReadI2C(I2CPort, 0xA0, 0x00, &RegData))
    {
        if (RegData == 0x00)
        {
            CBIOSReadI2C(I2CPort, 0xA0, 0x01, &RegData);

            if (RegData == 0xFF)
                Status = 1;
            else
                Status = 0;
        }
        else
            Status = 0;
    }
    else
        Status = 0;

    return Status;
}


RDC_EXPORT void WaitLong()
{
    while(!(InPort(COLOR_INPUT_STATUS1_READ) & BIT3));
    while(InPort(COLOR_INPUT_STATUS1_READ) & BIT3);
    while(!(InPort(COLOR_INPUT_STATUS1_READ) & BIT3));
}


RDC_EXPORT BYTE SenseCRT()
{
    BYTE bConnect = 0;
    
    
    
    SetSRReg(0x4F, 0x80, 0xFF);
    
    
    
    WaitLong();
    
    
    
    bConnect = GetSRReg(0x3C) & BIT0;
    
    
    
    SetSRReg(0x4F, 0x00, 0xFF);

    if (!bConnect)
        bConnect = 0;
    else
        bConnect = B_CRT;

    return(bConnect);
}


RDC_EXPORT void SetVBERerurnStatus(WORD VBEReturnStatus, CBIOS_ARGUMENTS *pCBiosArguments)
{
    pCBiosArguments->AX = VBEReturnStatus;
}





































RDC_EXPORT CBStatus OEM_QueryBiosInfo (PCBIOS_Extension pCBIOSExtension)
{
    BYTE ucDDRRate;
    WORD wDramSize;
    CBIOS_ARGUMENTS *pBiosArguments = pCBIOSExtension->pCBiosArguments;
        
        
        
        
    pBiosArguments->Ebx |= pCBIOSExtension->ulVBIOS_Version;
    
        
        
        
        
        
        
        
        
        
        
        
        
    pBiosArguments->Ecx = BIT16;                 
    wDramSize = (WORD)(GetCRReg(0xAA)&(BIT0 + BIT1 + BIT2));
    wDramSize = 8 << wDramSize;                  
    wDramSize <<= 6;                             
    pBiosArguments->Ecx |= (DWORD)wDramSize;     
    
    ucDDRRate = GetCRReg(0xAB);
    ucDDRRate &= BIT0;
    pBiosArguments->Ecx |= (DWORD)ucDDRRate;
        
        
        
        
        
        
    pBiosArguments->DX =  pCBIOSExtension->wVBIOSBuildYear;
    pBiosArguments->Edx <<=16;
    pBiosArguments->DL =  pCBIOSExtension->ucVBIOSBuildDate;
    pBiosArguments->DH =  pCBIOSExtension->ucVBIOSBuildMonth;
        
        
        
    pBiosArguments->Esi |=  pCBIOSExtension->dwSupportDevices;
        
        
        
    pBiosArguments->Edi |=  pCBIOSExtension->pVideoPhysicialAddress;
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_QueryBiosCaps (CBIOS_ARGUMENTS *pCBiosArguments)
{ 
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments); 
    return TRUE;
}



















































RDC_EXPORT CBStatus OEM_QueryExternalDeviceInfo (CBIOS_ARGUMENTS *pCBiosArguments)
{
    BYTE ucI2CPort, ucAddr;
    
    pCBiosArguments->BL = ReadScratch(IDX_SCRATCH_20);   
    pCBiosArguments->Ebx <<= 16;

    pCBiosArguments->BH = ReadScratch(IDX_SCRATCH_21);   

    pCBiosArguments->BL = ReadScratch(IDX_SCRATCH_22);   

    
    pCBiosArguments->Ecx = BIT16;                        

    
    CBIOSGetDeviceI2CInformation(HDMIIndex, &ucI2CPort, &ucAddr);
    pCBiosArguments->DH = ucI2CPort;    
    CBIOSGetDeviceI2CInformation(TVIndex, &ucI2CPort, &ucAddr);
    pCBiosArguments->DL = ucI2CPort;
    pCBiosArguments->Edx <<= 16;
    CBIOSGetDeviceI2CInformation(LCDIndex, &ucI2CPort, &ucAddr);
    pCBiosArguments->DH = ucI2CPort;    
    CBIOSGetDeviceI2CInformation(CRTIndex, &ucI2CPort, &ucAddr);
    pCBiosArguments->DL = ucI2CPort;

    
    pCBiosArguments->Edi = CBIOSGetPortI2CInfo();

    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    
    return TRUE;
}



































RDC_EXPORT CBStatus OEM_QueryDisplayPathInfo (PCBIOS_Extension pCBIOSExtension)
{
    BYTE ScratchTempData, ucDeviceIndex;
    CBIOS_ARGUMENTS *pCBiosArguments = pCBIOSExtension->pCBiosArguments;
    
    ucDeviceIndex = Get_DEV_ID(DISP1);
    pCBiosArguments->BL = GetDevicePort(ucDeviceIndex);
    pCBiosArguments->BL <<= 2;

    ScratchTempData = GetSRReg(0x58);               
    if (ScratchTempData & BIT0)
    {
        ScratchTempData &= BIT2 + BIT1;             
        ScratchTempData >>= 1;
        pCBiosArguments->BL |= ScratchTempData;
    }
    
    if(ReadScratch(IDX_DS_ENABLE))                  
    {
        if(GetSRReg(0x70)&BIT7)    
            pCBiosArguments->BL |= BIT0;            
            
        if(GetSRReg(0x70)&BIT7)    
            pCBiosArguments->BL |= BIT1;            
    }
    pCBiosArguments->BL <<= 4;
    pCBiosArguments->BL |= ucDeviceIndex;

    pCBiosArguments->Ebx <<= 7;
    pCBiosArguments->BL |= Get_RRATE_ID(DISP1);
    
    pCBiosArguments->Ebx <<= 9;   
    pCBiosArguments->BX |= Get_VESA_MODE(DISP1);
    
    
    ucDeviceIndex = Get_DEV_ID(DISP2);
    pCBiosArguments->CL = GetDevicePort(ucDeviceIndex);
    pCBiosArguments->CL <<= 2;
    ScratchTempData = GetSRReg(0x50);               
    if (ScratchTempData & BIT0)
    {
        ScratchTempData &= BIT2 + BIT1;             
        ScratchTempData >>= 1;
        pCBiosArguments->CL |= ScratchTempData;
    }
    pCBiosArguments->CL <<= 4;
    pCBiosArguments->CL |= ucDeviceIndex;

    pCBiosArguments->Ecx <<= 7;
    pCBiosArguments->CL |= Get_RRATE_ID(DISP2);
    
    pCBiosArguments->Ecx <<= 9;   
    pCBiosArguments->CX |= Get_VESA_MODE(DISP2);
    
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return TRUE;
}


RDC_EXPORT CBStatus OEM_QueryDeviceConnectStatus (PCBIOS_Extension pCBIOSExtension)
{
    BYTE    QuickCheck;
    CBIOS_ARGUMENTS *pBiosArguments = pCBIOSExtension->pCBiosArguments;
    pBiosArguments->BX = 0;

    
    
    
    QuickCheck = (BYTE)pBiosArguments->CX;

    if (QuickCheck & QUERYCRT)
    {
        
        if (pCBIOSExtension->dwSupportDevices & B_CRT)
        {
            pBiosArguments->BX |= SenseCRT();
        }
    }
    else if (QuickCheck & QUERYHDMI)
    {
        
        
        if ((pCBIOSExtension->dwSupportDevices & B_HDMI) ||
            (pCBIOSExtension->dwSupportDevices & B_DVI))
        {
            pBiosArguments->BX |= QueryHDMIConnectStatus(QuickCheck);
        }
    }
    else
    {
        
        
        if ((pCBIOSExtension->dwSupportDevices & B_HDMI) ||
            (pCBIOSExtension->dwSupportDevices & B_DVI))
        {
            pBiosArguments->BX |= QueryHDMIConnectStatus(QuickCheck);
        }

        
        if (pCBIOSExtension->dwSupportDevices & B_TV)
        {
            pBiosArguments->BX |= SenseTV();
        }    

        
        if (pCBIOSExtension->dwSupportDevices & B_LCD)
        {
            pBiosArguments->BX |= B_LCD;
        }

        
        if (pCBIOSExtension->dwSupportDevices & B_CRT)
        {
            pBiosArguments->BX |= SenseCRT();
        }

        
    }
    
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_QuerySupportedMode (CBIOS_ARGUMENTS *pCBiosArguments)
{
    int VESATableIndex = 0, VESATableSIZE = sizeof(VESATable)/sizeof(MODE_INFO);
    int RRateTableIndex = 0, RRateTableSize;
    int ModeNumIndex;
    WORD ModeNum, SerialNumber;
    
    SerialNumber = pCBiosArguments->CX;
    
    for (VESATableIndex = 0; VESATableIndex < VESATableSIZE; VESATableIndex++)
    {
    
        RRateTableSize = VESATable[VESATableIndex].RRTableCount;
        
        for (ModeNumIndex = 0; ModeNumIndex < 3; ModeNumIndex++)
        {
            switch (ModeNumIndex)
            {
                case 0:
                    ModeNum = VESATable[VESATableIndex].Mode_ID_8bpp;
                    break;
                case 1:
                    ModeNum = VESATable[VESATableIndex].Mode_ID_16bpp;
                    break;
                case 2:
                    ModeNum = VESATable[VESATableIndex].Mode_ID_32bpp;
                    break;
            }
            
            for (RRateTableIndex = 0; RRateTableIndex < RRateTableSize; RRateTableIndex++)
            {
                if ( !(VESATable[VESATableIndex].pRRTable[RRateTableIndex].Attribute & DISABLE) )
                {
                    if (SerialNumber == 0)
                    {
                        
                        pCBiosArguments->BX = ModeNum;
                        
                        
                        GetModeColorDepth(ModeNum, &VESATable[VESATableIndex], &pCBiosArguments->CL);
                        
                        
                        pCBiosArguments->CH = VESATable[VESATableIndex].pRRTable[RRateTableIndex].RRate_ID;
                        
                        
                        pCBiosArguments->Edx = VESATable[VESATableIndex].V_Size << 16 | VESATable[VESATableIndex].H_Size;
                        
                        pCBiosArguments->Edi = VESATable[VESATableIndex].pRRTable[RRateTableIndex].Clock;
                        pCBiosArguments->Esi = VESATable[VESATableIndex].pRRTable[RRateTableIndex].V_Total << 16 | VESATable[VESATableIndex].pRRTable[RRateTableIndex].H_Total;

                        SetVBERerurnStatus (VBEFunctionCallSuccessful, pCBiosArguments);

                        return TRUE;
                    }
                    else
                    {
                        SerialNumber--;
                    }
                }
            }
        }
    }

    SetVBERerurnStatus (VBEFunctionCallFail, pCBiosArguments);
    return TRUE;
    
}























RDC_EXPORT CBStatus OEM_QueryLCDPanelSizeMode (CBIOS_ARGUMENTS *pCBiosArguments)
{
    BYTE ucInData = pCBiosArguments->CL;
    if(ucInData < 3)
    {
        if(ucInData == 2)       
        {
            pCBiosArguments->BX = LCDTable[0].Mode_ID_32bpp;
            pCBiosArguments->CL = 32;
        }
        else if(ucInData == 1)  
        {
            pCBiosArguments->BX = LCDTable[0].Mode_ID_16bpp;
            pCBiosArguments->CL = 16;
        }
        else                    
        {
            pCBiosArguments->BX = LCDTable[0].Mode_ID_8bpp;
            pCBiosArguments->CL = 8;
        }
        pCBiosArguments->CH  = LCDTable[0].pPanelTable.Timing.RRate_ID;
        pCBiosArguments->Edx = Get_LCD_Panel_Size();
        SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    }
    else
    {
        SetVBERerurnStatus(VBEFunctionCallFail, pCBiosArguments);
    }
    return TRUE;
}
RDC_EXPORT CBStatus OEM_QueryLCD2PanelSizeMode (CBIOS_ARGUMENTS *pCBiosArguments)
{
    DWORD dwLCDPanelSize;

    dwLCDPanelSize = Get_LCD2_Panel_Size();
    
    if (dwLCDPanelSize)
    {
        pCBiosArguments->Ebx = dwLCDPanelSize; 
                                               
        SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    }
    else
    {
        SetVBERerurnStatus(VBEFunctionCallFail, pCBiosArguments);
    }
    return TRUE;
}

RDC_EXPORT CBStatus OEM_QueryTVConfiguration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    pCBiosArguments->BL = ReadScratch(IDX_SCRATCH_05);
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_SetSetTVFunction(CBIOS_ARGUMENTS *pCBiosArguments)
{
    PCBIOSTVFun_Disp_Info pCBTVFun = (PCBIOSTVFun_Disp_Info)pCBiosArguments->Ecx;
    BYTE bChange = pCBTVFun->bChange;
    if(bChange & DiffCCRS)
        SetTVCCRSLevel(pCBTVFun->ucCCRSLevel);
    if(bChange & DiffVScaler)
        CBIOSSetFS473VScalingLevel(pCBTVFun);
    if(bChange & DiffHPos)
        SetFS473HPosition(pCBTVFun->ucHPosition);
    if(bChange & DiffVPos)
        SetFS473VPosition(pCBTVFun->ucVPosition);
    if(bChange & DiffHScaler)
        SetFS473HSCaler(pCBTVFun);
        
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_QueryTV2Configuration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    pCBiosArguments->BL = ReadScratch(IDX_SCRATCH_06);
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_QueryHDMISupportedMode (CBIOS_ARGUMENTS *pCBiosArguments)
{
    int VESATableIndex = 0, VESATableSIZE = sizeof(HDMITable)/sizeof(MODE_INFO);
    int RRateTableIndex = 0, RRateTableSize;
    int ModeNumIndex;
    WORD ModeNum, SerialNumber;
    
    SerialNumber = pCBiosArguments->CX;
    
    for (VESATableIndex = 0; VESATableIndex < VESATableSIZE; VESATableIndex++)
    {
    
        RRateTableSize = HDMITable[VESATableIndex].RRTableCount;
        
        for (ModeNumIndex = 0; ModeNumIndex < 3; ModeNumIndex++)
        {
            switch (ModeNumIndex)
            {
                case 0:
                    ModeNum = HDMITable[VESATableIndex].Mode_ID_8bpp;
                    break;
                case 1:
                    ModeNum = HDMITable[VESATableIndex].Mode_ID_16bpp;
                    break;
                case 2:
                    ModeNum = HDMITable[VESATableIndex].Mode_ID_32bpp;
                    break;
            }
            
            for (RRateTableIndex = 0; RRateTableIndex < RRateTableSize; RRateTableIndex++)
            {
                if ( !(HDMITable[VESATableIndex].pRRTable[RRateTableIndex].Attribute & DISABLE) )
                {
                    if (SerialNumber == 0)
                    {
                        
                        pCBiosArguments->BX = ModeNum;
                        
                        
                        GetModeColorDepth(ModeNum, &HDMITable[VESATableIndex], &pCBiosArguments->CL);
                        
                        
                        pCBiosArguments->CH = HDMITable[VESATableIndex].pRRTable[RRateTableIndex].RRate_ID;
                        
                        
                        pCBiosArguments->Edx = HDMITable[VESATableIndex].V_Size << 16 | HDMITable[VESATableIndex].H_Size;
                        
                        SetVBERerurnStatus (VBEFunctionCallSuccessful, pCBiosArguments);

                        return TRUE;
                    }
                    else
                    {
                        SerialNumber--;
                    }
                }
            }
        }
    }

    SetVBERerurnStatus (VBEFunctionCallFail, pCBiosArguments);
    return TRUE;
    
}

RDC_EXPORT CBStatus OEM_SetActiveDisplayDevice (CBIOS_ARGUMENTS *pCBiosArguments)
{
    BYTE Display1DeviceID, NewDisplay1DeviceID;
    BYTE Display2DeviceID, NewDisplay2DeviceID;
    Display1DeviceID = Get_DEV_ID(DISP1);
    Display2DeviceID = Get_DEV_ID(DISP2);
    NewDisplay1DeviceID = pCBiosArguments->CL & 0x0F;
    NewDisplay2DeviceID = pCBiosArguments->CL >> 4;
    
    
    if(!NewDisplay1DeviceID &&
       !NewDisplay2DeviceID)
        return FALSE;
    
    
    if(CBIOSCheckDeviceAvailable(NewDisplay1DeviceID))
    {
        
        if (NewDisplay1DeviceID != Display1DeviceID)
        {
            DisableDisplayPathAndDevice(DISP1);

            if (!NewDisplay1DeviceID) 
            {
                Set_DEV_ID(0, DISP1);
            }
            Set_NEW_DEV_ID(NewDisplay1DeviceID, DISP1);
        }
    }
    
    
    if(CBIOSCheckDeviceAvailable(NewDisplay2DeviceID))
    {
        
        if (NewDisplay2DeviceID != Display2DeviceID)
        {
            DisableDisplayPathAndDevice(DISP2);

            if (!NewDisplay2DeviceID)
            {
                Set_DEV_ID(0, DISP2);
            }
            Set_NEW_DEV_ID(NewDisplay2DeviceID, DISP2);
        }
    }

    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return TRUE;
}
RDC_EXPORT CBStatus OEM_SetVESAModeForDisplay2(CBIOS_Extension *pCBIOSExtension)
{
    WORD    ModeNum = pCBIOSExtension->pCBiosArguments->CX & 0x01FF;
    CBIOSEDID_DETAILED_TIMING *pEDIDDetailedTiming = (CBIOSEDID_DETAILED_TIMING*)pCBIOSExtension->pCBiosArguments->Esi;
    WORD    Pitch = 0, wEDIDPolarity = 0;    
    MODE_INFO *pModeInfo = NULL;
    BYTE    ColorDepth = 0;
    WORD    VBEReturnStatus = VBEFunctionCallFail;
    BYTE ucDeviceID, ucDevicePort; 
    
    if (ModeNum <= 0x13)
    {
        
        VBEReturnStatus = VBEFunctionCallSuccessful;
    }
    else
    {
        if (CheckForModeAvailable(ModeNum))
        {
            Set_VESA_MODE(ModeNum, DISP2);
            ucDeviceID = Get_DEV_ID(DISP1);
            
            
            
            
            
            

            if(ucDeviceID != Get_NEW_DEV_ID(DISP2))
            {
                
                    
                
                    
                Set_DEV_ID(Get_NEW_DEV_ID(DISP2), DISP2);
            }

            
            ucDeviceID = Get_DEV_ID(DISP2);
            ucDevicePort = GetDevicePort(ucDeviceID);

            

            

            if(pEDIDDetailedTiming)
            {
                
                VESAEDIDTable[0].H_Size = pEDIDDetailedTiming->usHorDispEnd;
                VESAEDIDTable[0].V_Size = pEDIDDetailedTiming->usVerDispEnd;

                VESAEDIDTable[0].pRRTable->Clock = (ULONG)(pEDIDDetailedTiming->usPixelClock) * 10;
                
                VESAEDIDTable[0].pRRTable->H_Sync_End = 
                               pEDIDDetailedTiming->usHorDispEnd +
                               (USHORT)pEDIDDetailedTiming->ucHorBorder +
                               pEDIDDetailedTiming->usHorSyncStart +
                               pEDIDDetailedTiming->usHorSyncTime;

                VESAEDIDTable[0].pRRTable->H_Sync_Start = 
                                pEDIDDetailedTiming->usHorDispEnd + 
                                (USHORT)pEDIDDetailedTiming->ucHorBorder +
                                pEDIDDetailedTiming->usHorSyncStart;

                VESAEDIDTable[0].pRRTable->H_Total = 
                                 pEDIDDetailedTiming->usHorDispEnd + 
                                 pEDIDDetailedTiming->usHorBlankingTime +
                                 (USHORT)pEDIDDetailedTiming->ucHorBorder * 2;
                
                VESAEDIDTable[0].pRRTable->RRate_ID = RR60;

                VESAEDIDTable[0].pRRTable->V_Sync_End = 
                                pEDIDDetailedTiming->usVerDispEnd +
                                (USHORT)pEDIDDetailedTiming->ucVerBorder +
                                pEDIDDetailedTiming->usVerSyncStart +
                                pEDIDDetailedTiming->usVerSyncTime;

                VESAEDIDTable[0].pRRTable->V_Sync_Start = 
                                pEDIDDetailedTiming->usVerDispEnd + 
                                (USHORT)pEDIDDetailedTiming->ucVerBorder +
                                pEDIDDetailedTiming->usVerSyncStart;

                VESAEDIDTable[0].pRRTable->V_Total = 
                                 pEDIDDetailedTiming->usVerDispEnd + 
                                 pEDIDDetailedTiming->usVerBlankingTime +
                                 (USHORT)pEDIDDetailedTiming->ucVerBorder * 2;

                if ((pEDIDDetailedTiming->ucFlags & (BIT4+BIT3)) == 0x18)
                {                   
                    wEDIDPolarity |= pEDIDDetailedTiming->ucFlags;
                    wEDIDPolarity = ~(wEDIDPolarity);
                    VESAEDIDTable[0].pRRTable->Attribute = wEDIDPolarity;
                }
                else
                {
                    VESAEDIDTable[0].pRRTable->Attribute = (NHS|PVS);
                }
                
            }            
            
            LoadTiming(pCBIOSExtension, DISP2, ModeNum);

            GetModePitch(DISP2, ModeNum, &Pitch);

            SetPitch(DISP2, Pitch);
            switch(Get_DEV_ID(DISP2))
            {
                case LCDIndex:
                case LCD2Index:
                     if(!Get_MODE_INFO_From_LCD_Table(ModeNum,&pModeInfo))
                         Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo);
                    break;
                case HDMIIndex:
                case HDMI2Index:
                    if(!Get_MODE_INFO_From_HDMI_Table(ModeNum,&pModeInfo))
                         Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo);
                    break;
               default: 
                    Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo);
                    break;
            }

            GetModeColorDepth(ModeNum, pModeInfo, &ColorDepth);

            SetColorDepth(DISP2, ColorDepth);

            

            

            
            if(!(pCBIOSExtension->pCBiosArguments->CX & BIT15))
            {
                ClearFrameBuffer(DISP2, pCBIOSExtension->pVideoVirtualAddress, (DWORD)pModeInfo->H_Size, (DWORD)pModeInfo->V_Size, ColorDepth);
            }
            
            SetFIFO(DISP2);

            ConfigDigitalPort(DISP2);

            TurnOnDigitalPort(DISP2);

            

            
            TurnOnTxEncReg(DISP2,ucDevicePort);
            
            TurnOffDVPMask(ucDevicePort);

            SequencerOn(DISP2);

            VBEReturnStatus = VBEFunctionCallSuccessful;
        }
    }
        
    SetVBERerurnStatus(VBEReturnStatus, pCBIOSExtension->pCBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_SetDevicePowerState (CBIOS_ARGUMENTS *pCBiosArguments)
{
    
    BYTE display1DeviceID, display2DeviceID, TargetDevice, DMPSState;
    WORD VBEReturnStatus = VBEFunctionCallFail;
    
        
        
        
        
        
        
        
        
        
        
        
        
        
    
    TargetDevice = pCBiosArguments->CL & 0x0F;
    
        
        
        
        
    
    DMPSState = pCBiosArguments->DX & (BIT1+BIT0);
    
    if(DMPSState<=DeviceOFF)
    {
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        display1DeviceID = Get_DEV_ID(DISP1);
        
        
        
        
        
        
        
        
        
        
        
        
        
        
        display2DeviceID = Get_DEV_ID(DISP2);
        
        if(display1DeviceID == TargetDevice)
        {
            
                
                
                
            SetDPMS(DMPSState, DISP1);
            
            switch(display1DeviceID)
            {
            
            case LCDIndex:
                
                    
                    
                    
                if(DMPSState)
                    PowerSequenceOff();
                else
                    
                    PowerSequenceOn();
                break;
            
            case TVIndex:
                
                    
                    
                    
                        
                        
                SetTVDACPower(DMPSState);
                break;
            
            case DVIIndex:
            case HDMIIndex:
                
                
                    
                    
                    
                if(DMPSState)
                    TurnOffDigitalPort(DISP1);
                else
                    
                    TurnOnDigitalPort(DISP1);
                
                break;
            default:
                break;
            }
            
            VBEReturnStatus = VBEFunctionCallSuccessful;
        }
        
        else if(display2DeviceID == TargetDevice)
        {
            
                
                
                
            SetDPMS(DMPSState, DISP2);
            
            switch(display1DeviceID)
            {
            
            case LCDIndex:
                
                    
                    
                    
                if(DMPSState)
                    PowerSequenceOff();
                else
                    
                    PowerSequenceOn();
                break;
            
            case TVIndex:
                
                    
                    
                    
                        
                        
                SetTVDACPower(DMPSState);
                break;
            
            case DVIIndex:
            case HDMIIndex:
                
                
                    
                    
                    
                if(DMPSState)
                    TurnOffDigitalPort(DISP2);
                else
                    
                    TurnOnDigitalPort(DISP2);
                
                break;
            default:
                break;
            }
            
            VBEReturnStatus = VBEFunctionCallSuccessful;
        }
    }else
        
        VBEReturnStatus = VBEFunctionCallFail;
    SetVBERerurnStatus(VBEReturnStatus, pCBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_SetDisplay2Pitch(CBIOS_ARGUMENTS *pCBiosArguments)
{
    WORD    VBEReturnStatus = VBEFunctionCallFail;
    WORD    wPitch;

    wPitch = pCBiosArguments->CX;
    SetPitch(DISP2, wPitch);
    VBEReturnStatus = VBEFunctionCallSuccessful;
    
    SetVBERerurnStatus(VBEReturnStatus, pCBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_SetRefreshRate(CBIOS_ARGUMENTS *pCBiosArguments)
{
    BYTE RRateID, DisplayPath;

    RRateID = pCBiosArguments->CL & 0x7F;

    if (pCBiosArguments->BX == SetDisplay1RefreshRate)
        DisplayPath = DISP1;
    else
        DisplayPath = DISP2;

    Set_RRATE_ID(RRateID, DisplayPath);
    
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_SetSetTVConfiguration(CBIOS_ARGUMENTS *pCBiosArguments)
{
    Set_TV_TYPE(pCBiosArguments->CL);
    Set_TV_CONNECTION_TYPE(pCBiosArguments->CH);
    return TRUE;
}
RDC_EXPORT CBStatus OEM_SetTV2Configuration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return TRUE;
}

RDC_EXPORT CBStatus OEM_SetHDMIType(CBIOS_ARGUMENTS *pCBiosArguments)
{
    Set_HDMI_TYPE(pCBiosArguments->CL);
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return TRUE;
}
RDC_EXPORT CBStatus OEM_SetHDMIOutputSignal(CBIOS_ARGUMENTS *pCBiosArguments)
{
    Set_HDMI_Output_Signal(pCBiosArguments->CL);
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus OEM_VideoPOST (PCBIOS_Extension pCBIOSExtension)
{
    BYTE i=0, btemp=0;
    
    
    
    
    
    btemp = InIOPort(VGA_ENABLE); 
    btemp |= 1;
    OutIOPort(VGA_ENABLE,btemp);

    
    btemp = InIOPort(MISC_READ); 
    btemp |= 3;
    OutIOPort(MISC_WRITE,btemp);
    
    
    OutIOPort(COLOR_CRTC_INDEX,0x80);
    OutIOPort(COLOR_CRTC_DATA,0xA8);

    
    OutIOPort(COLOR_CRTC_INDEX,0xA1);
    btemp = InIOPort(COLOR_CRTC_DATA);
    btemp |= 4;
    OutIOPort(COLOR_CRTC_DATA,btemp);
    
    
    for(i = 0x81;i < 0x9F;i++)
        SetCRReg(i, 0x00, 0xFF);
    
    
    
    btemp = 0x00;
    SetCRReg(0xBB, btemp, 0xFF);
    
    
    

    
    SerialLoadTable(ExtendRegs);

    
    SetCRReg(0xDF, 0x00, BIT2);

    
    
    Set_NEW_DEV_ID(CRTIndex, DISP1);

    CBIOSInitialI2CReg();
    
    CBIOSGetSupportDevice(pCBIOSExtension);

    CBIOSUpdateDevSupFlag(pCBIOSExtension);

    
    if(pCBIOSExtension->dwSupportDevices & B_LCD)
    {
        CBIOSInitLCD(); 
    }

    if(pCBIOSExtension->dwSupportDevices & B_TV)
    {
        CBIOSInitTV(); 
    }

    if(pCBIOSExtension->dwSupportDevices & (B_HDMI|B_DVI))
    {
        
        
        CBIOSSetTXType(pCBIOSExtension);
    }

    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBIOSExtension->pCBiosArguments);

    return TRUE;
}

RDC_EXPORT CBStatus OEM_VSetEDIDInModeTable(CBIOS_Extension *pCBIOSExtension)
{
    CBIOSEDID_DETAILED_TIMING *pEDIDDetailedTiming = (CBIOSEDID_DETAILED_TIMING*)pCBIOSExtension->pCBiosArguments->Ecx;

    if(!pCBIOSExtension->pCBiosArguments->Ecx)
    {
        CBIOSDebugPrint((0, "CBIOS: Invalided EDID pointer !!\n"));
        return FALSE;
    }
    
    VESAEDIDTable[0].H_Size = pEDIDDetailedTiming->usHorDispEnd;
    VESAEDIDTable[0].V_Size = pEDIDDetailedTiming->usVerDispEnd;

    VESAEDIDTable[0].pRRTable->Clock = (ULONG)(pEDIDDetailedTiming->usPixelClock) * 10;
    
    VESAEDIDTable[0].pRRTable->H_Sync_End = 
                   pEDIDDetailedTiming->usHorDispEnd +
                   (USHORT)pEDIDDetailedTiming->ucHorBorder +
                   pEDIDDetailedTiming->usHorSyncStart +
                   pEDIDDetailedTiming->usHorSyncTime;

    VESAEDIDTable[0].pRRTable->H_Sync_Start = 
                    pEDIDDetailedTiming->usHorDispEnd + 
                    (USHORT)pEDIDDetailedTiming->ucHorBorder +
                    pEDIDDetailedTiming->usHorSyncStart;

    VESAEDIDTable[0].pRRTable->H_Total = 
                     pEDIDDetailedTiming->usHorDispEnd + 
                     pEDIDDetailedTiming->usHorBlankingTime +
                     (USHORT)pEDIDDetailedTiming->ucHorBorder * 2;
    
    VESAEDIDTable[0].pRRTable->RRate_ID = RR60;

    VESAEDIDTable[0].pRRTable->V_Sync_End = 
                    pEDIDDetailedTiming->usVerDispEnd +
                    (USHORT)pEDIDDetailedTiming->ucVerBorder +
                    pEDIDDetailedTiming->usVerSyncStart +
                    pEDIDDetailedTiming->usVerSyncTime;

    VESAEDIDTable[0].pRRTable->V_Sync_Start = 
                    pEDIDDetailedTiming->usVerDispEnd + 
                    (USHORT)pEDIDDetailedTiming->ucVerBorder +
                    pEDIDDetailedTiming->usVerSyncStart;

    VESAEDIDTable[0].pRRTable->V_Total = 
                     pEDIDDetailedTiming->usVerDispEnd + 
                     pEDIDDetailedTiming->usVerBlankingTime +
                     (USHORT)pEDIDDetailedTiming->ucVerBorder * 2;

    if ((pEDIDDetailedTiming->ucFlags & (BIT4+BIT3)) == 0x18)
    {                   
        VESAEDIDTable[0].pRRTable->Attribute = (WORD)(~(pEDIDDetailedTiming->ucFlags));
    }
    else
    {
        VESAEDIDTable[0].pRRTable->Attribute = (NHS|PVS);
    }
    
    return TRUE;
}

RDC_EXPORT CBStatus VBE_SetMode(CBIOS_Extension *pCBIOSExtension)
{
    WORD    ModeNum = pCBIOSExtension->pCBiosArguments->BX & 0x01FF;
    CBStatus    bUserSpecifiedTiming = ((pCBIOSExtension->pCBiosArguments->BX & BIT11) ? TRUE : FALSE);
    CBIOSEDID_DETAILED_TIMING *pEDIDDetailedTiming = (CBIOSEDID_DETAILED_TIMING*)pCBIOSExtension->pCBiosArguments->Esi;
    WORD    Pitch = 0, wEDIDPolarity=0;
    USHORT  usModeWidth, usModeHeight;
    MODE_INFO *pModeInfo = NULL;
    BYTE    ColorDepth = 0, bTemp = 0;
    WORD    VBEReturnStatus = VBEFunctionCallFail;
    BYTE    ucDeviceID, ucDevicePort;
        
    if (ModeNum <= 0x13) 
    {
        VBEReturnStatus = VBEFunctionCallSuccessful;
        CBIOSDebugPrint((0, "CBIOS: Invalided mode, Mode Num = %Xh\n", ModeNum));
    }else
    {
        if (CheckForModeAvailable(ModeNum)||bUserSpecifiedTiming)
        {
            Set_VESA_MODE(ModeNum, DISP1);
            ucDeviceID = Get_DEV_ID(DISP1);
            
            
            
            
            TurnOffScaler(DISP1);

            
            
            
            if(Get_DEV_ID(DISP1) != Get_NEW_DEV_ID(DISP1))
            {
                

                
                
                
                
                Set_DEV_ID(Get_NEW_DEV_ID(DISP1), DISP1);
            }

            
            ucDeviceID = Get_DEV_ID(DISP1);
            ucDevicePort = GetDevicePort(ucDeviceID);

            
            LoadDisplay1VESAModeInitRegs();
            
            if(pEDIDDetailedTiming)
            {
                CBIOSDebugPrint((0, "CBIOS: Updating %d x %d EDID mode\n", pEDIDDetailedTiming->usHorDispEnd, pEDIDDetailedTiming->usVerDispEnd));
                
                VESAEDIDTable[0].H_Size = pEDIDDetailedTiming->usHorDispEnd;
                VESAEDIDTable[0].V_Size = pEDIDDetailedTiming->usVerDispEnd;

                VESAEDIDTable[0].pRRTable->Clock = (ULONG)(pEDIDDetailedTiming->usPixelClock) * 10;
                
                VESAEDIDTable[0].pRRTable->H_Sync_End = 
                               pEDIDDetailedTiming->usHorDispEnd +
                               (USHORT)pEDIDDetailedTiming->ucHorBorder +
                               pEDIDDetailedTiming->usHorSyncStart +
                               pEDIDDetailedTiming->usHorSyncTime;

                VESAEDIDTable[0].pRRTable->H_Sync_Start = 
                                pEDIDDetailedTiming->usHorDispEnd + 
                                (USHORT)pEDIDDetailedTiming->ucHorBorder +
                                pEDIDDetailedTiming->usHorSyncStart;

                VESAEDIDTable[0].pRRTable->H_Total = 
                                 pEDIDDetailedTiming->usHorDispEnd + 
                                 pEDIDDetailedTiming->usHorBlankingTime +
                                 (USHORT)pEDIDDetailedTiming->ucHorBorder * 2;
                
                VESAEDIDTable[0].pRRTable->RRate_ID = RR60;

                VESAEDIDTable[0].pRRTable->V_Sync_End = 
                                pEDIDDetailedTiming->usVerDispEnd +
                                (USHORT)pEDIDDetailedTiming->ucVerBorder +
                                pEDIDDetailedTiming->usVerSyncStart +
                                pEDIDDetailedTiming->usVerSyncTime;

                VESAEDIDTable[0].pRRTable->V_Sync_Start = 
                                pEDIDDetailedTiming->usVerDispEnd + 
                                (USHORT)pEDIDDetailedTiming->ucVerBorder +
                                pEDIDDetailedTiming->usVerSyncStart;

                VESAEDIDTable[0].pRRTable->V_Total = 
                                 pEDIDDetailedTiming->usVerDispEnd + 
                                 pEDIDDetailedTiming->usVerBlankingTime +
                                 (USHORT)pEDIDDetailedTiming->ucVerBorder * 2;

                if ((pEDIDDetailedTiming->ucFlags & (BIT4+BIT3)) == 0x18)
                {                   
                    bTemp = (~(pEDIDDetailedTiming->ucFlags));
                    wEDIDPolarity = (WORD)bTemp;
                    VESAEDIDTable[0].pRRTable->Attribute = wEDIDPolarity;
                }
                else
                {
                    VESAEDIDTable[0].pRRTable->Attribute = (NHS|PVS);
                }

            }

            LoadTiming(pCBIOSExtension, DISP1, ModeNum);
            
            if(bUserSpecifiedTiming)
            {
                ColorDepth = pCBIOSExtension->pCBiosArguments->CL;
                usModeWidth = VESAEDIDTable[0].H_Size;
                usModeHeight = VESAEDIDTable[0].V_Size;
                Pitch = (usModeWidth * ((USHORT)ColorDepth/8) + 7) & 0xFFF8;
            }
            else
            {
                GetModePitch(DISP1, ModeNum, &Pitch);

                switch(ucDeviceID)
                {
                    case LCDIndex:
                    case LCD2Index:
                        if(!Get_MODE_INFO_From_LCD_Table(ModeNum,&pModeInfo))
                             Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo);
                        break;
                    case HDMIIndex:
                    case HDMI2Index:
                        if(!Get_MODE_INFO_From_HDMI_Table(ModeNum,&pModeInfo))
                             Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo);
                        break;
                    default: 
                        Get_MODE_INFO_From_VESA_Table(ModeNum,&pModeInfo);
                        break;
                }

                GetModeColorDepth(ModeNum, pModeInfo, &ColorDepth);

                usModeWidth = pModeInfo->H_Size;
                usModeHeight = pModeInfo->V_Size;
            }
            
            SetPitch(DISP1, Pitch);

            SetColorDepth(DISP1, ColorDepth);

            
            if(!(pCBIOSExtension->pCBiosArguments->BX & BIT15))
            {
                ClearFrameBuffer(DISP1, pCBIOSExtension->pVideoVirtualAddress, usModeWidth, usModeHeight, ColorDepth);
            }
            
            SetFIFO(DISP1);

            ConfigDigitalPort(DISP1);
            
            TurnOnDigitalPort(DISP1);

            
            
            TurnOnTxEncReg(DISP1,ucDevicePort);

            TurnOffDVPMask(ucDevicePort);
            
            SequencerOn(DISP1);

            VBEReturnStatus = VBEFunctionCallSuccessful;
        }
    }
    
    SetVBERerurnStatus(VBEReturnStatus, pCBIOSExtension->pCBiosArguments);
    return TRUE;
}

RDC_EXPORT CBStatus VBE_SetPitch(CBIOS_Extension *CBIOSExtension)
{
    WORD    VBEReturnStatus = VBEFunctionCallFail;
    WORD    wPitch;

    
    if(CBIOSExtension->pCBiosArguments->BL == 0x02)
    {
        wPitch = CBIOSExtension->pCBiosArguments->CX;
        SetPitch(DISP1, wPitch);
        VBEReturnStatus = VBEFunctionCallSuccessful;
    }
    
    SetVBERerurnStatus(VBEReturnStatus, CBIOSExtension->pCBiosArguments);
    return TRUE;
}












RDC_EXPORT CBStatus VBE_AccessEDID(CBIOS_Extension *CBIOSExtension)
{
    BYTE bConnectStatus = FALSE, bValue = 0;
    BYTE bI2C_PORT;
    BYTE *bpEDIDBuffer,*bpEDIDOriBuffer;
    DWORD dwEDIDBufferSize, dwCheckSum = 0, i = 0;

    bpEDIDOriBuffer = (BYTE*)(CBIOSExtension->pCBiosArguments->Ebx);
    bpEDIDBuffer = bpEDIDOriBuffer;
    dwEDIDBufferSize = CBIOSExtension->pCBiosArguments->Ecx;
    bI2C_PORT = (BYTE)(CBIOSExtension->pCBiosArguments->Edx);

    
    CBIOSReadI2C(bI2C_PORT, MonitorEDID,(BYTE)0x12,&bValue);

    if(bValue > 0x1)
    {
        for(i = 0;i < dwEDIDBufferSize;i++)
        {    
            CBIOSReadI2C(bI2C_PORT, MonitorEDID,(BYTE)i,&bValue);
            dwCheckSum += bValue;
            *(bpEDIDBuffer++) = bValue;
        }
    }
    else
    {    
        
        for(i = 0;i < 128;i++)
        {    
            CBIOSReadI2C(bI2C_PORT, MonitorEDID,(BYTE)i,&bValue);
            dwCheckSum += bValue;
            *(bpEDIDBuffer++) = bValue;
        }

        for(i = 128;i < dwEDIDBufferSize;i++)
        {    
            CBIOSReadI2C(bI2C_PORT, MonitorEDID,(BYTE)i,&bValue);
            *(bpEDIDBuffer++) = bValue;
        }

    }
    
    
    if((dwCheckSum & 0xFF) == 0x0)
    {
        bConnectStatus = TRUE;
    }
    else
    {
        memset((void*)(bpEDIDOriBuffer), 0, sizeof(BYTE) * dwEDIDBufferSize);
    }
       
    return bConnectStatus;
}

RDC_EXPORT CBStatus CInt10(CBIOS_Extension *pCBIOSExtension)
{
    CBStatus CInt10_Status = FALSE;

    
    pRelated_IOAddress = pCBIOSExtension->pjIOAddress;
    
    switch(pCBIOSExtension->pCBiosArguments->AX)
    {
        case VBEFunction02:            
            CInt10_Status = VBE_SetMode(pCBIOSExtension);
            break;
        case VBEFunction06:            
            CInt10_Status = VBE_SetPitch(pCBIOSExtension);
            break;
        case VBEFunction15:            
            CInt10_Status = VBE_AccessEDID(pCBIOSExtension);
            break;
        case OEMFunction:              
            switch(pCBIOSExtension->pCBiosArguments->BX)
            {
                case QueryBiosInfo:             
                    CInt10_Status = OEM_QueryBiosInfo(pCBIOSExtension);
                    break;
                case QueryBiosCaps:             
                    CInt10_Status = OEM_QueryBiosCaps(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryExternalDeviceInfo:   
                    CInt10_Status = OEM_QueryExternalDeviceInfo(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryDisplayPathInfo:      
                    CInt10_Status = OEM_QueryDisplayPathInfo(pCBIOSExtension);
                    break;
                case QueryDeviceConnectStatus:  
                    CInt10_Status = OEM_QueryDeviceConnectStatus(pCBIOSExtension);
                    break;
                case QuerySupportedMode:        
                    CInt10_Status = OEM_QuerySupportedMode(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryLCDPanelSizeMode:     
                    CInt10_Status = OEM_QueryLCDPanelSizeMode(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryLCD2PanelSizeMode:    
                    CInt10_Status = OEM_QueryLCD2PanelSizeMode(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryTVConfiguration:      
                    CInt10_Status = OEM_QueryTVConfiguration(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryHDMISupportMode:      
                    CInt10_Status = OEM_QueryHDMISupportedMode(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetActiveDisplayDevice:    
                    CInt10_Status = OEM_SetActiveDisplayDevice(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetVESAModeForDisplay2:    
                    CInt10_Status = OEM_SetVESAModeForDisplay2(pCBIOSExtension);
                    break;
                case SetDevicePowerState:       
                    CInt10_Status = OEM_SetDevicePowerState(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetDisplay2Pitch:          
                    CInt10_Status = OEM_SetDisplay2Pitch(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetDisplay1RefreshRate:    
                case SetDisplay2RefreshRate:    
                    CInt10_Status = OEM_SetRefreshRate(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetTVConfiguration:        
                    CInt10_Status = OEM_SetSetTVConfiguration(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetTVFunction:             
                    CInt10_Status = OEM_SetSetTVFunction(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetHDMIType:               
                    CInt10_Status = OEM_SetHDMIType(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetHDMIOutputSignal:       
                    CInt10_Status = OEM_SetHDMIOutputSignal(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetVideoPOST:              
                    CInt10_Status = OEM_VideoPOST(pCBIOSExtension);
                    break;
                case SetEDIDInModeTable:        
                    CInt10_Status = OEM_VSetEDIDInModeTable(pCBIOSExtension);
                    break;
                default:
                    pCBIOSExtension->pCBiosArguments->AX = 0x014F;
                    break;
            }
            break;
    }
    return CInt10_Status;
} 

