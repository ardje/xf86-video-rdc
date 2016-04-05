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
