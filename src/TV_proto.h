/*
 * Prototypes for TV.c
 */
extern BYTE bSetFS473TimingReg(BYTE bDisplayPath, WORD wModeNum);
extern BYTE bSetSAA7105TimingReg(BYTE bDisplayPath, WORD wModeNum);
extern BYTE Get_TV_CONNECTION_TYPE();
extern BYTE Get_TVEnc_TX_ID();
extern BYTE Get_TV_TYPE();
extern BYTE SenseFS473TV(BYTE I2CPort, BYTE I2CSlave);
extern BYTE SenseSAA7105TV(BYTE I2CPort, BYTE I2CSlave);
extern BYTE SenseTV();
extern void CBIOSInitTV();
extern void CBIOSSetFS473VScalingLevel(PCBIOSTVFun_Disp_Info pCBTVFun);
extern void DisableTVClock();
extern void EnableTVClock();
extern void InitTVEncReg(BYTE ucTVEncID);
extern void LoadTVTiming(CBIOS_Extension *pCBIOSExtension, BYTE DisplayPath, WORD ModeNum);
extern void SetFS473HPosition(BYTE bHPosition);
extern void SetFS473HSCaler(PCBIOSTVFun_Disp_Info pCBTVFun);
extern void SetFS473InitReg();
extern void SetFS473VPosition(BYTE bVPosition);
extern void SetSAA7105InitReg();
extern void SetTVCCRSLevel(BYTE bCCRSLevel);
extern void Set_TV_CONNECTION_TYPE(BYTE ucTVConnectionType);
extern void SetTVDACPower(BYTE bPowerstate);
extern void Set_TV_TYPE(BYTE ucTVType);
extern void UpdatePLLByTVEnc(DWORD dwPixelClock);
extern void vSetFS473CCRSLevel(BYTE bCCRSLevel);
extern void WriteFS473I2CData(BYTE ucIndex, WORD wData);

#undef RDC_EXPORT
#define RDC_EXPORT
