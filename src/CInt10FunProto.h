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
#ifndef _CINT10FP_H_
#define _CINT10FP_H_

void SetVBERerurnStatus(WORD VBEReturnStatus, CBIOS_ARGUMENTS *pCBiosArguments);
BYTE GetDevicePort(BYTE DeviceIndex);
void ResetATTR();
void EnableATTR();
void SetCRReg(BYTE bRegIndex, BYTE bRegValue, BYTE bMask);
BYTE GetCRReg(BYTE bRegIndex);
void SetSRReg(BYTE bRegIndex, BYTE bRegValue, BYTE bMask);
BYTE GetSRReg(BYTE bRegIndex);
void SetARReg(BYTE index,BYTE value);
BYTE GetARReg(BYTE index);
void SetGRReg(BYTE bRegIndex, BYTE bRegValue, BYTE bMask);
void SetMSReg(BYTE bRegValue);
WORD ReadRegFromHW(REG_OP *pRegOp);
void WriteRegToHW(REG_OP *pRegOp, WORD value);
BYTE AlignDataToLSB(BYTE Data, BYTE Mask);
BYTE AlignDataToMask(BYTE Data, BYTE Mask);
BYTE ReadScratch(WORD IndexMask);
void WriteScratch(WORD IndexMask, BYTE Data);
BYTE Get_DEV_ID(BYTE DisplayPath);
void Set_DEV_ID(BYTE DeviceID, BYTE DisplayPath);
BYTE Get_NEW_DEV_ID(BYTE DisplayPath);
void Set_NEW_DEV_ID(BYTE DeviceID, BYTE DisplayPath);
DWORD Get_LCD_Panel_Size();
void  Set_LCD_Panel_Size(DWORD dwHres,DWORD dwVres);
DWORD Get_LCD2_Panel_Size();
void  Set_LCD2_Panel_Size(DWORD dwHres,DWORD dwVres);
BYTE Get_RRATE_ID(BYTE DisplayPath);
void Set_RRATE_ID(BYTE RRateID, BYTE DisplayPath);
WORD Get_VESA_MODE(BYTE DisplayPath);
void Set_VESA_MODE(WORD ModeNum, BYTE DisplayPath);
BYTE Get_LVDS_TX_ID();
void CBIOSGetHDMIPanelSize(BYTE ucHDMIType, WORD* wHSize, WORD* wVSize);
void Set12BitDVP();
void Set24BitDVP();
void TurnOnCRTPad();
void TurnOffCRTPad();
void TurnOnDVP1Pad();
void TurnOffDVP1Pad();
void TurnOnDVP2Pad();
void TurnOffDVP2Pad();
void TurnOnDVP12Pad();
void TurnOffDVP12Pad();
void SequencerOn(BYTE DisplayPath);
void SequencerOff(BYTE DisplayPath);
void WaitPowerSequenceDone();
void PowerSequenceOn();
void PowerSequenceOff();
void TurnOnDVP1Mask();
void TurnOffDVP1Mask();
void TurnOnDVP2Mask();
void TurnOffDVP2Mask();
void UnLockTiming(BYTE DisplayPath);
void LockTiming(BYTE DisplayPath);
void SetHTotal(BYTE DisplayPath, WORD Value);
void SetHDisplayEnd(BYTE DisplayPath, WORD Value);
void SetHBlankingStart(BYTE DisplayPath, WORD Value);
void SetHBlankingEnd(BYTE DisplayPath, WORD Value);
void SetHSyncStart(BYTE DisplayPath, WORD Value);
void SetHSyncEnd(BYTE DisplayPath, WORD Value);
void SetVTotal(BYTE DisplayPath, WORD Value);
void SetVDisplayEnd(BYTE DisplayPath, WORD Value);
void SetVBlankingStart(BYTE DisplayPath, WORD Value);
void SetVBlankingEnd(BYTE DisplayPath, WORD Value);
void SetVSyncStart(BYTE DisplayPath, WORD Value);
void SetVSyncEnd(BYTE DisplayPath, WORD Value);
void SetPixelClock(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, DWORD Clock);
DWORD Difference(DWORD Value1, DWORD Value2);
PLL_Info ClockToPLLF4002A(DWORD Clock);
PLL_Info ClockToPLLF9003A(DWORD Clock);
void SetF9003APLLReg(BYTE DisplayPath, PLL_Info PLLInfo);
void SetF4002APLLReg(BYTE DisplayPath, PLL_Info PLLInfo);
void SetPolarity(BYTE DevicePort, WORD wValue);
void SetFIFO(BYTE DisplayPath);
void SetTimingRegs(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, MODE_INFO *pModeInfo, RRATE_TABLE *pRRateTable);
void SetTimingCentering(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, MODE_INFO *pModeInfo, RRATE_TABLE *pRRateTable);
void SetPitch(BYTE DisplayPath, WORD Value);
WORD GetPitch(BYTE DisplayPath);
WORD GetVDisplayEnd(BYTE DisplayPath);
void SetColorDepth(BYTE DisplayPath, BYTE Value);
void ConfigDigitalPort(BYTE DisplayPath);
void CheckLCDSyncStartValue(BYTE DisplayPath);
BYTE bHDMIUnscan(BYTE ucHDMIType);
void SetDisplay1UpScalingFactor(MODE_INFO *pModeInfo, MODE_INFO *pPanelInfo);
void SetDisplay2UpScalingFactor(MODE_INFO *pModeInfo, MODE_INFO *pPanelInfo);
void SetDisplay1DownScalingFactor(MODE_INFO *pModeInfo, MODE_INFO *pPanelInfo);
CBStatus Get_MODE_INFO_From_VESA_Table(WORD ModeNum, MODE_INFO **ppModeInfo);
CBStatus Get_MODE_INFO_From_HDMI_Table(WORD ModeNum, MODE_INFO **ppModeInfo);
CBStatus Get_MODE_INFO_From_LCD_Table(WORD ModeNum, MODE_INFO **ppModeInfo);
CBStatus GetModePointerFromVESATable(WORD ModeNum, BYTE RRIndex, MODE_INFO **ppModeInfo, RRATE_TABLE **ppRRateTable);
CBStatus GetModePointerFromHDMITable(WORD ModeNum, BYTE RRIndex, MODE_INFO **ppModeInfo, RRATE_TABLE **ppRRateTable);
void LoadVESATiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
void LoadLCDTiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
void LoadHDMITiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
void LoadTiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
CBStatus GetModeColorDepth(WORD ModeNum, MODE_INFO *pModeInfo, BYTE *pColorDepth);
CBStatus GetModePitch(BYTE DisplayPath, WORD ModeNum, WORD *pPitch);
void UnLockCR0ToCR7();
void LockCR0ToCR7();
CBStatus CheckForModeAvailable(WORD ModeNum);
void TurnOnDigitalPort(BYTE DisplayPath);
void TurnOffDigitalPort(BYTE DisplayPath);
BYTE GetVBIOSBootUpDevice(WORD* ucDeiceBit);
BYTE GetDevicePortConfig(BYTE DeviceIndex);
BYTE GetPortConnectPath(BYTE PortType);
WORD TransDevIDtoBit(BYTE DeviceIndex);
void TurnOnScaler(BYTE DisplayPath);
void TurnOffScaler(BYTE DisplayPath);
void SetDPAReg(BYTE ucDPADelay, BYTE ucDeviceID);
void SetDrivingReg(BYTE ucDrivingStatus);
void SerialLoadTable(REG_PACKAGE *pREG_PACKAGE_Table);
void LoadDisplay1VESAModeInitRegs();
void DisableDisplayPathAndDevice(BYTE DisplayPath);
void EnableSWPSByPass();
void DisableSWPSByPass();
void ResetLVDSChannelOutPut();
void DisableLCDDither();
void TurnOffTxEncReg(BYTE DisplayPath, BYTE bDigitalDevice);
void TurnOnTxEncReg(BYTE DisplayPath, BYTE bDigitalDevice);
void ClearFrameBuffer(BYTE DisplayPath, DWORD *pFrameBufferBase, DWORD ulWidth, DWORD ulHeight, BYTE bColorDepth);
void SetDPMS(BYTE DPMSState, BYTE DisplayPath);
void WaitDisplayPeriod();
void CBIOSI2CWriteClock(BYTE I2CPort, BYTE data);
void CBIOSI2CDelay(BYTE I2CPort);
void CBIOSI2CWriteData(BYTE I2CPort, BYTE data);
void CBIOSI2CStart(BYTE I2CPort);
void CBIOSSendI2CDataByte(BYTE I2CPort, BYTE Data);
BYTE CBIOSCheckACK(BYTE I2CPort);
BYTE CBIOSReceiveI2CDataByte(BYTE I2CPort, BYTE I2CSlave);
void CBIOSSendNACK(BYTE I2CPort);
void CBIOSI2CStop(BYTE I2CPort);
BYTE CBIOSReadI2C(BYTE I2CPort, BYTE I2CSlave, BYTE RegIdx, BYTE* RegData);
BYTE CBIOSWriteI2C(BYTE I2CPort, BYTE I2CSlave, BYTE RegIdx, BYTE RegData);
void CBIOSGetDeviceI2CInformation(BYTE ucDevice, BYTE* ucI2Cport, BYTE* ucI2CAddress);
DWORD CBIOSGetPortI2CInfo(void);
void CBIOSSetOutFormat(WORD wAttribute);
void CBIOSSetDither();
void CBIOSSetOpenLDI(WORD wAttribute);
WORD ConvertMStoCount(WORD wTDX);
void CBIOSSetTD0(WORD wTD0);
void CBIOSSetTD1(WORD wTD1);
void CBIOSSetTD2(WORD wTD2);
void CBIOSSetTD3(WORD wTD3);
void CBIOSSetTD5(WORD wTD5);
void CBIOSSetTD6(WORD wTD6);
void CBIOSSetTD7(WORD wTD7);
void LoadPowerSequenceTimer(void);
void CBIOSInitLCD();
void SetGPIOResetReg(BYTE bGPIOMask);
void CBIOSGetTMDSTxType(BYTE ucI2CPort);
void CBIOSSetTXType(PCBIOS_Extension pCBIOSExtension);
void CBIOSGetSupportDevice(PCBIOS_Extension pCBIOSExtension);
void CBIOSUpdateDevSupFlag(PCBIOS_Extension pCBIOSExtension);
void CBIOSInitPortConfig(BYTE* pjROMLinearAddr);
BYTE CBIOSCheckDeviceAvailable(BYTE NewDisplay1DeviceID);
void CBIOSInitLCDTable(BYTE* pjROMLinearAddr);
void CBIOSInitHDMITable(BYTE* pjROMLinearAddr);
void CBIOSGetVBIOSBuildDate(PCBIOS_Extension pCBIOSExtension);
void CBIOSGetVBIOSVersion(PCBIOS_Extension pCBIOSExtension);
void CBIOSGetVBIOSRomDeviceID(PCBIOS_Extension pCBIOSExtension);
void CBIOSInitialDataFromVBIOS(PCBIOS_Extension pCBIOSExtension);
void CBIOSInitialI2CReg(void);
void HandleShareConnector(BYTE *pDeviceIndex);
BYTE DetectMonitor(BYTE I2CPort);
void WaitLong();
BYTE SenseCRT();
CBStatus OEM_QueryBiosInfo (PCBIOS_Extension pCBIOSExtension);
CBStatus OEM_QueryBiosCaps (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_QueryExternalDeviceInfo (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_QueryDisplayPathInfo (PCBIOS_Extension pCBIOSExtension);
CBStatus OEM_QueryDeviceConnectStatus (PCBIOS_Extension pCBIOSExtension);
CBStatus OEM_QuerySupportedMode (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_QueryLCDPanelSizeMode (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_QueryLCD2PanelSizeMode (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_QueryTVConfiguration (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_SetSetTVFunction(CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_QueryTV2Configuration (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_QueryHDMISupportedMode (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_SetActiveDisplayDevice (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_SetVESAModeForDisplay2(CBIOS_Extension *pCBIOSExtension);
CBStatus OEM_SetDevicePowerState (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_SetDisplay2Pitch(CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_SetRefreshRate(CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_SetSetTVConfiguration(CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_SetTV2Configuration (CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_SetHDMIType(CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_SetHDMIOutputSignal(CBIOS_ARGUMENTS *pCBiosArguments);
CBStatus OEM_VideoPOST (PCBIOS_Extension pCBIOSExtension);
CBStatus OEM_VSetEDIDInModeTable(PCBIOS_Extension pCBIOSExtension);
CBStatus VBE_SetMode(CBIOS_Extension *pCBIOSExtension);
CBStatus VBE_SetPitch(CBIOS_Extension *CBIOSExtension);
CBStatus VBE_AccessEDID(CBIOS_Extension *CBIOSExtension);
CBStatus CInt10(CBIOS_Extension *pCBIOSExtension);
void cbGetHDMIModeInfo(WORD* wHSize, WORD* wVSize, BYTE* bRRCnt, BYTE ucHDMIType);
void cbSetHDMIModeInfo(WORD wHSize, WORD wVSize, BYTE ucHDMIType);

BYTE Get_TVEnc_TX_ID();
BYTE Get_TV_TYPE();
void Set_TV_TYPE(BYTE ucTVType);
BYTE Get_TV_CONNECTION_TYPE();
void Set_TV_CONNECTION_TYPE(BYTE ucTVConnectionType);
void WriteFS473I2CData(BYTE ucIndex, WORD wData);
void CBIOSSetFS473VScalingLevel(PCBIOSTVFun_Disp_Info pCBTVFun);
void LoadTVTiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
void SetSAA7105InitReg();
void SetFS473InitReg();
void InitTVEncReg(BYTE ucTVEncID);
void CBIOSInitTV();
void SetTVDACPower(BYTE bPowerstate);
void UpdatePLLByTVEnc(DWORD dwPixelClock);
BYTE bSetSAA7105TimingReg(BYTE bDisplayPath, WORD wModeNum);
BYTE bSetFS473TimingReg(BYTE bDisplayPath, WORD wModeNum);
BYTE SenseSAA7105TV(BYTE I2CPort, BYTE I2CSlave);
BYTE SenseFS473TV(BYTE I2CPort, BYTE I2CSlave);
BYTE SenseTV();
void EnableTVClock();
void DisableTVClock();
void vSetFS473CCRSLevel(BYTE bCCRSLevel);
void SetTVCCRSLevel(BYTE bCCRSLevel);
void SetFS473HPosition(BYTE bHPosition);
void SetFS473VPosition(BYTE bVPosition);
void SetFS473HSCaler(PCBIOSTVFun_Disp_Info pCBTVFun);


BYTE Get_HDMI_TYPE();
void Set_HDMI_TYPE(BYTE ucHDMIType);
BYTE Get_HDMI_Output_Signal(BYTE DisplayPath);
void Set_HDMI_Output_Signal(BYTE ucOutputSignal);
BYTE Get_TMDS_TX_ID();
void SetEP932MInitReg(BYTE ucDeviceID);
void GetHDMIVideoCodeID(BYTE ucHDMIType, BYTE* ucHDMIVCID);
void SetHDMIInfoframe(BYTE ucHDMIType);
void EnableTMDSReg(BYTE ucDeviceID, BYTE ucHDMIType);
void DisableTMDSReg();
void SetTMDSTxPW(BYTE ucPowerStatus);
BYTE QueryHDMIConnectStatus(BYTE QuickCheck);
void SetTMDSDPAReg(DWORD dwPixelClock, BYTE ucDeviceID);
#endif
