/*
 * External prototypes for rdc_extension.c
 */
#ifndef _rdc_extension_h_
#define _rdc_extension_h_
extern int EC_QueryLCDPWM(RDCRecPtr pRDC, char *level);
extern int EC_SetLCDPWM(RDCRecPtr pRDC, char *level);
extern int RDCGFXUtilityProc(xRDCGFXCommandReq* req);
extern UCHAR ucGetTV_CVBS_CCRSLevel(UCHAR *Level);
extern void CBIOS_SetTVEncDispRegModify(RDCRecPtr pRDC, PTV_Disp_Info pTVDispInfo, BYTE bChange);
extern void GetFS473PositionFromVBIOS(RDCRecPtr pRDC);
extern void GetSAA7105CCRSLevel(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR *Level);
extern void RDCDisplayExtensionInit(ScrnInfoPtr pScrn);
#endif
/*
 * Static prototypes for rdc_extension.c
 */
#ifdef _rdc_extension_c_
static int ProcRDCGFXCommand(ClientPtr client);
static int ProcRDCGFXDispatch(ClientPtr client);
static int ProcRDCGFXQueryVersion (ClientPtr client);
static void RDCGFXResetProc(ExtensionEntry* extEntry);
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
