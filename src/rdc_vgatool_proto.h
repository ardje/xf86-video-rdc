/*
 * Prototypes for rdc_vgatool.c
 */
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

#undef RDC_EXPORT
#define RDC_EXPORT
