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
 
#define MAX_HWC_WIDTH           64
#define MAX_HWC_HEIGHT          64
#define HWC_SIZE                (MAX_HWC_WIDTH*MAX_HWC_HEIGHT*2)
#define HQ_HWC_SIZE             (MAX_HWC_WIDTH*MAX_HWC_HEIGHT*4)
#define HWC_SIGNATURE_SIZE      32
#define HWC_ALIGN               32

#define HWC_MONO                0
#define HWC_COLOR               1


#define HWC_SIGNATURE_CHECKSUM  0x00
#define HWC_SIGNATURE_SizeX     0x04
#define HWC_SIGNATURE_SizeY     0x08
#define HWC_SIGNATURE_X         0x0C
#define HWC_SIGNATURE_Y         0x10
#define HWC_SIGNATURE_HOTSPOTX  0x14
#define HWC_SIGNATURE_HOTSPOTY  0x18


#define HWC_MMIO_CTRL           0x580
#define HWC_MMIO_OFFSET         0x584
#define HWC_MMIO_POSITION       0x588
#define HWC_MMIO_ADDRESS        0x58C

#include "rdc_cursor_proto.h"
