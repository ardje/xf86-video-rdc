/*
 * Prototypes for rdc_extension.c
 */
extern int EC_QueryLCDPWM(RDCRecPtr pRDC, char *level);
extern int EC_SetLCDPWM(RDCRecPtr pRDC, char *level);
extern int RDCGFXUtilityProc(xRDCGFXCommandReq* req);
extern UCHAR ucGetTV_CVBS_CCRSLevel(UCHAR *Level);
extern void CBIOS_SetTVEncDispRegModify(RDCRecPtr pRDC, PTV_Disp_Info pTVDispInfo, BYTE bChange);
extern void GetFS473PositionFromVBIOS(RDCRecPtr pRDC);
extern void GetSAA7105CCRSLevel(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR *Level);
extern void RDCDisplayExtensionInit(ScrnInfoPtr pScrn);

#undef RDC_EXPORT
#define RDC_EXPORT
