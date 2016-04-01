/*
 * Prototypes for vidinit.c
 */
extern unsigned long CMD(Cofe *RGB, int op);
extern void CheckBoundary(float *pfValue, float fMin, float fMax);
extern void QDec2Hex( Cofe *RGB );
extern void SetVIDColor(RDCRecPtr pRDC);

#undef RDC_EXPORT
#define RDC_EXPORT
