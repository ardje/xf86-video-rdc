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
 

// Rotation aperture control base for display
#define ROTAP_CTL0                  (ULONG *)(pRDC->MMIOVirtualAddr + 0x500)
#define ROTAP_START_ADDR0           (ULONG *)(pRDC->MMIOVirtualAddr + 0x504)
#define ROTAP_END_ADDR0             (ULONG *)(pRDC->MMIOVirtualAddr + 0x508)
#define ROTAP_PITCH0                (ULONG *)(pRDC->MMIOVirtualAddr + 0x50C)
#define ROTAP_SRC_PITCH_RECI0       (ULONG *)(pRDC->MMIOVirtualAddr + 0x5A0)
#define ROTAP_WH0                   (ULONG *)(pRDC->MMIOVirtualAddr + 0x5A4)
// Rotation BIT define
#define ROTAPS_ENABLE               BIT0
#define ROTAPS_32BPP                BIT1
#define ROTAPS_16BPP                0
#define ROTAPS_QUEUE_STATES         BIT31
#define ROTAPS_SRC_PITCH_MASK       0x000007FF
#define ROTAPS_SRC_PITCH_RECI_MASK  0x00000FFF
#define ROTAPS_DST_PITCH_MASK       0x07FF0000
#define ROTAPS_SRC_RECI_FACTOR      BIT30

// Rotation aperture control base for HW cursor
#define ROTAP_CTL1                  (ULONG *)(pRDC->MMIOVirtualAddr + 0x510)
#define ROTAP_START_ADDR1           (ULONG *)(pRDC->MMIOVirtualAddr + 0x514)
#define ROTAP_END_ADDR1             (ULONG *)(pRDC->MMIOVirtualAddr + 0x518)
#define ROTAP_PITCH1                (ULONG *)(pRDC->MMIOVirtualAddr + 0x51C)
#define ROTAP_SRC_PITCH_RECI1       (ULONG *)(pRDC->MMIOVirtualAddr + 0x5A8)
#define ROTAP_WH1                   (ULONG *)(pRDC->MMIOVirtualAddr + 0x5AC)

