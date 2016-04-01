/*
 * External prototypes for HDMI.c
 */
#ifndef _HDMI_h_
#define _HDMI_h_
extern BYTE Get_HDMI_Output_Signal(BYTE DisplayPath);
extern BYTE Get_HDMI_TYPE();
extern BYTE Get_TMDS_TX_ID();
extern BYTE QueryHDMIConnectStatus(BYTE QuickCheck);
extern void DisableTMDSReg();
extern void EnableTMDSReg(BYTE ucDeviceID, BYTE ucHDMIType);
extern void GetHDMIVideoCodeID(BYTE ucHDMIType, BYTE* ucHDMIVCID);
extern void SetEP932MInitReg(BYTE ucDeviceID);
extern void SetHDMIInfoframe(BYTE ucHDMIType);
extern void Set_HDMI_Output_Signal(BYTE ucOutputSignal);
extern void Set_HDMI_TYPE(BYTE ucHDMIType);
extern void SetTMDSDPAReg(DWORD dwPixelClock, BYTE ucDeviceID);
extern void SetTMDSTxPW(BYTE ucPowerStatus);
#endif
/*
 * Static prototypes for HDMI.c
 */
#ifdef _HDMI_c_
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
