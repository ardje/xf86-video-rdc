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
#include <config.h>
#endif

#define XF86_VERSION_NUMERIC(major,minor,patch,snap,dummy) \
         (((major) * 10000000) + ((minor) * 100000) + ((patch) * 1000) + snap)

#include "xf86.h"
#include "xf86_OSproc.h"
#if GET_ABI_MAJOR(ABI_VIDEODRV_VERSION) < 6
#include "xf86Resources.h"
#include "xf86RAC.h"
#endif
#include "xf86cmap.h"
#include "compiler.h"
#include "vgaHW.h"
#include "mipointer.h"
#include "micmap.h"

#include "fb.h"
#include "regionstr.h"
#include "xf86xv.h"
#include <X11/extensions/Xv.h>
#include "vbe.h"

#include "xf86Pci.h"


#include "xf86fbman.h"


#include "xf86Cursor.h"


#include "xf86Pci.h"


#define _rdcdual_driver_c_
#include "rdc.h"
#include "rdcdual_driver_proto.h"

#include "CInt10FunProto.h"

#ifdef HAVE_DUAL




#define MAX_WIDTH  4096
#define MAX_HEIGHT 1440

RDC_STATIC Bool RDCCreateScreenResources(ScreenPtr pScreen)
{
   ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
   RDCRecPtr pRDC = RDCPTR(pScrn);

   pScreen->CreateScreenResources = pRDC->CreateScreenResources;
   if (!(*pScreen->CreateScreenResources)(pScreen))
      return FALSE;


   return TRUE;
}

RDC_STATIC void RDCBlockHandler(ScreenPtr pScreen, pointer pTimeout, pointer pReadmask)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    RDCRecPtr pRDC = RDCPTR(pScrn);

    pScreen->BlockHandler = pRDC->BlockHandler;

    (*pScreen->BlockHandler) (pScreen, pTimeout, pReadmask);

    pRDC->BlockHandler = pScreen->BlockHandler;
    pScreen->BlockHandler = RDCBlockHandler;
}

RDC_EXPORT Bool RDCCursorInitDual(ScreenPtr pScreen)
{
    ScrnInfoPtr   pScrn = xf86ScreenToScrn(pScreen);
    RDCRecPtr     pRDC = RDCPTR(pScrn);

      
    pRDC->HWCInfo.pjHWCVirtualAddr = pRDC->FBVirtualAddr + pRDC->HWCInfo.ulHWCOffsetAddr; 

    *((volatile ULONG*)(pRDC->MMIOVirtualAddr + HWC_MMIO_CTRL)) = BIT31;

    pRDC->HWCInfo.width = MAX_HWC_WIDTH;
    pRDC->HWCInfo.height = MAX_HWC_WIDTH;
    pRDC->HWCInfo.xhot = 0;
    pRDC->HWCInfo.yhot = 0;
    pRDC->HWCInfo.offset_x = 0;
    pRDC->HWCInfo.offset_y = 0;
    
    return xf86_cursors_init(pScreen, MAX_HWC_WIDTH, MAX_HWC_HEIGHT,
			                (HARDWARE_CURSOR_TRUECOLOR_AT_8BPP |
                             HARDWARE_CURSOR_INVERT_MASK |        
                             HARDWARE_CURSOR_BIT_ORDER_MSBFIRST));
}

RDC_STATIC int rdc_output_clones (ScrnInfoPtr pScrn, int type_mask)
{
    xf86CrtcConfigPtr	config = XF86_CRTC_CONFIG_PTR (pScrn);
    int			o;
    int			index_mask = 0;

    for (o = 0; o < config->num_output; o++)
    {
    	xf86OutputPtr		output = config->output[o];
    	RDCOutputPrivatePtr	rdc_output = output->driver_private;
    	if (type_mask & (1 << rdc_output->type))
    	    index_mask |= (1 << o);
    }
    return index_mask;
}


RDC_STATIC void RDCSetupOutputs(ScrnInfoPtr pScrn)
{
   xf86CrtcConfigPtr	config = XF86_CRTC_CONFIG_PTR (pScrn);
   RDCRecPtr pRDC = RDCPTR(pScrn);
   int	    o, c;

   
   rdc_crt_init(pScrn);

   rdc_hdmi_init(pScrn);
   
   for (o = 0; o < config->num_output; o++)
   {
      xf86OutputPtr	   output = config->output[o];
      RDCOutputPrivatePtr rdc_output = output->driver_private;
      int crtc_mask;
      
      crtc_mask = 0;
      for (c = 0; c < config->num_crtc; c++)
      {
    	 xf86CrtcPtr	      crtc = config->crtc[c];
    	 RDCCrtcPrivatePtr   intel_crtc = crtc->driver_private;

    	 if (rdc_output->pipe_mask & (1 << intel_crtc->pipe))
    	    crtc_mask |= (1 << c);
          }
          output->possible_crtcs = crtc_mask;
          output->possible_clones = rdc_output_clones(pScrn, rdc_output->clone_mask);
   }
}

static DevPrivateKey uxa_pixmap_index;

RDC_STATIC Bool rdc_xf86crtc_resize(ScrnInfoPtr scrn, int width, int height)
{
    PixmapPtr   pixmap;
    int i;
    RDCRecPtr pRDC = RDCPTR(scrn);  
	ScreenPtr   screen = screenInfo.screens[scrn->scrnIndex];
	xf86CrtcConfigPtr   xf86_config = XF86_CRTC_CONFIG_PTR(scrn);
	scrn->displayWidth = (width+31) & ~31;
    int		old_x = scrn->virtualX;
    int		old_y = scrn->virtualY;

    if (old_x == width && old_y == height)
	    return TRUE;

    scrn->virtualX = width;
    scrn->virtualY = height;
    
    pixmap = screen->GetScreenPixmap(screen);

    dixSetPrivate(&screen->devPrivates, &uxa_pixmap_index, NULL);

    scrn->fbOffset = 0;

    screen->ModifyPixmapHeader(screen->GetScreenPixmap(screen),
			   width, height, -1, -1, scrn->displayWidth * 4,
			   pRDC->FBVirtualAddr + scrn->fbOffset);

    scrn->pixmapPrivate.ptr = pRDC->FBVirtualAddr + scrn->fbOffset;


    for (i = 0; i < xf86_config->num_crtc; i++) 
    {
        xf86CrtcPtr crtc = xf86_config->crtc[i];

        if (crtc->enabled && !crtc->transform_in_use)
            vSetDispalyStartAddress(crtc,crtc->x,crtc->y);
    }

    return TRUE;
}

static const xf86CrtcConfigFuncsRec rdc_xf86crtc_config_funcs = {
    rdc_xf86crtc_resize
};


RDC_STATIC Bool rdc_user_modesetting_init(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    int i, num_pipe;
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    vgaRegPtr vgaReg = &hwp->SavedReg;
	num_pipe = 2;
   
    for (i = 0; i < num_pipe; i++)
    {
        rdc_crtc_init(pScrn, i);
    }
    
    RDCSetupOutputs(pScrn);
    
    vgaHWUnlock(hwp);
    vgaHWSave(pScrn, vgaReg, VGA_SR_FONTS);
    
    if (!xf86InitialConfiguration (pScrn, TRUE))
    {
    	xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No valid modes.\n");
    	return FALSE;
    }
    
    vgaHWRestore(pScrn, vgaReg, VGA_SR_FONTS);
    vgaHWLock(hwp);
    
    return TRUE;
}

RDC_STATIC void RDCPreInitCrtcConfig(ScrnInfoPtr pScrn)
{      
    
    xf86CrtcConfigInit(pScrn, &rdc_xf86crtc_config_funcs);

    xf86CrtcSetSizeRange (pScrn, 320, 200, MAX_WIDTH, MAX_HEIGHT);     
}



RDC_STATIC Bool RDCPreInitDual(ScrnInfoPtr pScrn, int flags)
{
    EntityInfoPtr pEnt;
    vbeInfoPtr pVbe;
    vgaHWPtr hwp;
    int flags24;
    rgb defaultWeight = { 0, 0, 0 };
    Gamma zeros = { 0.0, 0.0, 0.0 };
    RDCRecPtr pRDC;
    ClockRangePtr clockRanges;
    int     i;
    char    *s = NULL;
    MessageType from;
    int     maxPitch = 0; 
    int     maxHeight = 0; 
    uint32_t   ulNBID, ulValue;
    long unsigned int uliValue;
#if XSERVER_LIBPCIACCESS
    struct pci_device *dev;
#endif
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCPreInit()==\n");
    
    if (pScrn->numEntities != 1)
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit1 RDCPreInit()== return FALSE\n");
        return FALSE;
    }
 
    pEnt = xf86GetEntityInfo(pScrn->entityList[0]);
 
    if (flags & PROBE_DETECT)
    {
        RDCProbeDDC(pScrn, pEnt->index);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit2 RDCPreInit()== return TRUE\n");
        return TRUE;
    }
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==RDCPreInit()==Before init VBE\n");

	if (xf86LoadSubModule(pScrn, "int10") && xf86LoadSubModule(pScrn, "vbe")) 
	{
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==RDCPreInit()==Load Vbe symbol\n");

        pVbe = VBEExtendedInit(NULL, 
                               pEnt->index,
				               SET_BIOS_SCRATCH | RESTORE_BIOS_SCRATCH);
        if (pVbe == NULL)
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==RDCPreInit()==pVbe = NULL\n");
    }
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==RDCPreInit()==After init VBE\n");
    
    if (pEnt->location.type != BUS_PCI)
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit3 RDCPreInit()== return FALSE\n");
        return FALSE;
    }

#ifndef XSERVER_LIBPCIACCESS
    if (xf86RegisterResources(pEnt->index, 0, ResExclusive))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit4 RDCPreInit()== return FALSE\n");
        return FALSE;
    }
#endif

    
    if (!xf86LoadSubModule(pScrn, "vgahw"))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit5 RDCPreInit()== return FALSE\n");
        return FALSE;
    }

    
    if (!xf86LoadSubModule(pScrn, "fb"))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit6 RDCPreInit()== return FALSE\n");
        return FALSE;
    }
    
    if (!xf86LoadSubModule(pScrn, "ramdac"))
       return FALSE;
               
    
    if (!vgaHWGetHWRec(pScrn))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit7 RDCPreInit()== return FALSE\n");
        return FALSE;
    }
    hwp = VGAHWPTR(pScrn);

    
    flags24 = Support32bppFb;
    
    if (!xf86SetDepthBpp(pScrn, 0, 0, 0, flags24))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit8 RDCPreInit()== return FALSE\n");
        return FALSE;
    }
    else
    {
        switch (pScrn->depth)
        {
            case 8:
            case 16:
            case 24:
                break;

            default:
                xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                           "Given depth (%d) is not supported by RDC driver\n",
                           pScrn->depth);
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit9 RDCPreInit()== return FALSE\n");
                return FALSE;
        }
    }

    xf86PrintDepthBpp(pScrn);

    switch (pScrn->bitsPerPixel)
    {
        case 8:
        case 16:
        case 32:
            break;

        default:
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                       "Given bpp (%d) is not supported by RDC driver\n",
                       pScrn->bitsPerPixel);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit10 RDCPreInit()== return FALSE\n");
            return FALSE;
    }
   
    
    pScrn->progClock = TRUE;
    pScrn->rgbBits = 6;
    pScrn->monitor = pScrn->confScreen->monitor; 
#ifndef XSERVER_LIBPCIACCESS
    pScrn->racMemFlags = RAC_FB | RAC_COLORMAP | RAC_CURSOR | RAC_VIEWPORT;
    pScrn->racIoFlags = RAC_COLORMAP | RAC_CURSOR | RAC_VIEWPORT;
#endif
       
    
    if (!xf86SetGamma(pScrn, zeros)) 
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "call xf86SetGamma failed \n");
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit11 RDCPreInit()== return FALSE\n");
        return FALSE;
    }


    if (!xf86SetWeight(pScrn, defaultWeight, defaultWeight))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit12 RDCPreInit()== return FALSE\n");
        return FALSE;
    }    
 
    if (!xf86SetDefaultVisual(pScrn, -1))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit13 RDCPreInit()== return FALSE\n");
        return FALSE;
    }      

    
    if (!RDCGetRec(pScrn))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "call RDCGetRec failed \n");
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit14 RDCPreInit()== return FALSE\n");
        return FALSE;
    }    

    
    
    pRDC = RDCPTR(pScrn);
    pRDC->pVbe = pVbe;
    pRDC->pEnt    = xf86GetEntityInfo(pScrn->entityList[0]);
    pRDC->PciInfo = xf86GetPciInfoForEntity(pRDC->pEnt->index);
    pRDC->SaveGeneration = -1;
#if !XSERVER_LIBPCIACCESS
    pRDC->PciTag  = pciTag(pRDC->PciInfo->bus, 
                           pRDC->PciInfo->device,
                           pRDC->PciInfo->func);
#endif
    
    pRDC->noHWC = TRUE; 
    pRDC->MMIOVPost = FALSE;
    pRDC->noAccel = TRUE;
    pRDC->SupportDualPath = TRUE;
#ifdef HAVE_XAA
    pRDC->AccelInfoPtr = NULL; 
#endif
    pRDC->MMIO2D = FALSE;
    pRDC->HWCInfoPtr = NULL;
    pRDC->ENGCaps = 0;
    pRDC->DeviceInfo.ScalerConfig.EnableDownScaling = FALSE;
    pRDC->bRandRRotation = FALSE;
    pRDC->rotate = RR_Rotate_0;
  
    
    RDCSetHWCaps(pRDC);

    
    xf86CollectOptions(pScrn, NULL);   
    if (!(pRDC->Options = malloc(sizeof(RDCOptions))))
    {      
        RDCFreeRec(pScrn);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit15 RDCPreInit()== return FALSE\n");
        return FALSE;
    }      
    memcpy(pRDC->Options, RDCOptions, sizeof(RDCOptions));
    xf86ProcessOptions(pScrn->scrnIndex, pScrn->options, pRDC->Options);

    
    if (xf86ReturnOptValBool(pRDC->Options, OPTION_RANDRROTATION, FALSE))
    {
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "Option RandRRotation = true\n");
        pRDC->bRandRRotation = TRUE;
        pRDC->rotate = RR_Rotate_0;
    }

    
    
    if (xf86ReturnOptValBool(pRDC->Options, OPTION_DOWN_SCALE, FALSE))
    {
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "Option DownScale = true\n");
        pRDC->DeviceInfo.ScalerConfig.EnableDownScaling = TRUE;
    }
    
    
    if (xf86ReturnOptValBool(pRDC->Options, OPTION_MMIO2D, FALSE)) 
    {
        pRDC->MMIO2D = TRUE;
    }
 
    
    pRDC->bHRatio = pRDC->bVRatio = 100;
    if (!xf86GetOptValULong(pRDC->Options, OPTION_HRATIO, &uliValue)) 
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "No HDMI underscan horizotal ratio options found\n");          
    }
    else
    {
        if(uliValue > 100)
            pRDC->bHRatio = 100;
	else
	    pRDC->bHRatio = (BYTE) uliValue;
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "HDMI underscan horizotal ratio %d\n",pRDC->bHRatio); 
    }

    
    if (!xf86GetOptValULong(pRDC->Options, OPTION_VRATIO, &uliValue)) 
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "No HDMI underscan vertical ratio options found\n");          
    }
    else
    {      
        if(uliValue > 100)
            pRDC->bVRatio = 100;
	else
	    pRDC->bVRatio = (BYTE) uliValue;
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "HDMI underscan vertical ratio %d\n",pRDC->bVRatio);    
    }

    
    
    if (!xf86ReturnOptValBool(pRDC->Options, OPTION_NOACCEL, TRUE) && (!pRDC->bRandRRotation))
    {
    	if((s = (char *)xf86GetOptValString(pRDC->Options, OPTION_ACCELMETHOD))) 
    	{
    	    if(!xf86NameCmp(s,"XAA")) 
    	    {
    		    pRDC->useEXA = FALSE;
    	    }
    	    else if(!xf86NameCmp(s,"EXA"))
    	    {
    		    pRDC->useEXA = TRUE;
    	    }
    	}
        
        pRDC->noAccel = FALSE; 
    }
    else
    {
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "X-config option disable 2D Accelerator \n");
        pRDC->noAccel = TRUE; 
    }

    
    if (!(pRDC->ENGCaps & ENG_CAP_2D))
    {
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "Engine Caps disable 2D Accelerator \n");
        pRDC->noAccel = TRUE;
    }
  
    
    if (!xf86ReturnOptValBool(pRDC->Options, OPTION_SW_CURSOR, TRUE))
    { 
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "Using Hardware Cursor\n");
        
        pRDC->noHWC = FALSE;  
        pRDC->HWCInfo.HWC_NUM = DEFAULT_HWC_NUM;
        
        if (!xf86GetOptValInteger(pRDC->Options, OPTION_HWC_NUM, &pRDC->HWCInfo.HWC_NUM))
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "No HWC_NUM options found\n");          
        }    
    }
    else
    {
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "X-config option disable Cursor Accelerator \n");
        pRDC->noHWC = TRUE;
        pRDC->HWCInfo.HWC_NUM = 0;
    }
    
    
    if (pRDC->pEnt->device->chipset && *pRDC->pEnt->device->chipset)
    {
        pScrn->chipset = pRDC->pEnt->device->chipset;
        from = X_CONFIG;
    }
    else if (pRDC->pEnt->device->chipID >= 0)
    {
        pScrn->chipset = (char *)xf86TokenToString(RDCChipsets,
                          pRDC->pEnt->device->chipID);
        from = X_CONFIG;
        xf86DrvMsgVerb(pScrn->scrnIndex, X_CONFIG, DefaultLevel, "ChipID override: 0x%04X\n",
                   pRDC->pEnt->device->chipID);
    }
    else 
    {
        from = X_PROBED;
        pScrn->chipset = (char *)xf86TokenToString(RDCChipsets,
                          DEVICE_ID(pRDC->PciInfo));
    }
   
    if (pRDC->pEnt->device->chipRev >= 0)
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_CONFIG, DefaultLevel, "ChipRev override: %d\n",
        pRDC->pEnt->device->chipRev);
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, from, DefaultLevel, "Chipset: \"%s\"\n",
               (pScrn->chipset != NULL) ? pScrn->chipset : "Unknown rdc");

    
#if XF86_VERSION_CURRENT < XF86_VERSION_NUMERIC(4,2,99,0,0)
     pRDC->IODBase = 0;
#else
     pRDC->IODBase = pScrn->domainIOBase;  
#endif
     
#if XSERVER_LIBPCIACCESS
    pRDC->RelocateIO = (ADDRESS)(pRDC->PciInfo->regions[2].base_addr + pRDC->IODBase);
#else    
    pRDC->RelocateIO = (ADDRESS)(pRDC->PciInfo->ioBase[2] + pRDC->IODBase);
#endif
    
    if (pRDC->pEnt->device->MemBase != 0) 
    {
        pRDC->FBPhysAddr = pRDC->pEnt->device->MemBase;
        from = X_CONFIG;
    }
    else
    {
        if (RDC_MEMBASE(pRDC->PciInfo, 0) != 0)
        {
            pRDC->FBPhysAddr = RDC_MEMBASE(pRDC->PciInfo, 0) & 0xFFF00000;
            from = X_PROBED;
        }
        else
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
                       "No valid FB address in PCI config space\n");
            RDCFreeRec(pScrn);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit16 RDCPreInit()== return FALSE\n");
            return FALSE;
        }
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "Linear framebuffer at 0x%lX\n",
               (unsigned long) pRDC->FBPhysAddr);

    if (pRDC->pEnt->device->IOBase != 0) 
    {
        pRDC->MMIOPhysAddr = pRDC->pEnt->device->IOBase;
        from = X_CONFIG;
    }
    else
    {
        if (RDC_MEMBASE(pRDC->PciInfo, 1))
        {
            pRDC->MMIOPhysAddr = RDC_MEMBASE(pRDC->PciInfo, 1) & 0xFFFF0000;
            from = X_PROBED;
        }
        else
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel,
                       "No valid MMIO address in PCI config space\n");
            RDCFreeRec(pScrn);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit17 RDCPreInit()== return FALSE\n");
            return FALSE;
        }
    }
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "IO registers at addr 0x%lX\n",
               (unsigned long) pRDC->MMIOPhysAddr);
    pRDC->MMIOMapSize = DEFAULT_MMIO_SIZE;

    if (!RDCMapMMIO(pScrn)) 
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Map Memory Map IO Failed \n");
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit19 RDCPreInit()== return FALSE\n");
        return FALSE;
    } 
    
    if (!RDCMapVBIOS(pScrn))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "==Exit RDCPreInit()==Map VBIOS Failed \n");
        return FALSE;
    }
    
    xf86CollectOptions(pScrn, NULL);
    
    if (!(pRDC->pCBIOSExtension = malloc(sizeof(CBIOS_Extension))))
    {      
        RDCFreeRec(pScrn);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit25 RDCPreInit()== return FALSE\n");
        return FALSE;
    }      
    memset(pRDC->pCBIOSExtension, 0, sizeof(CBIOS_Extension));

    
    if (!(pRDC->pCBIOSExtension->pCBiosArguments = malloc(sizeof(CBIOS_ARGUMENTS))))
    {      
        RDCFreeRec(pScrn);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit26 RDCPreInit()== return FALSE\n");
        return FALSE;
    }
    pRDC->pCBIOSExtension->pjIOAddress            = pRDC->MMIOVirtualAddr;
    pRDC->pCBIOSExtension->pjROMLinearAddr        = pRDC->BIOSVirtualAddr;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " Initial parameters from VGA BIOS rom \n");
    
    
    CBIOSInitialDataFromVBIOS(pRDC->pCBIOSExtension);
    
    if(pRDC->pCBIOSExtension->wDeviceID==M2012_DEVICE_ID)
    {
        pRDC->pCBIOSExtension->pfnCBIOS_TransVGAPLL  = ClockToPLLF9003A;
        pRDC->pCBIOSExtension->pfnCBIOS_SetVGAPLLReg = SetF9003APLLReg;
    }
    else 
    {
        pRDC->pCBIOSExtension->pfnCBIOS_TransVGAPLL  = ClockToPLLF4002A;
        pRDC->pCBIOSExtension->pfnCBIOS_SetVGAPLLReg = SetF4002APLLReg;
    }

    
    {
        
        CBIOS_ARGUMENTS *pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
        
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " CBIOS Query Bios Info\n");
        
        memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
        pCBiosArguments->Eax = OEMFunction;
        pCBiosArguments->Ebx = QueryBiosInfo;

        
        CInt10(pRDC->pCBIOSExtension);
        
        if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
        {
            
            
            pScrn->videoRam =  ((ULONG)(pCBiosArguments->Ecx & 0xFFFF) >> 4)*256;
            if(pRDC->pCBIOSExtension->wDeviceID==PCI_CHIP_M2200)
                pScrn->videoRam = 4096;
        }
        else
            pScrn->videoRam = GetVRAMInfo(pScrn)/1024;
            
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Video Memory Size= %d MB\n",pScrn->videoRam/1024);

        from = X_DEFAULT;

        if (pRDC->pEnt->device->videoRam) 
        {
            pScrn->videoRam = pRDC->pEnt->device->videoRam;
            from = X_CONFIG;
        }

        pRDC->FbMapSize = pScrn->videoRam * 1024;
        pRDC->AvailableFBsize = pRDC->FbMapSize;        
    
        
        
        if (RDCCheckCapture(pScrn))
        {
            xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "Reserved Capture buffer\n");
            
            
            if (pRDC->FbMapSize < VIDEOMEM_SIZE_16M)
            {
                xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "Video Memory in not enough for Capture & Xorg\n");
                return FALSE;
            }

            
            
            
            pRDC->AvailableFBsize -= CAPTURE_BUFFER_SIZE;
        }


        
        if (pRDC->ENGCaps & ENG_CAP_CR_SUPPORT)
        {
            xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "Reserve CommandQ buffer\n");

            pRDC->CMDQInfo.ulCMDQSize = DEFAULT_CMDQ_SIZE;   
            pRDC->AvailableFBsize = pRDC->AvailableFBsize - pRDC->CMDQInfo.ulCMDQSize;
            pRDC->CMDQInfo.ulCMDQOffsetAddr = pRDC->AvailableFBsize;
        }

        
        if (!pRDC->noHWC)
        {
            xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "Reserved Cursor Buffer\n");
            
            if (pRDC->ENGCaps & ENG_CAP_HWC_MMIO)
                pRDC->AvailableFBsize = 
                    pRDC->AvailableFBsize - (HQ_HWC_SIZE)*pRDC->HWCInfo.HWC_NUM;
            else
                pRDC->AvailableFBsize = 
                    pRDC->AvailableFBsize - (HWC_SIZE+HWC_SIGNATURE_SIZE)*pRDC->HWCInfo.HWC_NUM;
                
            pRDC->HWCInfo.ulHWCOffsetAddr = pRDC->AvailableFBsize;
        }

        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Available memory size= %d B\n",pRDC->AvailableFBsize);
    }

    
    if (!RDCMapMem(pScrn))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Map FB Memory Failed \n");
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit18 RDCPreInit()== return FALSE\n");
        return FALSE;
    }
    pRDC->pCBIOSExtension->pVideoVirtualAddress   = (ULONG*)(pRDC->FBVirtualAddr);
    pRDC->pCBIOSExtension->pVideoPhysicialAddress = pRDC->FBPhysAddr;

    
    {
        
        CBIOS_ARGUMENTS *pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
        
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " CBIOS Set HDMI Type\n");
        
#if XSERVER_LIBPCIACCESS
        dev = pci_device_find_by_slot(0, 0, 0, 0);
        pci_device_cfg_read_u32(dev, &ulValue, CIDOffset);
#else
        ulValue = pciReadLong(pciTag(0, 0, 0), CIDOffset);
#endif
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Customer ID = 0x%x\n",ulValue);
        
        
        memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
        pCBiosArguments->Eax = OEMFunction;
        pCBiosArguments->Ebx = SetHDMIType;

        if (ulValue == DMPDX2)
            pCBiosArguments->CL = HDMI720P;
        else
            pCBiosArguments->CL = HDMI1080P;

        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "HDMI Type = 0x%x\n",pCBiosArguments->CL);
            
         
        CInt10(pRDC->pCBIOSExtension);
    }

    
    vUpdateHDMIFakeMode(pScrn);
    
    
    pRDC->MemoryBandwidth = RDCGetMemBandWidth(pScrn);
    
    pScrn->memPhysBase = (ULONG)pRDC->FBPhysAddr;
    pScrn->fbOffset = 0;

    
    

    
    pRDC->modePool = RDCBuildModePool(pScrn);

    RDCPreInitCrtcConfig(pScrn);
   
   if (!rdc_user_modesetting_init(pScrn))
         return FALSE;
  
    if (pScrn->modes == NULL) {
       xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No modes.\n");
       PreInitCleanup(pScrn);
       return FALSE;
    }
    
    pScrn->currentMode = pScrn->modes;
 
    
    xf86SetDpi(pScrn, 0, 0);
    
    if (!pRDC->noAccel)
    {        
    	if (pRDC->useEXA)
        {
    	    XF86ModReqInfo req;
    	    int errmaj, errmin;
    	    memset(&req, 0, sizeof(req));
     
    	    req.majorversion = 2;
    	    req.minorversion = 0;
    	    
            if (!LoadSubModule(pScrn->module, "exa", NULL, NULL, NULL, &req,
    		                   &errmaj, &errmin)) 
    		{
                xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "Load EXA Module FALSE\n");
        		LoaderErrorMsg(NULL, "exa", errmaj, errmin);
        		RDCFreeRec(pScrn);
        		return FALSE;
    		}
	    }        
	    else
        {
            if (!xf86LoadSubModule(pScrn, "xaa"))
            {
                RDCFreeRec(pScrn);
                xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Exit22 RDCPreInit()== return FALSE\n");
                return FALSE;
            }       
        }
    }
    
    
    if(pRDC->pCBIOSExtension->wDeviceID==PCI_CHIP_M2200)
    {
        pRDC->AvailableFBsize = pRDC->FbMapSize;
    }

    if (pRDC->bRandRRotation)
    {
        pRDC->rotate = RR_Rotate_0;
        RDCApertureInit(pScrn);
    }      

    if (pRDC->ENGCaps & ENG_CAP_CR_SUPPORT)
    {
        pRDC->CMDQInfo.Disable2D = vCRDisable2D;
        pRDC->CMDQInfo.Enable2D = bCREnable2D ;
        pRDC->CMDQInfo.WaitEngIdle = vCRWaitEngIdle;
    }
    else if(pRDC->ENGCaps & ENG_CAP_2D)
    {
        pRDC->CMDQInfo.Disable2D = vDisable2D;
        pRDC->CMDQInfo.Enable2D = bEnable2D;
        pRDC->CMDQInfo.WaitEngIdle = vWaitEngIdle;
    }else 
    {
        pRDC->CMDQInfo.Disable2D   = NULL;
        pRDC->CMDQInfo.Enable2D    = NULL;
        pRDC->CMDQInfo.WaitEngIdle = NULL;
    }
  
    
    if (pRDC->DeviceInfo.ScalerConfig.EnableDownScaling)
        SetIndexRegMask(COLOR_CRTC_INDEX, 0x90, ~BIT4, BIT4);
    else
        SetIndexRegMask(COLOR_CRTC_INDEX, 0x90, ~BIT4, 0);
                
    
    pRDC->bDirectAccessFB = FALSE;
#if XSERVER_LIBPCIACCESS
    dev = pci_device_find_by_slot(0, 0, 0, 0);
    pci_device_cfg_read_u32(dev, &ulNBID, 0x00);
    if(R3308NBID == ulNBID)
    {   
        pci_device_cfg_read_u32(dev, &ulValue, 0x48);
        if (ulValue & DirectAccessFB)
            pRDC->bDirectAccessFB = TRUE;
    }
#else
    ulNBID = pciReadLong(pciTag(0, 0, 0), 0x00);
    if(R3308NBID == ulNBID)
    {   
        ulValue = pciReadLong(pciTag(0, 0, 0), 0x48);
        if (ulValue & DirectAccessFB)
            pRDC->bDirectAccessFB = TRUE;
    }
#endif
        
    
    pRDC->TVEncoderInfo[0].TVOut_HSize = 640;
    pRDC->TVEncoderInfo[0].TVOut_VSize = 480;

    
    pRDC->IoctlCR = FALSE;
    pRDC->iFBDev = open(RDC_DEV, O_RDWR);
    if (pRDC->iFBDev == -1)
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "Open device failed!==\n");
    else
    {
        unsigned int dwCaps;
        dwCaps = GetFBIFCaps(pRDC->iFBDev);

        
        if (dwCaps & IF_CAP_CR)
            pRDC->IoctlCR = TRUE;
    };

    
#ifndef XSERVER_LIBPCIACCESS
    xf86SetOperatingState(resVgaIo, pRDC->pEnt->index, ResUnusedOpr);
    xf86SetOperatingState(resVgaMem, pRDC->pEnt->index, ResDisableOpr);
#endif
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit24 RDCPreInit()== return TRUE\n");
    return TRUE;
}


RDC_STATIC Bool RDCScreenInitDual(ScreenPtr pScreen, int argc, char **argv)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    RDCRecPtr pRDC;
    vgaHWPtr hwp;   
    VisualPtr visual;
    CBIOS_ARGUMENTS *pCBiosArguments;

    
    BoxRec FBMemBox; 
    int    AvailFBSize;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCScreenInit()== \n");
    
    pRDC = RDCPTR(pScrn);
    hwp = VGAHWPTR(pScrn);
      
    
    AvailFBSize = pRDC->AvailableFBsize;

    pScrn->displayWidth = pScrn->virtualX+31 &~31;

    
    FBMemBox.x1 = 0;
    FBMemBox.y1 = 0;
    FBMemBox.x2 = MAX_WIDTH;
    FBMemBox.y2 = (AvailFBSize / (MAX_WIDTH * ((pScrn->bitsPerPixel+1)/8))) - 1;
    if (FBMemBox.y2 <= 0)
        FBMemBox.y2 = 32767; 
    
    if (xf86InitFBManager(pScreen, &FBMemBox))
    {
        int cpp = pScrn->bitsPerPixel / 8;
        int areaoffset = MAX_WIDTH * MAX_HEIGHT; 
        int size = (FBMemBox.y2 - (MAX_HEIGHT/cpp)) * MAX_WIDTH;

        if (xf86InitFBManagerLinear(pScreen, areaoffset, size))
        {
            xf86DrvMsg(pScrn->scrnIndex, X_INFO, "Using %ld bytes of offscreen memory for linear (offset=0x%x)\n", (AvailFBSize - areaoffset), areaoffset);
        }
        else
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "xf86InitFBManagerLinear() failed \n");
            return FALSE;
        }
    }
    else
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "xf86InitFBManager() failed \n");
        return FALSE;
    }      
    vFillRDCModeInfo (pScrn);      
 
    miClearVisualTypes();

    
    if (!miSetVisualTypes(pScrn->depth, miGetDefaultVisualMask(pScrn->depth), pScrn->rgbBits, pScrn->defaultVisual))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() Set Visual Type Fail== return FALSE\n");
        return FALSE;
    }
 
    if (!miSetPixmapDepths())
    {
        RDCSaveScreen(pScreen, SCREEN_SAVER_OFF);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() SetPixmapDepth fail== return FALSE\n");
        return FALSE;
    }

    vgaHWSetMmioFuncs(hwp, pRDC->MMIOVirtualAddr, 0);
    vgaHWGetIOBase(hwp);
    if (!vgaHWMapMem(pScrn))
      return FALSE;
      
    switch(pScrn->bitsPerPixel)
    {
        case 8:
        case 16:
        case 32:
            if (!fbScreenInit(pScreen, pRDC->FBVirtualAddr + pScrn->fbOffset,
                              pScrn->virtualX, pScrn->virtualY,
                              pScrn->xDpi, pScrn->yDpi,
                              pScrn->displayWidth, pScrn->bitsPerPixel))
            {
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() fbScreenInit fail== return FALSE\n");
                return FALSE;
            }
            break;

        default:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() Color Depth not supprt== return FALSE\n");
            return FALSE;                  
    }

    if (pScrn->bitsPerPixel > 8)
    {
        
        visual = pScreen->visuals + pScreen->numVisuals;
        while (--visual >= pScreen->visuals)
        {
            if ((visual->class | DynamicClass) == DirectColor)
            {
                visual->offsetRed = pScrn->offset.red;
                visual->offsetGreen = pScrn->offset.green;
                visual->offsetBlue = pScrn->offset.blue;
                visual->redMask = pScrn->mask.red;
                visual->greenMask = pScrn->mask.green;
                visual->blueMask = pScrn->mask.blue;
            }
        }
    }
     
    fbPictureInit(pScreen, 0, 0);
 
    xf86SetBlackWhitePixels(pScreen);

    if (!pRDC->noAccel)
    {
        if (!RDCAccelInit(pScreen))
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Hardware acceleration initialization failed\n");
            pRDC->noAccel = TRUE;           
        }
    }
     
    miInitializeBackingStore(pScreen);
    xf86SetBackingStore(pScreen);
    xf86SetSilkenMouse(pScreen);

    miDCInitialize(pScreen, xf86GetPointerScreenFuncs());

    if (pRDC->ENGCaps & ENG_CAP_CR_SUPPORT)
    {
        pRDC->CMDQInfo.InitCMDQInfo = bCRInitCMDQInfo;
        pRDC->CMDQInfo.EnableCMDQ = bCREnableCMDQ;
    }
    else if(pRDC->ENGCaps & ENG_CAP_2D) 
    {
        pRDC->CMDQInfo.InitCMDQInfo = bInitCMDQInfo;
        pRDC->CMDQInfo.EnableCMDQ = bEnableCMDQ;
    }else
    {
        pRDC->CMDQInfo.InitCMDQInfo = NULL;
        pRDC->CMDQInfo.EnableCMDQ   = NULL;
    }

    
    
    if (pRDC->ENGCaps & ENG_CAP_CR_SUPPORT)
    {
        pRDC->CMDQInfo.InitCMDQInfo(pScrn, pRDC);
        pRDC->CMDQInfo.EnableCMDQ(pRDC);
    }
    if (!RDCCursorInitDual(pScreen))
      xf86DrvMsg(pScrn->scrnIndex, X_ERROR,
		 "Hardware cursor initialization failed\n");

    if (!RDCEnterVTDual(pScrn))
      return FALSE;
      
    if (!miCreateDefColormap(pScreen))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit7 RDCScreenInit()== return FALSE\n");
        return FALSE;
    }

    if(!xf86HandleColormaps(pScreen, 256, (pScrn->depth == 8) ? 8 : pScrn->rgbBits,
                            vRDCLoadPalette, NULL,
                            CMAP_PALETTED_TRUECOLOR | CMAP_RELOAD_ON_MODE_SWITCH))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() xf86HandleColormaps fail== return FALSE\n");
        return FALSE;
    }

    
    xf86DPMSInit(pScreen, xf86DPMSSet, 0);  

    pRDC->BlockHandler = pScreen->BlockHandler;
    pScreen->BlockHandler = RDCBlockHandler;
    
    pScreen->SaveScreen = RDCSaveScreen;
    pRDC->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = RDCCloseScreen;
    pRDC->CreateScreenResources = pScreen->CreateScreenResources;
    pScreen->CreateScreenResources = RDCCreateScreenResources;
#if 0
    
    UTRemoveRestartFlag(pBIOSInfo);
#endif

    
    if (!xf86CrtcScreenInit (pScreen))
        return FALSE;
        
    
    RDCDisplayExtensionInit(pScrn);

    
    
    
    if (pRDC->ENGCaps & ENG_CAP_VIDEO_DISP)
        RDCVideoInit(pScreen);
    
    if (serverGeneration == 1)
        xf86ShowUnusedOptions(pScrn->scrnIndex, pScrn->options);
 
    RDCSave(pScrn);
    
    EC_DetectCaps(pScrn, &(pRDC->ECChipInfo));

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==RDCScreenInit() Normal Exit==\n");
    return TRUE;
} 

RDC_EXPORT Bool RDCSwitchModeDual(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
   xf86DrvMsg(pScrn->scrnIndex, X_INFO,"==Enter RDCSwitchModeM15()==\n");
   return xf86SetSingleMode (pScrn, mode, RR_Rotate_0);
}

RDC_EXPORT void RDCAdjustFrameDual(ScrnInfoPtr pScrn, int x, int y)
{
    RDCRecPtr   pRDC  = RDCPTR(pScrn);
    ULONG base;
    int rot_x, rot_y;
    int iMaxHorCoor, iMaxVerCoor;
    DisplayModePtr mode = pRDC->ModePtr;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCAdjustFrame(x = %d, y = %d)== \n", x, y);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pScrn->virtualX = %d\n", pScrn->virtualX);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pScrn->virtualY = %d\n", pScrn->virtualY);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pScrn->displayWidth = %d\n", pScrn->displayWidth);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pRDC->VideoModeInfo.ScreenWidth = %d\n", pRDC->VideoModeInfo.ScreenWidth);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pRDC->VideoModeInfo.ScreenHeight = %d\n", pRDC->VideoModeInfo.ScreenHeight);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pRDC->VideoModeInfo.ScreenPitch = %d\n", pRDC->VideoModeInfo.ScreenPitch);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pRDC->VideoModeInfo.Bpp = %d\n", pRDC->VideoModeInfo.Bpp);


    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCAdjustFrame()== \n");
}

        
RDC_STATIC Bool RDCEnterVTDual(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    vgaRegPtr vgaReg = &hwp->SavedReg;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCEnterVT()== \n");
   
    if (pRDC->SaveGeneration != serverGeneration)
    {
        pRDC->SaveGeneration = serverGeneration;
        
        vgaHWUnlock(hwp);
        vgaHWSave(pScrn, vgaReg, VGA_SR_FONTS);
    }

    
    
    memset(pRDC->FBVirtualAddr + pScrn->fbOffset, 0,
       pScrn->virtualY * pScrn->displayWidth * (pScrn->bitsPerPixel/8));
       
    if (!xf86SetDesiredModes (pScrn))
      return FALSE;
      
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit2 RDCEnterVT() Normal Exit== return TRUE\n");
    return TRUE;
}


RDC_STATIC void RDCLeaveVTDual(ScrnInfoPtr pScrn)
{
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    RDCRecPtr pRDC = RDCPTR(pScrn);
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCLeaveVT();== \n");
    
    if (!pRDC->noHWC)
        pRDC->HWCInfoPtr->HideCursor(pScrn);

    if ((!pRDC->noAccel) && pRDC->CMDQInfo.bInitialized)
        pRDC->CMDQInfo.Disable2D(pRDC);

    
    if (pRDC->bRandRRotation)
        *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x0;


    if (VBESetVBEMode(pRDC->pVbe, 3, NULL) == FALSE)
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCVBESetMode() Fail\n");
    }
    
    vgaHWLock(hwp);
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCLeaveVT() Normal Exit== \n");
}

RDC_STATIC void RDCFreeScreenDual(ScrnInfoPtr pScrn)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCFreeScreen()== \n");
    
    RDCFreeRec(pScrn);
    if (xf86LoaderCheckSymbol("vgaHWFreeHWRec"))
        vgaHWFreeHWRec(pScrn);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCFreeScreen()== \n");
}

RDC_STATIC ModeStatus RDCValidModeDual(ScrnInfoPtr pScrn, DisplayModePtr mode, Bool verbose, int flags)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    CBIOS_ARGUMENTS *pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
    USHORT wLCDHorSize, wLCDVerSize;
    USHORT wVESAModeHorSize, wVESAModeVerSize;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCValidMode() Verbose = %d, Flags = 0x%x==\n", 
               verbose, flags);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Mode name=%s, Width=%d, Height=%d, Refresh reate=%f==\n", 
               mode->name, mode->HDisplay, mode->VDisplay, mode->VRefresh);
    
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Query Display Path Info \n");

    if(!mode->Private)
         return MODE_BAD;
         
    return MODE_OK;
}            

RDC_EXPORT void RDCInitpScrnDual(ScrnInfoPtr pScrn)
{
   pScrn->PreInit = RDCPreInitDual;
   pScrn->ScreenInit = RDCScreenInitDual;
   pScrn->SwitchMode = RDCSwitchModeDual;
   pScrn->AdjustFrame = RDCAdjustFrameDual;
   pScrn->EnterVT = RDCEnterVTDual;
   pScrn->LeaveVT = RDCLeaveVTDual;
   pScrn->FreeScreen = RDCFreeScreenDual;
   pScrn->ValidMode = RDCValidModeDual;
}

#endif
