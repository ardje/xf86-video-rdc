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

#define     XPCBIOS             0
#define     XORGCBIOS           1

#if XPCBIOS
#include "Minidrv.h"
    #if WDDM_DRV
    
        #define CBIOSDebugPrint(x)
    #else
    
        #define CBIOSDebugPrint(x) VideoPortDebugPrint x 
    #endif
#endif

#if XORGCBIOS
#include "xf86.h"
#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#define RL2D(...) xf86DrvMsgVerb(0, X_INFO, 0,__FILE__ "(" STRINGIZE(__LINE__) "): " __VA_ARGS__)
#define RL2DSTRIP(n,...) RL2D(__VA_ARGS__)
#define CBIOSDebugPrint(n) RL2DSTRIP n 
#endif

