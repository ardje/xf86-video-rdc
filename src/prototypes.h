/*
 * Prototypes for CInt10.c
 */
extern BYTE AlignDataToLSB(BYTE Data, BYTE Mask);
extern BYTE AlignDataToMask(BYTE Data, BYTE Mask);
extern BYTE bHDMIUnscan(BYTE ucHDMIType);
extern BYTE CBIOSCheckACK(BYTE I2CPort);
extern BYTE CBIOSCheckDeviceAvailable(BYTE NewDisplay1DeviceID);
extern BYTE CBIOSReadI2C(BYTE I2CPort, BYTE I2CSlave, BYTE RegIdx, BYTE* RegData);
extern BYTE CBIOSReceiveI2CDataByte(BYTE I2CPort, BYTE I2CSlave);
extern BYTE CBIOSWriteI2C(BYTE I2CPort, BYTE I2CSlave, BYTE RegIdx, BYTE RegData);
extern BYTE DetectMonitor(BYTE I2CPort);
extern BYTE GetARReg(BYTE index);
extern BYTE GetCRReg(BYTE bRegIndex);
extern BYTE GetDevicePort(BYTE DeviceIndex);
extern BYTE GetDevicePortConfig(BYTE DeviceIndex);
extern BYTE Get_DEV_ID(BYTE DisplayPath);
extern BYTE Get_LVDS_TX_ID();
extern BYTE Get_NEW_DEV_ID(BYTE DisplayPath);
extern BYTE GetPortConnectPath(BYTE PortType);
extern BYTE Get_RRATE_ID(BYTE DisplayPath);
extern BYTE GetSRReg(BYTE bRegIndex);
extern BYTE GetVBIOSBootUpDevice(WORD* ucDeiceBit);
extern BYTE ReadScratch(WORD IndexMask);
extern BYTE SenseCRT();
extern CBStatus CheckForModeAvailable(WORD ModeNum);
extern CBStatus CInt10(CBIOS_Extension *pCBIOSExtension);
extern CBStatus GetModeColorDepth(WORD ModeNum, MODE_INFO *pModeInfo, BYTE *pColorDepth);
extern CBStatus Get_MODE_INFO_From_HDMI_Table(WORD ModeNum, MODE_INFO **ppModeInfo);
extern CBStatus Get_MODE_INFO_From_LCD_Table(WORD ModeNum, MODE_INFO **ppModeInfo);
extern CBStatus Get_MODE_INFO_From_VESA_Table(WORD ModeNum, MODE_INFO **ppModeInfo);
extern CBStatus GetModePitch(BYTE DisplayPath, WORD ModeNum, WORD *pPitch);
extern CBStatus GetModePointerFromHDMITable(WORD ModeNum, BYTE RRIndex, MODE_INFO **ppModeInfo, RRATE_TABLE **ppRRateTable);
extern CBStatus GetModePointerFromVESATable(WORD ModeNum, BYTE RRIndex, MODE_INFO **ppModeInfo, RRATE_TABLE **ppRRateTable);
extern CBStatus OEM_QueryBiosCaps (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_QueryBiosInfo (PCBIOS_Extension pCBIOSExtension);
extern CBStatus OEM_QueryDeviceConnectStatus (PCBIOS_Extension pCBIOSExtension);
extern CBStatus OEM_QueryDisplayPathInfo (PCBIOS_Extension pCBIOSExtension);
extern CBStatus OEM_QueryExternalDeviceInfo (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_QueryHDMISupportedMode (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_QueryLCD2PanelSizeMode (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_QueryLCDPanelSizeMode (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_QuerySupportedMode (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_QueryTV2Configuration (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_QueryTVConfiguration (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetActiveDisplayDevice (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetDevicePowerState (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetDisplay2Pitch(CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetHDMIOutputSignal(CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetHDMIType(CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetRefreshRate(CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetSetTVConfiguration(CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetSetTVFunction(CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetTV2Configuration (CBIOS_ARGUMENTS *pCBiosArguments);
extern CBStatus OEM_SetVESAModeForDisplay2(CBIOS_Extension *pCBIOSExtension);
extern CBStatus OEM_VideoPOST (PCBIOS_Extension pCBIOSExtension);
extern CBStatus OEM_VSetEDIDInModeTable(CBIOS_Extension *pCBIOSExtension);
extern CBStatus VBE_AccessEDID(CBIOS_Extension *CBIOSExtension);
extern CBStatus VBE_SetMode(CBIOS_Extension *pCBIOSExtension);
extern CBStatus VBE_SetPitch(CBIOS_Extension *CBIOSExtension);
extern DWORD CBIOSGetPortI2CInfo(void);
extern DWORD Difference(DWORD Value1, DWORD Value2);
extern DWORD Get_LCD2_Panel_Size();
extern DWORD Get_LCD_Panel_Size();
extern PLL_Info ClockToPLLF4002A(DWORD Clock);
extern PLL_Info ClockToPLLF9003A(DWORD Clock);
extern void cbGetHDMIModeInfo(WORD* wHSize, WORD* wVSize, BYTE* bRRCnt, BYTE ucHDMIType);
extern void CBIOSGetDeviceI2CInformation(BYTE ucDevice, BYTE* ucI2Cport, BYTE* ucI2CAddress);
extern void CBIOSGetHDMIPanelSize(BYTE ucHDMIType, WORD* wHSize, WORD* wVSize);
extern void CBIOSGetSupportDevice(PCBIOS_Extension pCBIOSExtension);
extern void CBIOSGetTMDSTxType(BYTE ucI2CPort);
extern void CBIOSGetVBIOSBuildDate(PCBIOS_Extension pCBIOSExtension);
extern void CBIOSGetVBIOSRomDeviceID(PCBIOS_Extension pCBIOSExtension);
extern void CBIOSGetVBIOSVersion(PCBIOS_Extension pCBIOSExtension);
extern void CBIOSI2CDelay(BYTE I2CPort);
extern void CBIOSI2CStart(BYTE I2CPort);
extern void CBIOSI2CStop(BYTE I2CPort);
extern void CBIOSI2CWriteClock(BYTE I2CPort, BYTE data);
extern void CBIOSI2CWriteData(BYTE I2CPort, BYTE data);
extern void CBIOSInitHDMITable(BYTE* pjROMLinearAddr);
extern void CBIOSInitialDataFromVBIOS(PCBIOS_Extension pCBIOSExtension);
extern void CBIOSInitialI2CReg(void);
extern void CBIOSInitLCD();
extern void CBIOSInitLCDTable(BYTE* pjROMLinearAddr);
extern void CBIOSInitPortConfig(BYTE* pjROMLinearAddr);
extern void CBIOSSendI2CDataByte(BYTE I2CPort, BYTE Data);
extern void CBIOSSendNACK(BYTE I2CPort);
extern void CBIOSSetDither();
extern void CBIOSSetOpenLDI(WORD wAttribute);
extern void CBIOSSetOutFormat(WORD wAttribute);
extern void CBIOSSetTD0(WORD wTD0);
extern void CBIOSSetTD1(WORD wTD1);
extern void CBIOSSetTD2(WORD wTD2);
extern void CBIOSSetTD3(WORD wTD3);
extern void CBIOSSetTD5(WORD wTD5);
extern void CBIOSSetTD6(WORD wTD6);
extern void CBIOSSetTD7(WORD wTD7);
extern void CBIOSSetTXType(PCBIOS_Extension pCBIOSExtension);
extern void CBIOSUpdateDevSupFlag(PCBIOS_Extension pCBIOSExtension);
extern void cbSetHDMIModeInfo(WORD wHSize, WORD wVSize, BYTE ucHDMIType);
extern void CheckLCDSyncStartValue(BYTE DisplayPath) ;
extern void ClearFrameBuffer(BYTE DisplayPath, DWORD *pFrameBufferBase, DWORD ulWidth, DWORD ulHeight, BYTE bColorDepth);
extern void ConfigDigitalPort(BYTE DisplayPath);
extern void DisableDisplayPathAndDevice(BYTE DisplayPath);
extern void DisableLCDDither();
extern void DisableSWPSByPass();
extern void EnableATTR();
extern void EnableSWPSByPass();
extern void HandleShareConnector(BYTE *pDeviceIndex);
extern void LoadDisplay1VESAModeInitRegs();
extern void LoadHDMITiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
extern void LoadLCDTiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
extern void LoadPowerSequenceTimer(void);
extern void LoadTiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
extern void LoadVESATiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
extern void LockCR0ToCR7();
extern void LockTiming(BYTE DisplayPath);
extern void PowerSequenceOff();
extern void PowerSequenceOn();
extern void ResetATTR();
extern void ResetLVDSChannelOutPut();
extern void SequencerOff(BYTE DisplayPath);
extern void SequencerOn(BYTE DisplayPath);
extern void SerialLoadTable(REG_PACKAGE *pREG_PACKAGE_Table);
extern void Set12BitDVP();
extern void Set24BitDVP();
extern void SetARReg(BYTE index,BYTE value);
extern void SetColorDepth(BYTE DisplayPath, BYTE Value);
extern void SetCRReg(BYTE bRegIndex, BYTE bRegValue, BYTE bMask);
extern void Set_DEV_ID(BYTE DeviceID, BYTE DisplayPath);
extern void SetDisplay1DownScalingFactor(MODE_INFO *pModeInfo, MODE_INFO *pPanelInfo);
extern void SetDisplay1UpScalingFactor(MODE_INFO *pModeInfo, MODE_INFO *pPanelInfo);
extern void SetDisplay2UpScalingFactor(MODE_INFO *pModeInfo, MODE_INFO *pPanelInfo);
extern void SetDPAReg(BYTE ucDPADelay, BYTE ucDeviceID);
extern void SetDPMS(BYTE DPMSState, BYTE DisplayPath);
extern void SetDrivingReg(BYTE ucDrivingStatus);
extern void SetF4002APLLReg(BYTE DisplayPath, PLL_Info PLLInfo);
extern void SetF9003APLLReg(BYTE DisplayPath, PLL_Info PLLInfo);
extern void SetFIFO(BYTE DisplayPath);
extern void SetGPIOResetReg(BYTE bGPIOMask);
extern void SetGRReg(BYTE bRegIndex, BYTE bRegValue, BYTE bMask);
extern void SetHBlankingEnd(BYTE DisplayPath, WORD Value);
extern void SetHBlankingStart(BYTE DisplayPath, WORD Value);
extern void SetHDisplayEnd(BYTE DisplayPath, WORD Value);
extern void SetHSyncEnd(BYTE DisplayPath, WORD Value);
extern void SetHSyncStart(BYTE DisplayPath, WORD Value);
extern void SetHTotal(BYTE DisplayPath, WORD Value);
extern void  Set_LCD2_Panel_Size(DWORD dwHres,DWORD dwVres);
extern void  Set_LCD_Panel_Size(DWORD dwHres,DWORD dwVres);
extern void SetMSReg(BYTE bRegValue);
extern void Set_NEW_DEV_ID(BYTE DeviceID, BYTE DisplayPath);
extern void SetPitch(BYTE DisplayPath, WORD Value);
extern void SetPixelClock(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, DWORD Clock);
extern void SetPolarity(BYTE DevicePort, WORD wValue);
extern void Set_RRATE_ID(BYTE RRateID, BYTE DisplayPath);
extern void SetSRReg(BYTE bRegIndex, BYTE bRegValue, BYTE bMask);
extern void SetTimingCentering(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, MODE_INFO *pModeInfo, RRATE_TABLE *pRRateTable);
extern void SetTimingRegs(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, MODE_INFO *pModeInfo, RRATE_TABLE *pRRateTable);
extern void SetVBERerurnStatus(WORD VBEReturnStatus, CBIOS_ARGUMENTS *pCBiosArguments);
extern void SetVBlankingEnd(BYTE DisplayPath, WORD Value);
extern void SetVBlankingStart(BYTE DisplayPath, WORD Value);
extern void SetVDisplayEnd(BYTE DisplayPath, WORD Value);
extern void Set_VESA_MODE(WORD ModeNum, BYTE DisplayPath);
extern void SetVSyncEnd(BYTE DisplayPath, WORD Value);
extern void SetVSyncStart(BYTE DisplayPath, WORD Value);
extern void SetVTotal(BYTE DisplayPath, WORD Value);
extern void TurnOffCRTMask();
extern void TurnOffCRTPad();
extern void TurnOffDigitalPort(BYTE DisplayPath);
extern void TurnOffDVP12Pad();
extern void TurnOffDVP1Mask();
extern void TurnOffDVP1Pad();
extern void TurnOffDVP2Mask();
extern void TurnOffDVP2Pad();
extern void TurnOffDVPMask(BYTE bDigitalDevice);
extern void TurnOffScaler(BYTE DisplayPath);
extern void TurnOffTxEncReg(BYTE DisplayPath, BYTE bDigitalDevice);
extern void TurnOnCRTMask();
extern void TurnOnCRTPad();
extern void TurnOnDigitalPort(BYTE DisplayPath);
extern void TurnOnDVP12Pad();
extern void TurnOnDVP1Mask();
extern void TurnOnDVP1Pad();
extern void TurnOnDVP2Mask();
extern void TurnOnDVP2Pad();
extern void TurnOnDVPMask(BYTE bDigitalDevice);
extern void TurnOnScaler(BYTE DisplayPath);
extern void TurnOnTxEncReg(BYTE DisplayPath ,BYTE bDigitalDevice);
extern void UnLockCR0ToCR7();
extern void UnLockTiming(BYTE DisplayPath);
extern void WaitDisplayPeriod();
extern void WaitLong();
extern void WaitPowerSequenceDone();
extern void WriteRegToHW(REG_OP *pRegOp, WORD value);
extern void WriteScratch(WORD IndexMask, BYTE Data);
extern WORD ConvertMStoCount(WORD wTDX);
extern WORD GetPitch(BYTE DisplayPath);
extern WORD GetVDisplayEnd(BYTE DisplayPath);
extern WORD Get_VESA_MODE(BYTE DisplayPath);
extern WORD ReadRegFromHW(REG_OP *pRegOp);
extern WORD TransDevIDtoBit(BYTE DeviceIndex);

/*
 * Prototypes for HDMI.c
 */

/*
 * Prototypes for TV.c
 */

/*
 * Prototypes for gamma.c
 */

/*
 * Prototypes for rdc_2dtool.c
 */
extern Bool bCREnable2D(RDCRecPtr pRDC);
extern Bool bCREnableCMDQ(RDCRecPtr pRDC);
extern Bool bCRInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern Bool bEnable2D(RDCRecPtr pRDC);
extern Bool bEnableCMDQ(RDCRecPtr pRDC);
extern Bool bInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern int FireCRCMDQ(int iDev, unsigned int *pCmd, unsigned int size);
extern UCHAR *pjRequestCMDQ( RDCRecPtr pRDC, ULONG   ulDataLen);
extern unsigned int GetFBIFCaps(int iDev);
extern void vCRDisable2D(RDCRecPtr pRDC);
extern void vCRWaitEngIdle(RDCRecPtr pRDC);
extern void vDisable2D(RDCRecPtr pRDC);
extern void vWaitEngIdle(RDCRecPtr pRDC);

/*
 * Prototypes for rdc_accel.c
 */

/*
 * Prototypes for rdc_cursor.c
 */

/*
 * Prototypes for rdc_driver.c
 */
extern Bool RDCCloseScreen(ScreenPtr pScreen);
extern Bool RDCDriverFunc(ScrnInfoPtr pScrn, xorgDriverFuncOp op, pointer data);
extern Bool RDCGetRec(ScrnInfoPtr pScrn);
extern Bool RDCModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern Bool RDCRandRGetInfo(ScrnInfoPtr pScrn, Rotation *rotations);
extern Bool RDCRandRSetConfig(ScrnInfoPtr pScrn, xorgRRConfig *config);
extern Bool RDCSaveScreen(ScreenPtr pScreen, Bool unblack);
extern Bool RDCSwitchMode(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern const OptionInfoRec * RDCAvailableOptions(int chipid, int busid);
extern const OptionInfoRec *RDCAvailableOptions(int chipid, int busid);;
extern void RDCAdjustFrame(ScrnInfoPtr pScrn, int x, int y);
extern void RDCApertureInit(ScrnInfoPtr pScrn);
extern void RDCFreeRec(ScrnInfoPtr pScrn);
extern void RDCPointerMoved(ScrnInfoPtr pScrn, int x, int y);
extern void RDCProbeDDC(ScrnInfoPtr pScrn, int index);
extern void RDCRestore(ScrnInfoPtr pScrn);
extern void RDCSave(ScrnInfoPtr pScrn);
extern void RDCSetHWCaps(RDCRecPtr pRDC);
extern void TurnDirectAccessFBON(ScrnInfoPtr pScrn, Bool bTurnOn);
extern void vFillRDCModeInfo (ScrnInfoPtr pScrn);
extern void vUpdateHDMIFakeMode(ScrnInfoPtr pScrn);
extern xf86MonPtr RDCDoDDC(ScrnInfoPtr pScrn, int index);

/*
 * Prototypes for rdc_extension.c
 */

/*
 * Prototypes for rdc_mode.c
 */

/*
 * Prototypes for rdc_tool.c
 */

/*
 * Prototypes for rdc_vgatool.c
 */

/*
 * Prototypes for rdc_video.c
 */

/*
 * Prototypes for rdcdual_display.c
 */

/*
 * Prototypes for rdcdual_driver.c
 */

/*
 * Prototypes for vidinit.c
 */

#define RDC_EXPORT
