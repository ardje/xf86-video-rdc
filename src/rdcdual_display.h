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
#ifndef _RDCDUAL_DISPLAY_H_
#define _RDCDUAL_DISPLAY_H_

#include "xorgVersion.h"


void rdcPipeSetBase(xf86CrtcPtr crtc, int x, int y);
void rdcWaitForVblank(ScrnInfoPtr pScrn);
void rdc0DescribeOutputConfiguration(ScrnInfoPtr pScrn);
void rdc_set_new_crtc_bo(ScrnInfoPtr pScrn);
void rdc_crtc_disable(xf86CrtcPtr crtc, Bool disable_pipe);

xf86CrtcPtr rdcGetLoadDetectPipe(xf86OutputPtr output, DisplayModePtr mode, int *dpms_mode);
void rdcReleaseLoadDetectPipe(xf86OutputPtr output, int dpms_mode);
void rdc_crtc_init(ScrnInfoPtr pScrn, int pipe);
DisplayModePtr i830_crtc_mode_get(ScrnInfoPtr pScrn, xf86CrtcPtr crtc);
void rdc_output_prepare (xf86OutputPtr output);
void rdc_output_commit (xf86OutputPtr output);

#endif
