/*
 * External prototypes for vidinit.c
 */
#ifndef _vidinit_h_
#define _vidinit_h_
extern unsigned long CMD(Cofe *RGB, int op);
extern void CheckBoundary(float *pfValue, float fMin, float fMax);
extern void QDec2Hex( Cofe *RGB );
extern void SetVIDColor(RDCRecPtr pRDC);
#endif
/*
 * Static prototypes for vidinit.c
 */
#ifdef _vidinit_c_
#endif


#undef RDC_EXPORT
#define RDC_EXPORT
#undef RDC_STATIC
#define RDC_STATIC static
