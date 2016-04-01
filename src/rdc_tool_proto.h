/*
 * Prototypes for rdc_tool.c
 */
extern Bool RDCMapMem(ScrnInfoPtr pScrn);
extern Bool RDCMapMMIO(ScrnInfoPtr pScrn);
extern Bool RDCMapVBIOS(ScrnInfoPtr pScrn);
extern Bool RDCUnmapMem(ScrnInfoPtr pScrn);
extern Bool RDCUnmapVBIOS(ScrnInfoPtr pScrn);
extern ULONG EC_ReadPortUchar(BYTE *port, BYTE *value);
extern void EC_DetectCaps(ScrnInfoPtr pScrn, ECINFO* pECChip);
extern void EC_WritePortUchar(BYTE *port, BYTE data);
extern void RDCUnmapMMIO(ScrnInfoPtr pScrn);

#undef RDC_EXPORT
#define RDC_EXPORT
