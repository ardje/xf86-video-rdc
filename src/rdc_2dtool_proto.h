/*
 * External prototypes for rdc_2dtool.c
 */
#ifndef _rdc_2dtool_h_
#define _rdc_2dtool_h_
extern Bool bCREnable2D(RDCRecPtr pRDC);
extern Bool bCREnableCMDQ(RDCRecPtr pRDC);
extern Bool bCRInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern Bool bEnable2D(RDCRecPtr pRDC);
extern Bool bEnableCMDQ(RDCRecPtr pRDC);
extern Bool bInitCMDQInfo(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
extern int FireCRCMDQ(int iDev, PKT_SC *pCmd, unsigned int size);
extern UCHAR *pjRequestCMDQ( RDCRecPtr pRDC, ULONG   ulDataLen);
extern unsigned int GetFBIFCaps(int iDev);
extern void vCRDisable2D(RDCRecPtr pRDC);
extern void vCRWaitEngIdle(RDCRecPtr pRDC);
extern void vDisable2D(RDCRecPtr pRDC);
extern void vWaitEngIdle(RDCRecPtr pRDC);
#endif
/*
 * Static prototypes for rdc_2dtool.c
 */
#ifdef _rdc_2dtool_c_
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
