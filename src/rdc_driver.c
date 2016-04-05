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

#define _rdc_driver_c_
#include "rdc.h"
#include "rdcdual_driver_proto.h"

#include "CInt10FunProto.h"


#if XSERVER_LIBPCIACCESS
#define RDC_DEVICE_MATCH(d,i) \
    { PCI_VENDOR_RDC, (d), PCI_MATCH_ANY, PCI_MATCH_ANY, 0, 0, (i) }

static const struct pci_id_match rdc_device_match[] = {
   RDC_DEVICE_MATCH (PCI_CHIP_M2010, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2010_A0, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2011, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2012, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2013, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2014, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2015, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2200, 0 ),
    { 0, 0, 0 },
};
#endif


_X_EXPORT DriverRec RDC = {
   RDC_VERSION,
   RDC_DRIVER_NAME,
   RDCIdentify,
#if XSERVER_LIBPCIACCESS
   NULL,
#else
   RDCProbe,
#endif
   RDCAvailableOptions,
   NULL,
   0,
   NULL,
#if XSERVER_LIBPCIACCESS
   rdc_device_match,
   rdc_pci_probe
#endif
};


#ifdef RDC_GARBAGE
const char *vgahwSymbols[] = {
    "vgaHWFreeHWRec",
    "vgaHWGetHWRec",
    "vgaHWGetIOBase",
    "vgaHWGetIndex",
    "vgaHWInit",
    "vgaHWLock",
    "vgaHWMapMem",
    "vgaHWProtect",
    "vgaHWRestore",
    "vgaHWSave",
    "vgaHWSaveScreen",
    "vgaHWSetMmioFuncs",
    "vgaHWUnlock",
    "vgaHWUnmapMem",
    NULL
};

const char *fbSymbols[] = {
    "fbPictureInit",
    "fbScreenInit",
    NULL
};

const char *vbeSymbols[] = {
    "VBEInit",
    "VBEExtendedInit",
    "VBEFreeModeInfo",
    "VBEFreeVBEInfo",
    "VBEGetModeInfo",
    "VBEGetModePool",
    "VBEGetVBEInfo",
    "VBEGetVBEMode",
    "VBEPrintModes",
    "VBESaveRestore",
    "VBESetDisplayStart",
    "VBESetGetDACPaletteFormat",
    "VBESetGetLogicalScanlineLength",
    "VBESetGetPaletteData",
    "VBESetModeNames",
    "VBESetModeParameters",
    "VBESetVBEMode",
    "VBEValidateModes",
    "vbeDoEDID",
    "vbeFree",
    NULL
};

#ifdef XFree86LOADER
static const char *vbeOptionalSymbols[] = {
    "VBEDPMSSet",
    "VBEGetPixelClock",
    NULL
};
#endif

const char *ddcSymbols[] = {
    "xf86PrintEDID",
    "xf86SetDDCproperties",
    NULL
};

const char *int10Symbols[] = {
    "xf86ExecX86int10",
    "xf86InitInt10",
    "xf86Int10AllocPages",
    "xf86int10Addr",
    NULL
};

static const char *exaSymbols[] = {
  "exaDriverAlloc",
  "exaDriverInit",
  "exaDriverFini",
  "exaOffscreenAlloc",
  "exaOffscreenFree",
  "exaGetPixmapPitch",
  "exaGetPixmapOffset",
  "exaWaitSync",
  NULL
};

#ifdef HAVE_XAA
const char *xaaSymbols[] = {
    "XAACreateInfoRec",
    "XAADestroyInfoRec",
    "XAAInit",
    "XAACopyROP",
    "XAAPatternROP",
    NULL
};
#endif

const char *ramdacSymbols[] = {
    "xf86CreateCursorInfoRec",
    "xf86DestroyCursorInfoRec",
    "xf86InitCursor",
    NULL
};

#endif
#ifdef XFree86LOADER

static MODULESETUPPROTO(RDCSetup);

static XF86ModuleVersionInfo RDCVersRec = {
    RDC_DRIVER_NAME,
    MODULEVENDORSTRING,
    MODINFOSTRING1,
    MODINFOSTRING2,
    XORG_VERSION_CURRENT,
    RDC_MAJOR_VERSION, RDC_MINOR_VERSION, RDC_PATCH_VERSION,
    ABI_CLASS_VIDEODRV,
#ifdef PATCH_ABI_VERSION
    SET_ABI_VERSION(0, 5),
#else 
    ABI_VIDEODRV_VERSION,
#endif
    MOD_CLASS_VIDEODRV,
    {0, 0, 0, 0}
};

_X_EXPORT XF86ModuleData rdcModuleData = { &RDCVersRec, RDCSetup, NULL };

RDC_STATIC pointer RDCSetup(pointer module, pointer opts, int *errmaj, int *errmin)
{
    static Bool setupDone = FALSE;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCSetup()== \n");
    
    

    if (!setupDone)
    {
        setupDone = TRUE;
        xf86AddDriver(&RDC, module, 
#if XSERVER_LIBPCIACCESS
		      HaveDriverFuncs
#else
		      0
#endif
                     );

        
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 RDCSetup()== return TRUE\n");
        return (pointer) TRUE;
    }
    else
    {
        if (errmaj)
        *errmaj = LDR_ONCEONLY;

        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit2 RDCSetup()== return NULL\n");
        return NULL;
    }
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit3 RDCSetup()== \n");
}

#endif    


RDC_STATIC void RDCIdentify(int flags)
{
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCIdentify()== \n");
    xf86PrintChipsets(RDC_NAME, "Driver for RDC Graphics Chipsets", RDCChipsets);
    xf86DrvMsgVerb(1, X_INFO, DefaultLevel, "==Exit RDCIdentify()== \n");
}

RDC_EXPORT const OptionInfoRec * RDCAvailableOptions(int chipid, int busid)
{
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCAvailableOptions()== return RDCOptions\n");
    return RDCOptions;
}

#if XSERVER_LIBPCIACCESS

RDC_STATIC Bool rdc_pci_probe (DriverPtr driver, int entity_num, struct pci_device *device, intptr_t match_data)
{
    ScrnInfoPtr	    pScrn = NULL;
    EntityInfoPtr   entity;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter rdc_pci_probe== \n");

    pScrn = xf86ConfigPciEntity (pScrn, 0, entity_num, RDCPciChipsets,
				NULL,
				NULL, NULL, NULL, NULL);
    if (pScrn != NULL)
    {
	pScrn->driverVersion = RDC_VERSION;
	pScrn->driverName = RDC_DRIVER_NAME;
	pScrn->name = RDC_NAME;
	pScrn->Probe = NULL;

	entity = xf86GetEntityInfo (entity_num);
	
	switch (DEVICE_ID(device)) {
        case PCI_VENDOR_RDC_M2010_A0:
        case PCI_CHIP_M2010:
        case PCI_CHIP_M2010_A0:
        case PCI_CHIP_M2011:
        case PCI_CHIP_M2012:
        case PCI_CHIP_M2013:
        case PCI_CHIP_M2014:
        case PCI_CHIP_M2015:
        case PCI_CHIP_M2200:
            pScrn->PreInit = RDCPreInit;
            pScrn->ScreenInit = RDCScreenInit;
            pScrn->SwitchMode = RDCSwitchMode;
            pScrn->AdjustFrame = RDCAdjustFrame;   
            pScrn->EnterVT = RDCEnterVT;
            pScrn->LeaveVT = RDCLeaveVT;
            pScrn->FreeScreen = RDCFreeScreen;
            pScrn->ValidMode = RDCValidMode;
            break;

	default:
        RL2D("Unsupported setup? Why are we here?");
	    break;
	}
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit rdc_pci_probe== \n");
    return pScrn != NULL;
}
#else

RDC_STATIC Bool RDCProbe(DriverPtr drv, int flags)
{
    int i, numUsed, numDevSections, *usedChips;
    Bool foundScreen = FALSE;
    GDevPtr *devSections;   

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCProbe== \n");

    
    if ((numDevSections = xf86MatchDevice(RDC_DRIVER_NAME, &devSections)) <= 0)
    {
        xf86DrvMsg(0, X_ERROR, "==Exit1 RDCProbe()== return FALSE\n");
        return FALSE;
    }

    
    if (xf86GetPciVideoInfo() == NULL)
    {
        xf86DrvMsg(0, X_ERROR, "==Exit2 RDCProbe()== return FALSE\n");
        return FALSE;
    }

    numUsed = xf86MatchPciInstances(RDC_NAME, PCI_VENDOR_RDC,
                                    RDCChipsets, RDCPciChipsets,
                                    devSections, numDevSections,
                                    drv, &usedChips);
    xf86DrvMsgVerb(0, X_INFO, 5, "numUsed = %d\n", numUsed);
    free(devSections);

    if (flags & PROBE_DETECT) 
    {
        if (numUsed > 0)
        foundScreen = TRUE;
    }
    else
    {
        for (i = 0; i < numUsed; i++) 
        {
            ScrnInfoPtr pScrn = NULL;

            
            if ((pScrn = xf86ConfigPciEntity(pScrn, 0, usedChips[i],
                                             RDCPciChipsets, 0, 0, 0, 0, 0)))
            {
                pScrn->driverVersion = RDC_VERSION;
                pScrn->driverName = RDC_DRIVER_NAME;
                pScrn->name = RDC_NAME;
            
                pScrn->Probe = RDCProbe;
                pScrn->PreInit = RDCPreInit;
                pScrn->ScreenInit = RDCScreenInit;
                pScrn->SwitchMode = RDCSwitchMode;
                pScrn->AdjustFrame = RDCAdjustFrame;   
                pScrn->EnterVT = RDCEnterVT;
                pScrn->LeaveVT = RDCLeaveVT;
                pScrn->FreeScreen = RDCFreeScreen;
                pScrn->ValidMode = RDCValidMode;
            
                foundScreen = TRUE;        
            } 
        }  
    }        

    free(usedChips);

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit3 RDCProbe()== return(foundScreen=%X)\n", foundScreen);
    return foundScreen;
}
#endif


RDC_STATIC Bool RDCPreInit(ScrnInfoPtr pScrn, int flags)
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
#if !XSERVER_LIBPCIACCESS
    pRDC->PciTag  = pciTag(pRDC->PciInfo->bus, 
                           pRDC->PciInfo->device,
                           pRDC->PciInfo->func);
#endif
    
    pRDC->noHWC = FALSE; 
    pRDC->MMIOVPost = FALSE;
    pRDC->noAccel = FALSE;
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
    {
        long unsigned int value;
        if (!xf86GetOptValULong(pRDC->Options, OPTION_HRATIO, &value)) 
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "No HDMI underscan horizotal ratio options found\n");          
        }
        else
        {
	    
            if(value > 100)
                pRDC->bHRatio = 100;
            else
                pRDC->bHRatio=(BYTE) value;
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "HDMI underscan horizotal ratio %d\n",pRDC->bHRatio); 
        }

        
        if (!xf86GetOptValULong(pRDC->Options, OPTION_VRATIO, &value)) 
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "No HDMI underscan vertical ratio options found\n");          
        }
        else
        {      
            if(value > 100)
                pRDC->bVRatio = 100;
            else
                pRDC->bVRatio = (BYTE) value; 
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "HDMI underscan vertical ratio %d\n",pRDC->bVRatio);    
        }
    }

    
    
    if (!xf86ReturnOptValBool(pRDC->Options, OPTION_NOACCEL, FALSE) && (!pRDC->bRandRRotation))
    {
    	if((s = (char *)xf86GetOptValString(pRDC->Options, OPTION_ACCELMETHOD))) 
    	{
#ifdef HAVE_XAA
    	    if(!xf86NameCmp(s,"XAA")) 
    	    {
    		    pRDC->useEXA = FALSE;
    	    }
    	    else if(!xf86NameCmp(s,"EXA"))
    	    {
#endif
    		    pRDC->useEXA = TRUE;
#ifdef HAVE_XAA
    	    }
#endif
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
  
    
    if (!xf86ReturnOptValBool(pRDC->Options, OPTION_SW_CURSOR, FALSE))
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

    
    pScrn->monitor->DDC = RDCDoDDC(pScrn, pRDC->pEnt->index);    


    
    pScrn->modePool = RDCBuildModePool(pScrn);
    
   if (!pScrn->modePool) 
   {
      xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "== No Video BIOS modes for chosen ==\n");
      return FALSE;
   }
    
    clockRanges = xnfcalloc(sizeof(ClockRange), 1);
    clockRanges->next = NULL;
    clockRanges->minClock = 9500;
    clockRanges->maxClock = 266950;
    clockRanges->clockIndex = -1;               
    clockRanges->interlaceAllowed = FALSE;
    clockRanges->doubleScanAllowed = FALSE;

    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "Before xf86ValidateModes()\n");
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->modes = 0x%x\n", pScrn->modes);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->modePool = 0x%x\n", pScrn->modePool);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->display->modes = 0x%x\n", pScrn->display->modes);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " *pScrn->display->modes = %s\n", *pScrn->display->modes);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->bitsPerPixel = %d\n", pScrn->bitsPerPixel);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->display->virtualX = %d\n", pScrn->display->virtualX);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->display->virtualY = %d\n", pScrn->display->virtualY);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pRDC->FbMapSize = 0x%x\n", pRDC->FbMapSize);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->virtualX = %d\n", pScrn->virtualX);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->virtualY = %d\n", pScrn->virtualY);

    maxPitch = pRDC->ulMaxPitch;
    maxHeight = pRDC->ulMaxHeight;
    
    if ((pRDC->bRandRRotation) &&
        (maxPitch > 1280))
    {
        maxPitch = 1280;
        maxHeight = 1024;
    }
    
    i = xf86ValidateModes(pScrn, pScrn->modePool,
                          pScrn->display->modes, clockRanges,
                          0, 320, maxPitch, 8 * pScrn->bitsPerPixel,
                          200, maxHeight,
                          pScrn->display->virtualX, pScrn->display->virtualY,
                          pRDC->FbMapSize, LOOKUP_BEST_REFRESH);
                          
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, "After xf86ValidateModes()\n");
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->modes = 0x%x\n", pScrn->modes);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->modePool = 0x%x\n", pScrn->modePool);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->display->modes = 0x%x\n", pScrn->display->modes);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " *pScrn->display->modes = %s\n", *pScrn->display->modes);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->bitsPerPixel = %d\n", pScrn->bitsPerPixel);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->display->virtualX = %d\n", pScrn->display->virtualX);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->display->virtualY = %d\n", pScrn->display->virtualY);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pRDC->FbMapSize = 0x%x\n", pRDC->FbMapSize);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->virtualX = %d\n", pScrn->virtualX);
    xf86DrvMsgVerb(0, X_INFO, InternalLevel, " pScrn->virtualY = %d\n", pScrn->virtualY);

    if (i == -1)
    {
        RDCFreeRec(pScrn);
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Exit20 RDCPreInit()== return FALSE\n");
        return FALSE;
    }

    xf86PruneDriverModes(pScrn);

    if (!i || !pScrn->modes)
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "No valid modes found\n");
        RDCFreeRec(pScrn);
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Exit21 RDCPreInit()== return FALSE\n");
        return FALSE;
    }

    xf86SetCrtcForModes(pScrn, INTERLACE_HALVE_V);
 
    pScrn->currentMode = pScrn->modes;
 
    xf86PrintModes(pScrn);
 
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

    if(pRDC->noAccel)
    {        
        SetIndexRegMask(COLOR_CRTC_INDEX, 0xA4, 0xFE, 0x00);       
        SetIndexRegMask(COLOR_CRTC_INDEX, 0xA3, ~0x20, 0x00);       
        *(ULONG *)MMIOREG_1ST_FLIP &=  ~0x80000000;
    }
    
    
    pRDC->bDirectAccessFB = FALSE;
#if XSERVER_LIBPCIACCESS
    dev = pci_device_find_by_slot(0, 0, 0, 0);
    pci_device_cfg_read_u32(dev, &ulNBID, 0x00);  
    pci_device_cfg_read_u32(dev, &ulValue, 0x48);

    if (ulValue & DirectAccessFB)
            pRDC->bDirectAccessFB = TRUE;
#else
    ulNBID = pciReadLong(pciTag(0, 0, 0), 0x00);  
    ulValue = pciReadLong(pciTag(0, 0, 0), 0x48);

    if (ulValue & DirectAccessFB)
            pRDC->bDirectAccessFB = TRUE;

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


RDC_STATIC Bool RDCScreenInit(ScreenPtr pScreen, int argc, char **argv)
{
    ScrnInfoPtr pScrn=xf86ScreenToScrn(pScreen);
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
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "virtual X = %d\n", pScrn->virtualX);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "virtual Y = %d\n", pScrn->virtualY);
    
    FBMemBox.x1 = 0;
    FBMemBox.y1 = 0;
    FBMemBox.x2 = pScrn->displayWidth;
    FBMemBox.y2 = (AvailFBSize / (pScrn->displayWidth * ((pScrn->bitsPerPixel+1)/8))) - 1;
    if (FBMemBox.y2 <= 0)
        FBMemBox.y2 = 32767; 
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "AvailFBSize = %d\n", AvailFBSize);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "FBMemBox: x1 = %d, y1 = %d, x2 = %d, y2 = %d\n",FBMemBox.x1, FBMemBox.y1, FBMemBox.x2, FBMemBox.y2);

    if (xf86InitFBManager(pScreen, &FBMemBox))
    {
        int cpp = pScrn->bitsPerPixel / 8;
        int areaoffset = (pScrn->virtualY) * pScrn->displayWidth; 
        int size = (FBMemBox.y2 - (pScrn->virtualY/cpp)) * pScrn->displayWidth;

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

    vgaHWSetMmioFuncs(hwp, pRDC->MMIOVirtualAddr, 0);
    vgaHWGetIOBase(hwp);
 
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
     
    xf86SetBackingStore(pScreen);
    xf86SetSilkenMouse(pScreen);

    miDCInitialize(pScreen, xf86GetPointerScreenFuncs());

    
    if (!pRDC->noHWC)
    {
        if (!RDCCursorInit(pScreen)) 
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Hardware cursor initialization failed\n");
            pRDC->noHWC = TRUE;                      
        }
    }

    if (pRDC->bRandRRotation)
    {
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "assign pScrn->DriverFunc\n");
        pScrn->DriverFunc = RDCDriverFunc;
        pScrn->PointerMoved = RDCPointerMoved;
    }      
    
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

    
    xf86DPMSInit(pScreen, RDCDisplayPowerManagementSet, 0);
   
    pScreen->SaveScreen = RDCSaveScreen;
    pRDC->CloseScreen = pScreen->CloseScreen;
    pScreen->CloseScreen = RDCCloseScreen;
#if 0
    
    UTRemoveRestartFlag(pBIOSInfo);
#endif
    
    RDCDisplayExtensionInit(pScrn);

    
    if (pRDC->ENGCaps & ENG_CAP_VIDEO_DISP)
        RDCVideoInit(pScreen);
    
    if (serverGeneration == 1)
        xf86ShowUnusedOptions(pScrn->scrnIndex, pScrn->options);

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
    
    RDCSave(pScrn);

    
    pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
    
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Query Display Path Info \n");

    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->AX = OEMFunction;
    pCBiosArguments->BX = QueryDisplayPathInfo;
    
    
    CInt10(pRDC->pCBIOSExtension);

    pRDC->DeviceInfo.ucNewDeviceID = (pCBiosArguments->Ebx & 0xf0000) >> 16;
    
    
    pRDC->CMDQInfo.bInitialized = FALSE;
    
    if (!RDCModeInit(pScrn, pScrn->currentMode))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Mode Init Failed \n");
        return FALSE;
    }   

    RDCSaveScreen(pScreen, FALSE);
    RDCAdjustFrame(pScrn, pScrn->frameX0, pScrn->frameY0);

    EC_DetectCaps(pScrn, &(pRDC->ECChipInfo));

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==RDCScreenInit() Normal Exit==\n");
    return TRUE;
} 

RDC_EXPORT Bool RDCSwitchMode(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    Bool RetStatus = FALSE;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSwitchMode()== \n");
   
    RetStatus = RDCModeInit(pScrn, mode);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "== RDCSwitchMode() Exit== return %X\n", RetStatus);
    return RetStatus;
}

RDC_EXPORT void RDCAdjustFrame(ScrnInfoPtr pScrn, int x, int y)
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

    if ((pRDC->DeviceInfo.ScalerConfig.EnableDownScaling) && (pRDC->DeviceInfo.ScalerConfig.EnableHorDownScaler))
    {
        if (mode->HDisplay > 1024)
        {
            iMaxHorCoor = 1024 - 1;
        }
        else
        {
            iMaxHorCoor = (int)mode->HDisplay - 1;
        }
    }
    else
    {
        iMaxHorCoor = (int)pRDC->VideoModeInfo.ScreenWidth - 1;
    }

    if ((pRDC->DeviceInfo.ScalerConfig.EnableDownScaling) && (pRDC->DeviceInfo.ScalerConfig.EnableVerDownScaler))
    {
        if (mode->VDisplay > 768)
        {
            iMaxVerCoor = 768 - 1;
        }
        else
        {
            iMaxVerCoor = (int)mode->VDisplay - 1;
        }
    }
    else
    {
        iMaxVerCoor = (int)pRDC->VideoModeInfo.ScreenHeight - 1;
    }

    if (pRDC->bRandRRotation)
    {
        switch(pRDC->rotate)
        {
            case RR_Rotate_0:
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_0\n");
                rot_x = x;
                rot_y = y;
                break;

            case RR_Rotate_90:
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_90\n");
                rot_x = y;
                rot_y = pScrn->displayWidth -1 - iMaxVerCoor - x;
                break;

            case RR_Rotate_180:
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_180\n");
                rot_x = pScrn->displayWidth - iMaxHorCoor - x -1 ;
                rot_y = pScrn->virtualY - iMaxVerCoor - y - 1;
                break;


            case RR_Rotate_270:
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_270\n");
                rot_x = pScrn->displayWidth - iMaxHorCoor - y - 1;
                rot_y = x;
                break;

            default:
                xf86DrvMsgVerb(pScrn->scrnIndex, X_ERROR, ErrorLevel, "  Unexpected rotation in RDCAdjustFrame\n");
                return;
        }
    }
    else
    {
        rot_x = x;
        rot_y = y;
    }
    
    base = rot_y * pRDC->VideoModeInfo.ScreenPitch + 
           rot_x * pRDC->VideoModeInfo.Bpp;

    if ((x == 0)&&(y ==0))
    {
        pRDC->ulVirtualDesktopOffset = base;
    }
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  base = %x\n", base);
    vSetStartAddressCRT1(pRDC, base);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCAdjustFrame()== \n");
}

        
RDC_STATIC Bool RDCEnterVT(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCEnterVT()== \n");
    if (!RDCModeInit(pScrn, pScrn->currentMode))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit1 RDCEnterVT() RDCModeInit Fail== return FALSE\n");
        return FALSE;
    }

    RDCAdjustFrame(pScrn, pScrn->frameX0, pScrn->frameY0);

    
    if (pRDC->bRandRRotation)
    {
        switch(pRDC->rotate)
        {
            case RR_Rotate_0:
                *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x0;
                break;

            case RR_Rotate_90:
                *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x5;
                break;

            case RR_Rotate_180:
                *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x6;
                break;

            case RR_Rotate_270:
                *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x7;
                break;

            default:
                xf86DrvMsgVerb(pScrn->scrnIndex, X_ERROR, ErrorLevel, "Unexpected rotation in RDCRandRSetConfig\n");
                return FALSE;
        }
    }

    
    if(pRDC->noAccel)
    {        
        SetIndexRegMask(COLOR_CRTC_INDEX, 0xA4, 0xFE, 0x00);       
        SetIndexRegMask(COLOR_CRTC_INDEX, 0xA3, ~0x20, 0x00);       
        *(ULONG *)MMIOREG_1ST_FLIP &=  ~0x80000000;
    }
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit2 RDCEnterVT() Normal Exit== return TRUE\n");
    return TRUE;
}


RDC_STATIC void RDCLeaveVT(ScrnInfoPtr pScrn)
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

RDC_STATIC void RDCFreeScreen(ScrnInfoPtr pScrn)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCFreeScreen()== \n");
    
    RDCFreeRec(pScrn);
    if (xf86LoaderCheckSymbol("vgaHWFreeHWRec"))
        vgaHWFreeHWRec(pScrn);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCFreeScreen()== \n");
}

RDC_STATIC ModeStatus RDCValidMode(ScrnInfoPtr pScrn, DisplayModePtr mode, Bool verbose, int flags)
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

    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->AX = OEMFunction;
    pCBiosArguments->BX = QueryDisplayPathInfo;
    
    
    CInt10(pRDC->pCBIOSExtension);

    pRDC->DeviceInfo.ucDeviceID = (pCBiosArguments->Ebx & 0xf0000) >> 16;
    

    if (mode->Flags & V_INTERLACE)
    {
        if (verbose)
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_PROBED, InfoLevel,
                       "==Removing interlaced mode \"%s\"\n==", mode->name);
        }
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "== RDCValidMode() Fail, Not Interlace Mode==\n");
        return MODE_NO_INTERLACE;
    }

    if (pRDC->DeviceInfo.ucDeviceID == TVIndex) 
    {
        if (((mode->HDisplay == 640) && (mode->VDisplay == 480)) ||
            ((mode->HDisplay == 800) && (mode->VDisplay == 600)) ||
            ((mode->HDisplay ==1024) && (mode->VDisplay == 768)))
        {
            return MODE_OK;
        }else
        {
            return MODE_BAD;
        }
                    
    }
    else
    {
        if (((DEVICE_ID(pRDC->PciInfo) == PCI_CHIP_M2010) || (DEVICE_ID(pRDC->PciInfo) == PCI_CHIP_M2010_A0)))
        {
            if ((mode->HDisplay > 1680) || (mode->VDisplay > 1050))
            {
                return MODE_BAD;
            }
            if ((mode->HDisplay >= 1280) && 
                ((pScrn->bitsPerPixel > 16) || (mode->VRefresh > 62.0)))
            {
                return MODE_BAD;
            }
        }

        if (DEVICE_ID(pRDC->PciInfo) == PCI_CHIP_M2200)
        {
            if ((mode->HDisplay > 1024) || (mode->VDisplay > 768))
            {
                return MODE_BAD;
            }
            
            if (mode->HDisplay == 1024 && pScrn->bitsPerPixel > 8)
            {
                return MODE_BAD;
            }
        }
        
        
        if ((mode->HDisplay < 320) || (mode->VDisplay < 240))
        {
            return MODE_BAD;
        }

        if (mode->HDisplay ==720) 
        {
            return MODE_BAD;
        }
        
    }
    
    
    
    
    

    if (DEVICE_ID(pRDC->PciInfo) == PCI_CHIP_M2010)
    {
        return  RDCFilterModeByBandWidth(pScrn, mode);
    }
    else
    {
        return MODE_OK;
    }
}            




RDC_EXPORT Bool RDCGetRec(ScrnInfoPtr pScrn)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCGetRec()== \n");

    if (pScrn->driverPrivate)
    {
        RL("driverPrivate already allocated before allocating?\n");
        return TRUE;
    }

    pScrn->driverPrivate = xnfcalloc(sizeof(RDCRec), 1);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCGetRec()== return TRUE\n");
    return TRUE;
}

RDC_EXPORT void RDCFreeRec(ScrnInfoPtr pScrn)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCFreeRec()== \n");
    
    if (!pScrn)
    {
        xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==RDCFreeRec() pScrn is NULL== \n");
        return;
    }
    
    if (!pScrn->driverPrivate)
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCFreeRec() driverPrivate is FALSE== \n");
        return;
    }
    
    free(pScrn->driverPrivate);
    pScrn->driverPrivate = 0;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit3 RDCFreeRec()== \n");
}

RDC_EXPORT Bool RDCSaveScreen(ScreenPtr pScreen, Bool unblack)
{
    Bool RetStatus;
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCSaveScreen(unblack = %d)== \n", unblack);
    
    RetStatus = vgaHWSaveScreen(pScreen, unblack);
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 RDCSaveScreen()== return RetStatus\n");
    return RetStatus;
}

RDC_EXPORT Bool RDCCloseScreen(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86ScreenToScrn(pScreen);
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    RDCRecPtr pRDC = RDCPTR(pScrn);
    Bool RetStatus;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCCloseScreen(); Screen Index = 0x%x == \n",pScrn->scrnIndex);
    
    
    if (pRDC->bRandRRotation)
        *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x0;
        
    if (pScrn->vtSema == TRUE)
    {  
        if (!pRDC->noHWC)
            pRDC->HWCInfoPtr->HideCursor(pScrn);
          
        if ((!pRDC->noAccel) && pRDC->CMDQInfo.bInitialized)
            pRDC->CMDQInfo.Disable2D(pRDC);
         
        if (VBESetVBEMode(pRDC->pVbe, 3, NULL) == FALSE)
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCVBESetMode() Fail\n");
            return FALSE;
        }
        
        vgaHWLock(hwp);

    }

    RDCUnmapMem(pScrn);
    RDCUnmapMMIO(pScrn);
    RDCUnmapVBIOS(pScrn);
    
    vgaHWUnmapMem(pScrn);

#ifdef HAVE_XAA
    if(pRDC->AccelInfoPtr)
    {
        XAADestroyInfoRec(pRDC->AccelInfoPtr);
        pRDC->AccelInfoPtr = NULL;
    }
#endif
 
    if(pRDC->HWCInfoPtr)
    {
        xf86DestroyCursorInfoRec(pRDC->HWCInfoPtr);
        pRDC->HWCInfoPtr = NULL;
    }
 
    pScrn->vtSema = FALSE;
    pScreen->CloseScreen = pRDC->CloseScreen;

    
#ifdef HAVE_XAA    
    RetStatus = (*pScreen->CloseScreen) (scrnIndex, pScreen);
#else
    RetStatus = (*pScreen->CloseScreen) (pScreen);
#endif
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCCloseScreen()== return(RetStatus=%X)\n", RetStatus);
    return RetStatus;
}

RDC_EXPORT void RDCSave(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC;
    vgaRegPtr vgaReg;
    RDCRegPtr RDCReg;   

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSave()== \n");
    pRDC = RDCPTR(pScrn);
    vgaReg = &VGAHWPTR(pScrn)->SavedReg;
    RDCReg = &pRDC->SavedReg;
     
        
    vgaHWSave(pScrn, vgaReg, VGA_SR_ALL);
    
    
    vRDCOpenKey(pScrn);

    
    GetIndexReg(COLOR_CRTC_INDEX, 0xC0, &(RDCReg->ucPLLValue[0]));
    GetIndexReg(COLOR_CRTC_INDEX, 0xC1, &(RDCReg->ucPLLValue[1]));
    GetIndexReg(COLOR_CRTC_INDEX, 0xCF, &(RDCReg->ucPLLValue[2]));

    GetIndexReg(COLOR_CRTC_INDEX, 0xA3, &(RDCReg->ucCRA3));
    GetIndexReg(SEQ_INDEX, 0x58, &(RDCReg->ucSR58));
    GetIndexReg(SEQ_INDEX, 0x70, &(RDCReg->ucSR70));
    GetIndexReg(SEQ_INDEX, 0x74, &(RDCReg->ucSR74));

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSave()== \n");
}

RDC_EXPORT void RDCRestore(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC;
    vgaRegPtr vgaReg;
    RDCRegPtr RDCReg;   
    
    pRDC = RDCPTR(pScrn);
    vgaReg = &VGAHWPTR(pScrn)->SavedReg;
    RDCReg = &pRDC->SavedReg;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCRestore()== \n");
     
        
    vgaHWProtect(pScrn, TRUE);   
    vgaHWRestore(pScrn, vgaReg, VGA_SR_ALL);          
    vgaHWProtect(pScrn, FALSE);   
    
    
    vRDCOpenKey(pScrn);
    
       
    SetIndexReg(COLOR_CRTC_INDEX, 0xC0, RDCReg->ucPLLValue[0]);
    SetIndexReg(COLOR_CRTC_INDEX, 0xC1, RDCReg->ucPLLValue[1]);
    SetIndexReg(COLOR_CRTC_INDEX, 0xCF, RDCReg->ucPLLValue[2]);

    SetIndexRegMask(COLOR_CRTC_INDEX, 0xA3, ~0x20, RDCReg->ucCRA3 & 0x20);
    
    SetIndexRegMask(COLOR_CRTC_INDEX, 0xBB, 0xFF, 0);

    SetIndexReg(SEQ_INDEX, 0x58, RDCReg->ucSR58);

    SetIndexReg(SEQ_INDEX, 0x70, RDCReg->ucSR70);

    SetIndexReg(SEQ_INDEX, 0x74, RDCReg->ucSR74);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCRestore()== \n");    
}

RDC_EXPORT void RDCProbeDDC(ScrnInfoPtr pScrn, int index)
{
    vbeInfoPtr pVbe;
 
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCProbeDDC()== \n");
    
    if (xf86LoadSubModule(pScrn, "vbe")) 
    {
        pVbe = VBEInit(NULL, index);
        ConfiguredMonitor = vbeDoEDID(pVbe, NULL);
        vbeFree(pVbe);
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCProbeDDC()== \n");
}

RDC_EXPORT xf86MonPtr RDCDoDDC(ScrnInfoPtr pScrn, int index)
{
    vbeInfoPtr pVbe;
    xf86MonPtr MonInfo = NULL;
    RDCRecPtr pRDC = RDCPTR(pScrn);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCDoDDC()== \n");
    
    if (xf86ReturnOptValBool(pRDC->Options, OPTION_NO_DDC, FALSE))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCDoDDC()== return(MonInfo)\n");
        return MonInfo;
    }

    pVbe = pRDC->pVbe;
    
    pRDC->pCBIOSExtension->bEDIDValid = FALSE;
    if (pVbe)
    {
        MonInfo = vbeDoEDID(pVbe, NULL);
        xf86PrintEDID(MonInfo);
        xf86SetDDCproperties(pScrn, MonInfo);
        if (MonInfo != NULL)
        {
            if(MonInfo->rawData[0] == 0 &&
               MonInfo->rawData[1] == 0xFF)
            {
                EDID_DETAILED_TIMING EDIDDetailedTimingList;
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "Get Device EDID, updating into CBIOS EDID table\n");
                
                
                CreateEDIDDetailedTimingList(MonInfo->rawData, 128, &EDIDDetailedTimingList);
                CBIOS_SetEDIDToModeTable(pScrn, &EDIDDetailedTimingList);
                pRDC->pCBIOSExtension->bEDIDValid = TRUE;

                
                pRDC->pCBIOSExtension->wCRTDefaultH = EDIDDetailedTimingList.usHorDispEnd;		
                pRDC->pCBIOSExtension->wCRTDefaultV = EDIDDetailedTimingList.usVerDispEnd;
            }
        }
    }
    else
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel,
          "this driver cannot do DDC without VBE\n");
    }
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit2 RDCDoDDC()== return (MonInfo)\n");
    return MonInfo;
}

RDC_EXPORT void vFillRDCModeInfo (ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter vFillRDCModeInfo()== \n");

    pRDC = RDCPTR(pScrn);
    
    pRDC->VideoModeInfo.ScreenWidth = pScrn->virtualX;   
    pRDC->VideoModeInfo.ScreenHeight = pScrn->virtualY;   
    pRDC->VideoModeInfo.bitsPerPixel = pScrn->bitsPerPixel;   
    pRDC->VideoModeInfo.Bpp         =  (pScrn->bitsPerPixel + 1) / 8;
    pRDC->VideoModeInfo.ScreenPitch = pScrn->displayWidth * pRDC->VideoModeInfo.Bpp;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit1 vFillRDCModeInfo()== \n");
}

RDC_EXPORT Bool RDCModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    vgaHWPtr hwp;
    RDCRecPtr pRDC;
    CBIOS_ARGUMENTS *pCBiosArguments;
    UCHAR ucHdmiType;
    
    hwp = VGAHWPTR(pScrn);
    pRDC = RDCPTR(pScrn);

    xorgRRConfig    config;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCModeInit()== \n");


    
    
    pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    
    
    if (pRDC->DeviceInfo.ucNewDeviceID == LCDIndex)
    {
        
        pCBiosArguments->AX = OEMFunction;
        pCBiosArguments->BX = QueryLCDPanelSizeMode;
        pCBiosArguments->CL = 0;
        
        
        CInt10(pRDC->pCBIOSExtension);
        
        
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Query LCD Panel Size Mode \n");

        if (pCBiosArguments->AX == VBEFunctionCallSuccessful)
        {
            pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = pCBiosArguments->DX;
            pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = pCBiosArguments->Edx >> 16;
	    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Query LCD Panel Size Mode result: %ld,%ld \n",pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution,pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution);
        }
        else
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_ERROR, ErrorLevel, "Query LCD Panel Size Fail(%04X)\n", pCBiosArguments->AX);
            return FALSE;
        }

        if (pRDC->bRandRRotation || pRDC->noHWC)
        {
            
            if (pRDC->DeviceInfo.ScalerConfig.EnableDownScaling)
            {
                if ((mode->HDisplay > 1024) || (mode->VDisplay > 768))
                    return FALSE;
            }
            else
            {
                if ((mode->HDisplay > pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution) || 
                    (mode->VDisplay > pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution))
                    return FALSE;
            }
        }
        
    }
    else if (pRDC->DeviceInfo.ucNewDeviceID == HDMIIndex)
    {
        
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Query HDMI Configuration \n");
        
            ucHdmiType = Get_HDMI_TYPE() & 0x0F;
            switch (ucHdmiType)
            {
                case HDMI720P:
                    pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = 1280;
                    pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = 720;
                    break;
                case HDMI480P:
                    pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = 720;
                    pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = 480;
                    break;
                case HDMI1080P:
                    pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = 1920;
                    pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = 1080;
                    break;
                case HDMI640:
                default:
                    pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = 640;
                    pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = 480;
                    break;
            }

            
            if (pRDC->bHRatio != 100 || pRDC->bVRatio != 100)
            {            
                pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = (((pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution*pRDC->bHRatio)/100) + 1) & ~1;
                pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = (((pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution*pRDC->bVRatio)/100) + 1) & ~1;
            };
    }
    else if (pRDC->DeviceInfo.ucNewDeviceID == TVIndex)
    {
        UCHAR ucTvType;

        if (pRDC->bEnableTVPanning)
        {
            pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = pRDC->TVEncoderInfo[0].TVOut_HSize;
            pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = pRDC->TVEncoderInfo[0].TVOut_VSize;
        }
        else
        {
            pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = 1024;
            pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = 768;
        };
        
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Query TV Configuration \n");
        
        pCBiosArguments->AX = OEMFunction;
        pCBiosArguments->BX = QueryTVConfiguration;
        
        
        CInt10(pRDC->pCBIOSExtension);

        ucTvType = pCBiosArguments->BL & (0x0f);

        if (ucTvType == NTSC)
        {
            if (!(((mode->HDisplay == 640) && (mode->VDisplay == 480)) ||
                  ((mode->HDisplay == 720) && (mode->VDisplay == 480)) ||
                  ((mode->HDisplay == 800) && (mode->VDisplay == 600)) ||
                  ((mode->HDisplay == 1024) && (mode->VDisplay == 768))))
                return FALSE;
        }
        else if (ucTvType == PAL)
        {
            if (!(((mode->HDisplay == 640) && (mode->VDisplay == 480)) ||
                  ((mode->HDisplay == 720) && (mode->VDisplay == 576)) ||
                  ((mode->HDisplay == 800) && (mode->VDisplay == 600)) ||
                  ((mode->HDisplay == 1024) && (mode->VDisplay == 768))))
                return FALSE;
        }
    }
    else if (pRDC->DeviceInfo.ucNewDeviceID == CRTIndex && pRDC->pCBIOSExtension->bEDIDValid)
    {
        pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = pRDC->pCBIOSExtension->wCRTDefaultH;
        pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = pRDC->pCBIOSExtension->wCRTDefaultV;
    }
    else 
    {
        if(pRDC->ENGCaps & ENG_CAP_CR_SUPPORT)
        {
            pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = 4095;
            pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = 2047;
        }else
        {
            pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = 1024;
            pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = 768;
        }
    }

    if (pRDC->DeviceInfo.ucNewDeviceID != LCDIndex &&
        pRDC->DeviceInfo.ucNewDeviceID != TVIndex &&
        pRDC->DeviceInfo.ucNewDeviceID != HDMIIndex &&
        (pRDC->DeviceInfo.ucNewDeviceID == CRTIndex && !pRDC->pCBIOSExtension->bEDIDValid)) 
    {
        if (mode->PrivFlags & LCD_TIMING)
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCModeInit() Fail== LCD timing is not supported!!\n");
            return FALSE;
        }

        
        
        if ((ULONG)mode->HDisplay > pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution)
            return FALSE;
        if ((ULONG)mode->VDisplay > pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution)
            return FALSE;
    }


    
    if (!pRDC->noHWC)
        pRDC->HWCInfoPtr->HideCursor(pScrn);

    if ((!pRDC->noAccel) && pRDC->CMDQInfo.bInitialized)
        pRDC->CMDQInfo.Disable2D(pRDC);

    
    vgaHWUnlock(hwp);

    if (!vgaHWInit(pScrn, mode))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCModeInit() vgaHWInit Fail== return FALSE\n");
        return FALSE;
    }
    
    pScrn->vtSema = TRUE;
    pRDC->ModePtr = mode;

    if (!RDCSetMode(pScrn, mode))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCModeInit() RDCSetMode Fail== return FALSE\n");
        return FALSE;
    }    

    
    config.rotation = pRDC->rotate;
    RDCRandRSetConfig(pScrn, &config);

    
    pRDC->VideoModeInfo.ScreenWidth = mode->HDisplay;
    pRDC->VideoModeInfo.ScreenHeight = mode->VDisplay;

    
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Query Display Path Info \n");

    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->AX = OEMFunction;
    pCBiosArguments->BX = QueryDisplayPathInfo;
    
    
    CInt10(pRDC->pCBIOSExtension);
    if (pCBiosArguments->AX == VBEFunctionCallSuccessful)
    {
        pRDC->DeviceInfo.ucDeviceID = (pCBiosArguments->Ebx & 0x000F0000) >> 16;
        pRDC->DeviceInfo.ucDisplayPath = 1;
        pRDC->DeviceInfo.ScalerConfig.EnableHorScaler = ((pCBiosArguments->Ebx & 0x00200000) ? TRUE : FALSE);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "H scaler enable = %d\n", pRDC->DeviceInfo.ScalerConfig.EnableHorScaler);
        pRDC->DeviceInfo.ScalerConfig.EnableVerScaler = ((pCBiosArguments->Ebx & 0x00100000) ? TRUE : FALSE);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "V scaler enable = %d\n", pRDC->DeviceInfo.ScalerConfig.EnableVerScaler);
    }
    else
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_ERROR, ErrorLevel, "Query Display Path Info Fail(%04X)\n", pCBiosArguments->AX);
        pRDC->DeviceInfo.ScalerConfig.EnableHorScaler = FALSE;
        pRDC->DeviceInfo.ScalerConfig.EnableVerScaler = FALSE;
    }

    
    
    pRDC->DeviceInfo.ScalerConfig.EnableHorDownScaler = FALSE;
    pRDC->DeviceInfo.ScalerConfig.EnableVerDownScaler = FALSE;
    pRDC->DeviceInfo.ScalerConfig.EnableHorUpScaler = FALSE;
    pRDC->DeviceInfo.ScalerConfig.EnableVerUpScaler = FALSE;
    
    if (pCBiosArguments->AX == VBEFunctionCallSuccessful)
    {
        if (pRDC->DeviceInfo.ScalerConfig.EnableHorScaler)
        {
            if ((ULONG)mode->HDisplay > pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution)
            {
                if (pRDC->DeviceInfo.ScalerConfig.EnableDownScaling)
                {
                    if (pRDC->DeviceInfo.ucDeviceID == LCDIndex)
                    {
                        pRDC->DeviceInfo.ScalerConfig.EnableHorDownScaler = TRUE;

                        if ((ULONG)mode->HDisplay > 1024)
                            pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor = (1024 << 8) / pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution;
                        else
                            pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor = (((ULONG)mode->HDisplay) << 8) / pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution;
                            
                        pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor++;
                    }
                }
            }
            else if (pRDC->DeviceInfo.ucDeviceID == CRTIndex &&
                     pRDC->pCBIOSExtension->bEDIDValid)
            {
                pRDC->DeviceInfo.ScalerConfig.EnableHorUpScaler = TRUE;
                pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor = (((ULONG)mode->HDisplay) << 12) / pRDC->pCBIOSExtension->wCRTDefaultH;
            }
            else if ((ULONG)mode->HDisplay < pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution)
            {
                pRDC->DeviceInfo.ScalerConfig.EnableHorUpScaler = TRUE;
                pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor = (((ULONG)mode->HDisplay) << 12) / pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution;
            }
        }

        if (pRDC->DeviceInfo.ScalerConfig.EnableVerScaler)
        {
            if ((ULONG)mode->VDisplay > pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution)
            {
                if (pRDC->DeviceInfo.ScalerConfig.EnableDownScaling)
                {
                    if (pRDC->DeviceInfo.ucDeviceID == LCDIndex)
                    {
                        pRDC->DeviceInfo.ScalerConfig.EnableVerDownScaler = TRUE;

                        if ((ULONG)mode->VDisplay > 768)
                            pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor = (768 << 8) / pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution;
                        else
                            pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor = (((ULONG)mode->VDisplay) << 8) / pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution;
                            
                        pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor++;
                    }
                }
            }
            else if (pRDC->DeviceInfo.ucDeviceID == CRTIndex &&
                     pRDC->pCBIOSExtension->bEDIDValid)
            {
                pRDC->DeviceInfo.ScalerConfig.EnableVerUpScaler = TRUE;
                pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor =  (((ULONG)mode->VDisplay) << 11) / pRDC->pCBIOSExtension->wCRTDefaultV;
            }
            else if ((ULONG)mode->VDisplay < pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution)
            {
                pRDC->DeviceInfo.ScalerConfig.EnableVerUpScaler = TRUE;
                pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor =  (((ULONG)mode->VDisplay) << 11) /pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution;
            }
        }
    }

    
    if(pRDC->DeviceInfo.ucDeviceID == TVIndex)
    {
        
        if(pRDC->bEnableTVPanning && (mode->HDisplay > pRDC->TVEncoderInfo[TV_1].TVOut_HSize))
            pRDC->TVEncoderInfo[TV_1].bEnableHPanning = TRUE;
        else
            pRDC->TVEncoderInfo[TV_1].bEnableHPanning = FALSE;
        
        if(pRDC->bEnableTVPanning && (mode->VDisplay > pRDC->TVEncoderInfo[TV_1].TVOut_VSize))
            pRDC->TVEncoderInfo[TV_1].bEnableVPanning = TRUE;
        else
            pRDC->TVEncoderInfo[TV_1].bEnableVPanning = FALSE;
        
        pRDC->TVEncoderInfo[TV_1].ModeHSize = mode->HDisplay;
        pRDC->TVEncoderInfo[TV_1].ModeVSize = mode->VDisplay;
    }

    
    pRDC->HWCInfo.iScreenOffset_x = 0;
    pRDC->HWCInfo.iScreenOffset_y = 0;
    RDCAdjustFrame(pScrn, 0, 0);
 
    vgaHWProtect(pScrn, FALSE);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 7, "==RDCModeInit() Enable 2D== \n");
    if (!pRDC->noAccel) 
    {
        if (!pRDC->CMDQInfo.Enable2D(pRDC)) 
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Enable 2D failed\n");  
            pRDC->noAccel = TRUE;                        
        }           
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 7, "==RDCModeInit() Enable Cursor== \n");
    if (!pRDC->noHWC) 
    {
        if (!bInitHWC(pScrn, pRDC))
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Init HWC failed\n");
            pRDC->noHWC = TRUE;                         
        }           
    }

    
    if (pRDC->ENGCaps & ENG_CAP_CR_SUPPORT)
    {
        pRDC->CMDQInfo.InitCMDQInfo(pScrn, pRDC);
        pRDC->CMDQInfo.EnableCMDQ(pRDC);
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCModeInit()== return TRUE\n");
    return TRUE;
}

RDC_EXPORT void RDCSetHWCaps(RDCRecPtr pRDC)
{
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCSetHWCaps() Entry==\n");

    switch(DEVICE_ID(pRDC->PciInfo))
    {
        case PCI_CHIP_M2010_A0:
            break;
            
        case PCI_CHIP_M2010:
            pRDC->ENGCaps = ENG_CAP_2D;
            pRDC->ENGCaps |= ENG_CAP_VIDEO_DISP |
                             ENG_CAP_HWC_MMIO;
            break;
            
        case PCI_CHIP_M2012:
            pRDC->ENGCaps = ENG_CAP_2D;
            pRDC->ENGCaps |= ENG_CAP_VIDEO_DISP |
                             ENG_CAP_VIDEO_POST |
                             ENG_CAP_HWC_MMIO |
                             ENG_CAP_CR_SUPPORT;
            break;
            
        case PCI_CHIP_M2013:
        case PCI_CHIP_M2014:
        case PCI_CHIP_M2015:
            pRDC->ENGCaps = ENG_CAP_2D;
            pRDC->ENGCaps |= ENG_CAP_VIDEO_DISP |
                             ENG_CAP_VIDEO_POST |
                             ENG_CAP_HWC_MMIO |
                             ENG_CAP_CR_SUPPORT |
                             ENG_CAP_HWROTATE |
                             ENG_CAP_EXTENDFLIPADDRS |
                             ENG_CAPS_ADV_ROT;
            break;
            
        case PCI_CHIP_M2200:
            pRDC->ENGCaps = ENG_CAP_HWC_MMIO;
            break;
        case PCI_CHIP_M2011:
            break;
            
        default:
            break;
    }
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCSetHWCaps() Exit Caps = 0x%x==\n",pRDC->ENGCaps);
}


RDC_EXPORT Bool RDCRandRGetInfo(ScrnInfoPtr pScrn, Rotation *rotations)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "RDCRandRGetInfo\n");
    
    *rotations = (RR_Rotate_0|RR_Rotate_90|RR_Rotate_180|RR_Rotate_270);

    return TRUE;
}

RDC_EXPORT Bool RDCRandRSetConfig(ScrnInfoPtr pScrn, xorgRRConfig *config)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "RDCRandRSetConfig\n");
    RDCRecPtr   pRDC = RDCPTR(pScrn);
    ULONG       ulROTAP_CTL0 = 0;   
    uint64 dwdwROTAP_SRC_PITCH_RECI=0;
    DWORD  dwROTAP_SRC_PITCH_RECI=0;
    
    if (pScrn->bitsPerPixel == 16)
    {
        ulROTAP_CTL0 |= ROTAPS_16BPP;
    }
    else if ((pScrn->bitsPerPixel == 24) || (pScrn->bitsPerPixel == 32))
    {
        ulROTAP_CTL0 |= ROTAPS_32BPP;
    }
    else
    {
        return FALSE;
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  config->width = %d\n", config->width);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  config->height = %d\n", config->height);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  config->rate = %d\n", config->rate);

    switch(config->rotation)
    {
        case RR_Rotate_0:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_0\n");
            if (pRDC->bDirectAccessFB)
            {
                TurnDirectAccessFBON(pScrn, TRUE);
            }
            pRDC->rotate = config->rotation;
            *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x0;
            ulROTAP_CTL0 &= ~(ROTAPS_ENABLE);

            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  0x8094 = %08x\n", *(pRDC->MMIOVirtualAddr + 0x8094));
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_END_ADDR0 = %08x\n", *ROTAP_END_ADDR0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_PITCH0 = %08x\n", *ROTAP_PITCH0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_SRC_PITCH_RECI0 = %08x\n", *ROTAP_SRC_PITCH_RECI0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_WH0 = %08x\n", *ROTAP_WH0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_CTL0 = %08x\n", *ROTAP_CTL0);
            break;

        case RR_Rotate_90:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_90\n");
            if (pRDC->bDirectAccessFB)
            {
                TurnDirectAccessFBON(pScrn, FALSE);
            }
            pRDC->rotate = config->rotation;
            *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x5;
            *ROTAP_END_ADDR0 = pRDC->VideoModeInfo.ScreenPitch * pScrn->displayWidth;
            *ROTAP_PITCH0 = (pScrn->displayWidth << 16) | pScrn->displayWidth;
            *ROTAP_WH0 = ((pScrn->displayWidth-1) << 16) | (pScrn->displayWidth - 1);
            ulROTAP_CTL0 |= ROTAPS_ENABLE;

            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  0x8094 = %08x\n", *(pRDC->MMIOVirtualAddr + 0x8094));
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_END_ADDR0 = %08x\n", *ROTAP_END_ADDR0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_PITCH0 = %08x\n", *ROTAP_PITCH0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_SRC_PITCH_RECI0 = %08x\n", *ROTAP_SRC_PITCH_RECI0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_WH0 = %08x\n", *ROTAP_WH0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_CTL0 = %08x\n", *ROTAP_CTL0);
            break;

        case RR_Rotate_180:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_180\n");
            if (pRDC->bDirectAccessFB)
            {
                TurnDirectAccessFBON(pScrn, FALSE);
            }
            pRDC->rotate = config->rotation;
            *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x6;
            *ROTAP_END_ADDR0 = pRDC->VideoModeInfo.ScreenPitch * pScrn->virtualY;
            *ROTAP_PITCH0 = (pScrn->displayWidth << 16) | pScrn->displayWidth;
            *ROTAP_WH0 = ((pScrn->virtualY-1) << 16) | (pScrn->displayWidth - 1);
            ulROTAP_CTL0 |= ROTAPS_ENABLE;

            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  0x8094 = %08x\n", *(pRDC->MMIOVirtualAddr + 0x8094));
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_END_ADDR0 = %08x\n", *ROTAP_END_ADDR0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_PITCH0 = %08x\n", *ROTAP_PITCH0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_SRC_PITCH_RECI0 = %08x\n", *ROTAP_SRC_PITCH_RECI0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_WH0 = %08x\n", *ROTAP_WH0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_CTL0 = %08x\n", *ROTAP_CTL0);
            break;


        case RR_Rotate_270:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_270\n");
            if (pRDC->bDirectAccessFB)
            {
                TurnDirectAccessFBON(pScrn, FALSE);
            }
            pRDC->rotate = config->rotation;
            *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x7;
            *ROTAP_END_ADDR0 = pRDC->VideoModeInfo.ScreenPitch * pScrn->displayWidth;
            *ROTAP_PITCH0 = (pScrn->displayWidth << 16) | pScrn->displayWidth;
            *ROTAP_WH0 = ((pScrn->displayWidth-1) << 16) | (pScrn->displayWidth - 1);
            ulROTAP_CTL0 |= ROTAPS_ENABLE;
            
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  0x8094 = %08x\n", *(pRDC->MMIOVirtualAddr + 0x8094));
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_END_ADDR0 = %08x\n", *ROTAP_END_ADDR0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_PITCH0 = %08x\n", *ROTAP_PITCH0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_SRC_PITCH_RECI0 = %08x\n", *ROTAP_SRC_PITCH_RECI0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_WH0 = %08x\n", *ROTAP_WH0);
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  ROTAP_CTL0 = %08x\n", *ROTAP_CTL0);
            break;

        default:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_ERROR, ErrorLevel, "Unexpected rotation in RDCRandRSetConfig\n");
            if (pRDC->bDirectAccessFB)
            {
                TurnDirectAccessFBON(pScrn, TRUE);
            }
            pRDC->rotate = RR_Rotate_0;
            return FALSE;
    }
    
    if(pRDC->ENGCaps & ENG_CAPS_ADV_ROT)
    {                
        dwdwROTAP_SRC_PITCH_RECI = ROTAPS_SRC_RECI_FACTOR;
        dwdwROTAP_SRC_PITCH_RECI = ((dwdwROTAP_SRC_PITCH_RECI<<2)<<1)/pScrn->displayWidth;
        dwdwROTAP_SRC_PITCH_RECI = (dwdwROTAP_SRC_PITCH_RECI+1)>>1;
        dwROTAP_SRC_PITCH_RECI = (DWORD)dwdwROTAP_SRC_PITCH_RECI; 
        *ROTAP_SRC_PITCH_RECI0 = dwROTAP_SRC_PITCH_RECI;
    }
    else
    {
        *ROTAP_SRC_PITCH_RECI0 = ((1 << 30) + (pScrn->displayWidth >> 1)) / pScrn->displayWidth;
    }
    
    *ROTAP_CTL0 = ulROTAP_CTL0;
    
    RDCAdjustFrame(pScrn, 0, 0);
    
    return TRUE;
}




RDC_EXPORT Bool RDCDriverFunc(ScrnInfoPtr pScrn, xorgDriverFuncOp op, pointer data)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "RDCDriverFunc Operation: %d\n", op);
    
    switch(op)
    {
        case RR_GET_INFO:          
            return RDCRandRGetInfo(pScrn, (Rotation*)data);
        case RR_SET_CONFIG:          
            return RDCRandRSetConfig(pScrn, (xorgRRConfig*)data);
        default:
            return FALSE;
    }
    return FALSE;
}

RDC_EXPORT void RDCApertureInit(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    *ROTAP_CTL0             = 0;
    *ROTAP_START_ADDR0      = 0;
    *ROTAP_END_ADDR0        = 0;
    *ROTAP_PITCH0           = 0;
    *ROTAP_SRC_PITCH_RECI0  = 0;
    *ROTAP_WH0              = 0;

    *ROTAP_CTL1             = 0;
    *ROTAP_START_ADDR1      = 0;
    *ROTAP_END_ADDR1        = 0;
    *ROTAP_PITCH0           = 0;
    *ROTAP_SRC_PITCH_RECI1  = 0;
    *ROTAP_WH1              = 0;

}

RDC_EXPORT void TurnDirectAccessFBON(ScrnInfoPtr pScrn, Bool bTurnOn)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);
    uint32_t ulNBID = 0x0, ulValue = 0x0;
#if XSERVER_LIBPCIACCESS
    struct pci_device *dev;
#endif

#if XSERVER_LIBPCIACCESS
    dev = pci_device_find_by_slot(0, 0, 0, 0);
    pci_device_cfg_read_u32(dev, &ulValue, 0x48);
#else
    ulValue = pciReadLong(pciTag(0, 0, 0), 0x48);
#endif

    if(bTurnOn)
    {
        ulValue |= DirectAccessFB;
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "Direct Access FB turn ON\n");
    }
    else
    {
        ulValue &= ~(DirectAccessFB);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "Direct Access FB turn OFF\n");
    }

        
#if XSERVER_LIBPCIACCESS
    pci_device_cfg_write_u32(dev, ulValue, 0x48);
#else
    pciWriteLong(pciTag(0, 0, 0), 0x48, ulValue);
#endif
}


RDC_EXPORT void RDCPointerMoved(ScrnInfoPtr pScrn, int x, int y)
{

}

RDC_EXPORT void vUpdateHDMIFakeMode(ScrnInfoPtr pScrn)
{
    WORD wHSize, wVSize;
    RDCRecPtr pRDC;
    WORD w8bppModeNum=0, w16bppModeNum=0, w32bppModeNum=0;
    MODE_INFO *pMode = &VESATable;
    pRDC = RDCPTR(pScrn);

    switch(Get_HDMI_TYPE())
    {
    case HDMI1080P:
         w8bppModeNum  = 0x156;
         w16bppModeNum = 0x157;
         w32bppModeNum = 0x158;
         wHSize = 1920;
         wVSize = 1080;
         break;
    case HDMI720P:
         w8bppModeNum  = 0x147;
         w16bppModeNum = 0x148;
         w32bppModeNum = 0x149;
         wHSize = 1280;
         wVSize = 720;
         break;
    }
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "Orginal H Size = %d, V Size = %d for HDMI type %d\n", wHSize, wVSize, Get_HDMI_TYPE());
    wHSize = (((wHSize*(pRDC->bHRatio))/100) + 1) & ~1;
    wVSize = (((wVSize*(pRDC->bVRatio))/100) + 1) & ~1;
    cbSetHDMIModeInfo(wHSize, wVSize, Get_HDMI_TYPE());
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "Update  H Size = %d, V Size = %d for HDMI type %d\n", wHSize, wVSize, Get_HDMI_TYPE());

    for (;;)
    {
        if (pMode->Mode_ID_8bpp == w8bppModeNum &&
            pMode->Mode_ID_16bpp == w16bppModeNum &&
            pMode->Mode_ID_32bpp == w32bppModeNum)
            {
                pMode->H_Size = wHSize;
                pMode->V_Size = wVSize;
                break;
            }
            else
                pMode++;
    };
};

