/*
 * External prototypes for rdc_vgatool.c
 */
#ifndef _rdc_vgatool_h_
#define _rdc_vgatool_h_
extern Bool bRDCRegInit(ScrnInfoPtr pScrn);
extern Bool RDCCheckCapture(ScrnInfoPtr pScrn);
extern Bool RDCFilterModeByBandWidth(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern BYTE GetReg(WORD base);
extern CBStatus CBIOS_SetEDIDToModeTable(ScrnInfoPtr pScrn, EDID_DETAILED_TIMING *pEDIDDetailedTiming);
extern ULONG GetVRAMInfo(ScrnInfoPtr pScrn);
extern ULONG RDCGetMemBandWidth(ScrnInfoPtr pScrn);
extern void CreateEDIDDetailedTimingList(UCHAR *ucEdidBuffer, ULONG ulEdidBufferSize, EDID_DETAILED_TIMING *pEDIDDetailedTiming);
extern void GetIndexRegMask(WORD base, BYTE index, BYTE mask, BYTE* val);
extern void GetIndexReg(WORD base, BYTE index, BYTE* val);
extern void ParseEDIDDetailedTiming(UCHAR *pucDetailedTimingBlock, EDID_DETAILED_TIMING *pEDIDDetailedTiming);
extern void RDCDisplayPowerManagementSet(ScrnInfoPtr pScrn, int PowerManagementMode, int flags);
extern void SetIndexRegMask(WORD base, BYTE index, BYTE mask, BYTE val);
extern void SetIndexReg(WORD base, BYTE index, BYTE val);
extern void SetReg(WORD base, BYTE val);
extern void VGA_LOAD_PALETTE_INDEX(BYTE index, BYTE red, BYTE green, BYTE blue);
extern void vRDCLoadPalette(ScrnInfoPtr pScrn, int numColors, int *indices, LOCO *colors, VisualPtr pVisual);
extern void vRDCOpenKey(ScrnInfoPtr pScrn);
extern void vSetDispalyStartAddress(xf86CrtcPtr crtc, int x, int y);
extern void vSetStartAddressCRT1(RDCRecPtr pRDC, ULONG base);
#endif
/*
 * Static prototypes for rdc_vgatool.c
 */
#ifdef _rdc_vgatool_c_
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
