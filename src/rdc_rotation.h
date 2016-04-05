/* 
 * Copyright (C) 2009 RDC Semiconductor Co.,Ltd
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * For technical support : 
 *     <rdc_xorg@rdc.com.tw>
 */



#define ROTAP_CTL0                  (ULONG *)(pRDC->MMIOVirtualAddr + 0x500)
#define ROTAP_START_ADDR0           (ULONG *)(pRDC->MMIOVirtualAddr + 0x504)
#define ROTAP_END_ADDR0             (ULONG *)(pRDC->MMIOVirtualAddr + 0x508)
#define ROTAP_PITCH0                (ULONG *)(pRDC->MMIOVirtualAddr + 0x50C)
#define ROTAP_SRC_PITCH_RECI0       (ULONG *)(pRDC->MMIOVirtualAddr + 0x5A0)
#define ROTAP_WH0                   (ULONG *)(pRDC->MMIOVirtualAddr + 0x5A4)

#define ROTAPS_ENABLE               BIT0
#define ROTAPS_32BPP                BIT1
#define ROTAPS_16BPP                0
#define ROTAPS_QUEUE_STATES         BIT31
#define ROTAPS_SRC_PITCH_MASK       0x000007FF
#define ROTAPS_SRC_PITCH_RECI_MASK  0x00000FFF
#define ROTAPS_DST_PITCH_MASK       0x07FF0000
#define ROTAPS_SRC_RECI_FACTOR      BIT30


#define ROTAP_CTL1                  (ULONG *)(pRDC->MMIOVirtualAddr + 0x510)
#define ROTAP_START_ADDR1           (ULONG *)(pRDC->MMIOVirtualAddr + 0x514)
#define ROTAP_END_ADDR1             (ULONG *)(pRDC->MMIOVirtualAddr + 0x518)
#define ROTAP_PITCH1                (ULONG *)(pRDC->MMIOVirtualAddr + 0x51C)
#define ROTAP_SRC_PITCH_RECI1       (ULONG *)(pRDC->MMIOVirtualAddr + 0x5A8)
#define ROTAP_WH1                   (ULONG *)(pRDC->MMIOVirtualAddr + 0x5AC)

