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
 


void SetVBERerurnStatus(USHORT VBEReturnStatus, CBIOS_ARGUMENTS *pCBiosArguments);

void SetTimingRegs(UCHAR ucDisplayPath, MODE_INFO *pModeInfo, RRATE_TABLE *pRRateTable);
void SetHTotal(UCHAR DisplayPath, USHORT Value);
void SetHDisplayEnd(UCHAR DisplayPath, USHORT Value);
void SetHBlankingStart(UCHAR DisplayPath, USHORT Value);
void SetHBlankingEnd(UCHAR DisplayPath, USHORT Value);
void SetHSyncStart(UCHAR DisplayPath, USHORT Value);
void SetHSyncEnd(UCHAR DisplayPath, USHORT Value);
void SetVTotal(UCHAR DisplayPath, USHORT Value);
void SetVDisplayEnd(UCHAR DisplayPath, USHORT Value);
void SetVBlankingStart(UCHAR DisplayPath, USHORT Value);
void SetVBlankingEnd(UCHAR DisplayPath, USHORT Value);
void SetVSyncStart(UCHAR DisplayPath, USHORT Value);
void SetVSyncEnd(UCHAR DisplayPath, USHORT Value);
void SetPixelClock(UCHAR DisplayPath, ULONG Clock);
void SetHSource(UCHAR bDisplayPath, USHORT wValue);
void SetHorDownScaleSrcSize(USHORT wValue);
void SetVerDownScaleSrcSize(USHORT wValue);
PLL_Info ClockToPLLF9003A(ULONG Clock);
void SetDPLL(UCHAR DisplayPath, PLL_Info PLLInfo);
void SetPolarity(UCHAR DevicePort, UCHAR Value);
void SetFIFO(UCHAR DisplayPath);
void SetPitch(UCHAR DisplayPath, USHORT Value);
USHORT GetPitch(UCHAR DisplayPath);
USHORT GetVDisplayEnd(UCHAR DisplayPath);
void SetColorDepth(UCHAR DisplayPath, UCHAR Value);
void ConfigDigitalPort(UCHAR bDisplayPath);
void LoadTiming(UCHAR DisplayPath, USHORT ModeNum);
void LoadVESATiming(UCHAR bDisplayPath, USHORT wModeNum);
void LoadLCDTiming(UCHAR bDisplayPath, USHORT wModeNum);
void LoadTVTiming(UCHAR bDisplayPath, USHORT wModeNum, UCHAR bDeviceIndex);
void SetScalingFactor(UCHAR bDisplayPath, MODE_INFO *pUserModeInfo, MODE_INFO *pPanelModeInfo);
void SetHorScalingFactor(UCHAR bDisplayPath, USHORT wValue);
void SetVerScalingFactor(UCHAR bDisplayPath, USHORT wValue);
void SetHorDownScalingFactor(UCHAR bDisplayPath, USHORT wValue);
void SetVerDownScalingFactor(UCHAR bDisplayPath, USHORT wValue);
CI_STATUS isLCDFitMode(UCHAR bDeviceIndex, USHORT wModeNum);
CI_STATUS GetModePointerFromVESATable(USHORT wModeNum, UCHAR bRRIndex, MODE_INFO **ppModeInfo, RRATE_TABLE **ppRRateTable);
CI_STATUS GetModePointerFromLCDTable(UCHAR bDeviceIndex, MODE_INFO **ppModeInfo, PANEL_TABLE **ppPanelTable);
CI_STATUS Get_MODE_INFO(USHORT wModeNum, MODE_INFO **ppModeInfo);
CI_STATUS Get_MODE_INFO_From_LCD_Table(UCHAR bDeviceIndex, MODE_INFO **ppModeInfo);
CI_STATUS Get_MODE_INFO_From_VESA_Table(USHORT wModeNum, MODE_INFO **ppModeInfo);
CI_STATUS GetModeColorDepth(USHORT wModeNum, MODE_INFO *pModeInfo, UCHAR *pbColorDepth);

CI_STATUS GetModePitch(USHORT ModeNum, USHORT *pPitch);
USHORT ReadRegFromHW(REG_OP *pRegOp);
void WriteRegToHW(REG_OP *pRegOp, USHORT value);
void UnLockCR0ToCR7();
void LockCR0ToCR7();
CI_STATUS CheckForModeAvailable(USHORT ModeNum);
CI_STATUS CheckForNewDeviceAvailable(UCHAR bDeviceIndex);
void Display1TurnOnTX();
void Display1TurnOffTX();
void Display2TurnOnTX();
void Display2TurnOffTX();
void TurnOnDigitalPort(UCHAR bDeviceIndex);
void TurnOffDigitalPort(UCHAR bDeviceIndex);
UCHAR GetPortConnectPath(UCHAR PortType);
USHORT TransDevIDtoBit(UCHAR DeviceIndex);
void TurnOnCRTPad();
void TurnOffCRTPad();
void TurnOnDVP1Pad();
void TurnOffDVP1Pad();
void TurnOnDVP2Pad();
void TurnOffDVP2Pad();
void TurnOnDVP12Pad();
void TurnOffDVP12Pad();
void TurnOnScaler(UCHAR bDisplayPath);
void TurnOffScaler(UCHAR bDisplayPath);
void TurnOnHorScaler(UCHAR bDisplayPath);
void TurnOffHorScaler(UCHAR bDisplayPath);
void TurnOnVerScaler(UCHAR bDisplayPath);
void TurnOffVerScaler(UCHAR bDisplayPath);
void TurnOnHorDownScaler(UCHAR bDisplayPath);
void TurnOffHorDownScaler(UCHAR bDisplayPath);
void TurnOnVerDownScaler(UCHAR bDisplayPath);
void TurnOffVerDownScaler(UCHAR bDisplayPath);
void Set12BitDVP();
void Set24BitDVP();
void SetDVP1DPA(UCHAR ucDPA);
void SetDVP2DPA(UCHAR ucDPA);
void TurnOnDAC();
void TurnOffDAC();
void SerialLoadTable(UCHAR **ppucDisplay1VESAModeInitRegs, UCHAR ucI2Cport, UCHAR ucI2CAddr);
void SerialLoadRegBits(UCHAR **ppucDisplay1VESAModeInitRegs, UCHAR ucRegGroup, UCHAR ucI2Cport, UCHAR ucI2CAddr);
void SerialLoadReg(UCHAR **ppucDisplay1VESAModeInitRegs, UCHAR ucRegGroup, UCHAR ucI2Cport, UCHAR ucI2CAddr);
void LoadDisplay1VESAModeInitRegs();
void VESASetBIOSData(USHORT ModeNum);
void DisableDisplayPathAndDevice(UCHAR DisplayPath);
CI_STATUS GetDevicePortConfig(UCHAR bDeviceIndex, PORT_CONFIG **ppPortConfig);
void PowerSequenceOn();
void PowerSequenceOff();
void SequencerOn(UCHAR DisplayPath);
void SequencerOff(UCHAR bDisplayPath);
void ControlPwrSeqOn(UCHAR bDeviceIndex);
void ControlPwrSeqOff(UCHAR bDeviceIndex);
void LongWait();
UCHAR Get_DEV_ID(UCHAR DisplayPath);
void Set_DEV_ID(UCHAR DeviceID, UCHAR DisplayPath);
UCHAR Get_NEW_DEV_ID(UCHAR DisplayPath);
void Set_NEW_DEV_ID(UCHAR DeviceID, UCHAR DisplayPath);
USHORT Get_LCD_H_SIZE();
USHORT Get_LCD_V_SIZE();
ULONG Get_LCD_SIZE();
USHORT Get_LCD2_H_SIZE();
USHORT Get_LCD2_V_SIZE();
ULONG Get_LCD2_SIZE();
UCHAR Get_RRATE_ID(UCHAR DisplayPath);
void Set_RRATE_ID(UCHAR RRateID, UCHAR DisplayPath);
UCHAR Get_LCD_TABLE_INDEX(void);
UCHAR Get_LCD2_TABLE_INDEX(void);
void Set_LCD_TABLE_INDEX(UCHAR bLCDIndex);
UCHAR Get_TV_ConnectorType(void);
UCHAR Get_TV_Type(void);
void Set_TV_Confiuration(UCHAR TVConfig);
USHORT Get_VESA_MODE(UCHAR DisplayPath);
void Set_VESA_MODE(USHORT ModeNum, UCHAR DisplayPath);
void ResetATTR();
void EnableATTR();
void SetCRReg(UCHAR bRegIndex, UCHAR bRegValue, UCHAR bMask);
UCHAR GetCRReg(UCHAR bRegIndex);
void SetSRReg(UCHAR bRegIndex, UCHAR bRegValue, UCHAR bMask);
UCHAR GetSRReg(UCHAR bRegIndex);
void SetARReg(UCHAR index,UCHAR value);
UCHAR GetARReg(UCHAR index);
void SetGRReg(UCHAR bRegIndex, UCHAR bRegValue, UCHAR bMask);
void SetMSReg(UCHAR bRegValue);
UCHAR GetIS1Reg();
void ClearFrameBuffer(UCHAR DisplayPath,ULONG *pFrameBufferBase,MODE_INFO *pModeInfo, UCHAR bColorDepth);
ULONG Difference(ULONG Value1, ULONG Value2);
UCHAR ReadScratch(USHORT IndexMask);
void WriteScratch(USHORT IndexMask, UCHAR Data);
UCHAR AlignDataToLSB(UCHAR bData, UCHAR bMask);
UCHAR AlignDataToMask(UCHAR bData, UCHAR bMask);
void SetDPMS(UCHAR DPMSState, UCHAR DisplayPath);
CI_STATUS DetectMonitor();
void WaitDisplayPeriod();
void WaitPowerSequenceDone();
CI_STATUS CheckForDSTNPanel(UCHAR bDeviceIndex);
USHORT GetVESAMEMSize();
void SetDeviceSupport();
void SetTV_CVBS_CCRSLevel(UCHAR *Level);
UCHAR ucGetTV_CVBS_CCRSLevel(UCHAR *Level);


void I2CWriteClock(UCHAR I2CPort, UCHAR data);
void I2CDelay(UCHAR I2CPort);
void I2CWriteData(UCHAR I2CPort, UCHAR data);
void I2CStart(UCHAR I2CPort);
void SendI2CDataByte(UCHAR I2CPort, UCHAR Data);
UCHAR CheckACK(UCHAR I2CPort);
UCHAR ReceiveI2CDataByte(UCHAR I2CPort, UCHAR I2CSlave);
void SendNACK(UCHAR I2CPort);
void I2CStop(UCHAR I2CPort);
UCHAR ReadI2C(UCHAR I2CPort, UCHAR I2CSlave, UCHAR RegIdx, UCHAR* RegData);
UCHAR WriteI2C(UCHAR I2CPort, UCHAR I2CSlave, UCHAR RegIdx, UCHAR RegData);
