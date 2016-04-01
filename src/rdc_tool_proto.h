/*
 * External prototypes for rdc_tool.c
 */
#ifndef _rdc_tool_h_
#define _rdc_tool_h_
extern Bool RDCMapMem(ScrnInfoPtr pScrn);
extern Bool RDCMapMMIO(ScrnInfoPtr pScrn);
extern Bool RDCMapVBIOS(ScrnInfoPtr pScrn);
extern Bool RDCUnmapMem(ScrnInfoPtr pScrn);
extern Bool RDCUnmapVBIOS(ScrnInfoPtr pScrn);
extern ULONG EC_ReadPortUchar(BYTE *port, BYTE *value);
extern void EC_DetectCaps(ScrnInfoPtr pScrn, ECINFO* pECChip);
extern void EC_WritePortUchar(BYTE *port, BYTE data);
extern void RDCUnmapMMIO(ScrnInfoPtr pScrn);
#endif
/*
 * Static prototypes for rdc_tool.c
 */
#ifdef _rdc_tool_c_
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
