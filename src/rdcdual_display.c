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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <math.h>
#include <sys/ioctl.h>

#include "xf86.h"
#include "rdc.h"
#include "rdcdual_display.h"
#include "xf86Modes.h"
#include "rdc_mode.h"

#ifdef HAVE_DUAL

extern Bool RDCModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode);

Bool GetDisaplyStatus(ScrnInfoPtr pScrn)
{
    xf86CrtcConfigPtr xf86_config = XF86_CRTC_CONFIG_PTR(pScrn);
    RDCRecPtr pRDC = RDCPTR(pScrn);   
    int i,j, active_outputs = 0;
    Bool bEnableDual = FALSE;
    xf86CrtcPtr crtc;

    for (i = 0; i < xf86_config->num_crtc; i++)
    {
        crtc = xf86_config->crtc[i];
        
        for (j = 0; j < xf86_config->num_output; j++)
        {
            xf86OutputPtr  output = xf86_config->output[j];
            RDCOutputPrivatePtr rdc_output = output->driver_private;
            if (output->crtc == crtc && rdc_output->dpms_mode == DPMSModeOn)
    	        active_outputs++;             
        }
    }
    
    if(active_outputs == 2)
    {
        xf86CrtcPtr  crtc1 = xf86_config->crtc[0];
        xf86CrtcPtr  crtc2 = xf86_config->crtc[1]; 
        int         HDispaly_total = crtc1->mode.HDisplay + crtc2->mode.HDisplay;
        if(HDispaly_total == pScrn->virtualX)
            bEnableDual = TRUE;
    }

    return bEnableDual;
}


static void
rdc_crtc_dpms(xf86CrtcPtr crtc, int mode)
{
    ScrnInfoPtr pScrn = crtc->scrn;
    RDCRecPtr pRDC = RDCPTR(pScrn);
    RDCCrtcPrivatePtr rdc_crtc = crtc->driver_private;
    int pipe = rdc_crtc->pipe;
    Bool disable_pipe = TRUE;

    
    switch (mode) {
    case DPMSModeOn:
    case DPMSModeStandby:
    case DPMSModeSuspend:
	
	break;
    case DPMSModeOff:
	    rdc_crtc->enabled = FALSE;
	break;
    }

    rdc_crtc->dpms_mode = mode;
}

static Bool
rdc_crtc_lock (xf86CrtcPtr crtc)
{
    

    return FALSE;
}

static void
rdc_crtc_unlock (xf86CrtcPtr crtc)
{

}

static Bool
rdc_crtc_mode_fixup(xf86CrtcPtr crtc, DisplayModePtr mode,
		     DisplayModePtr adjusted_mode)
{
    return TRUE;
}

static void
rdc_crtc_prepare (xf86CrtcPtr crtc)
{
    RDCCrtcPrivatePtr	rdc_crtc = crtc->driver_private;
}



static void
rdc_crtc_mode_set(xf86CrtcPtr crtc, DisplayModePtr mode,
		   DisplayModePtr adjusted_mode,
		   int x, int y)
{
    ScrnInfoPtr pScrn = crtc->scrn;
    xf86CrtcConfigPtr   xf86_config = XF86_CRTC_CONFIG_PTR(pScrn);
    RDCRecPtr pRDC = RDCPTR(pScrn);
    RDCCrtcPrivatePtr rdc_crtc = crtc->driver_private;
    int i;    
    DisplayModePtr currmode = pRDC->modePool;
    

    while(currmode)
    {            
        if(adjusted_mode->VDisplay == currmode->VDisplay &&
           adjusted_mode->HDisplay == currmode->HDisplay &&
           adjusted_mode->Clock == currmode->Clock)
        {                     
            break;
        }
        currmode = currmode->next;
    }


    for (i = 0; i < xf86_config->num_output; i++)
    {
        xf86OutputPtr  output = xf86_config->output[i];
        RDCOutputPrivatePtr rdc_output = output->driver_private;
        
	    if (output->crtc != crtc)
	        continue;


        if(rdc_output->type == RDC_OUTPUT_ANALOG)
            RDCModeInit(pScrn, currmode);
        else if(rdc_output->type == RDC_OUTPUT_HDMI)
            RDCModeInit(pScrn, currmode);

        vSetDispalyStartAddress(crtc,crtc->x,crtc->y);                  
    }

}


static void
rdc_crtc_commit (xf86CrtcPtr crtc)
{
    RDCCrtcPrivatePtr	rdc_crtc = crtc->driver_private;
    Bool		deactivate = FALSE;
    rdc_crtc->enabled = TRUE;

}


static void
rdc_crtc_gamma_set(xf86CrtcPtr crtc, CARD16 *red, CARD16 *green, CARD16 *blue,
		    int size)
{
    RDCCrtcPrivatePtr rdc_crtc = crtc->driver_private;
}

void
rdc_crtc_set_cursor_colors (xf86CrtcPtr crtc, int bg, int fg)
{
    ScrnInfoPtr		scrn = crtc->scrn;

}

void
rdc_crtc_set_cursor_position (xf86CrtcPtr crtc, int x, int y)
{
    ScrnInfoPtr		scrn = crtc->scrn;

}

void
rdc_crtc_show_cursor (xf86CrtcPtr crtc)
{
    ScrnInfoPtr		scrn = crtc->scrn;

}

void
rdc_crtc_hide_cursor (xf86CrtcPtr crtc)
{
    ScrnInfoPtr		scrn = crtc->scrn; 
}

void 
rdc_crtc_load_cursor_image (xf86CrtcPtr crtc, CARD8 *image)
{
    ScrnInfoPtr		scrn = crtc->scrn;
}

#ifdef ARGB_CURSOR
void
rdc_crtc_load_cursor_argb (xf86CrtcPtr crtc, CARD32 *image)
{
    RDCRecPtr		    pRDC = RDCPTR(crtc->scrn);
    RDCCrtcPrivatePtr	rdc_crtc = RDCCrtcPrivate(crtc);
    int			        pipe = rdc_crtc->pipe;
}
#endif

#if RANDR_13_INTERFACE
static void
rdc_crtc_set_origin(xf86CrtcPtr crtc, int x, int y)
{
    	
}
#endif

static const xf86CrtcFuncsRec rdc_crtc_funcs = {
    .dpms = rdc_crtc_dpms,
    .save = NULL, 
    .restore = NULL, 
    .lock = rdc_crtc_lock,
    .unlock = rdc_crtc_unlock,
    .mode_fixup = rdc_crtc_mode_fixup,
    .prepare = rdc_crtc_prepare,
    .mode_set = rdc_crtc_mode_set,
    .commit = rdc_crtc_commit,
    .gamma_set = rdc_crtc_gamma_set,
    .shadow_create = NULL,
    .shadow_allocate = NULL,
    .shadow_destroy = NULL,
    .set_cursor_colors = rdc_crtc_set_cursor_colors,
    .set_cursor_position = rdc_crtc_set_cursor_position,
    .show_cursor = rdc_crtc_show_cursor,
    .hide_cursor = rdc_crtc_hide_cursor,
    .load_cursor_image = rdc_crtc_load_cursor_image,
    .load_cursor_argb = rdc_crtc_load_cursor_argb,
    .destroy = NULL, 
#if RANDR_13_INTERFACE
    .set_origin = rdc_crtc_set_origin,
#endif
};



void
rdc_crtc_init(ScrnInfoPtr pScrn, int pipe)
{
    xf86CrtcPtr crtc;
    RDCCrtcPrivatePtr rdc_crtc;
    int i;

    crtc = xf86CrtcCreate (pScrn, &rdc_crtc_funcs);
    if (crtc == NULL)
	return;

    rdc_crtc = xnfcalloc (sizeof (RDCCrtcPrivateRec), 1);
    rdc_crtc->pipe = pipe;
    rdc_crtc->dpms_mode = DPMSModeOff;
    rdc_crtc->plane = pipe;

    crtc->driver_private = rdc_crtc;
}         


static void
rdc_crt_dpms(xf86OutputPtr output, int mode)
{
    ScrnInfoPtr	    pScrn = output->scrn;
    RDCRecPtr	        pRDC = RDCPTR(pScrn);
    UCHAR SEQ01, CRB6;


    SEQ01=CRB6=0;

    vRDCOpenKey(pScrn);
   
    switch (mode) 
    {
    case DPMSModeOn:
        
        SEQ01 = 0x00;
        CRB6 = 0x00;
        break;
        
    case DPMSModeStandby:
        
        SEQ01 = 0x20;
        CRB6  = 0x01;     
        break;
        
    case DPMSModeSuspend:
        
        SEQ01 = 0x20;
        CRB6  = 0x02;           
        break;
        
    case DPMSModeOff:
        
        SEQ01 = 0x20;
        CRB6  = 0x03;           
        break;
    }

    SetIndexRegMask(SEQ_INDEX,0x01, 0xDF, SEQ01);
    SetIndexRegMask(COLOR_CRTC_INDEX,0xB6, 0xFC, CRB6);
}

static void
rdc_crt_save (xf86OutputPtr output)
{
    ScrnInfoPtr	pScrn = output->scrn;
    RDCRecPtr	        pRDC = RDCPTR(pScrn);
}

static void
rdc_crt_restore (xf86OutputPtr output)
{
    ScrnInfoPtr	pScrn = output->scrn;
    RDCRecPtr	        pRDC = RDCPTR(pScrn);
}

static int
rdc_crt_mode_valid(xf86OutputPtr output, DisplayModePtr pMode)
{ 
    if(!pMode->Private)
         return MODE_BAD;
         
    return MODE_OK;
}

static Bool
rdc_crt_mode_fixup(xf86OutputPtr output, DisplayModePtr mode,
		    DisplayModePtr adjusted_mode)
{
    return TRUE;
}

void
rdc_output_prepare (xf86OutputPtr output)
{
    output->funcs->dpms (output, DPMSModeOff);
}

void
rdc_output_commit (xf86OutputPtr output)
{
    output->funcs->dpms (output, DPMSModeOn);
}

static void
rdc_crt_mode_set(xf86OutputPtr output, DisplayModePtr mode,
		  DisplayModePtr adjusted_mode)
{
    
}



static xf86OutputStatus
rdc_crt_detect(xf86OutputPtr output)
{  
    return XF86OutputStatusConnected;
}

static DisplayModePtr
rdc_crt_get_modes (xf86OutputPtr output)
{
    ScrnInfoPtr		    pScrn = output->scrn;
    DisplayModePtr      modes;
   
    modes = RDCBuildModePool(pScrn); 
         
    return modes;
}


static void
rdc_crt_destroy (xf86OutputPtr output)
{
    if (output->driver_private)
	xfree (output->driver_private);
}

#ifdef RANDR_GET_CRTC_INTERFACE
static xf86CrtcPtr
rdc_crt_get_crtc(xf86OutputPtr output)
{
    ScrnInfoPtr	pScrn = output->scrn; 
    xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR (pScrn);
    xf86CrtcPtr crtc = config->crtc[0];

    return crtc;
}
#endif

static const xf86OutputFuncsRec rdc_crt_output_funcs = {
    .dpms = rdc_crt_dpms,
    .save = rdc_crt_save,
    .restore = rdc_crt_restore,
    .mode_valid = rdc_crt_mode_valid,
    .mode_fixup = rdc_crt_mode_fixup,
    .prepare = rdc_output_prepare,
    .mode_set = rdc_crt_mode_set,
    .commit = rdc_output_commit,
    .detect = rdc_crt_detect,
    .get_modes = rdc_crt_get_modes,
    .destroy = rdc_crt_destroy,

    

};

void
rdc_crt_init(ScrnInfoPtr pScrn)
{
    xf86OutputPtr	    output;
    RDCOutputPrivatePtr    rdc_output;

    output = xf86OutputCreate (pScrn, &rdc_crt_output_funcs, "VGA");
    if (!output)
	return;
    rdc_output = xnfcalloc (sizeof (RDCOutputPrivateRec), 1);
    if (!rdc_output)
    {
	xf86OutputDestroy (output);
	return;
    }
    rdc_output->type = RDC_OUTPUT_ANALOG;
    
	rdc_output->pipe_mask = ((1 << 0) | (1 << 1));
    rdc_output->clone_mask = ((1 << RDC_OUTPUT_ANALOG) |
			                 (1 << RDC_OUTPUT_DVO_TMDS));
    
    output->driver_private = rdc_output;
    output->interlaceAllowed = FALSE;
    output->doubleScanAllowed = FALSE;
}

static int
rdc_hdmi_mode_valid(xf86OutputPtr output, DisplayModePtr pMode)
{
    if(!pMode->Private)
         return MODE_BAD;

    return MODE_OK;

}


static Bool
rdc_hdmi_mode_fixup(xf86OutputPtr output, DisplayModePtr mode,
		     DisplayModePtr adjusted_mode)
{
    return TRUE;
}


static void
rdc_hdmi_dpms(xf86OutputPtr output, int mode)
{

}

static void
rdc_hdmi_save(xf86OutputPtr output)
{

}

static void
rdc_hdmi_restore(xf86OutputPtr output)
{

}

static void
rdc_hdmi_destroy (xf86OutputPtr output)
{
    RDCOutputPrivatePtr intel_output = output->driver_private;

    if (intel_output != NULL) 
    {
	    xfree(intel_output);
    }
}

static void
rdc_hdmi_mode_set(xf86OutputPtr output, DisplayModePtr mode,
		  DisplayModePtr adjusted_mode)
{
    
}

static xf86OutputStatus
rdc_hdmi_detect(xf86OutputPtr output)
{  
    return XF86OutputStatusConnected;
}

static DisplayModePtr
rdc_hdmi_get_modes (xf86OutputPtr output)
{
    ScrnInfoPtr		    pScrn = output->scrn;
    DisplayModePtr      modes;

    modes = RDCBuildModePool(pScrn);
  
    return modes;

}


#ifdef RANDR_GET_CRTC_INTERFACE
static xf86CrtcPtr
rdc_hdmi_get_crtc(xf86OutputPtr output)
{
    ScrnInfoPtr	pScrn = output->scrn; 
    xf86CrtcConfigPtr config = XF86_CRTC_CONFIG_PTR (pScrn);
    xf86CrtcPtr crtc = config->crtc[1];

    return crtc;
}
#endif

static const xf86OutputFuncsRec rdc_hdmi_output_funcs = {
    .dpms = rdc_hdmi_dpms,
    .save = rdc_hdmi_save,
    .restore = rdc_hdmi_restore,
    .mode_valid = rdc_hdmi_mode_valid,
    .mode_fixup = rdc_hdmi_mode_fixup,
    .prepare = rdc_output_prepare,
    .mode_set = rdc_hdmi_mode_set,
    .commit = rdc_output_commit,
    .detect = rdc_hdmi_detect,
    .get_modes = rdc_hdmi_get_modes,
    .destroy = rdc_hdmi_destroy,
    
};

void
rdc_hdmi_init(pScrn)
{
    xf86OutputPtr	    output;
    RDCOutputPrivatePtr    rdc_output;

    output = xf86OutputCreate (pScrn, &rdc_hdmi_output_funcs, "HDMI");
    if (!output)
	return;
    rdc_output = xnfcalloc (sizeof (RDCOutputPrivateRec), 1);
    if (!rdc_output)
    {
	xf86OutputDestroy (output);
	return;
    }
    
    rdc_output->type = RDC_OUTPUT_HDMI;
	rdc_output->pipe_mask = ((1 << 0) | (1 << 1));
    rdc_output->clone_mask = (1 << RDC_OUTPUT_HDMI);
    
    output->driver_private = rdc_output;
    output->interlaceAllowed = FALSE;
    output->doubleScanAllowed = FALSE;
    
}
#endif
