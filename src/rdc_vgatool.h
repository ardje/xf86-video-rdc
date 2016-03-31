/*
 * Copyright (C) 2009 RDC Semiconductor Co.,Ltd
 * All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sub license, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice (including the
 * next paragraph) shall be included in all copies or substantial portions
 * of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
 * IN NO EVENT SHALL PRECISION INSIGHT AND/OR ITS SUPPLIERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * For technical support : 
 *     <rdc_xorg@rdc.com.tw>
 */
 



#define VIDEOMEM_SIZE_04M       0x00400000
#define VIDEOMEM_SIZE_08M       0x00800000
#define VIDEOMEM_SIZE_16M       0x01000000
#define VIDEOMEM_SIZE_32M       0x02000000
#define VIDEOMEM_SIZE_64M       0x04000000
#define VIDEOMEM_SIZE_128M      0x08000000
#define VIDEOMEM_SIZE_256M      0x10000000
#define VIDEOMEM_SIZE_512M      0x20000000

#define AR_PORT_WRITE           (pRDC->RelocateIO + 0x40)
#define MISC_PORT_WRITE         (pRDC->RelocateIO + 0x42)
#define SEQ_PORT                (pRDC->RelocateIO + 0x44)
#define DAC_INDEX_WRITE         (pRDC->RelocateIO + 0x48)
#define DAC_DATA                (pRDC->RelocateIO + 0x49)
#define GR_PORT                 (pRDC->RelocateIO + 0x4E)
#define CRTC_PORT               (pRDC->RelocateIO + 0x54)
#define CRTC_DATA               (pRDC->RelocateIO + 0x55)
#define INPUT_STATUS1_READ      (pRDC->RelocateIO + 0x5A)
#define MISC_PORT_READ          (pRDC->RelocateIO + 0x4C)

#define GetReg(base)                    inb(base)
#define SetReg(base,val)                outb(base,val)

#define GetIndexReg(base,index,val)     \
    do {                                \
        outb(base,index);               \
        val = inb(base+1);              \
    } while (0)
                                        
#define SetIndexReg(base,index, val)    \
    do {                                \
        outb(base,index);               \
        outb(base+1,val);                 \
    } while (0)
                                        
#define GetIndexRegMask(base,index, and, val)   \
    do {                                        \
        outb(base,index);                       \
        val = (inb(base+1) & and);              \
    } while (0)
    
#define SetIndexRegMask(base,index, and, val)   \
    do {                                        \
        UCHAR __Temp;                           \
        outb(base,index);                       \
         __Temp = (inb((base)+1)&(and))|(val & ~(and));  \
        SetIndexReg(base,index,__Temp);         \
    } while (0)

#define VGA_LOAD_PALETTE_INDEX(index, red, green, blue)     \
    {                                                       \
        UCHAR __junk;                                       \
        SetReg(DAC_INDEX_WRITE,(UCHAR)(index));             \
        __junk = GetReg(SEQ_PORT);                          \
        SetReg(DAC_DATA,(UCHAR)(red));                      \
        __junk = GetReg(SEQ_PORT);                          \
        SetReg(DAC_DATA,(UCHAR)(green));                    \
        __junk = GetReg(SEQ_PORT);                          \
        SetReg(DAC_DATA,(UCHAR)(blue));                     \
        __junk = GetReg(SEQ_PORT);                          \
    }
