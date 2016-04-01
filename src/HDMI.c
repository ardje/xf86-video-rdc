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
#include "HDMITbl.h"
#include "CInt10.h"
#include "CInt10FunProto.h"

RDC_EXPORT BYTE Get_HDMI_TYPE()
{
    return ReadScratch(IDX_HDMI1_TYPE);
}

RDC_EXPORT void Set_HDMI_TYPE(BYTE ucHDMIType)
{
    WriteScratch(IDX_HDMI1_TYPE, ucHDMIType);
}

RDC_EXPORT BYTE Get_HDMI_Output_Signal(BYTE DisplayPath)
{
    return ReadScratch(IDX_HDMI1_CONNECTION_TYPE);
}

RDC_EXPORT void Set_HDMI_Output_Signal(BYTE ucOutputSignal)
{
    WriteScratch(IDX_HDMI1_CONNECTION_TYPE, ucOutputSignal);
}

RDC_EXPORT BYTE Get_TMDS_TX_ID()
{
    return ReadScratch(IDX_TMDS1_TX_ID);
}

RDC_EXPORT void SetEP932MInitReg(BYTE ucDeviceID)
{
    HDMI_I2C_REG *pInitEP932MReg = (HDMI_I2C_REG*)(&EP932M_INIT_Reg);
    BYTE ucI2CPort, ucAddr;
    CBIOSGetDeviceI2CInformation(ucDeviceID, &ucI2CPort, &ucAddr);
    if(ucDeviceID == DVIIndex)
    {
        CBIOSWriteI2C(ucI2CPort, ucAddr, pInitEP932MReg->ucIndex,pInitEP932MReg->ucData);
        pInitEP932MReg = (HDMI_I2C_REG*)((int)pInitEP932MReg + sizeof(HDMI_I2C_REG));
        CBIOSWriteI2C(ucI2CPort, ucAddr, pInitEP932MReg->ucIndex,pInitEP932MReg->ucData);
    }
    else 
    {
        do{
            CBIOSWriteI2C(ucI2CPort, ucAddr, pInitEP932MReg->ucIndex,pInitEP932MReg->ucData);
            pInitEP932MReg = (HDMI_I2C_REG*)((int)pInitEP932MReg + sizeof(HDMI_I2C_REG));
        }while(pInitEP932MReg->ucIndex!=0xFF);
    }
    CBIOSDebugPrint((0, "==Set EP932M Init Reg Completed !!==\n"));
}


RDC_EXPORT void GetHDMIVideoCodeID(BYTE ucHDMIType, BYTE* ucHDMIVCID)
{
    
    
    switch(ucHDMIType)
    {
        case HDMI640:
            *ucHDMIVCID = 1;  
        break;
        case HDMI720P:
            *ucHDMIVCID = 4;  
            CBIOSDebugPrint((0, "CBIOS : HDMI 720P Type\n"));
        break;
        case HDMI1080I:
            *ucHDMIVCID = 5;  
        break;
        case HDMI480P:        
            *ucHDMIVCID = 2; 
          
        break;
        case HDMI1080P:
            *ucHDMIVCID = 16; 
            CBIOSDebugPrint((0, "CBIOS : HDMI 1080P Type\n"));
        break;
        default:
            *ucHDMIVCID = 16; 
            CBIOSDebugPrint((0, "VGA HDMI Type Error, Can not find HDMI Video Code ID !!==\n"));
        break;
    }
}

RDC_EXPORT void SetHDMIInfoframe(BYTE ucHDMIType)
{
    BYTE ucI2CPort, ucAddr, i, ucHDMIVCID;
    BYTE Temp_Data[15];
    CBIOSGetDeviceI2CInformation(HDMIIndex, &ucI2CPort, &ucAddr);
    GetHDMIVideoCodeID(ucHDMIType, &ucHDMIVCID);
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    

    for(i=0;i<15;i++)
        Temp_Data[i]=0;
        
    Temp_Data[1] |= 0x00; 
    Temp_Data[1] |= 0x10; 

        
    Temp_Data[1] |= 0x00; 
                          
                          
    
    
    switch(ucHDMIVCID)
    {
        case  4: case  5: case 16: case 19: case 20: case 31: case 32: 
        case 33: case 34: case 39: case 40: case 41: case 46: case 47:      
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            
            Temp_Data[2] |= 0x80;
            break;
        default:                                                            
            if(ucHDMIVCID<76)                                               
                Temp_Data[2] |= 0x40;
            else
                Temp_Data[2] |= 0x80;
            break;

    }
    
    Temp_Data[2] |= 0x08; 
    if(ucHDMIVCID < EP932_VDO_Settings_Max) 
        Temp_Data[2] |= EP932_VDO_Settings[ucHDMIVCID].AR_PR & 0x30;
    Temp_Data[2] |= 0x0;
    if(ucHDMIVCID < EP932_VDO_Settings_IT_Start)
        Temp_Data[4] |= EP932_VDO_Settings[ucHDMIVCID].VideoCode;
    if(ucHDMIVCID < EP932_VDO_Settings_Max)
        Temp_Data[5] |= (EP932_VDO_Settings[ucHDMIVCID].AR_PR & 0x0C) >> 2;

    
    Temp_Data[0] = 0x91; 
                         
                         
                         
    for(i=1; i<6; ++i) {
        Temp_Data[0] += Temp_Data[i];
    }
    Temp_Data[0] = ~(Temp_Data[0] - 1); 
    
    for(i=0; i<14; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, Addr_EP932M_AVI_Start+i, Temp_Data[i]);

    
    
    
    
    
    
    
    
    
    
    

    for(i=0;i<15;i++)
        Temp_Data[i]=0;
    
    Temp_Data[1] = 0x1;  
                         
                         
                         
                         
                         
    Temp_Data[2] = 0xF;  
    
    
    Temp_Data[4] = 0x0;
    
    Temp_Data[0] = 0x8F; 
                         
                         
                         
    for(i=1; i<6; ++i)
        Temp_Data[0] += Temp_Data[i];
    Temp_Data[0] = ~(Temp_Data[0] - 1);
    for(i=0; i<6; i++)
        CBIOSWriteI2C(ucI2CPort, ucAddr, Addr_EP932M_ADO_Start+i ,Temp_Data[i]);
    
    CBIOSWriteI2C(ucI2CPort, ucAddr, Addr_EP932M_Fire_All ,0xF0);
}

RDC_EXPORT void EnableTMDSReg(BYTE ucDeviceID, BYTE ucHDMIType)
{
    
    BYTE ucI2CPort, ucAddr;
    
        
        
        
    CBIOSGetDeviceI2CInformation(ucDeviceID, &ucI2CPort, &ucAddr);
    
    switch(Get_TMDS_TX_ID())
    {
        
        case TMDS_ID_ITE6610:
            break;
        
        case TMDS_ID_SII162:
            
            CBIOSWriteI2C(ucI2CPort, ucAddr, TMDS_Power, 0x3B); 
            break;
        case TMDS_ID_EP932M:
            
            SetEP932MInitReg(ucDeviceID);
            
            if(ucDeviceID == HDMIIndex)
            {
                
                SetHDMIInfoframe(ucHDMIType);
            }
            break;
        
        case TX_None:
        default:
            break;
    }
}

RDC_EXPORT void DisableTMDSReg()
{
    BYTE ucI2CPort, ucAddr;
    CBIOSGetDeviceI2CInformation(DVIIndex, &ucI2CPort, &ucAddr);

    switch(Get_TMDS_TX_ID())
    {
        case TMDS_ID_ITE6610:
            
            break;
        case TMDS_ID_SII162:
        case TMDS_ID_EP932M:
            CBIOSWriteI2C(ucI2CPort, ucAddr, TMDS_Power, 0x34); 
            break;
        case TX_None:
        default:
            break;
    }
}

RDC_EXPORT void SetTMDSTxPW(BYTE ucPowerStatus)
{
    BYTE ucI2CPort, ucAddr, ucData=0;
    BYTE ucTxID = Get_TMDS_TX_ID();
    CBIOSGetDeviceI2CInformation(DVIIndex, &ucI2CPort, &ucAddr);

    switch(ucTxID)
    {
        case TMDS_ID_ITE6610:
            
            break;
        case TMDS_ID_SII162:
        case TMDS_ID_EP932M:
            CBIOSReadI2C(ucI2CPort, ucAddr, TMDS_Power, &ucData);
            if(ucPowerStatus!=DeviceON)
            {
                ucData &= ~TMDS_Power_Bit;
                CBIOSWriteI2C(ucI2CPort, ucAddr, TMDS_Power, ucData); 
            }else
            {
                ucData |= TMDS_Power_Bit;
                CBIOSWriteI2C(ucI2CPort, ucAddr, TMDS_Power, ucData); 
            }
            break;
        case TX_None:
        default:
            break;
    }

}


RDC_EXPORT BYTE QueryHDMIConnectStatus(BYTE QuickCheck)
{
    BYTE I2CPort, I2CSlave;
    BYTE bConnect;
    BYTE R80, R81, RData;
    BYTE MaxIndex, Index, TagCode, CodeLength;
    
    MaxIndex = Index = TagCode = CodeLength = 0;    
    R80 = R81 = RData = 0;
    
    CBIOSGetDeviceI2CInformation(HDMIIndex, &I2CPort, &I2CSlave);

    
    I2CSlave = 0xA0;

    bConnect = DetectMonitor(I2CPort);

    if (bConnect)
    {
        if (!QuickCheck)
        {
            CBIOSReadI2C(I2CPort, I2CSlave, 0x80, &R80);
            CBIOSReadI2C(I2CPort, I2CSlave, 0x81, &R81);
            
            if ((R80 == 0x02) && (R81 == 0x03))
            {
                CBIOSReadI2C(I2CPort, I2CSlave, 0x82, &RData);

                MaxIndex = RData + 0x80;
                Index = 0x84;

                while (Index < MaxIndex)
                {
                    CBIOSReadI2C(I2CPort, I2CSlave, Index, &RData);
                    Index++;

                    TagCode = RData >> 5;
                    if (TagCode == 0x03)
                        break;
                
                    CodeLength = RData & 0x1F;
                    Index += CodeLength;
                }

                
                
                if (TagCode == 0x03)
                {
                    CBIOSReadI2C(I2CPort, I2CSlave, Index, &RData);

                    
                    
                    if (RData == 0x03)
                    {
                        Index++;
                        CBIOSReadI2C(I2CPort, I2CSlave, Index, &RData);

                        if (RData == 0x0C)
                        {
                            Index++;
                            CBIOSReadI2C(I2CPort, I2CSlave, Index, &RData);

                            if (RData == 0x00)
                                bConnect = B_HDMI;
                            else
                                bConnect = B_DVI;
                        }
                        else
                            bConnect = B_DVI;
                    }                    
                    else
                        bConnect = B_DVI;
                }
                else
                    bConnect = B_DVI;
            }
            else
                bConnect = B_DVI;
        }
        else
            bConnect = B_HDMI;
    }
    else
        bConnect = 0;

    return (bConnect);
}

RDC_EXPORT void SetTMDSDPAReg(DWORD dwPixelClock, BYTE ucDeviceID)
{
    
    BYTE ucI2CPort, ucAddr, i=0;
    
        
        
        
    CBIOSGetDeviceI2CInformation(ucDeviceID, &ucI2CPort, &ucAddr);
    
    if(TMDS_ID_EP932M==Get_TMDS_TX_ID())
    {
        
        SetGPIOResetReg(GPIO2); 
        for(i=0; i<(sizeof(TMDS_EP932M)/sizeof(DVPDPA_TABLE)); i++)
        {
            
            if(dwPixelClock<TMDS_EP932M[i].Clock)
            {
                i--;
                break;
            }
        }
    }
    
    
    if(i>0)
    {
        
        SetDrivingReg(DrivingData);
    }else
    {
        
        SetDrivingReg(0);
    }
    
    SetDPAReg(TMDS_EP932M[i].VGAValue, ucDeviceID);
    
    CBIOSWriteI2C(ucI2CPort, ucAddr, TMDS_EP932M[i].TX_Index, TMDS_EP932M[i].TX_Data);
}

