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
 

#include <string.h>

#ifndef XFree86Module
#include "compiler.h"
#else
#include "xf86_ansic.h"
#endif

#include "typedef.h"
#include "CInt10.h"
#include "CInt10FunProto.h"
#include "CInt10Tbl.h"

#define I2C_ERROR    0
#define I2C_OK       1
#define I2CWriteCMD  0
#define I2CReadCMD   1


USHORT Relocate_IOAddress;

extern UCHAR bSetSAA7105Reg(UCHAR bDisplayPath, USHORT wModeNum, UCHAR ucTVType, UCHAR ucI2Cport, UCHAR ucDevAddress);
extern void SetSAA7105DACPower(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR ucTVConnectorType);
extern void SetSAA7105InitReg(UCHAR ucI2Cport, UCHAR ucDevAddress);
extern void SetSAA7105CCRSLevel(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR Level);
extern void GetSAA7105CCRSLevel(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR *Level);


__inline void OutPort(UCHAR Index,UCHAR Value)     
{              

    outb(Relocate_IOAddress+Index, Value);

    return;                          
}                                                                       

__inline UCHAR InPort(UCHAR Index)                        
{                                                                      
    UCHAR bInVal = 0x0;    

    bInVal = inb(Relocate_IOAddress+Index);
    
    return bInVal;                                                 
}

void I2CWriteClock(UCHAR I2CPort, UCHAR data)
{
    ULONG i;
    UCHAR ucMaskData=1, ucTmpData;
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

void I2CDelay(UCHAR I2CPort)
{
    UCHAR     i,jtemp;
    
    for (i=0;i<100;i++)
        jtemp = GetCRReg(I2CPort);
}

void I2CWriteData(UCHAR I2CPort, UCHAR data)
{

    ULONG i;
    UCHAR ucMaskData=0x4, ucTmpData;
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

void I2CStart(UCHAR I2CPort)
{
    I2CWriteClock(I2CPort, 0x00);               
    I2CDelay(I2CPort);
    I2CWriteData(I2CPort, 0x01);                
    I2CDelay(I2CPort);    
    I2CWriteClock(I2CPort, 0x01);               
    I2CDelay(I2CPort);    
    I2CWriteData(I2CPort, 0x00);                
    I2CDelay(I2CPort);    
    I2CWriteClock(I2CPort, 0x01);                  
    I2CDelay(I2CPort);                    
}

void SendI2CDataByte(UCHAR I2CPort, UCHAR Data)
{
    char i;

    for (i=7;i>=0;i--)
    {
        I2CWriteClock(I2CPort, 0x00);           
        I2CDelay(I2CPort);         
        if((Data>>i)&0x1)
            I2CWriteData(I2CPort, 0x1);           
        else
            I2CWriteData(I2CPort, 0x0);           
        
        I2CDelay(I2CPort);         
        
        I2CWriteClock(I2CPort, 0x01);           
        I2CDelay(I2CPort);                           
    }                
}

UCHAR CheckACK(UCHAR I2CPort)
{    
    I2CWriteClock(I2CPort, 0x00);               
    I2CDelay(I2CPort);    
    I2CWriteData(I2CPort, 0x01);                
    I2CDelay(I2CPort);    
    I2CWriteClock(I2CPort, 0x01);               
    I2CDelay(I2CPort);             
    return ((GetCRReg(I2CPort) & 0x20) ? 0:1);                
}

UCHAR ReceiveI2CDataByte(UCHAR I2CPort, UCHAR I2CSlave)
{
    UCHAR jData=0, jTempData;
    char i;
    ULONG j;

    for (i=7;i>=0;i--)
    {
        I2CWriteClock(I2CPort, 0x00);                
        I2CDelay(I2CPort);     
            
        I2CWriteData(I2CPort, 0x01);                 
        I2CDelay(I2CPort);         
        
        I2CWriteClock(I2CPort, 0x01);                
        I2CDelay(I2CPort);           
        
        for (j=0; j<0x1000; j++)
        {   
            if (((GetCRReg(I2CPort) & 0x10) >> 4))
                break;
        }    
                    
        jTempData =  (GetCRReg(I2CPort) & 0x20) >> 5;
        jData |= ((jTempData & 0x01) << i); 
        
        I2CWriteClock(I2CPort, 0x01);                
        I2CDelay(I2CPort);                           
    }    
    
    return (jData);                              
}

void SendNACK(UCHAR I2CPort)
{
    I2CWriteClock(I2CPort, 0x00);               
    I2CDelay(I2CPort);    
    I2CWriteData(I2CPort, 0x01);                
    I2CDelay(I2CPort);    
    I2CWriteClock(I2CPort, 0x01);               
    I2CDelay(I2CPort);    
}

void I2CStop(UCHAR I2CPort)
{
    I2CWriteClock(I2CPort, 0x00);               
    I2CDelay(I2CPort);    
    I2CWriteData(I2CPort, 0x00);                
    I2CDelay(I2CPort);    
    I2CWriteClock(I2CPort, 0x01);               
    I2CDelay(I2CPort);    
    I2CWriteData(I2CPort, 0x01);                
    I2CDelay(I2CPort);    
    I2CWriteClock(I2CPort, 0x01);                
    I2CDelay(I2CPort);                      
}


UCHAR ReadI2C(UCHAR I2CPort, UCHAR I2CSlave, UCHAR RegIdx, UCHAR* RegData)
{
    I2CStart(I2CPort);

    
    SendI2CDataByte(I2CPort, I2CSlave);
    
    if (!CheckACK(I2CPort))
    {
        return I2C_ERROR;
    }    

    
    SendI2CDataByte(I2CPort, RegIdx);

    if (!CheckACK(I2CPort))
    {
        return I2C_ERROR;
    }    
    
    I2CStart(I2CPort);
   
    
    SendI2CDataByte(I2CPort, I2CSlave+1);
    
    if (!CheckACK(I2CPort))
    {
        return I2C_ERROR;
    }    
    
    *RegData = ReceiveI2CDataByte(I2CPort, I2CSlave);

    SendNACK(I2CPort);

    I2CStop(I2CPort);

    return I2C_OK;       
}

UCHAR WriteI2C(UCHAR I2CPort, UCHAR I2CSlave, UCHAR RegIdx, UCHAR RegData)
{
    I2CStart(I2CPort);

    
    SendI2CDataByte(I2CPort, I2CSlave);
    if (!CheckACK(I2CPort))
    {
        return I2C_ERROR;
    }    

    
    SendI2CDataByte(I2CPort, RegIdx);
    if (!CheckACK(I2CPort))
    {
        return I2C_ERROR;
    }    

    
    SendI2CDataByte(I2CPort, RegData);
    if (!CheckACK(I2CPort))
    {
        return I2C_ERROR;
    }
    
    SendNACK(I2CPort);
         
    I2CStop(I2CPort);
    return I2C_OK;       
}


void SetVBERerurnStatus(USHORT VBEReturnStatus, CBIOS_ARGUMENTS *pCBiosArguments)
{
    pCBiosArguments->reg.x.AX = VBEReturnStatus;
}


void SetTimingRegs(UCHAR ucDisplayPath, MODE_INFO *pModeInfo, RRATE_TABLE *pRRateTable)
{
    USHORT usHBorder = 0, usVBorder = 0;
    USHORT usHtotal, usHDispEnd, usHBlankStart, usHBlankEnd, usHSyncStart, usHSyncEnd;
    USHORT usVtotal, usVDispEnd, usVBlankStart, usVBlankEnd, usVSyncStart, usVSyncEnd;
    ULONG  ulPixelClock;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter SetTimingRegs()==\n");

    if (pRRateTable->Attribute & HB)
    {
        usHBorder = 8;
    }

    if (pRRateTable->Attribute & VB)
    {
        usVBorder = 8;
    }

    usHtotal =      pModeInfo->H_Size + usHBorder*2 + pRRateTable->H_Blank_Time;
    usHDispEnd =    pModeInfo->H_Size;
    usHBlankStart = pModeInfo->H_Size + usHBorder;
    usHBlankEnd =   pModeInfo->H_Size + usHBorder + pRRateTable->H_Blank_Time;
    usHSyncStart =  pRRateTable->H_Sync_Start;
    usHSyncEnd =    pRRateTable->H_Sync_Start + pRRateTable->H_Sync_Time;

    usVtotal =      pModeInfo->V_Size + usVBorder*2 + pRRateTable->V_Blank_Time;
    usVDispEnd =    pModeInfo->V_Size;
    usVBlankStart = pModeInfo->V_Size + usVBorder;
    usVBlankEnd =   pModeInfo->V_Size + usVBorder + pRRateTable->V_Blank_Time;
    usVSyncStart =  pRRateTable->V_Sync_Start;
    usVSyncEnd =    pRRateTable->V_Sync_Start + pRRateTable->V_Sync_Time;

    ulPixelClock =  pRRateTable->Clock;
    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "H total = %d\n", usHtotal);
    SetHTotal(ucDisplayPath, usHtotal);
    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "H disp end = %d\n", usHDispEnd);
    SetHDisplayEnd(ucDisplayPath, usHDispEnd);

    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "H blank start = %d\n", usHBlankStart);
    SetHBlankingStart(ucDisplayPath, usHBlankStart);

    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "H blank end = %d\n", usHBlankEnd);
    SetHBlankingEnd(ucDisplayPath, usHBlankEnd);

    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "H sync start = %d\n", usHSyncStart);
    SetHSyncStart(ucDisplayPath, usHSyncStart);

    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "H sync end = %d\n", usHSyncEnd);
    SetHSyncEnd(ucDisplayPath, usHSyncEnd);

    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "V total = %d\n", usVtotal);
    SetVTotal(ucDisplayPath, usVtotal);
    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "V disp end = %d\n", usVDispEnd);
    SetVDisplayEnd(ucDisplayPath, usVDispEnd);

    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "V blank start = %d\n", usVBlankStart);
    SetVBlankingStart(ucDisplayPath, usVBlankStart);

    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "V blank end = %d\n", usVBlankEnd);
    SetVBlankingEnd(ucDisplayPath, usVBlankEnd);

    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "V sync start = %d\n", usVSyncStart);
    SetVSyncStart(ucDisplayPath, usVSyncStart);

    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "V sync end = %d\n", usVSyncEnd);
    SetVSyncEnd(ucDisplayPath, usVSyncEnd);
    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "Pixel clock = %d\n", ulPixelClock);
    SetPixelClock(ucDisplayPath, ulPixelClock);

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit SetTimingRegs()==\n");
}


void SetHTotal(UCHAR DisplayPath, USHORT Value)
{
    
    Value -= 40;
    
    
    Value += 7;

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


void SetHDisplayEnd(UCHAR DisplayPath, USHORT Value)
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


void SetHBlankingStart(UCHAR DisplayPath, USHORT Value)
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


void SetHBlankingEnd(UCHAR DisplayPath, USHORT Value)
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


void SetHSyncStart(UCHAR DisplayPath, USHORT Value)
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


void SetHSyncEnd(UCHAR DisplayPath, USHORT Value)
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


void SetVTotal(UCHAR DisplayPath, USHORT Value)
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


void SetVDisplayEnd(UCHAR DisplayPath, USHORT Value)
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


void SetVBlankingStart(UCHAR DisplayPath, USHORT Value)
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


void SetVBlankingEnd(UCHAR DisplayPath, USHORT Value)
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


void SetVSyncStart(UCHAR DisplayPath, USHORT Value)
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


void SetVSyncEnd(UCHAR DisplayPath, USHORT Value)
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


void SetPixelClock(UCHAR bDisplayPath, ULONG dwClock)
{
    PLL_Info PLLInfo;
    
    PLLInfo = ClockToPLLF9003A(dwClock);
    SetDPLL(bDisplayPath, PLLInfo);
}

void SetHSource(UCHAR bDisplayPath, USHORT wValue)
{
    wValue -= 1;
    
    if(bDisplayPath == DISP1)
    {
        WriteRegToHW(HSource1, wValue);
    }
    else
    {
        WriteRegToHW(HSource2, wValue);
    }
}


void SetHorDownScaleSrcSize(USHORT wValue)
{
    if (bLCDSUPPORT)
    {
        WriteRegToHW(HorDownScaleSrcSize1, wValue);
    }
}


void SetVerDownScaleSrcSize(USHORT wValue)
{
    if (bLCDSUPPORT)
    {
        WriteRegToHW(VerDownScaleSrcSize1, wValue);
    }
}


PLL_Info ClockToPLLF9003A(ULONG Clock)
{
    ULONG MSCount, NSCount, RSCount, FCKVCO, FCKOUT;    
    ULONG NearestClock = 0xFFFFFFFF;
    PLL_Info PLLInfo;
    
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
                        PLLInfo.NS = (UCHAR)NSCount;
                        PLLInfo.RS = (UCHAR)RSCount-1;
                    }
                }
            }
        }

    }

    return PLLInfo;

}


void SetDPLL(UCHAR DisplayPath, PLL_Info PLLInfo)
{
    UCHAR RetValue; 

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


void SetPolarity(UCHAR DevicePort, UCHAR Value)
{
    Value ^= BIT2+BIT1; 
    
    switch (DevicePort)
    {
        case CRT_PORT:
            OutPort(MISC_WRITE, ((Value<<5) & 0xC0) | (InPort(MISC_READ) & 0x3F));
            break;
        
        case DVP1:
        case DVP12:
            SetSRReg(0x20, Value>>1, BIT1+BIT0);
            break;

        case DVP2:
            SetSRReg(0x20, Value<<2, BIT4+BIT3);
            break;
    }
}


void SetFIFO(UCHAR DisplayPath)
{
    UCHAR *pucPCIDataStruct = (UCHAR*)PCIDataStruct;

    if (DisplayPath == DISP1)
    {
        if (*(USHORT*)(pucPCIDataStruct + OFF_DID) == 0x2010)
        {
            SetCRReg(0xA7, 0x5F, 0xFF);
            SetCRReg(0xA6, 0x3F, 0xFF);
        }
        else if (*(USHORT*)(pucPCIDataStruct + OFF_DID) == 0x2012)
        {
            SetCRReg(0xA7, 0x9F, 0xFF);
            SetCRReg(0xA6, 0x7F, 0xFF);
        }
        else
        {
            SetCRReg(0xA7, 0x3F, 0xFF);
            SetCRReg(0xA6, 0x3F, 0xFF);
        }
    }
    else if (DisplayPath == DISP2)
    {
        SetCRReg(0x35, 0x3F, 0xFF);
        SetCRReg(0x34, 0x3F, 0xFF);
    }
}


void SetPitch(UCHAR DisplayPath, USHORT Value)
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


USHORT GetPitch(UCHAR DisplayPath)
{
    USHORT wPitch;
    
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


USHORT GetVDisplayEnd(UCHAR DisplayPath)
{
    USHORT  wDisplayEnd = 0x0;
    
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


void SetColorDepth(UCHAR DisplayPath, UCHAR Value)
{
    UCHAR bSetBit = 0x0;

    switch(Value)
    {
        case 8:
            bSetBit = (UCHAR)BIT0;
            break;
        case 16:
            bSetBit = (UCHAR)BIT2;
            break;
        case 32:
            bSetBit = (UCHAR)BIT3;
            break;
        default:
            return;
    }

    if(DisplayPath == DISP1)
    {
        SetCRReg(0xA3, bSetBit, 0x0F);
    }
    else
    {
        
        if(Value == 8)
            return;
        
        SetCRReg(0x33, bSetBit, 0x0F);
    }
    
}


void ConfigDigitalPort(UCHAR bDisplayPath)
{
    PORT_CONFIG *pDevicePortConfig;
    UCHAR bDeviceIndex;
    UCHAR bRegValue;

    bDeviceIndex = Get_DEV_ID(bDisplayPath);

    if(bDisplayPath == DISP1)
    {
        bRegValue = 0x3;
    }
    else
    {
        bRegValue = 0x4;
    }

    if (GetDevicePortConfig(bDeviceIndex, &pDevicePortConfig))
    {
        switch(pDevicePortConfig->PortID)
        {
            case CRT_PORT:
                SetSRReg(0x1F, bRegValue, BIT2);
                break;
                
            case DVP1:
                Set12BitDVP();
                SetSRReg(0x1F, bRegValue, BIT0);
                break;
                
            case DVP2:
                Set12BitDVP();
                SetSRReg(0x1F, bRegValue, BIT1);
                break;
                
            case DVP12:
                Set24BitDVP();
                SetSRReg(0x1F, bRegValue, BIT0);
                break;
        }
    }
}


void LoadTiming(UCHAR bDisplayPath, USHORT wModeNum)
{
    UCHAR bDeviceIndex = Get_DEV_ID(bDisplayPath);

     

    switch(bDeviceIndex)
    {
        case CRT_ID:
        case CRT2_ID:
        case DVI_ID:
        case DVI2_ID:
            LoadVESATiming(bDisplayPath, wModeNum);
            break;

        case LCD_ID:
        case LCD2_ID:
            LoadLCDTiming(bDisplayPath, wModeNum);
        case TV_ID:
        case TV2_ID:
            LoadTVTiming(bDisplayPath, wModeNum, bDeviceIndex);
            TurnOnTVClock();
            break;
    }

}


void LoadVESATiming(UCHAR bDisplayPath, USHORT wModeNum)
{
    UCHAR bR_Rate_value = 0x0;
    MODE_INFO *pModeInfo = NULL;
    RRATE_TABLE *pRRateTable = NULL;
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter LoadVESATiming()==\n");
    bR_Rate_value = Get_RRATE_ID(bDisplayPath);
    
    if(GetModePointerFromVESATable(wModeNum, bR_Rate_value, &pModeInfo, &pRRateTable))
    {
        SetTimingRegs(bDisplayPath, pModeInfo, pRRateTable);
    }
    else
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "Mode not found!!\n");
        
    }
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit LoadVESATiming()==\n");
}

void LoadLCDTiming(UCHAR bDisplayPath, USHORT wModeNum)
{
    UCHAR bDeviceIndex = Get_DEV_ID(bDisplayPath);
    MODE_INFO *pPanelModeInfo, *pUserModeInfo;
    PANEL_TABLE *pPanelTable;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter LoadLCDTiming()==\n");

    if(GetModePointerFromLCDTable(bDeviceIndex,  &pPanelModeInfo, &pPanelTable))
    {
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "&pPanelTable->Timing = 0x%x\n", &pPanelTable->Timing);
        SetTimingRegs(bDisplayPath, pPanelModeInfo, &pPanelTable->Timing);
        Get_MODE_INFO(wModeNum, &pUserModeInfo);
        SetScalingFactor(bDisplayPath, pUserModeInfo, pPanelModeInfo);
    }
    else
    {
        
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit LoadLCDTiming()==\n");
}


void LoadTVTiming(UCHAR bDisplayPath, USHORT wModeNum, UCHAR bDeviceIndex)
{
    UCHAR ucTVType;
    PORT_CONFIG *pDevicePortConfig;    
    ucTVType = Get_TV_Type();
    if(GetDevicePortConfig(bDeviceIndex, &pDevicePortConfig))
    {
        if(pDevicePortConfig->TX_Enc_ID == ENC_SAA7105)
        {
            if(bSetSAA7105Reg(bDisplayPath, wModeNum, ucTVType, pDevicePortConfig->TX_I2C_Port, pDevicePortConfig->TX_I2C_Addr))
            {
                xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==SAA7105 Load TV Timing Success()==\n");
                if(pDevicePortConfig->PortID & DVP1) 
                    SetDVP1DPA(0x4);
                else
                    SetDVP2DPA(0x4);
            }
            else
                xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==SAA7105 Load TV Timing Fail()==\n");
        }else
            xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Can not Load TV Timing ==\n");
    }else
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Can not Load TV Timing ==\n");
}


void SetScalingFactor(UCHAR bDisplayPath, MODE_INFO *pUserModeInfo, MODE_INFO *pPanelModeInfo)
{
    ULONG dwScalingFactor;

    USHORT usUserModeHSize, usUserModeVSize;
    USHORT usPanelModeHSize, usPanelModeVSize;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter SetScalingFactor()==\n");
    usUserModeHSize = pUserModeInfo->H_Size;

    usUserModeVSize = pUserModeInfo->V_Size;

    usPanelModeHSize = pPanelModeInfo->H_Size;

    usPanelModeVSize = pPanelModeInfo->V_Size;


    TurnOffHorScaler(bDisplayPath);

    TurnOffVerScaler(bDisplayPath);

    SetHSource(bDisplayPath, usUserModeHSize);

    if (bDS_SUPPORT)
    {
        SetHorDownScaleSrcSize(usUserModeHSize);
        if (usPanelModeHSize < usUserModeHSize)
        {
            dwScalingFactor = ((ULONG)usUserModeHSize << 8) / usPanelModeHSize;
            SetHorDownScalingFactor(DISP1, (USHORT)dwScalingFactor);
            TurnOnHorDownScaler(DISP1);
        }
    }

    if (usPanelModeHSize > usUserModeHSize)
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "Enable H scaler\n");
        dwScalingFactor = ((ULONG)usUserModeHSize << 12) / usPanelModeHSize;
        SetHorScalingFactor(bDisplayPath, dwScalingFactor);
        TurnOnHorScaler(bDisplayPath);
        TurnOnScaler(bDisplayPath);
    }

    if (bDS_SUPPORT)
    {
        SetVerDownScaleSrcSize(usUserModeVSize);
        if (usPanelModeVSize < usUserModeVSize)
        {
            dwScalingFactor = ((ULONG)usUserModeVSize << 8) / usPanelModeVSize;
            SetVerDownScalingFactor(DISP1, (USHORT)dwScalingFactor);
            TurnOnVerDownScaler(DISP1);
        }
    }

    if (usPanelModeVSize > usUserModeVSize)
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "Enable V scaler\n");
        dwScalingFactor = ((ULONG)usUserModeVSize << 11) / usPanelModeVSize;
        SetVerScalingFactor(bDisplayPath, dwScalingFactor);
        TurnOnVerScaler(bDisplayPath);
        TurnOnScaler(bDisplayPath);
    }
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit SetScalingFactor()==\n");
}

void SetHorScalingFactor(UCHAR bDisplayPath, USHORT wValue)
{
    if(bDisplayPath == DISP1)
    {
        WriteRegToHW(HScalingFactor1, wValue);
    }
    else
    {
        WriteRegToHW(HScalingFactor2, wValue);
    }
}

void SetVerScalingFactor(UCHAR bDisplayPath, USHORT wValue)
{
    if(bDisplayPath == DISP1)
    {
        WriteRegToHW(VScalingFactor1, wValue);
    }
    else
    {
        WriteRegToHW(VScalingFactor2, wValue);
    }
}


void SetHorDownScalingFactor(UCHAR bDisplayPath, USHORT wValue)
{
    if (bLCDSUPPORT)
    {
        if (bDisplayPath == DISP1)
        {
            WriteRegToHW(HorDownScaleFactor1, wValue);
        }
    }
}


SetVerDownScalingFactor(UCHAR bDisplayPath, USHORT wValue)
{
    if (bLCDSUPPORT)
    {
        if (bDisplayPath == DISP1)
        {
            WriteRegToHW(VerDownScaleFactor1, wValue);
        }
    }
}

CI_STATUS isLCDFitMode(UCHAR bDeviceIndex, USHORT wModeNum)
{
    MODE_INFO *pModeInfo;
    
    if (Get_MODE_INFO_From_LCD_Table(bDeviceIndex, &pModeInfo))
    {
        if ((wModeNum == pModeInfo->Mode_ID_8bpp) ||
            (wModeNum == pModeInfo->Mode_ID_16bpp) ||
            (wModeNum == pModeInfo->Mode_ID_32bpp))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

CI_STATUS GetModePointerFromVESATable(USHORT wModeNum, UCHAR ucRRIndex, MODE_INFO **ppModeInfo, RRATE_TABLE **ppRRateTable)
{
    int iRRateTableIndex;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter GetModePointerFromVESATable()==\n");
 
    
    if(Get_MODE_INFO_From_VESA_Table(wModeNum, ppModeInfo))
    {
        
        *ppRRateTable = (RRATE_TABLE*)((int)(*ppModeInfo) + sizeof(MODE_INFO));

        for(iRRateTableIndex = 0; iRRateTableIndex < (*ppModeInfo)->RRTableCount; iRRateTableIndex++, (*ppRRateTable)++)
        {
            xf86DrvMsgVerb(0, X_INFO, InternalLevel, "*ppRRateTable = 0x%x\n", *ppRRateTable);

            if(((*ppRRateTable)->RRate_ID == ucRRIndex) && (!((*ppRRateTable)->Attribute & DISABLE)))
            {
                xf86DrvMsgVerb(0, X_INFO, InternalLevel, "*ppRRateTable = 0x%x\n", *ppRRateTable);
                xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit1 GetModePointerFromVESATable()== return success\n");
                return true;
            }
        }
    }

    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit2 GetModePointerFromVESATable()== return fail!!\n");
    return false;
}


CI_STATUS GetModePointerFromLCDTable(UCHAR bDeviceIndex, MODE_INFO **ppModeInfo, PANEL_TABLE **ppPanelTable)
{
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter GetModePointerFromLCDTable()==\n");
    if (Get_MODE_INFO_From_LCD_Table(bDeviceIndex, ppModeInfo))
    {
        *ppPanelTable = (PANEL_TABLE*)((int)(*ppModeInfo) + sizeof(MODE_INFO));
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "*ppPanelTable = 0x%x\n", *ppPanelTable);
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 GetModePointerFromLCDTable()== return success\n");
        return true;
    }
    else
    {
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 GetModePointerFromLCDTable()== return fail\n");
        return false;
    }
}

CI_STATUS Get_MODE_INFO(USHORT wModeNum, MODE_INFO **ppModeInfo)
{
    if (Get_MODE_INFO_From_VESA_Table(wModeNum, ppModeInfo))
    {
        return true;
    }
    else if(isLCDFitMode(LCD_ID, wModeNum))
    {
        Get_MODE_INFO_From_LCD_Table(LCD_ID, ppModeInfo);
        return true;
    }
    else if(isLCDFitMode(LCD2_ID, wModeNum))
    {
        Get_MODE_INFO_From_LCD_Table(LCD2_ID, ppModeInfo);
        return true;
    }
    else
    {
        return false;
    }
}

CI_STATUS Get_MODE_INFO_From_LCD_Table(UCHAR bDeviceIndex, MODE_INFO **ppModeInfo)
{
    int i;
    UCHAR bLCDTableIndex;
     
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter Get_MODE_INFO_From_LCD_Table()==\n");
    
    if (bLCDSUPPORT)
    {
        *ppModeInfo = pLCDTable;
        
        if (bDeviceIndex == LCD_ID)
        {
            bLCDTableIndex = Get_LCD_TABLE_INDEX();
        }
        else if (bDeviceIndex == LCD2_ID)
        {
            bLCDTableIndex = Get_LCD2_TABLE_INDEX();
        }
        else
        {
            xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 Get_MODE_INFO_From_LCD_Table()== return fail!!\n");
            return false;
        }


        if (bLCDTableIndex == 0)
        {
            xf86DrvMsgVerb(0, X_INFO, InfoLevel, "LCD Index = 0\n");
            xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit2 Get_MODE_INFO_From_LCD_Table()== return fail!!\n");
            return false;
        }

        while ((*ppModeInfo)->H_Size != 0xFFFF)
        {
            xf86DrvMsgVerb(0, X_INFO, InternalLevel, "(*ppModeInfo)->H_Size = %d\n", (*ppModeInfo)->H_Size);

            if (bLCDTableIndex == 1)
            {
                xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit3 Get_MODE_INFO_From_LCD_Table()== return success\n");
                return true;
            }
            (*ppModeInfo) = (MODE_INFO*)((int)*ppModeInfo + sizeof(MODE_INFO) + sizeof(PANEL_TABLE));
            bLCDTableIndex--;
        }
    }
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit4 Get_MODE_INFO_From_LCD_Table()== return fail!!\n");

    return false;
    
}

#if 0
CI_STATUS Get_MODE_INFO_From_VESA_Table(USHORT wModeNum, MODE_INFO **ppModeInfo)
{
    int i;
    VESA_TABLE *pVESATable = VESATable;
    
    for( i = 0; i < (sizeof(VESATable)/sizeof(VESA_TABLE)); i++, pVESATable++)
    {
        if((pVESATable->ModeInfo.Mode_ID_8bpp == wModeNum) || (pVESATable->ModeInfo.Mode_ID_16bpp == wModeNum)|| (pVESATable->ModeInfo.Mode_ID_32bpp == wModeNum))
        {
            *ppModeInfo = &(pVESATable->ModeInfo);
            return true;
        }
    }
    
    return false;
}
#else
CI_STATUS Get_MODE_INFO_From_VESA_Table(USHORT wModeNum, MODE_INFO **ppModeInfo)
{
    UCHAR ucColorDepth;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter Get_MODE_INFO_From_VESA_Table()==\n");

    *ppModeInfo = pVESATable;

    while((*ppModeInfo)->H_Size != 0xFFFF)
    {
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "*ppModeInfo = 0x%x\n", *ppModeInfo);
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "(*ppModeInfo)->H_Size = %d\n", (*ppModeInfo)->H_Size);

        if (GetModeColorDepth(wModeNum, *ppModeInfo, &ucColorDepth))
        {
            xf86DrvMsgVerb(0, X_INFO, InfoLevel, "*ppModeInfo = 0x%x\n", *ppModeInfo);
            xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit1 Get_MODE_INFO_From_VESA_Table()== return success\n");
            return true;
        }

        *ppModeInfo = (MODE_INFO*)((*ppModeInfo)->RRTableCount * sizeof(RRATE_TABLE) + sizeof(MODE_INFO) + (void*)(*ppModeInfo));
    }

    *ppModeInfo = (MODE_INFO*)(&CInt10VESATable);

    while((*ppModeInfo)->H_Size != 0xFFFF)
    {
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "*ppModeInfo = 0x%x\n", *ppModeInfo);
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "(*ppModeInfo)->H_Size = %d\n", (*ppModeInfo)->H_Size);

        if (GetModeColorDepth(wModeNum, *ppModeInfo, &ucColorDepth))
        {
            xf86DrvMsgVerb(0, X_INFO, InfoLevel, "*ppModeInfo = 0x%x\n", *ppModeInfo);
            xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit1 Get_MODE_INFO_From_VESA_Table()== return success\n");
            return true;
        }

        *ppModeInfo = (MODE_INFO*)((*ppModeInfo)->RRTableCount * sizeof(RRATE_TABLE) + sizeof(MODE_INFO) + (void*)(*ppModeInfo));
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit2 Get_MODE_INFO_From_VESA_Table()== return fail!!\n");
    return false;
}
#endif

CI_STATUS GetModeColorDepth(USHORT wModeNum, MODE_INFO *pModeInfo, UCHAR *pucColorDepth)
{
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter GetModeColorDepth()==\n");
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "pModeInfo->Mode_ID_8bpp = 0x%x\n", pModeInfo->Mode_ID_8bpp);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "pModeInfo->Mode_ID_16bpp = 0x%x\n", pModeInfo->Mode_ID_16bpp);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "pModeInfo->Mode_ID_32bpp = 0x%x\n", pModeInfo->Mode_ID_32bpp);
#endif
    if(pModeInfo->Mode_ID_8bpp == wModeNum)
    {
        *pucColorDepth = 8;
#if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 GetModeColorDepth()== *pucColorDepth = %d\n", *pucColorDepth);
#endif
        return true;
    }
    else if(pModeInfo->Mode_ID_16bpp == wModeNum)
    {
        *pucColorDepth = 16;
#if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit2 GetModeColorDepth()== *pucColorDepth = %d\n", *pucColorDepth);
#endif
        return true;
    }
    else if(pModeInfo->Mode_ID_32bpp == wModeNum)
    {
        *pucColorDepth = 32;
#if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit3 GetModeColorDepth()== *pucColorDepth = %d\n", *pucColorDepth);
#endif
        return true;
    }
    else
    {
        *pucColorDepth = 0;
#if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit4 GetModeColorDepth()== *pucColorDepth = %d\n", *pucColorDepth);
#endif
        return false;
    }    
}

CI_STATUS GetModePitch(USHORT ModeNum, USHORT *pPitch)
{
    MODE_INFO *pModeInfo = NULL;
    UCHAR ucColorDepth = 0;
    
    
    if(!Get_MODE_INFO(ModeNum, &pModeInfo))
    {
        return false;
    }
    else
    {
        if(!GetModeColorDepth(ModeNum, pModeInfo, &ucColorDepth))
        {
            return false;
        }
        else
        {
            ucColorDepth = ucColorDepth >> 4;
            *pPitch = ((pModeInfo->H_Size << ucColorDepth)+0x7)&0xFFF8;
        }
    }

    return true;
}

USHORT ReadRegFromHW(REG_OP *pRegOp)
{
    USHORT wValue = 0x0;
    UCHAR    btemp = 0x0, bMasktemp = 0x0;

    while((pRegOp->RegGroup)!= NR)
    {        
        if(pRegOp->RegGroup == CR)
        {
            
            OutPort(COLOR_CRTC_INDEX,(pRegOp->RegIndex));
            btemp = (UCHAR)InPort(COLOR_CRTC_DATA);
        }
        else
        {
            
            OutPort(SEQ_INDEX,(pRegOp->RegIndex));
            btemp = (UCHAR)InPort(SEQ_DATA);
        }
        
        bMasktemp = (pRegOp->RegMask);

        
        btemp &= bMasktemp;

        
        while(!(bMasktemp & BIT0))
        {
            bMasktemp = bMasktemp >> 1;
            btemp = btemp >> 1;
        }

        
        wValue |= (((USHORT)btemp) << (pRegOp->RegShiftBit));
        
        
        pRegOp++;

    }

    return wValue;
}


void WriteRegToHW(REG_OP *pRegOp, USHORT value)
{
    UCHAR btemp, btemp1;
    UCHAR bCount;
    UCHAR bMasktemp;

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
            btemp1 = (UCHAR)InPort(COLOR_CRTC_DATA);
            btemp1 &= ~(pRegOp->RegMask);
            btemp1 |= btemp;
            OutPort(COLOR_CRTC_DATA,btemp1);
        }
        else
        {
            
            OutPort(SEQ_INDEX,(pRegOp->RegIndex));
            btemp1 = (UCHAR)InPort(SEQ_DATA);
            btemp1 &= ~(pRegOp->RegMask);
            btemp1 |= btemp;
            OutPort(SEQ_DATA,btemp1);            
        }

        pRegOp++;
        
    }
 
}


void UnLockCR0ToCR7()
{
    
    SetCRReg(0x11, 0x00, BIT7);
}


void LockCR0ToCR7()
{
    
    SetCRReg(0x11, 0x80, BIT7);
}


CI_STATUS CheckForModeAvailable(USHORT ModeNum)
{
    MODE_INFO *pModeInfo = NULL;
    return Get_MODE_INFO_From_VESA_Table(ModeNum, &pModeInfo);
}

CI_STATUS CheckForNewDeviceAvailable(UCHAR bDeviceIndex)
{
    PORT_CONFIG *pDevicePortConfig;
    
    return GetDevicePortConfig(bDeviceIndex, &pDevicePortConfig);
}

void Display1TurnOnTX()
{

}

void Display1TurnOffTX()
{

}

void Display2TurnOnTX()
{

}

void Display2TurnOffTX()
{

}


void TurnOnDigitalPort(UCHAR bDeviceIndex)
{
    PORT_CONFIG *pDevicePortConfig;

    if (GetDevicePortConfig(bDeviceIndex, &pDevicePortConfig))
    {
        switch(pDevicePortConfig->PortID)
        {
            case CRT_PORT:
                TurnOnDAC();
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
        }
    }
}       

void TurnOffDigitalPort(UCHAR bDeviceIndex)
{
    PORT_CONFIG *pDevicePortConfig;
    
    if (GetDevicePortConfig(bDeviceIndex, &pDevicePortConfig))
    
    switch(pDevicePortConfig->PortID)
    {
        case CRT_PORT:
            TurnOffCRTPad();
//            TurnOffDAC();
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
    }
}       


UCHAR GetPortConnectPath(UCHAR PortType)
{
    UCHAR SR1F, PortMask;

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


USHORT TransDevIDtoBit(UCHAR DeviceIndex)
{
    return (1 << (DeviceIndex - 1));
}

void TurnOnCRTPad()
{
    SetCRReg(0xA8, 0x00, BIT7);
}

void TurnOffCRTPad()
{
    SetCRReg(0xA8, 0x80, BIT7);
}

void TurnOnDVP1Pad()
{
    SetCRReg(0xA3, 0x80, BIT7);
}

void TurnOffDVP1Pad()
{
    SetCRReg(0xA3, 0x00, BIT7);
}

void TurnOnDVP2Pad()
{
    SetCRReg(0xA3, 0x40, BIT6);
}

void TurnOffDVP2Pad()
{
    SetCRReg(0xA3, 0x00, BIT6);
}

void TurnOnDVP12Pad()
{
    TurnOnDVP1Pad();
    TurnOnDVP2Pad();
}

void TurnOffDVP12Pad()
{
    TurnOffDVP1Pad();
    TurnOffDVP2Pad();
}

void TurnOnScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x58, BIT0, BIT0);
    else
        SetSRReg(0x50, BIT0, BIT0);
}

void TurnOffScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x58, 0, BIT0);
    else
        SetSRReg(0x50, 0, BIT0);
}

void TurnOnHorScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x58, BIT2, BIT2);
    else
        SetSRReg(0x50, BIT2, BIT2);
}

void TurnOffHorScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x58, 0, BIT2);
    else
        SetSRReg(0x50, 0, BIT2);
}

void TurnOnVerScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x58, BIT1, BIT1);
    else
        SetSRReg(0x50, BIT1, BIT1);
}

void TurnOffVerScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x58, 0, BIT1);
    else
        SetSRReg(0x50, 0, BIT1);
}


void TurnOnHorDownScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x74, BIT7, BIT7);
}

void TurnOffHorDownScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x74, 0, BIT7);
}

void TurnOnVerDownScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x70, BIT7, BIT7);
}

void TurnOffVerDownScaler(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
        SetSRReg(0x70, 0, BIT7);
}

void Set12BitDVP()
{
    SetSRReg(0x1E, 0x00, BIT3);
}

void Set24BitDVP()
{
    SetSRReg(0x1E, 0x08, BIT3);
}

void SetDVP1DPA(UCHAR ucDPA)
{
    UCHAR ucTmp;
    ucTmp = ucDPA&0x7;
    SetSRReg(0x21, 0x07, ucTmp);
}

void SetDVP2DPA(UCHAR ucDPA)
{
    UCHAR ucTmp;
    ucTmp = (ucDPA&0x7)<<3;
    SetSRReg(0x21, 0x38, ucTmp);
}

void TurnOnPowerSequenceByPASS()
{
    SetSRReg(0x32, 0x01, BIT0);
}

void TurnOffPowerSequenceByPASS()
{
    SetSRReg(0x32, 0x00, BIT0);
}

void TurnOnDAC()
{
    SetCRReg(0xDF, 0x00, BIT2);
}
void TurnOffDAC()
{
    SetCRReg(0xDF, 0x04, BIT2);
}

void TurnOnTVClock()
{
    UCHAR ucTVConnectorType;
    PORT_CONFIG *pDevicePortConfig;
    ucTVConnectorType = Get_TV_ConnectorType();
    if(GetDevicePortConfig(TV_ID, &pDevicePortConfig))
    {
        if(pDevicePortConfig->TX_Enc_ID == ENC_SAA7105)
        {
            SetSAA7105DACPower(pDevicePortConfig->TX_I2C_Port, pDevicePortConfig->TX_I2C_Addr,ucTVConnectorType);
        }
    }
    SetCRReg(0xD0, 0x40, BIT6);
    TurnOnPowerSequenceByPASS();
}

void TurnOffTVClock()
{
    PORT_CONFIG *pDevicePortConfig;
    TurnOffPowerSequenceByPASS();
    if(GetDevicePortConfig(TV_ID, &pDevicePortConfig))
    {
        if(pDevicePortConfig->TX_Enc_ID == ENC_SAA7105)
        {
            SetSAA7105DACPower(pDevicePortConfig->TX_I2C_Port, pDevicePortConfig->TX_I2C_Addr,0);
        }
    }
    SetCRReg(0xD0, 0x00, BIT6);
}

#if 0
void Display1HWResetOn()
{
    SetCRReg(0x17, 0x00, BIT7);
}
void Display1HWResetOff()
{
    SetCRReg(0x17, 0x80, BIT7);
}

void Display2HWResetOn()
{
    SetCRReg(0x33, 0x00, BIT4);
}
void Display2HWResetOn()
{
    SetCRReg(0x33, 0x10, BIT4);
}
#endif


void SerialLoadTable(UCHAR **ppucTablePointer, UCHAR ucI2Cport, UCHAR ucI2CAddr)
{
    UCHAR ucRegGroup;

    while (**ppucTablePointer != 0xFF)
    {
        ucRegGroup = **ppucTablePointer;
        (*ppucTablePointer)++;
        if (ucRegGroup & BITS)
        {
            SerialLoadRegBits(ppucTablePointer, ucRegGroup, 0, 0);
        }
        else
        {
            SerialLoadReg(ppucTablePointer, ucRegGroup, 0, 0);
        }
    }
    (*ppucTablePointer)++;
}


void SerialLoadRegBits(UCHAR **ppucTablePointer, UCHAR ucRegGroup, UCHAR ucI2Cport, UCHAR ucI2CAddr)
{
    UCHAR ucRegIndex, ucRegValue, ucMask;
    ucRegGroup &= 0x7F;

    while (**ppucTablePointer != 0xFF)
    {
        ucRegIndex = **ppucTablePointer;
        (*ppucTablePointer)++;

        ucRegValue = **ppucTablePointer;
        (*ppucTablePointer)++;

        ucMask = **ppucTablePointer;
        (*ppucTablePointer)++;

        switch(ucRegGroup)
        {
            case SR:
                SetSRReg(ucRegIndex, ucRegValue, ucMask);
                break;

            case CR:
                SetCRReg(ucRegIndex, ucRegValue, ucMask);
                break;
                
            case I2C:
                break;
        }
    }
    (*ppucTablePointer)++;
}


void SerialLoadReg(UCHAR **ppucTablePointer, UCHAR ucRegGroup, UCHAR ucI2Cport, UCHAR ucI2CAddr)
{
    UCHAR ucRegIndex, ucRegValue;

    while (**ppucTablePointer != 0xFF)
    {
        ucRegIndex = **ppucTablePointer;
        (*ppucTablePointer)++;

        ucRegValue = **ppucTablePointer;
        (*ppucTablePointer)++;

        switch(ucRegGroup)
        {
            case SR:
                SetSRReg(ucRegIndex, ucRegValue, 0xFF);
                break;

            case CR:
                SetCRReg(ucRegIndex, ucRegValue, 0xFF);
                break;
                
            case GR:
                SetGRReg(ucRegIndex, ucRegValue, 0xFF);
                break;

            case I2C:
                break;
        }
    }
    (*ppucTablePointer)++;
}


void LoadDisplay1VESAModeInitRegs()
{
    UCHAR *pucDisplay1VESAModeInitRegs = (UCHAR*)Display1VESAModeInitRegs;
    
    OutPort(MISC_WRITE, 0x2F);

    UnLockCR0ToCR7();
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "&pucDisplay1VESAModeInitRegs = 0x%x\n", &pucDisplay1VESAModeInitRegs);
    SerialLoadTable(&pucDisplay1VESAModeInitRegs, 0, 0);

    ResetATTR();
    
    SetARReg(0x10, *pucDisplay1VESAModeInitRegs);
    pucDisplay1VESAModeInitRegs++;
    SetARReg(0x11, *pucDisplay1VESAModeInitRegs);
    pucDisplay1VESAModeInitRegs++;
    SetARReg(0x12, *pucDisplay1VESAModeInitRegs);
    pucDisplay1VESAModeInitRegs++;
    SetARReg(0x13, *pucDisplay1VESAModeInitRegs);
    pucDisplay1VESAModeInitRegs++;
    SetARReg(0x14, *pucDisplay1VESAModeInitRegs);
    
    EnableATTR();
}

void VESASetBIOSData(USHORT ModeNum)
{
    
}


void DisableDisplayPathAndDevice(UCHAR bDisplayPath)
{
    UCHAR bDeviceIndex = Get_DEV_ID(bDisplayPath);

    
    ControlPwrSeqOff(bDeviceIndex);

    

    
    TurnOffDigitalPort(bDeviceIndex);
    
    
    SequencerOff(bDisplayPath);
}

CI_STATUS GetDevicePortConfig(UCHAR bDeviceIndex, PORT_CONFIG **ppDevicePortConfig)
{
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter GetDevicePortConfig()==\n");

    *ppDevicePortConfig = pPortConfig;
    
    while(((*ppDevicePortConfig)->DevID != 0xFF) && ((*ppDevicePortConfig)->Attribute & Dev_SUPPORT))
    {
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "(*ppDevicePortConfig)->DevID = %x\n", (*ppDevicePortConfig)->DevID);
        
        if (bDeviceIndex == (*ppDevicePortConfig)->DevID)
        {
            xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit1 GetDevicePortConfig()== return success\n");
            return true;
        }
        (*ppDevicePortConfig)++;
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit1 GetDevicePortConfig()== return fail!!\n");
    return false;
}

void PowerSequenceOn()
{
    
    if (!(GetSRReg(0x11) & BIT0))
    {
        
        SetSRReg(0x32, BIT1, BIT1);
        
        
        SetSRReg(0x11, BIT0, BIT0);
        
        
        WaitPowerSequenceDone();
    }
}

void PowerSequenceOff()
{
    
    if (GetSRReg(0x11) & BIT0)
    {
        
        SetSRReg(0x11, 0, BIT0);
        
        
        WaitPowerSequenceDone();
        
        
        SetSRReg(0x32, 0, BIT1);
    }
}

void SequencerOn(UCHAR DisplayPath)
{
    if (DisplayPath == DISP1)
        SetSRReg(0x01, 0, BIT5);
    else
        SetCRReg(0x33, 0, BIT0);
}

void SequencerOff(UCHAR bDisplayPath)
{
    if (bDisplayPath == DISP1)
    {
        if (!(GetSRReg(0x01) & BIT5))
        {
            LongWait();
        }
        SetSRReg(0x01, BIT5, BIT5);
    }
    else
        SetCRReg(0x33, BIT0, BIT0);
}

void ControlPwrSeqOn(UCHAR bDeviceIndex)
{
    PORT_CONFIG *pDevicePortConfig;
    
    if ((TransDevIDtoBit(bDeviceIndex) & (B_LCD+B_LCD2)) && (GetDevicePortConfig(bDeviceIndex, &pDevicePortConfig)))
    {
        if (pDevicePortConfig->Attribute & TX_PS)
        {
            if (pDevicePortConfig->TX_Enc_ID == TX_VT1636)
            {

            }
        }
        else if ((pDevicePortConfig->PortID == DVP1) || (pDevicePortConfig->PortID == DVP12))
        {
            PowerSequenceOn();
        }
    }    
}

void ControlPwrSeqOff(UCHAR bDeviceIndex)
{
    PORT_CONFIG *pDevicePortConfig;
    
    if ((TransDevIDtoBit(bDeviceIndex) & (B_LCD+B_LCD2)) && (GetDevicePortConfig(bDeviceIndex, &pDevicePortConfig)))
    {
        if (pDevicePortConfig->Attribute & TX_PS)
        {
            if (pDevicePortConfig->TX_Enc_ID == TX_VT1636)
            {

            }
        }
        else if ((pDevicePortConfig->PortID == DVP1) || (pDevicePortConfig->PortID == DVP12))
        {
            PowerSequenceOff();
        }
    }    
}

void LongWait()
{
     while (GetIS1Reg() & BIT3);
     
     while (!(GetIS1Reg() & BIT3));
}

UCHAR Get_DEV_ID(UCHAR DisplayPath)
{
    return ((DisplayPath == DISP1) ? ReadScratch(IDX_IGA1_DEV_ID) : ReadScratch(IDX_IGA2_DEV_ID));
}


void Set_DEV_ID(UCHAR DeviceID, UCHAR DisplayPath)
{
    if (DisplayPath == DISP1)
        WriteScratch(IDX_IGA1_DEV_ID, DeviceID);
    else
        WriteScratch(IDX_IGA2_DEV_ID, DeviceID);
}


UCHAR Get_NEW_DEV_ID(UCHAR DisplayPath)
{
    return ((DisplayPath == DISP1) ? ReadScratch(IDX_NEW_IGA1_DEV_ID) : ReadScratch(IDX_NEW_IGA2_DEV_ID));
}

void Set_NEW_DEV_ID(UCHAR DeviceID, UCHAR DisplayPath)
{
    if (DisplayPath == DISP1)
        WriteScratch(IDX_NEW_IGA1_DEV_ID, DeviceID);
    else
        WriteScratch(IDX_NEW_IGA2_DEV_ID, DeviceID);
}

USHORT Get_LCD_H_SIZE()
{
    USHORT wValue;

    wValue = (USHORT)ReadScratch(IDX_LCD_H_SIZE_OVERFLOW);
    wValue <<= 8;
    wValue |= (USHORT)ReadScratch(IDX_LCD_H_SIZE);
    
    return wValue;
}

USHORT Get_LCD_V_SIZE()
{
    USHORT wValue;

    wValue = (USHORT)ReadScratch(IDX_LCD_V_SIZE_OVERFLOW);
    wValue <<= 8;
    wValue |= (USHORT)ReadScratch(IDX_LCD_V_SIZE);
    
    return wValue;
}

ULONG Get_LCD_SIZE()
{
    ULONG dwValue;

    dwValue = (ULONG)Get_LCD_V_SIZE();
    dwValue <<= 16;
    dwValue |= (ULONG)Get_LCD_H_SIZE();
    
    return dwValue;
}

USHORT Get_LCD2_H_SIZE()
{
    USHORT wValue;

    wValue = (USHORT)ReadScratch(IDX_LCD2_H_SIZE_OVERFLOW);
    wValue <<= 8;
    wValue |= (USHORT)ReadScratch(IDX_LCD2_H_SIZE);
    
    return wValue;
}

USHORT Get_LCD2_V_SIZE()
{
    USHORT wValue;

    wValue = (USHORT)ReadScratch(IDX_LCD2_V_SIZE_OVERFLOW);
    wValue <<= 8;
    wValue |= (USHORT)ReadScratch(IDX_LCD2_V_SIZE);
    
    return wValue;
}

ULONG Get_LCD2_SIZE()
{
    ULONG dwValue;

    dwValue = (ULONG)Get_LCD2_V_SIZE();
    dwValue <<= 16;
    dwValue |= (ULONG)Get_LCD2_H_SIZE();
    
    return dwValue;
}

UCHAR Get_RRATE_ID(UCHAR DisplayPath)
{
    return ((DisplayPath == DISP1) ? ReadScratch(IDX_IGA1_RRATE_ID) : ReadScratch(IDX_IGA2_RRATE_ID));
}

void Set_RRATE_ID(UCHAR RRateID, UCHAR DisplayPath)
{
    if (DisplayPath == DISP1)
        WriteScratch(IDX_IGA1_RRATE_ID, RRateID);
    else
        WriteScratch(IDX_IGA2_RRATE_ID, RRateID);
}

UCHAR Get_LCD_TABLE_INDEX(void)
{
    return ReadScratch(IDX_LCD1_TABLE_INDEX);
}

UCHAR Get_LCD2_TABLE_INDEX(void)
{
    return ReadScratch(IDX_LCD2_TABLE_INDEX);
}

void Set_LCD_TABLE_INDEX(UCHAR bLCDIndex)
{
    WriteScratch(IDX_LCD1_TABLE_INDEX, bLCDIndex);
}

UCHAR Get_TV_ConnectorType(void)
{
    return ReadScratch(IDX_TV1_CONNECTION_TYPE);
}

UCHAR Get_TV_Type(void)
{
    return ReadScratch(IDX_TV1_TYPE);
}

void Set_TV_Confiuration(UCHAR TVConfig)
{
    WriteScratch(IDX_TV1_TYPE, (TVConfig&0xF));
    WriteScratch(IDX_TV1_CONNECTION_TYPE, ((TVConfig>>4)&0x3));
}

USHORT Get_VESA_MODE(UCHAR DisplayPath)
{
    UCHAR VESAMode, VESAModeOver;

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
    
    return  ((USHORT)VESAModeOver) << 8 | (USHORT)VESAMode;
}

void Set_VESA_MODE(USHORT ModeNum, UCHAR DisplayPath)
{
    if (DisplayPath == DISP1)
    {
        WriteScratch(IDX_IGA1_VESA_MODE, (UCHAR)ModeNum);
        WriteScratch(IDX_IGA1_VESA_MODE_OVERFLOW, (UCHAR)(ModeNum >> 8));
    }
    else
    {
        WriteScratch(IDX_IGA2_VESA_MODE, (UCHAR)ModeNum);
        WriteScratch(IDX_IGA2_VESA_MODE_OVERFLOW, (UCHAR)(ModeNum >> 8));
    }
}

void ResetATTR()
{
    InPort(COLOR_INPUT_STATUS1_READ);
    InPort(MONO_INPUT_STATUS1_READ);
}

void EnableATTR()
{
    ResetATTR();
    OutPort(ATTR_DATA_WRITE, 0x20);
}


void SetCRReg(UCHAR bRegIndex, UCHAR bRegValue, UCHAR bMask)
{
    UCHAR btemp = 0x0;
    
    if(bMask != 0xFF)
    {
        OutPort(COLOR_CRTC_INDEX,bRegIndex);
        btemp = (UCHAR)InPort(COLOR_CRTC_DATA);
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


UCHAR GetCRReg(UCHAR bRegIndex)
{
    UCHAR btemp = 0x0;
    
    OutPort(COLOR_CRTC_INDEX,bRegIndex);
    btemp = (UCHAR)InPort(COLOR_CRTC_DATA);
        
    return btemp;
}


void SetSRReg(UCHAR bRegIndex, UCHAR bRegValue, UCHAR bMask)
{
    UCHAR btemp = 0x0;
    
    if(bMask != 0xFF)
    {
        OutPort(SEQ_INDEX,bRegIndex);
        btemp = (UCHAR)InPort(SEQ_DATA);
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


UCHAR GetSRReg(UCHAR bRegIndex)
{
    UCHAR btemp = 0x0;
    
    OutPort(SEQ_INDEX,bRegIndex);
    btemp = (UCHAR)InPort(SEQ_DATA);
        
    return btemp;
}

void SetARReg(UCHAR index,UCHAR value)
{
    OutPort(ATTR_DATA_WRITE,index);
    OutPort(ATTR_DATA_WRITE,value);
}

UCHAR GetARReg(UCHAR index)
{
    UCHAR bTmp;
    InPort(COLOR_INPUT_STATUS1_READ);
    OutPort(ATTR_DATA_WRITE,index);
    bTmp = (UCHAR)InPort(ATTR_DATA_READ);
    InPort(COLOR_INPUT_STATUS1_READ);
    OutPort(ATTR_DATA_WRITE,BIT5);
    return bTmp;
}


void SetGRReg(UCHAR bRegIndex, UCHAR bRegValue, UCHAR bMask)
{
    UCHAR btemp = 0x0;
    
    if(bMask != 0xFF)
    {
        OutPort(GRAPH_INDEX,bRegIndex);
        btemp = (UCHAR)InPort(GRAPH_DATA);
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

void SetMSReg(UCHAR bRegValue)
{
    OutPort(MISC_WRITE,bRegValue);
}

UCHAR GetIS1Reg()
{
    return InPort(COLOR_INPUT_STATUS1_READ);
}


void ClearFrameBuffer(UCHAR DisplayPath,ULONG *pFrameBufferBase, MODE_INFO *pModeInfo, UCHAR ucColorDepth)
{
    ULONG dwWidth = (ULONG)pModeInfo->H_Size, dwHeight = (ULONG)pModeInfo->V_Size, dwFactor  = 0;
    ULONG i = 0;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter ClearFrameBuffer()==\n");
    
    
    
    switch(ucColorDepth)
    {
        case 8:
        case 16:
        case 32:    
          dwFactor = 32 / ucColorDepth;
          break;
          
        default:
            return;
    }

    
    for(i = 0;i<((dwWidth*dwHeight)/dwFactor);i++)
    {
        *(pFrameBufferBase+i) = 0x00000000;
    }
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit ClearFrameBuffer()==\n");
    
}

ULONG Difference(ULONG Value1, ULONG Value2)
{
    if (Value1 > Value2)
        return (Value1 - Value2);
    else
        return (Value2 - Value1);
}


UCHAR ReadScratch(USHORT IndexMask)
{
    UCHAR Index = (UCHAR)(IndexMask >> 8);
    UCHAR Mask = (UCHAR)IndexMask;
    UCHAR RetValue;

    RetValue = GetCRReg(Index);
    RetValue &= Mask;
    
    RetValue = AlignDataToLSB(RetValue, Mask);

    return RetValue;
}


void WriteScratch(USHORT IndexMask, UCHAR Data)
{
    UCHAR Index = (UCHAR)(IndexMask >> 8);
    UCHAR Mask = (UCHAR)IndexMask;

    Data = AlignDataToMask(Data, Mask);
    Data &= Mask;
    SetCRReg(Index, Data, Mask);
}


UCHAR AlignDataToLSB(UCHAR bData, UCHAR bMask)
{
    bData &= bMask;
    
    while ((bMask & BIT0) == 0)
    {
        bData >>= 1;
        bMask >>= 1;
    }
    
    return bData;
}


UCHAR AlignDataToMask(UCHAR bData, UCHAR bMask)
{
    while ((bMask & BIT0) == 0)
    {
        bData <<= 1;
        bMask >>= 1;
    }
    
    return bData;
}


void SetDPMS(UCHAR DPMSState, UCHAR DisplayPath)
{
    UCHAR RegValue;

    if (DPMSState > DPMS__OFF)
        RegValue = 3;
    else if (DPMSState <= DPMS__SUSPEND)
        RegValue = DPMSState;
        
    if (DisplayPath == DISP1)
        SetCRReg(0xB6, RegValue, BIT1+BIT0);
    else if (DisplayPath == DISP2)
        SetCRReg(0x3E, RegValue, BIT1+BIT0);

}

CI_STATUS DetectMonitor()
{
    CI_STATUS ConnectStatus;

    
    SetCRReg(0xA9, 0x80, 0xFF);

    
    SetCRReg(0xA8, BIT6, BIT6);

    
    WaitDisplayPeriod();

    
    ConnectStatus = ((InPort(INPUT_STATUS_0_READ) & BIT4) ? true : false);

    SetCRReg(0xA8, 0x00, BIT6);

    return ConnectStatus;
}

void WaitDisplayPeriod()
{
     while ((InPort(COLOR_INPUT_STATUS1_READ)&BIT0) == 1);
     
     while ((InPort(COLOR_INPUT_STATUS1_READ)&BIT0) == 0);
}


void WaitPowerSequenceDone()
{
    UCHAR SR32;
     
    SR32 = GetSRReg(0x32);

    
    while(SR32 == GetSRReg(0x32));
}

CI_STATUS CheckForDSTNPanel(UCHAR bDeviceIndex)
{
    PORT_CONFIG *pDevicePortConfig;
    
    if (GetDevicePortConfig(bDeviceIndex, &pDevicePortConfig))
    {
        if ((pDevicePortConfig->TX_Enc_ID == DSTN) && 
            ((pDevicePortConfig->PortID == DVP1) || (pDevicePortConfig->PortID == DVP12)))
        {
            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

USHORT GetVESAMEMSize()
{
    UCHAR bHWStrapping;
    bHWStrapping = (GetCRReg(0xAA) & (BIT2+BIT1+BIT0));
    
    return (2 << (bHWStrapping+3));
}


void SetTV_CVBS_CCRSLevel(UCHAR *Level)
{
    PORT_CONFIG *pDevicePortConfig = pPortConfig;
    if (GetDevicePortConfig(TV_ID, &pDevicePortConfig))
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "CCRSLevel Set = %x\n",*Level);
        if(pDevicePortConfig->TX_Enc_ID == ENC_SAA7105)
            SetSAA7105CCRSLevel(pDevicePortConfig->TX_I2C_Port,pDevicePortConfig->TX_I2C_Addr,*Level);
    }    

}

UCHAR ucGetTV_CVBS_CCRSLevel(UCHAR *Level)
{
    PORT_CONFIG *pDevicePortConfig = pPortConfig;
    if (GetDevicePortConfig(TV_ID, &pDevicePortConfig))
    {
        if(pDevicePortConfig->TX_Enc_ID == ENC_SAA7105)
            GetSAA7105CCRSLevel(pDevicePortConfig->TX_I2C_Port,pDevicePortConfig->TX_I2C_Addr, Level);
    }
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "CCRSLevel Get = %x\n",*Level);
    return TRUE;
}

void SetDeviceSupport()
{
    PORT_CONFIG *pDevicePortConfig = pPortConfig;
    
    if (GetDevicePortConfig(CRT_ID, &pDevicePortConfig))
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "CRT supported\n");
        bCRTSUPPORT = true;
    }
    
    if (GetDevicePortConfig(LCD_ID, &pDevicePortConfig))
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "LCD supported\n");
        WriteScratch(IDX_LVDS1_TX_ID,pDevicePortConfig->TX_Enc_ID);
        bLCDSUPPORT = true;        
    }

    if (GetDevicePortConfig(DVI_ID, &pDevicePortConfig))
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "DVI supported\n");
        WriteScratch(IDX_TMDS1_TX_ID,pDevicePortConfig->TX_Enc_ID);
        bDVISUPPORT = true;        
    }
    
    if (GetDevicePortConfig(TV_ID, &pDevicePortConfig))
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "TV supported\n");
        WriteScratch(IDX_TV1_ENCODER_ID,pDevicePortConfig->TX_Enc_ID);
        if(pDevicePortConfig->TX_Enc_ID == ENC_SAA7105)
            SetSAA7105InitReg(pDevicePortConfig->TX_I2C_Port,pDevicePortConfig->TX_I2C_Addr);
        bTVSUPPORT = true;
    }    
}

CI_STATUS VBE_SetMode(CBIOS_Extension *pCBIOSExtension)
{
    USHORT    wModeNum = pCBIOSExtension->pCBiosArguments->reg.x.BX & 0x01FF;
    USHORT    wPitch = 0;
    MODE_INFO   *pModeInfo = NULL;
    UCHAR    bColorDepth = 0;
    UCHAR    bCurDeviceID, bNewDeviceID;
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry VBE_SetMode Mode number 0x%x== \n",wModeNum);
#endif
    if (wModeNum < 0x100)
    {
        SetVBERerurnStatus(VBEFunctionCallFail, pCBIOSExtension->pCBiosArguments);
        return true;
    }

    bCurDeviceID = Get_DEV_ID(DISP1);
    bNewDeviceID = Get_NEW_DEV_ID(DISP1);

    if(!Get_MODE_INFO(wModeNum, &pModeInfo))
    {
        SetVBERerurnStatus(VBEFunctionCallFail, pCBIOSExtension->pCBiosArguments);
    #if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 VBE_SetMode return 0x%x== \n",VBEFunctionCallFail);
    #endif
        return true;
    }
    
    Set_VESA_MODE(wModeNum, DISP1);
    
    
    SequencerOff(DISP1);

    
    TurnOffScaler(DISP1);

    
    TurnOffTVClock();
    
    if (bCurDeviceID != bNewDeviceID)
    {
        
        ControlPwrSeqOff(bCurDeviceID);

        

        
        TurnOffDigitalPort(bCurDeviceID);
        
        
        Set_DEV_ID(bNewDeviceID, DISP1);
    }

    

    
    LoadDisplay1VESAModeInitRegs();

    
    LoadTiming(DISP1, wModeNum);

    
    GetModePitch(wModeNum, &wPitch);
    SetPitch(DISP1, wPitch);

    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, " \n");
    Get_MODE_INFO(wModeNum, &pModeInfo);
    GetModeColorDepth(wModeNum, pModeInfo, &bColorDepth);
    SetColorDepth(DISP1, bColorDepth);

    

    

    

    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, " \n");
    SetFIFO(DISP1);

    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, " \n");
    ConfigDigitalPort(DISP1);

    TurnOnDigitalPort(bNewDeviceID);

    

    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, " \n");
    ControlPwrSeqOn(bNewDeviceID);

    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, " \n");
    SequencerOn(DISP1);
    
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBIOSExtension->pCBiosArguments);
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit2 VBE_SetMode return 0x%x== \n",VBEFunctionCallSuccessful);
#endif
    return true;
}

CI_STATUS VBE_SetGetScanLineLength (CBIOS_ARGUMENTS *pCBiosArguments)
{
    USHORT    wModeNum;
    MODE_INFO   *pModeInfo = NULL;
    UCHAR    bColorDepth;
    UCHAR    bDispalyPath;
    ULONG   dwVESAMemSizeInBytes;
    USHORT    wMaxPitchInBytes, wCurrentVDispEnd;
    USHORT    VBEReturnStatus = VBEFunctionCallFail;
    USHORT    wPitchToBeSet = pCBiosArguments->reg.x.CX; 
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry VBE_SetGetScanLineLength== \n");
#endif
    if (pCBiosArguments->reg.x.AX == 0x4f06)
    {
        bDispalyPath = DISP1;
    }
    else if ((pCBiosArguments->reg.x.AX == 0x4f14) && ((pCBiosArguments->reg.lh.BH == 0x87)||(pCBiosArguments->reg.lh.BH == 0x08)))
    {
        bDispalyPath = DISP2;
    }
    else
    {
        SetVBERerurnStatus(VBEFunctionCallFail, pCBiosArguments);
    #if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 VBE_SetGetScanLineLength return 0x%x== \n",VBEFunctionCallFail);
    #endif
        return true;
    }
    
    if (pCBiosArguments->reg.lh.BL <=3)
    {
        wModeNum = Get_VESA_MODE(bDispalyPath);
        
        Get_MODE_INFO(wModeNum, &pModeInfo);
        
        if (GetModeColorDepth(wModeNum, pModeInfo, &bColorDepth))
        {
            VBEReturnStatus = VBEFunctionCallNotSupported;

            dwVESAMemSizeInBytes = ((ULONG)GetVESAMEMSize()) << 20;
            
            wCurrentVDispEnd = GetVDisplayEnd(bDispalyPath);
            
            
            if (((ULONG)(dwVESAMemSizeInBytes / (ULONG)wCurrentVDispEnd) & 0xFFFF0000) == 0)
            {
                wMaxPitchInBytes = (USHORT)(dwVESAMemSizeInBytes / (ULONG)wCurrentVDispEnd) & 0xFFF8;
            }
            else
            {
                wMaxPitchInBytes = 0xFFF8;
            }
        #if CBIOS_DEBUG
            xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==VBE_SetGetScanLineLength Color Depth = 0x%x== \n",bColorDepth);
            xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==VBE_SetGetScanLineLength Mem Size = 0x%x== \n",dwVESAMemSizeInBytes);
            xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==VBE_SetGetScanLineLength Current Disp End = 0x%x== \n",wCurrentVDispEnd);
            xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==VBE_SetGetScanLineLength Max Pitch = 0x%x== \n",wMaxPitchInBytes);
        #endif
            if ((pCBiosArguments->reg.lh.BL == 0) || (pCBiosArguments->reg.lh.BL == 2))
            {
                if (pCBiosArguments->reg.lh.BL == 0)
                {
                    
                    wPitchToBeSet <<= (bColorDepth >> 4);
                }
                
                if (wPitchToBeSet <= wMaxPitchInBytes)
                {
                    SetPitch(bDispalyPath, wPitchToBeSet);
                }
                else
                {
                    SetVBERerurnStatus(VBEReturnStatus, pCBiosArguments);
                    return true;
                }
            }

            if (pCBiosArguments->reg.lh.BL == 3)
            {
                
                pCBiosArguments->reg.x.BX = wMaxPitchInBytes;
            }
            else
            {
                
                pCBiosArguments->reg.x.BX =GetPitch(bDispalyPath);
            }

            
            
            pCBiosArguments->reg.x.CX = pCBiosArguments->reg.x.BX >> (bColorDepth >> 4);
            pCBiosArguments->reg.x.DX = (dwVESAMemSizeInBytes / (USHORT)pCBiosArguments->reg.x.BX);
        }
    }
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit2 VBE_SetGetScanLineLength return 0x%x== \n",VBEReturnStatus);
#endif
    SetVBERerurnStatus(VBEReturnStatus, pCBiosArguments);
    return true;
}

CI_STATUS OEM_QueryBiosInfo (CBIOS_ARGUMENTS *pCBiosArguments)
{
    USHORT    VBEReturnStatus = VBEFunctionCallFail;
    char bProjCode;
    int wCustomerCode;
    char bRelVersion;
    char szPrj[2], szMajor[3], szMinor[3] ;
    int wYear;
    char bMonth, bDay;
    char szYear[5], szMonth[3], szDay[3];
    char i,ucCRAA,ucCRAB;
    

#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_QueryBiosInfo()== \n");
#endif

    

    
    
    
    if (BiosInfoData != NULL)
    {
        szPrj[1] = szMajor[2] = szMinor[2] = '\0';
    	strncpy(szPrj, ((char *)BiosInfoData)+0x10, 1);
    	strncpy(szMajor, ((char *)BiosInfoData)+0x12, 2);
    	strncpy(szMinor, ((char *)BiosInfoData)+0x15, 2);

    	
    	bProjCode = atoi(szPrj) ;
    	wCustomerCode = atoi(szMajor);
    	bRelVersion = atoi(szMinor);
        pCBiosArguments->reg.ex.EBX = (bProjCode<<24) | (wCustomerCode<<8) | bRelVersion;

#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "Bios version : %d\n",pCBiosArguments->reg.ex.EBX);
#endif
    }


    

    
    
    ucCRAA = GetCRReg(0xAA);
    pCBiosArguments->reg.ex.ECX = ucCRAA; 
    pCBiosArguments->reg.ex.ECX <<= 6;          

    
    ucCRAB = GetCRReg(0xAB);
    ucCRAB &= BIT0 + BIT1 + BIT2;                       
    pCBiosArguments->reg.ex.ECX |= ucCRAB;

    

    
    if (BiosInfoData != NULL)
    {
        szMonth[2] = szDay[2] = szYear[4] = '\0';
        strncpy(szMonth, ((char *)BiosInfoData)+0x47, 2);
        strncpy(szDay, ((char *)BiosInfoData)+0x4a, 2);
        strncpy(szYear, ((char *)BiosInfoData)+0x56, 4);

        wYear = atoi(szYear);
        bMonth = atoi(szMonth);
        bDay = atoi(szDay);
        pCBiosArguments->reg.ex.EDX = (wYear<<16) | (bMonth<8) | bDay;
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "Bios date : %d\n",pCBiosArguments->reg.ex.EDX);
#endif
    }

    
    pCBiosArguments->reg.x.SI = 0;
    if(bCRTSUPPORT)
    {
        pCBiosArguments->reg.x.SI |= B_CRT;
    }
    if(bLCDSUPPORT)
    {
        pCBiosArguments->reg.x.SI |= B_LCD;
    }
    if(bTVSUPPORT)
    {
        pCBiosArguments->reg.x.SI |= B_TV;
    }
    if(bDVISUPPORT)
    {
        pCBiosArguments->reg.x.SI |= B_DVI;
    }
    
    

    VBEReturnStatus = VBEFunctionCallSuccessful;
    SetVBERerurnStatus(VBEReturnStatus, pCBiosArguments);
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit OEM_QueryBiosInfo()== \n");
#endif
    return true;
}
CI_STATUS OEM_QueryBiosCaps (CBIOS_ARGUMENTS *pCBiosArguments)
{
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_QueryBiosCaps()== \n");
#endif


#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit OEM_QueryBiosCaps()== \n");
#endif
    return true;
}

CI_STATUS OEM_QueryExternalDeviceInfo (CBIOS_ARGUMENTS *pCBiosArguments)
{
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_QueryExternalDeviceInfo()== \n");
#endif

    pCBiosArguments->reg.lh.BL = ReadScratch(IDX_SCRATCH_20);
    pCBiosArguments->reg.ex.EBX <<= 16;

    pCBiosArguments->reg.lh.BH = ReadScratch(IDX_SCRATCH_21);

    pCBiosArguments->reg.lh.BL = ReadScratch(IDX_SCRATCH_22);

    pCBiosArguments->reg.ex.ECX = BIT16;

    pCBiosArguments->reg.x.DX = 0xFFFF;

    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit OEM_QueryExternalDeviceInfo()== \n");
#endif
    return true;
}

CI_STATUS OEM_QueryDisplayPathInfo (CBIOS_ARGUMENTS *pCBiosArguments)
{
    UCHAR ScratchTempData;
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_QueryDisplayPathInfo()== \n");
#endif
    pCBiosArguments->reg.ex.EBX = 0;

    pCBiosArguments->reg.lh.BL |= Get_NEW_DEV_ID(DISP1);

    pCBiosArguments->reg.ex.EBX <<= 2;
    ScratchTempData = GetSRReg(0x58);               
    if (ScratchTempData & BIT0)
    {
        ScratchTempData &= (BIT2 + BIT1);             
        ScratchTempData >>= 1;
        pCBiosArguments->reg.lh.BL |= ScratchTempData;
    }

    if (bDS_SUPPORT)
    {
        ScratchTempData = GetSRReg(0x70) & BIT7;
        ScratchTempData >>= 7;
        pCBiosArguments->reg.lh.BL |= ScratchTempData;
        ScratchTempData = GetSRReg(0x74) & BIT7;
        ScratchTempData >>= 6;
        pCBiosArguments->reg.lh.BL |= ScratchTempData;
    }
    
    pCBiosArguments->reg.ex.EBX <<= 4;
    pCBiosArguments->reg.lh.BL |= Get_DEV_ID(DISP1);

    pCBiosArguments->reg.ex.EBX <<= 7;
    pCBiosArguments->reg.lh.BL |= Get_RRATE_ID(DISP1);
    
    pCBiosArguments->reg.ex.EBX <<= 9;   
    pCBiosArguments->reg.x.BX |= Get_VESA_MODE(DISP1);
    

    pCBiosArguments->reg.ex.ECX = 0;

    pCBiosArguments->reg.lh.CL |= Get_NEW_DEV_ID(DISP1);

    pCBiosArguments->reg.ex.ECX <<= 2;
    ScratchTempData = GetSRReg(0x50);               
    if (ScratchTempData & BIT0)
    {
        ScratchTempData &= (BIT2 + BIT1);             
        ScratchTempData >>= 1;
        pCBiosArguments->reg.lh.BL |= ScratchTempData;
    }
    
    pCBiosArguments->reg.ex.ECX <<= 4;
    pCBiosArguments->reg.lh.CL |= Get_DEV_ID(DISP2);

    pCBiosArguments->reg.ex.ECX <<= 7;
    pCBiosArguments->reg.lh.CL |= Get_RRATE_ID(DISP2);
    
    pCBiosArguments->reg.ex.ECX <<= 9;   
    pCBiosArguments->reg.x.CX |= Get_VESA_MODE(DISP2);
    
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit OEM_QueryDisplayPathInfo()== \n");
#endif
    return true;

}

CI_STATUS OEM_QueryDeviceConnectStatus (CBIOS_ARGUMENTS *pCBiosArguments)
{
    UCHAR *pucPCIDataStruct = (UCHAR*)PCIDataStruct;
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_QueryDeviceConnectStatus()== \n");
#endif
    pCBiosArguments->reg.x.BX = 0;
    
    if (*(USHORT*)(pucPCIDataStruct + OFF_DID) == 0x2010)
    {
        SetSRReg(0x4f, 0x82, 0xFF);
        LongWait();
        if (GetSRReg(0x3c) & BIT0)
        {
            pCBiosArguments->reg.x.BX |= B_CRT;   
        }
    }
    else
    {
        
    }
    
    if (bLCDSUPPORT)
    {
        pCBiosArguments->reg.x.BX |= B_LCD;
    }

    if (bDVISUPPORT)
    {
        if (*(USHORT*)(((UCHAR*)PCIDataStruct) + OFF_DID) == 0x2010)
        {
            if (GetSRReg(0x3c) & BIT1)
            {
                pCBiosArguments->reg.x.BX |= B_DVI;   
            }
        }
        else
        {
            
        }
    }

    if (bTVSUPPORT)
    {
        
        pCBiosArguments->reg.x.BX |= B_TV;
    }
    
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit OEM_QueryDeviceConnectStatus()== \n");
#endif
    return true;
}

CI_STATUS OEM_QuerySupportedMode (CBIOS_ARGUMENTS *pCBiosArguments)
{
    MODE_INFO   *pModeInfo;
    RRATE_TABLE *pRRateTable;
    int RRateTableIndex = 0;
    int ModeNumIndex;
    USHORT wModeNum;
    USHORT wSerialNumber;
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter OEM_QuerySupportedMode()== \n");
#endif
    wSerialNumber = pCBiosArguments->reg.x.CX;
    pModeInfo     = (MODE_INFO*)pVESATable;
    
    while (pModeInfo->H_Size != 0xFFFF)
    {
#if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "pModeInfo->H_Size = %d \n", pModeInfo->H_Size);
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "pModeInfo->V_Size = %d \n", pModeInfo->V_Size);
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "sizeof(RRATE_TABLE) = %d \n", sizeof(RRATE_TABLE));
#endif
        for (ModeNumIndex = 0; ModeNumIndex < 3; ModeNumIndex++)
        {
            switch (ModeNumIndex)
            {
                case 0:
                    wModeNum = pModeInfo->Mode_ID_8bpp;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "case 0: wModeNum = 0x%x \n", wModeNum);
                    break;
                case 1:
                    wModeNum = pModeInfo->Mode_ID_16bpp;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "case 1: wModeNum = 0x%x \n", wModeNum);
                    break;
                case 2:
                    wModeNum = pModeInfo->Mode_ID_32bpp;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "case 2: wModeNum = 0x%x \n", wModeNum);
                    break;
            }

            pRRateTable = (RRATE_TABLE*)((int)pModeInfo + sizeof(MODE_INFO));

            for (RRateTableIndex = 0; RRateTableIndex < pModeInfo->RRTableCount; RRateTableIndex++, pRRateTable++)
            {

                xf86DrvMsgVerb(0, X_INFO, 5, "pRRateTable = 0x%x \n", pRRateTable);

                if ((pRRateTable->Attribute & DISABLE) == 0)
                {
                    if (wSerialNumber == 0)
                    {
                        
                        pCBiosArguments->reg.x.BX = wModeNum;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "mode num = 0x%x \n", pCBiosArguments->reg.x.BX);

                        
                        GetModeColorDepth(wModeNum, pModeInfo, &pCBiosArguments->reg.lh.CL);
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "color depth = %d \n", pCBiosArguments->reg.lh.CL);
                        
                        
                        pCBiosArguments->reg.lh.CH = pRRateTable->RRate_ID;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "RRate ID = %d \n", pCBiosArguments->reg.lh.CH);
                        
                        
                        pCBiosArguments->reg.ex.EDX = (((ULONG)pModeInfo->V_Size) << 16) | (ULONG)pModeInfo->H_Size;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "H x V = %d x %d \n", pCBiosArguments->reg.x.DX, pCBiosArguments->reg.ex.EDX>>16);
                        
                        
                        pCBiosArguments->reg.x.SI = pRRateTable->Attribute;
                        
                        
                        pCBiosArguments->reg.ex.EDI = pRRateTable->Clock;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "dot clk = %dkhz \n", pCBiosArguments->reg.ex.EDI);

                        SetVBERerurnStatus (VBEFunctionCallSuccessful, pCBiosArguments);

                    #if CBIOS_DEBUG
                        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit1 OEM_QuerySupportedMode() return 0x%x== \n", VBEFunctionCallSuccessful);
                    #endif
                        return true;
                    }
                    else
                    {
                        wSerialNumber--;
                    }
                }
            }
        }
        pModeInfo = (MODE_INFO*)((int)pModeInfo + sizeof(MODE_INFO) + pModeInfo->RRTableCount*sizeof(RRATE_TABLE));
    }

    pModeInfo = (MODE_INFO*)(&CInt10VESATable);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "*pModeInfo = %X \n", *pModeInfo);
    
    while (pModeInfo->H_Size != 0xFFFF)
    {
#if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "pModeInfo->H_Size = %d \n", pModeInfo->H_Size);
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "pModeInfo->V_Size = %d \n", pModeInfo->V_Size);
        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "sizeof(RRATE_TABLE) = %d \n", sizeof(RRATE_TABLE));
#endif
        for (ModeNumIndex = 0; ModeNumIndex < 3; ModeNumIndex++)
        {
            switch (ModeNumIndex)
            {
                case 0:
                    wModeNum = pModeInfo->Mode_ID_8bpp;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "case 0: wModeNum = 0x%x \n", wModeNum);
                    break;
                case 1:
                    wModeNum = pModeInfo->Mode_ID_16bpp;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "case 1: wModeNum = 0x%x \n", wModeNum);
                    break;
                case 2:
                    wModeNum = pModeInfo->Mode_ID_32bpp;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "case 2: wModeNum = 0x%x \n", wModeNum);
                    break;
            }

            pRRateTable = (RRATE_TABLE*)((int)pModeInfo + sizeof(MODE_INFO));

            for (RRateTableIndex = 0; RRateTableIndex < pModeInfo->RRTableCount; RRateTableIndex++, pRRateTable++)
            {

                xf86DrvMsgVerb(0, X_INFO, 5, "pRRateTable = 0x%x \n", pRRateTable);

                if ((pRRateTable->Attribute & DISABLE) == 0)
                {
                    if (wSerialNumber == 0)
                    {
                        
                        pCBiosArguments->reg.x.BX = wModeNum;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "mode num = 0x%x \n", pCBiosArguments->reg.x.BX);

                        
                        GetModeColorDepth(wModeNum, pModeInfo, &pCBiosArguments->reg.lh.CL);
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "color depth = %d \n", pCBiosArguments->reg.lh.CL);
                        
                        
                        pCBiosArguments->reg.lh.CH = pRRateTable->RRate_ID;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "RRate ID = %d \n", pCBiosArguments->reg.lh.CH);
                        
                        
                        pCBiosArguments->reg.ex.EDX = (((ULONG)pModeInfo->V_Size) << 16) | (ULONG)pModeInfo->H_Size;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "H x V = %d x %d \n", pCBiosArguments->reg.x.DX, pCBiosArguments->reg.ex.EDX>>16);
                        
                        
                        pCBiosArguments->reg.x.SI = pRRateTable->Attribute;
                        
                        
                        pCBiosArguments->reg.ex.EDI = pRRateTable->Clock;
                        xf86DrvMsgVerb(0, X_INFO, InternalLevel, "dot clk = %dkhz \n", pCBiosArguments->reg.ex.EDI);

                        SetVBERerurnStatus (VBEFunctionCallSuccessful, pCBiosArguments);

                    #if CBIOS_DEBUG
                        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit1 OEM_QuerySupportedMode() return 0x%x== \n", VBEFunctionCallSuccessful);
                    #endif
                        return true;
                    }
                    else
                    {
                        wSerialNumber--;
                    }
                }
            }
        }
        pModeInfo = (MODE_INFO*)((int)pModeInfo + sizeof(MODE_INFO) + pModeInfo->RRTableCount*sizeof(RRATE_TABLE));
    }        

    SetVBERerurnStatus (VBEFunctionCallFail, pCBiosArguments);
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "  Exit2 OEM_QuerySupportedMode() return 0x%x== \n", VBEFunctionCallFail);
#endif    
    return true;

}

CI_STATUS OEM_QueryLCDPanelSizeMode (CBIOS_ARGUMENTS *pCBiosArguments)
{
    MODE_INFO   *pModeInfo;
    PANEL_TABLE *pPanelTable;
    UCHAR        bColorDepth = pCBiosArguments->reg.lh.CL;
    USHORT        wModeNum;
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter OEM_QueryLCDPanelSizeMode()== \n");
#endif

    SetVBERerurnStatus (VBEFunctionCallFail, pCBiosArguments);
    
    if (!bLCDSUPPORT)
        return true;
        
    if (!GetModePointerFromLCDTable(LCD_ID, &pModeInfo, &pPanelTable))
        return true;

    switch (bColorDepth)
    {
        case 0:
            wModeNum = pModeInfo->Mode_ID_8bpp;
            pCBiosArguments->reg.lh.CL = 8;
            break;
        case 1:
            wModeNum = pModeInfo->Mode_ID_16bpp;
            pCBiosArguments->reg.lh.CL = 16;
            break;
        case 2:
            wModeNum = pModeInfo->Mode_ID_32bpp;
            pCBiosArguments->reg.lh.CL = 32;
            break;
        default:
            SetVBERerurnStatus (VBEFunctionCallFail, pCBiosArguments);
#if CBIOS_DEBUG
            xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit2 OEM_QueryLCDPanelSizeMode() return 0x%x== \n", VBEFunctionCallFail);
#endif    
            return true;
    }
        
    
    pCBiosArguments->reg.x.BX = wModeNum;
    
    
    pCBiosArguments->reg.lh.CH = pPanelTable->Timing.RRate_ID;
    
    
    pCBiosArguments->reg.ex.EDX = (ULONG)pModeInfo->H_Size  | ((ULONG)pModeInfo->V_Size) << 16;
    
    
    pCBiosArguments->reg.x.SI = pPanelTable->Timing.Attribute;
    
    
    pCBiosArguments->reg.ex.EDI = pPanelTable->Timing.Clock;
    
    SetVBERerurnStatus (VBEFunctionCallSuccessful, pCBiosArguments);
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 OEM_QueryLCDPanelSizeMode() return 0x%x== \n", VBEFunctionCallSuccessful);
#endif
    return true;
}

CI_STATUS OEM_QueryLCDPWMLevel(CBIOS_ARGUMENTS *pCBiosArguments)
{
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_QueryLCDPWMLevel()== \n");
#endif
    SetVBERerurnStatus (VBEFunctionCallFail, pCBiosArguments);
    
    if (!bLCDSUPPORT)
        return true;

    if ((GetSRReg(0x30)&0x03) == 0x03 )
        pCBiosArguments->reg.lh.BL = GetSRReg(0x30);
    else
        pCBiosArguments->reg.lh.BL = 0;
        
    SetVBERerurnStatus (VBEFunctionCallSuccessful, pCBiosArguments);
    
    return true;
}

CI_STATUS OEM_QueryTVConfiguration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    
    pCBiosArguments->reg.lh.BH = Get_TV_ConnectorType();
    pCBiosArguments->reg.lh.BL = Get_TV_Type();
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return true;
}
CI_STATUS OEM_QueryTV2Configuration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_QueryHDTVConfiguration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_QueryHDTV2Configuration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_QueryHDMIConfiguration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_QueryHDMI2Configuration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_SetActiveDisplayDevice (CBIOS_ARGUMENTS *pCBiosArguments)
{
    UCHAR bDeviceIndex1 = Get_DEV_ID(DISP1);
    UCHAR bDeviceIndex2 = Get_DEV_ID(DISP2);
    UCHAR bNewDeviceIndex1 = pCBiosArguments->reg.lh.CL & 0x0F;
    UCHAR bNewDeviceIndex2 = (pCBiosArguments->reg.lh.CL >> 4) & 0x0F;
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_SetActiveDisplayDevice()== \n");
#endif

    
    if ((!CheckForNewDeviceAvailable(bNewDeviceIndex1)&& (bNewDeviceIndex1!=0)) || (!CheckForNewDeviceAvailable(bNewDeviceIndex2)&& (bNewDeviceIndex2!=0)))
    {
        SetVBERerurnStatus(VBEFunctionCallFail, pCBiosArguments);
    #if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 OEM_SetActiveDisplayDevice() return 0x%x== \n", VBEFunctionCallFail);
    #endif
        return true;
    }
    
    if(CheckForDSTNPanel(bNewDeviceIndex1) || CheckForDSTNPanel(bNewDeviceIndex2))
    {
        bNewDeviceIndex1 = 0;
    }

    if (bDeviceIndex1 != bNewDeviceIndex1)
    {
        if (bNewDeviceIndex1 == 0)
        {
            DisableDisplayPathAndDevice(DISP1);
            Set_DEV_ID(bNewDeviceIndex1, DISP1);
        }
        Set_NEW_DEV_ID(bNewDeviceIndex1, DISP1);
    }

    if (bDeviceIndex2 != bNewDeviceIndex2)
    {
        if (bNewDeviceIndex2 == 0)
        {
            DisableDisplayPathAndDevice(DISP2);
            Set_DEV_ID(bNewDeviceIndex2, DISP2);
        }
        Set_NEW_DEV_ID(bNewDeviceIndex2, DISP2);
    }

    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit2 OEM_SetActiveDisplayDevice() return 0x%x== \n", VBEFunctionCallSuccessful);
#endif
    return true;
}
CI_STATUS OEM_SetVESAModeForDisplay2 (CBIOS_Extension *pCBIOSExtension)
{
    USHORT    wModeNum = pCBIOSExtension->pCBiosArguments->reg.x.CX & 0x01FF;
    USHORT    wPitch = 0;
    MODE_INFO   *pModeInfo = NULL;
    UCHAR    bColorDepth = 0;
    UCHAR    bCurDeviceID, bNewDeviceID;

#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_SetVESAModeForDisplay2()== \n");
#endif

    if (wModeNum < 0x100)
    {
        SetVBERerurnStatus(VBEFunctionCallFail, pCBIOSExtension->pCBiosArguments);
    #if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 OEM_SetVESAModeForDisplay2() return 0x%x== \n", VBEFunctionCallFail);
    #endif
        return true;
    }
    
    bCurDeviceID = Get_DEV_ID(DISP2);
    bNewDeviceID = Get_NEW_DEV_ID(DISP2);

    
    if(!Get_MODE_INFO(wModeNum, &pModeInfo))
    {
        SetVBERerurnStatus(VBEFunctionCallFail, pCBIOSExtension->pCBiosArguments);
    #if CBIOS_DEBUG
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit2 OEM_SetVESAModeForDisplay2() return 0x%x== \n", VBEFunctionCallFail);
    #endif
        return true;
    }
    
    Set_VESA_MODE(wModeNum, DISP2);
    
    
    SequencerOff(DISP2);

    
    TurnOffScaler(DISP2);

    if (bCurDeviceID != bNewDeviceID)
    {
        
        ControlPwrSeqOff(bCurDeviceID);

        

        
        TurnOffDigitalPort(bCurDeviceID);

        
        Set_DEV_ID(bNewDeviceID, DISP2);
    }
    
    
    
    
    LoadTiming(DISP2, wModeNum);

    
    GetModePitch(wModeNum, &wPitch);
    SetPitch(DISP2, wPitch);

    
    Get_MODE_INFO(wModeNum, &pModeInfo);
    GetModeColorDepth(wModeNum, pModeInfo, &bColorDepth);
    SetColorDepth(DISP2, bColorDepth);

    

    

    
    if(!(pCBIOSExtension->pCBiosArguments->reg.x.CX & BIT15))
    {
        ClearFrameBuffer(DISP2,(ULONG*)(pCBIOSExtension->VideoVirtualAddress),pModeInfo,bColorDepth);
    }

    
    SetFIFO(DISP2);

    
    ConfigDigitalPort(DISP2);

    TurnOnDigitalPort(bNewDeviceID);

    

    
    ControlPwrSeqOn(bNewDeviceID);
    
    SequencerOn(DISP2);
    
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBIOSExtension->pCBiosArguments);
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit3 OEM_SetVESAModeForDisplay2() return 0x%x== \n", VBEFunctionCallSuccessful);
#endif
    return true;
}

CI_STATUS OEM_SetDevicePowerState (CBIOS_ARGUMENTS *pCBiosArguments)
{
    UCHAR display1DeviceID, display2DeviceID, TargetDevice, DMPSState;
    USHORT VBEReturnStatus = VBEFunctionCallFail;
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_SetDevicePowerState()== \n");
#endif

    TargetDevice = pCBiosArguments->reg.lh.CL & 0x0F;
    DMPSState = pCBiosArguments->reg.lh.CL & (BIT1+BIT0);
    display1DeviceID = Get_DEV_ID(DISP1);
    display2DeviceID = Get_DEV_ID(DISP2);

    if (display1DeviceID == TargetDevice)
    {
        SetDPMS(DMPSState, DISP1);
        VBEReturnStatus = VBEFunctionCallSuccessful;
    }
    else if (display2DeviceID == TargetDevice)
    {
        SetDPMS(DMPSState, DISP2);
        VBEReturnStatus = VBEFunctionCallSuccessful;
    }

    SetVBERerurnStatus(VBEReturnStatus, pCBiosArguments);
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit OEM_SetDevicePowerState() return 0x%x== \n", VBEFunctionCallSuccessful);
#endif
    return true;
}

CI_STATUS OEM_SetRefreshRate (CBIOS_ARGUMENTS *pCBiosArguments)
{
    UCHAR bRRateID, bDisplayPath;
    bRRateID = pCBiosArguments->reg.lh.CL & 0x7F;
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_SetRefreshRate()== \n");
#endif

    if ( pCBiosArguments->reg.x.BX == SetDisplay1RefreshRate)
        bDisplayPath = DISP1;
    else
        bDisplayPath = DISP2;

    Set_RRATE_ID(bRRateID, bDisplayPath);

    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit OEM_SetRefreshRate() return 0x%x== \n", VBEFunctionCallSuccessful);
#endif
    return true;
}

CI_STATUS OEM_SetLCDPWMLevel (CBIOS_ARGUMENTS *pCBiosArguments)
{
    SetSRReg(0x2F,7,0xFF);
    SetSRReg(0x30,pCBiosArguments->reg.lh.CL,0xFF);
    return true;
}

CI_STATUS OEM_SetTVConfiguration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    Set_TV_Confiuration(pCBiosArguments->reg.lh.CL);
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
    return true;
}
CI_STATUS OEM_SetTV2Configuration (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_SetHDTVConnectType (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_SetHDTV2ConnectType (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_SetHDMIType (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_SetHDMIOutputSignal (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}
CI_STATUS OEM_SetHDMI2OutputSignal (CBIOS_ARGUMENTS *pCBiosArguments)
{
    return true;
}

CI_STATUS OEM_VideoPOST (CBIOS_ARGUMENTS *pCBiosArguments)
{
    UCHAR i = 0,btemp = 0x0,btemp1 = 0x0;
    CI_STATUS bDDRII400 = true;
    UCHAR *pucPOSTInItRegs = POSTInItRegs;
    UCHAR *pucDDRII400Tbl = DDRII400Tbl;
    UCHAR *pucDDRII533Tbl = DDRII533Tbl;
    UCHAR *pucExtendRegs2 = ExtendRegs2;

#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Entry OEM_VideoPOST()== \n");
#endif

    
    btemp = InPort(RIO_VGA_ENABLE); 
    btemp |= 1;
    OutPort(RIO_VGA_ENABLE,btemp);

    
    btemp = InPort(MISC_READ); 
    btemp |= 3;
    OutPort(MISC_WRITE,btemp);
    
    
    SetCRReg(0x80, 0xA8, 0xFF);
    
    
    for(i = 0x81;i < 0x9F;i++)
    {
        SetCRReg(i, 0x00, 0xFF);
    }

    
    
    btemp = GetCRReg(0xAB);
    if((btemp & 0x3) == 0x3)
    {
        bDDRII400 = false;
    }

    if(bDDRII400)
    {
        
        SetCRReg(0xD9, 0x00, 0x80);
        SetCRReg(0xD8, 0x9B, 0xFF);
    }
    else
    {
        
        SetCRReg(0xD9, 0x80, 0x80);
        SetCRReg(0xD8, 0x78, 0xFF);
    }
    
    
    
    btemp = 0x00;
    SetCRReg(0xBB, btemp, 0xFF);
    
    
    

    
    SerialLoadTable(&pucPOSTInItRegs, 0, 0);

    
    Set_NEW_DEV_ID(0, DISP1);

    if(bDDRII400)
    {
        
        SerialLoadTable(&pucDDRII400Tbl, 0, 0);
    }
    else
    {
        
        SerialLoadTable(&pucDDRII533Tbl, 0, 0);    
    }

    
    do{
        btemp = GetCRReg(0x5D);
        btemp &= 0x80;
        btemp1 = GetCRReg(0x5E);
        btemp1 &= 0x01;
    }while((btemp != 0x80)||(btemp1 != 0x01));

    
    SerialLoadTable(&pucExtendRegs2, 0, 0);
    
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit OEM_VideoPOST()== \n");
#endif
    return true;
}

void* SearchString(char *pcKeyWord, UCHAR *from)
{
    int i, lenKeyWord;

    lenKeyWord = strlen(pcKeyWord);
    for(i = 0; i < BIOS_ROM_SIZE; i++)
    {
        if ((*pcKeyWord == *(from+i)) && !memcmp(pcKeyWord, from+i, lenKeyWord))
        {
            return from+i;
        }
    }

    return NULL;
}

void ParseTable(char* pcKeyWord, UCHAR *from, UCHAR **pointer)
{
    *pointer = (UCHAR*)SearchString(pcKeyWord, from);
    *pointer += strlen(pcKeyWord);
}

CI_STATUS OEM_CINT10DataInit (CBIOS_ARGUMENTS *pCBiosArguments)
{
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "BIOS virtual = %x", pCBiosArguments->reg.ex.ECX);
    
    ParseTable("PCFG", (UCHAR*)pCBiosArguments->reg.ex.ECX, (UCHAR**)(&pPortConfig));
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "Port Config = %x", pPortConfig);
    
    ParseTable("VPIT", (UCHAR*)pCBiosArguments->reg.ex.ECX, (UCHAR**)(&pVESATable));
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "VESA Table = %x", pVESATable);
    
    ParseTable("LCDTBL", (UCHAR*)pCBiosArguments->reg.ex.ECX, (UCHAR**)(&pLCDTable));
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "LCD Table = %x", pLCDTable);

    ParseTable("PCIR", (UCHAR*)pCBiosArguments->reg.ex.ECX, (UCHAR**)(&PCIDataStruct));
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "PCI Data Struct = %x", PCIDataStruct);

    BiosInfoData = (UCHAR*)pCBiosArguments->reg.ex.ECX;
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "Bios info data (header) = %x", BiosInfoData);

    ParseTable("D1INIT", (UCHAR*)pCBiosArguments->reg.ex.ECX, (UCHAR**)(&Display1VESAModeInitRegs));
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "Display1 VESA Mode Init Regs = %x", Display1VESAModeInitRegs);

    SetDeviceSupport();

    if (*(USHORT*)(PCIDataStruct + OFF_DID) == 0x2012)
    {
        bDS_SUPPORT = true;
    }
    
#if 0 
    ParseTable("??????", &ExtendRegs);  
    ParseTable("??????", &ExtendRegs2); 
    ParseTable("??????", &DDRII400Tbl); 
    ParseTable("??????", &DDRII533Tbl); 
#endif
    SetVBERerurnStatus(VBEFunctionCallSuccessful, pCBiosArguments);
}

CI_STATUS CInt10(CBIOS_Extension *pCBIOSExtension)
{
    CI_STATUS CInt10_Status = false;
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter CInt10(EAX = %x, EBX = %x, ECX = %x, EDX = %x, ESI = %x, EDI = %x)==\n",
                   pCBIOSExtension->pCBiosArguments->reg.ex.EAX, pCBIOSExtension->pCBiosArguments->reg.ex.EBX,
                   pCBIOSExtension->pCBiosArguments->reg.ex.ECX, pCBIOSExtension->pCBiosArguments->reg.ex.EDX,
                   pCBIOSExtension->pCBiosArguments->reg.ex.ESI, pCBIOSExtension->pCBiosArguments->reg.ex.EDI);
#endif


    
    Relocate_IOAddress = pCBIOSExtension->IOAddress;

    switch(pCBIOSExtension->pCBiosArguments->reg.x.AX)
    {
        case VBESetMode:
            CInt10_Status = VBE_SetMode(pCBIOSExtension);
            break;

        case VBESetGetScanLineLength:
            CInt10_Status = VBE_SetGetScanLineLength(pCBIOSExtension->pCBiosArguments);
            break;
            
        case OEMFunction:
            switch(pCBIOSExtension->pCBiosArguments->reg.x.BX)
            {
                case QueryBiosInfo:             
                    CInt10_Status = OEM_QueryBiosInfo(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryBiosCaps:             
                    CInt10_Status = OEM_QueryBiosCaps(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryExternalDeviceInfo:   
                    CInt10_Status = OEM_QueryExternalDeviceInfo(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryDisplayPathInfo:      
                    CInt10_Status = OEM_QueryDisplayPathInfo(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryDeviceConnectStatus:  
                    CInt10_Status = OEM_QueryDeviceConnectStatus(pCBIOSExtension->pCBiosArguments);
                    break;
                case QuerySupportedMode:        
                    CInt10_Status = OEM_QuerySupportedMode(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryLCDPanelSizeMode:      
                    CInt10_Status = OEM_QueryLCDPanelSizeMode(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryLCDPWMLevel:           
                    CInt10_Status = OEM_QueryLCDPWMLevel(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryTVConfiguration:       
                    CInt10_Status = OEM_QueryTVConfiguration(pCBIOSExtension->pCBiosArguments);
                    break;
                case QueryTV2Configuration:
                    break;
                case QueryHDTVConfiguration:
                    break;
                case QueryHDTV2Configuration:
                    break;
                case QueryHDMIConfiguration:
                    break;
                case QueryHDMI2Configuration:
                    break;
                case QueryDisplay2Pitch:
                case GetDisplay2MaxPitch:
                    CInt10_Status = VBE_SetGetScanLineLength(pCBIOSExtension->pCBiosArguments);
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
                case SetDisplay1RefreshRate:    
                case SetDisplay2RefreshRate:    
                    CInt10_Status = OEM_SetRefreshRate(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetLCDPWMLevel:            
                    CInt10_Status = OEM_SetLCDPWMLevel(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetTVConfiguration:        
                    CInt10_Status = OEM_SetTVConfiguration(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetHDTVConnectType:
                    break;
                case SetHDTV2ConnectType:
                    break;
                case SetHDMIType:
                    break;
                case SetHDMI2Type:
                    break;
                case SetHDMIOutputSignal:
                    break;
                case SetHDMI2OutputSignal:
                    break;
                case SetDisplay2PitchInPixels:
                case SetDisplay2PitchInBytes:
                    CInt10_Status = VBE_SetGetScanLineLength(pCBIOSExtension->pCBiosArguments);
                    break;
                case SetVideoPOST:
                    CInt10_Status = OEM_VideoPOST(pCBIOSExtension->pCBiosArguments);
                    break;
                case CINT10DataInit:
                    CInt10_Status = OEM_CINT10DataInit(pCBIOSExtension->pCBiosArguments);
                    break;
                default:
                    pCBIOSExtension->pCBiosArguments->reg.x.AX = 0x014F;
                    break;
            }
            break;

        default:
            break;
    }
#if CBIOS_DEBUG
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit CInt10(EAX = %x, EBX = %x, ECX = %x, EDX = %x, ESI = %x, EDI = %x)== \n",
                   pCBIOSExtension->pCBiosArguments->reg.ex.EAX, pCBIOSExtension->pCBiosArguments->reg.ex.EBX,
                   pCBIOSExtension->pCBiosArguments->reg.ex.ECX, pCBIOSExtension->pCBiosArguments->reg.ex.EDX,
                   pCBIOSExtension->pCBiosArguments->reg.ex.ESI, pCBIOSExtension->pCBiosArguments->reg.ex.EDI);
#endif
    
    return CInt10_Status;
} 

