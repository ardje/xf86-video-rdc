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
#include "BiosDef.h"

#ifndef _CINT10_H_
#define _CINT10_H_

typedef struct _CBIOS_ARGUMENTS {
    union
    {
        struct
        {
            DWORD   Eax;
            DWORD   Ebx;
            DWORD   Ecx;
            DWORD   Edx;
            DWORD   Esi;
            DWORD   Edi;
        };

        struct
        {
            WORD    AX;
            WORD    dummy1;
            WORD    BX;
            WORD    dummy2;
            WORD    CX;
            WORD    dummy3;
            WORD    DX;
            WORD    dummy4;
            WORD    SI;
            WORD    dummy5;
            WORD    DI;
        };

        struct
        {
            BYTE    AL;
            BYTE    AH;
            WORD    dummy6;
            BYTE    BL;
            BYTE    BH;
            WORD    dummy7;
            BYTE    CL;
            BYTE    CH;
            WORD    dummy8;
            BYTE    DL;
            BYTE    DH;
            BYTE    dummy9;
        };
    };

} CBIOS_ARGUMENTS;

typedef struct _PLL_Info {
    BYTE NS;
    BYTE MS;
    BYTE RS;
}PLL_Info;


typedef PLL_Info (*PFN_CBIOS_TransVGAPLL)    (DWORD);
typedef void     (*PFN_CBIOS_SetVGAPLLReg)   (BYTE, PLL_Info);

typedef struct _CBIOS_Extension {

    
    CBIOS_ARGUMENTS *pCBiosArguments;

    
    DWORD *pVideoVirtualAddress;         
    DWORD pVideoPhysicialAddress;        
    BYTE* pjIOAddress;                   
    BYTE* pjROMLinearAddr;               
    WORD wVenderID;
    WORD wDeviceID;
    DWORD ulVBIOS_Version;
    WORD  wVBIOSBuildYear;
    BYTE ucVBIOSBuildMonth;
    BYTE ucVBIOSBuildDate;
    DWORD dwSupportDevices;
    
    BYTE ucDisp1DevIndex;
    BYTE ucDisp2DevIndex;
    BYTE bPLLFromTVEnc;
    BYTE bDuoView;                      
    BYTE bEDIDValid;
    WORD DisplayOneModeH;
    WORD DisplayOneModeV;
    WORD wCRTDefaultH;
    WORD wCRTDefaultV;
    
    BYTE bLCDSupport;
    BYTE bCRTSupport;
    BYTE bHDMISupport;
    BYTE bDVISupport;
    BYTE bTVSupport;
    
    PFN_CBIOS_TransVGAPLL pfnCBIOS_TransVGAPLL;
    PFN_CBIOS_SetVGAPLLReg pfnCBIOS_SetVGAPLLReg;
} CBIOS_Extension,*PCBIOS_Extension;

typedef struct _CBIOSEDID_DETAILED_TIMING {
    BYTE    bValid;                         
    USHORT  usPixelClock;                   

    USHORT  usHorDispEnd;                   
    USHORT  usHorBlankingTime;              
    USHORT  usHorSyncStart;                 
    USHORT  usHorSyncTime;                  
    BYTE   ucHorBorder;                     
    
    USHORT  usVerDispEnd;                   
    USHORT  usVerBlankingTime;              
    USHORT  usVerSyncStart;                 
    USHORT  usVerSyncTime;                  
    BYTE   ucVerBorder;                     
    BYTE   ucFlags;                         
} CBIOSEDID_DETAILED_TIMING, *PCBIOSEDID_DETAILED_TIMING;

typedef struct _CBIOSTVFun_Disp_Info
{
    BYTE    ucCCRSLevel;        
    BYTE    ucHPosition;        
    BYTE    ucVPosition;        
    int     iHScaler;           
    BYTE    ucVScaler;          
    BYTE    bTVType;            
    BYTE    bChange;            
    BYTE    bEnableHPanning;    
    WORD    wTVOut_HSize;       
    WORD    wModeHres;          
} CBIOSTVFun_Disp_Info,*PCBIOSTVFun_Disp_Info;


#pragma   pack(push)
#pragma   pack(1)

typedef struct _RRATE_TABLE {
    DWORD   Clock;          
    BYTE    RRate_ID;
    WORD    Attribute;
    WORD    H_Total;
    WORD    H_Sync_Start;
    WORD    H_Sync_End;
    WORD    V_Total;
    WORD    V_Sync_Start;
    WORD    V_Sync_End;
} RRATE_TABLE;

typedef struct _MODE_INFO{
    WORD    H_Size;
    WORD    V_Size;
    WORD    Mode_ID_8bpp;
    WORD    Mode_ID_16bpp;
    WORD    Mode_ID_32bpp;
    RRATE_TABLE *pRRTable;
    BYTE    RRTableCount;
} MODE_INFO;

typedef struct _PANEL_TABLE {
    WORD    TD0;
    WORD    TD1;
    WORD    TD2;
    WORD    TD3;
    WORD    TD5;
    WORD    TD6;
    WORD    TD7;
    BYTE    PWM_Clock;
    WORD    Reserved1;
    WORD    Reserved2;
    RRATE_TABLE Timing;
} PANEL_TABLE;

typedef struct _PANEL_INFO{
    WORD    H_Size;
    WORD    V_Size;
    WORD    Mode_ID_8bpp;
    WORD    Mode_ID_16bpp;
    WORD    Mode_ID_32bpp;
    BYTE    PanelTableCount;
    PANEL_TABLE pPanelTable;
} PANEL_INFO;

typedef struct _REG_OP {
    BYTE    RegGroup;
    BYTE    RegMask;
    BYTE    RegIndex;
    BYTE    RegShiftBit;
} REG_OP;

typedef struct _REG_PACKAGE {
    BYTE    RegGroup;
    BYTE    RegIndex;
    BYTE    RegValue;
    BYTE    RegMask;
} REG_PACKAGE;

typedef struct _PORT_CONFIG {
    BYTE    DeviceIndex;
    BYTE    PortID;
    BYTE    TX_ENC_ID;
    WORD    TX_I2C_Port_Addr;
    BYTE    Attribute;  
} PORT_CONFIG;

typedef struct _TxEncDetect
{
    BYTE    ucTxIndex;
    BYTE    ucTxAddr;
    BYTE    ucD_IDIndex;
    BYTE    ucD_IDData;
} TxEncDetect;
#pragma   pack(pop)

#endif
