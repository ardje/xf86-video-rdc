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



typedef struct {
    UCHAR   MISC;
    UCHAR   SEQ[4];
    UCHAR   CRTC[25];
    UCHAR   AReg[20];
    UCHAR   GReg[9];
    
} VBIOS_STDTABLE_STRUCT, *PVBIOS_STDTABLE_STRUCT;

typedef struct {
    UCHAR   ucNS;
    UCHAR   ucMS;
    UCHAR   ucRS;
} VBIOS_DCLK_INFO, *PVBIOS_DCLK_INFO;

typedef struct _RRateInfo{
    float fRefreshRate;
    Bool  BInterlaced;
    UCHAR ucRRateIndex;
}RRateInfo;

typedef struct _MODE_PRIVATE
{
    USHORT  Mode_ID_8bpp;
    USHORT  Mode_ID_16bpp;
    USHORT  Mode_ID_32bpp;
    UCHAR   ucRRate_ID;
} MODE_PRIVATE;


