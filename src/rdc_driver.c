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
#include "mibstore.h"
#include "vgaHW.h"
#include "mipointer.h"
#include "micmap.h"

#include "fb.h"
#include "regionstr.h"
#include "xf86xv.h"
#include <X11/extensions/Xv.h>
#include "vbe.h"

#include "xf86PciInfo.h"
#include "xf86Pci.h"


#include "xf86fbman.h"


#include "xf86Cursor.h"


#include "rdc.h"


#if !XSERVER_LIBPCIACCESS
extern Bool RDCMapMem(ScrnInfoPtr pScrn);
#endif
extern Bool RDCUnmapMem(ScrnInfoPtr pScrn);
extern Bool RDCMapMMIO(ScrnInfoPtr pScrn);
extern void RDCUnmapMMIO(ScrnInfoPtr pScrn);
extern Bool RDCMapVBIOS(ScrnInfoPtr pScrn);
extern Bool RDCUnmapVBIOS(ScrnInfoPtr pScrn);

extern void vRDCOpenKey(ScrnInfoPtr pScrn);
extern Bool bRDCRegInit(ScrnInfoPtr pScrn);
extern ULONG GetVRAMInfo(ScrnInfoPtr pScrn);
extern Bool RDCFilterModeByBandWidth(ScrnInfoPtr pScrn, DisplayModePtr mode);
extern ULONG RDCGetMemBandWidth(ScrnInfoPtr pScrn);
extern void vRDCLoadPalette(ScrnInfoPtr pScrn, int numColors, int *indices, LOCO *colors, VisualPtr pVisual);
extern void RDCDisplayPowerManagementSet(ScrnInfoPtr pScrn, int PowerManagementMode, int flags);
extern void vSetStartAddressCRT1(RDCRecPtr pRDC, ULONG base);
extern Bool RDCSetMode(ScrnInfoPtr pScrn, DisplayModePtr mode);

extern Bool RDCAccelInit(ScreenPtr pScreen);

extern Bool RDCCursorInit(ScreenPtr pScreen);
extern DisplayModePtr RDCBuildModePool(ScrnInfoPtr pScrn);
extern void RDCVideoInit(ScreenPtr pScreen);


static void RDCIdentify(int flags);
const OptionInfoRec *RDCAvailableOptions(int chipid, int busid);
#if XSERVER_LIBPCIACCESS
static Bool rdc_pci_probe (DriverPtr drv, int entity_num, struct pci_device *dev, intptr_t match_data);
#else
static Bool RDCProbe(DriverPtr drv, int flags);
#endif
static Bool RDCPreInit(ScrnInfoPtr pScrn, int flags);
static Bool RDCScreenInit(int Index, ScreenPtr pScreen, int argc, char **argv);
Bool RDCSwitchMode(int scrnIndex, DisplayModePtr mode, int flags);
void RDCAdjustFrame(int scrnIndex, int x, int y, int flags);
static Bool RDCEnterVT(int scrnIndex, int flags);
static void RDCLeaveVT(int scrnIndex, int flags);
static void RDCFreeScreen(int scrnIndex, int flags);
static ModeStatus RDCValidMode(int scrnIndex, DisplayModePtr mode, Bool verbose, int flags);


static Bool RDCGetRec(ScrnInfoPtr pScrn);
static void RDCFreeRec(ScrnInfoPtr pScrn);
static Bool RDCSaveScreen(ScreenPtr pScreen, Bool unblack);
static Bool RDCCloseScreen(int scrnIndex, ScreenPtr pScreen);
static void RDCSave(ScrnInfoPtr pScrn);
static void RDCRestore(ScrnInfoPtr pScrn);
static void RDCProbeDDC(ScrnInfoPtr pScrn, int index);
static xf86MonPtr RDCDoDDC(ScrnInfoPtr pScrn, int index);
static void vFillRDCModeInfo (ScrnInfoPtr pScrn);
static Bool RDCModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode);
static void RDCSetHWCaps(RDCRecPtr pRDC);

static Bool RDCRandRGetInfo(ScrnInfoPtr pScrn, Rotation *rotations);
static Bool RDCRandRSetConfig(ScrnInfoPtr pScrn, xorgRRConfig *config);
static Bool RDCDriverFunc(ScrnInfoPtr pScrn, xorgDriverFuncOp op, pointer data);
static void RDCApertureInit(ScrnInfoPtr pScrn);











#if XSERVER_LIBPCIACCESS
#define RDC_DEVICE_MATCH(d,i) \
    { PCI_VENDOR_RDC, (d), PCI_MATCH_ANY, PCI_MATCH_ANY, 0, 0, (i) }

static const struct pci_id_match rdc_device_match[] = {
   RDC_DEVICE_MATCH (PCI_CHIP_M2010, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2010_A0, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2011, 0 ),
   RDC_DEVICE_MATCH (PCI_CHIP_M2012, 0 ),
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



static SymTabRec RDCChipsets[] = {
    {PCI_CHIP_M2010_A0, "M2010_A0"},
    {PCI_CHIP_M2010, "M2010"},
    {PCI_CHIP_M2011, "M2011"},
    {PCI_CHIP_M2012, "M2012"},
    {-1,              NULL}
};

static PciChipsets RDCPciChipsets[] = {
    {PCI_CHIP_M2010, PCI_CHIP_M2010, RES_SHARED_VGA},
    {PCI_CHIP_M2010_A0, PCI_CHIP_M2010_A0, RES_SHARED_VGA},
    {PCI_CHIP_M2011, PCI_CHIP_M2011, RES_SHARED_VGA},
    {PCI_CHIP_M2012, PCI_CHIP_M2012, RES_SHARED_VGA},
    {-1,              -1,              RES_UNDEFINED }
};

typedef enum {
    OPTION_NOACCEL,
    OPTION_MMIO2D,   
    OPTION_SW_CURSOR,
    OPTION_HWC_NUM,
    OPTION_ENG_CAPS,   
    OPTION_DBG_SELECT,
    OPTION_NO_DDC,
    OPTION_ACCELMETHOD,
    OPTION_RANDRROTATION    
} RDCOpts;

static const OptionInfoRec RDCOptions[] = {
    {OPTION_NOACCEL,    "NoAccel",      OPTV_BOOLEAN,   {0},    FALSE},
    {OPTION_MMIO2D,     "MMIO2D",       OPTV_BOOLEAN,   {0},    FALSE},   
    {OPTION_SW_CURSOR,  "SWCursor",     OPTV_BOOLEAN,   {0},    FALSE},    
    {OPTION_HWC_NUM,    "HWCNumber",    OPTV_INTEGER,   {0},    FALSE},
    {OPTION_ENG_CAPS,   "ENGCaps",      OPTV_INTEGER,   {0},    FALSE},                    
    {OPTION_NO_DDC,     "NoDDC",        OPTV_BOOLEAN,   {0},    FALSE},
    {OPTION_ACCELMETHOD, "AccelMethod", OPTV_STRING,    {0},    FALSE},
    {OPTION_RANDRROTATION, "RandRRotation", OPTV_BOOLEAN,    {0},    FALSE},
    {-1,                NULL,           OPTV_NONE,      {0},    FALSE}
};

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

const char *xaaSymbols[] = {
    "XAACreateInfoRec",
    "XAADestroyInfoRec",
    "XAAInit",
    "XAACopyROP",
    "XAAPatternROP",
    NULL
};

const char *ramdacSymbols[] = {
    "xf86CreateCursorInfoRec",
    "xf86DestroyCursorInfoRec",
    "xf86InitCursor",
    NULL
};


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

static pointer
RDCSetup(pointer module, pointer opts, int *errmaj, int *errmin)
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


static void
RDCIdentify(int flags)
{
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCIdentify()== \n");
    xf86PrintChipsets(RDC_NAME, "Driver for RDC Graphics Chipsets", RDCChipsets);
    xf86DrvMsgVerb(1, X_INFO, DefaultLevel, "==Exit RDCIdentify()== \n");
}

const OptionInfoRec *
RDCAvailableOptions(int chipid, int busid)
{
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCAvailableOptions()== return RDCOptions\n");
    return RDCOptions;
}

#if XSERVER_LIBPCIACCESS

static Bool rdc_pci_probe (DriverPtr		driver,
			   int		        entity_num,
			   struct pci_device	*device,
			   intptr_t		match_data)
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
	    break;
	}
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit rdc_pci_probe== \n");
    return pScrn != NULL;
}
#else

static Bool
RDCProbe(DriverPtr drv, int flags)
{
    int i, numUsed, numDevSections, *usedChips;
    Bool foundScreen = FALSE;
    GDevPtr *devSections;   

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCProbe== \n");

    
    if ((numDevSections = xf86MatchDevice(RDC_DRIVER_NAME, &devSections)) <= 0)
    {
        xf86DrvMsgVerb(0, X_INFO, 0, "==Exit1 RDCProbe()== return FALSE\n");
        return FALSE;
    }

    
    if (xf86GetPciVideoInfo() == NULL)
    {
        xf86DrvMsgVerb(0, X_INFO, 0, "==Exit2 RDCProbe()== return FALSE\n");
        return FALSE;
    }

    numUsed = xf86MatchPciInstances(RDC_NAME, PCI_VENDOR_RDC,
                                    RDCChipsets, RDCPciChipsets,
                                    devSections, numDevSections,
                                    drv, &usedChips);
    xf86DrvMsgVerb(0, X_INFO, 5, "numUsed = %d\n", numUsed);
    xfree(devSections);

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

    xfree(usedChips);

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit3 RDCProbe()== return(foundScreen=%X)\n", foundScreen);
    return foundScreen;
}
#endif


static Bool
RDCPreInit(ScrnInfoPtr pScrn, int flags)
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
    pRDC->PciTag  = pciTag(pRDC->PciInfo->bus, pRDC->PciInfo->device,
                           pRDC->PciInfo->func);
#endif

    
    xf86CollectOptions(pScrn, NULL);   
    if (!(pRDC->Options = xalloc(sizeof(RDCOptions))))
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
    else
    {
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "Option RandRRotation = FALSE\n");
        pRDC->bRandRRotation = FALSE;
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
    VGAHWPTR(pScrn)->PIOOffset = pRDC->PIOOffset = pRDC->IODBase + pRDC->PciInfo->regions[2].base_addr - 0x380;
    pRDC->RelocateIO = (IOADDRESS)(pRDC->PciInfo->regions[2].base_addr + pRDC->IODBase);
#else
    VGAHWPTR(pScrn)->PIOOffset = pRDC->PIOOffset = pRDC->IODBase + pRDC->PciInfo->ioBase[2] - 0x380;
    
    pRDC->RelocateIO = (IOADDRESS)(pRDC->PciInfo->ioBase[2] + pRDC->IODBase);
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
          
    pScrn->videoRam = GetVRAMInfo(pScrn) / 1024;
    from = X_DEFAULT;


    if (pRDC->pEnt->device->videoRam) 
    {
        pScrn->videoRam = pRDC->pEnt->device->videoRam;
        from = X_CONFIG;
    }

    pRDC->FbMapSize = pScrn->videoRam * 1024;
    pRDC->AvailableFBsize = pRDC->FbMapSize;
    
    pRDC->MMIOMapSize = DEFAULT_MMIO_SIZE;

    
    pRDC->MemoryBandwidth = RDCGetMemBandWidth(pScrn);

    
    if (!RDCMapMem(pScrn))
    {
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Map FB Memory Failed \n");
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit18 RDCPreInit()== return FALSE\n");
        return FALSE;
    }
   
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

    
    {
        CBIOS_ARGUMENTS CBiosArguments;
        CBIOS_Extension CBiosExtension;
     
        CBiosExtension.pCBiosArguments = &CBiosArguments;
        CBiosExtension.IOAddress = (USHORT)(pRDC->RelocateIO);
        CBiosExtension.VideoVirtualAddress = (ULONG)(pRDC->FBVirtualAddr);

        CBiosArguments.reg.x.AX = OEMFunction;
        CBiosArguments.reg.x.BX = CINT10DataInit;
        CBiosArguments.reg.ex.ECX = (ULONG)pRDC->BIOSVirtualAddr;
        CInt10(&CBiosExtension);
    }
    
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

    xf86DrvMsgVerb(0, X_INFO, 0, "Before xf86ValidateModes()\n");
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->modes = 0x%x\n", pScrn->modes);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->modePool = 0x%x\n", pScrn->modePool);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->display->modes = 0x%x\n", pScrn->display->modes);
    xf86DrvMsgVerb(0, X_INFO, 0, " *pScrn->display->modes = %s\n", *pScrn->display->modes);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->bitsPerPixel = %d\n", pScrn->bitsPerPixel);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->display->virtualX = %d\n", pScrn->display->virtualX);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->display->virtualY = %d\n", pScrn->display->virtualY);
    xf86DrvMsgVerb(0, X_INFO, 0, " pRDC->FbMapSize = 0x%x\n", pRDC->FbMapSize);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->virtualX = %d\n", pScrn->virtualX);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->virtualY = %d\n", pScrn->virtualY);

#ifdef FPGA
    i = xf86ValidateModes(pScrn, pScrn->modePool,
                          pScrn->display->modes, clockRanges,
                          0, 320, 1024, 8 * pScrn->bitsPerPixel,
                          200, 768,
                          pScrn->display->virtualX, pScrn->display->virtualY,
                          pRDC->FbMapSize, LOOKUP_BEST_REFRESH);
#else
    if (pRDC->bRandRRotation)
    {
        maxPitch = 1280;
        maxHeight = 1024;
    }
    else
    {
        maxPitch = 1920;
        maxHeight = 1200;
    }
    
    i = xf86ValidateModes(pScrn, pScrn->modePool,
                          pScrn->display->modes, clockRanges,
                          0, 320, maxPitch, 8 * pScrn->bitsPerPixel,
                          200, maxHeight,
                          pScrn->display->virtualX, pScrn->display->virtualY,
                          pRDC->FbMapSize, LOOKUP_BEST_REFRESH);
#endif
    xf86DrvMsgVerb(0, X_INFO, 0, "After xf86ValidateModes()\n");
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->modes = 0x%x\n", pScrn->modes);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->modePool = 0x%x\n", pScrn->modePool);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->display->modes = 0x%x\n", pScrn->display->modes);
    xf86DrvMsgVerb(0, X_INFO, 0, " *pScrn->display->modes = %s\n", *pScrn->display->modes);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->bitsPerPixel = %d\n", pScrn->bitsPerPixel);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->display->virtualX = %d\n", pScrn->display->virtualX);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->display->virtualY = %d\n", pScrn->display->virtualY);
    xf86DrvMsgVerb(0, X_INFO, 0, " pRDC->FbMapSize = 0x%x\n", pRDC->FbMapSize);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->virtualX = %d\n", pScrn->virtualX);
    xf86DrvMsgVerb(0, X_INFO, 0, " pScrn->virtualY = %d\n", pScrn->virtualY);

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

    pRDC->AvailableFBsize -= CAPTURE_BUFFER_SIZE;
 
    
    pRDC->noAccel = TRUE;
    pRDC->AccelInfoPtr = NULL; 
    pRDC->CMDQInfo.ulCMDQSize = 0;      

    if (pRDC->bRandRRotation)
    {
        RDCApertureInit(pScrn);
    }      

    
#ifdef    Accel_2D
    if (!xf86ReturnOptValBool(pRDC->Options, OPTION_NOACCEL, FALSE))
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
        
        pRDC->noAccel = FALSE; 
       
        pRDC->MMIO2D = TRUE;
#ifndef    MMIO_2D                   
        if (!xf86ReturnOptValBool(pRDC->Options, OPTION_MMIO2D, FALSE)) 
        {
            pRDC->CMDQInfo.ulCMDQSize = DEFAULT_CMDQ_SIZE;   
            pRDC->AvailableFBsize = pRDC->AvailableFBsize - pRDC->CMDQInfo.ulCMDQSize;
            pRDC->CMDQInfo.ulCMDQOffsetAddr = pRDC->AvailableFBsize;
            pRDC->MMIO2D = FALSE;        
        }    
#endif

        pRDC->ENGCaps = ENG_CAP_ALL;
        if (!xf86GetOptValInteger(pRDC->Options, OPTION_ENG_CAPS, &pRDC->ENGCaps)) 
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "No ENG Capability options found\n");          
        }
   }
#endif   

    
    RDCSetHWCaps(pRDC);
    
    
    pRDC->noHWC = TRUE; 
    pRDC->HWCInfoPtr = NULL;
#ifdef HWC   
    if (!xf86ReturnOptValBool(pRDC->Options, OPTION_SW_CURSOR, FALSE))
    {
        if (!xf86LoadSubModule(pScrn, "ramdac"))
        {
            RDCFreeRec(pScrn);
            xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Exit23 RDCPreInit()== return FALSE\n");
            return FALSE;
        }
      
        pRDC->noHWC = FALSE;  
        pRDC->HWCInfo.HWC_NUM = DEFAULT_HWC_NUM;
        
        if (!xf86GetOptValInteger(pRDC->Options, OPTION_HWC_NUM, &pRDC->HWCInfo.HWC_NUM))
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "No HWC_NUM options found\n");          
        }    

        if (pRDC->ENGCaps & ENG_CAP_HWC_MMIO)
            pRDC->AvailableFBsize = 
                pRDC->AvailableFBsize - (HQ_HWC_SIZE)*pRDC->HWCInfo.HWC_NUM;
        else
            pRDC->AvailableFBsize = 
                pRDC->AvailableFBsize - (HWC_SIZE+HWC_SIGNATURE_SIZE)*pRDC->HWCInfo.HWC_NUM;
                
        pRDC->HWCInfo.ulHWCOffsetAddr = pRDC->AvailableFBsize;
    }    
#endif

    
#ifndef XSERVER_LIBPCIACCESS
    xf86SetOperatingState(resVgaIo, pRDC->pEnt->index, ResUnusedOpr);
    xf86SetOperatingState(resVgaMem, pRDC->pEnt->index, ResDisableOpr);
#endif
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit24 RDCPreInit()== return TRUE\n");
    return TRUE;
}


static Bool
RDCScreenInit(int scrnIndex, ScreenPtr pScreen, int argc, char **argv)
{
    ScrnInfoPtr pScrn;
    RDCRecPtr pRDC;
    vgaHWPtr hwp;   
    VisualPtr visual;
 
    
    BoxRec FBMemBox;   
    int    AvailFBSize;     

    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==Enter RDCScreenInit()== \n");
    
    pScrn = xf86Screens[pScreen->myNum];
    pRDC = RDCPTR(pScrn);
    hwp = VGAHWPTR(pScrn);
      
    
    AvailFBSize = pRDC->AvailableFBsize;

    FBMemBox.x1 = 0;
    FBMemBox.y1 = 0;
    FBMemBox.x2 = pScrn->displayWidth;
    FBMemBox.y2 = (AvailFBSize / (pScrn->displayWidth * ((pScrn->bitsPerPixel+1)/8))) - 1;
    if (FBMemBox.y2 <= 0)
        FBMemBox.y2 = 32767; 
    
    xf86DrvMsgVerb(scrnIndex, X_INFO, InfoLevel, "AvailFBSize = %d\n", AvailFBSize);
    xf86DrvMsgVerb(scrnIndex, X_INFO, InfoLevel, "FBMemBox: x1 = %d, y1 = %d, x2 = %d, y2 = %d\n",FBMemBox.x1, FBMemBox.y1, FBMemBox.x2, FBMemBox.y2);

    if (!xf86InitFBManager(pScreen, &FBMemBox))
    {
        xf86DrvMsg(scrnIndex, X_ERROR, "Failed to init memory manager\n");
        return FALSE;
    }      
       
    vgaHWGetIOBase(hwp);
 
    vFillRDCModeInfo (pScrn);      
 
    miClearVisualTypes();

    
    if (!miSetVisualTypes(pScrn->depth, miGetDefaultVisualMask(pScrn->depth), pScrn->rgbBits, pScrn->defaultVisual))
    {
        xf86DrvMsgVerb(scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() Set Visual Type Fail== return FALSE\n");
        return FALSE;
    }
 
    if (!miSetPixmapDepths())
    {
        RDCSaveScreen(pScreen, SCREEN_SAVER_OFF);
        xf86DrvMsgVerb(scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() SetPixmapDepth fail== return FALSE\n");
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
                xf86DrvMsgVerb(scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() fbScreenInit fail== return FALSE\n");
                return FALSE;
            }
            break;

        default:
            xf86DrvMsgVerb(scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() Color Depth not supprt== return FALSE\n");
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

#ifdef Accel_2D
    if (!pRDC->noAccel)
    {
        if (!RDCAccelInit(pScreen))
        {
            xf86DrvMsg(scrnIndex, X_ERROR, "Hardware acceleration initialization failed\n");
            pRDC->noAccel = TRUE;           
        }
    }
#endif 
     
    //miInitializeBackingStore(pScreen);
    xf86SetBackingStore(pScreen);
    xf86SetSilkenMouse(pScreen);

    miDCInitialize(pScreen, xf86GetPointerScreenFuncs());

    
    if (!pRDC->noHWC)
    {
        if (!RDCCursorInit(pScreen)) 
        {
            xf86DrvMsg(scrnIndex, X_ERROR, "Hardware cursor initialization failed\n");
            pRDC->noHWC = TRUE;                      
        }
    }

    if (pRDC->bRandRRotation)
    {
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "assign pScrn->DriverFunc\n");
        pScrn->DriverFunc = RDCDriverFunc;
    }      
    
    if (!miCreateDefColormap(pScreen))
    {
        xf86DrvMsgVerb(scrnIndex, X_INFO, ErrorLevel, "==Exit7 RDCScreenInit()== return FALSE\n");
        return FALSE;
    }

    if(!xf86HandleColormaps(pScreen, 256, (pScrn->depth == 8) ? 8 : pScrn->rgbBits,
                            vRDCLoadPalette, NULL,
                            CMAP_PALETTED_TRUECOLOR | CMAP_RELOAD_ON_MODE_SWITCH))
    {
        xf86DrvMsgVerb(scrnIndex, X_INFO, ErrorLevel, "==RDCScreenInit() xf86HandleColormaps fail== return FALSE\n");
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

    
#ifdef Accel_2D
    if (!pRDC->noAccel)
    {
        if ((DEVICE_ID(pRDC->PciInfo) == PCI_CHIP_M2010) || (DEVICE_ID(pRDC->PciInfo) == PCI_CHIP_M2010_A0))
        {
            bInitCMDQInfo(pScrn, pRDC);
        }
        else
        {
            bCRInitCMDQInfo(pScrn, pRDC);
        }
    }
#endif
    
    RDCSave(pScrn);     
    if (!RDCModeInit(pScrn, pScrn->currentMode))
    {
        xf86DrvMsg(scrnIndex, X_ERROR, "Mode Init Failed \n");
        return FALSE;
    }   

    RDCSaveScreen(pScreen, FALSE);
    RDCAdjustFrame(scrnIndex, pScrn->frameX0, pScrn->frameY0, 0);

    EC_DetectCaps(pScrn, &(pRDC->ECChipInfo));

    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==RDCScreenInit() Normal Exit==\n");
    return TRUE;
} 

Bool RDCSwitchMode(int scrnIndex, DisplayModePtr mode, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    Bool RetStatus = FALSE;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCSwitchMode()== \n");
   
    RetStatus = RDCModeInit(pScrn, mode);

    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "== RDCSwitchMode() Exit== return %X\n", RetStatus);
    return RetStatus;
}

void
RDCAdjustFrame(int scrnIndex, int x, int y, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    RDCRecPtr   pRDC  = RDCPTR(pScrn);
    ULONG base;
    int rot_x, rot_y;

    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==Enter RDCAdjustFrame(x = %d, y = %d)== \n", x, y);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pScrn->virtualX = %d\n", pScrn->virtualX);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pScrn->virtualY = %d\n", pScrn->virtualY);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pScrn->displayWidth = %d\n", pScrn->displayWidth);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pRDC->VideoModeInfo.ScreenWidth = %d\n", pRDC->VideoModeInfo.ScreenWidth);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pRDC->VideoModeInfo.ScreenHeight = %d\n", pRDC->VideoModeInfo.ScreenHeight);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pRDC->VideoModeInfo.ScreenPitch = %d\n", pRDC->VideoModeInfo.ScreenPitch);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  pRDC->VideoModeInfo.Bpp = %d\n", pRDC->VideoModeInfo.Bpp);

    
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
            rot_y = pScrn->displayWidth - pRDC->VideoModeInfo.ScreenHeight - x;
            break;

        case RR_Rotate_180:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_180\n");
            rot_x = pScrn->displayWidth - pRDC->VideoModeInfo.ScreenWidth - x;
            rot_y = pScrn->virtualY - pRDC->VideoModeInfo.ScreenHeight - y;
            break;


        case RR_Rotate_270:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, " case RR_Rotate_270\n");
            rot_x = pScrn->displayWidth - pRDC->VideoModeInfo.ScreenWidth - y;
            rot_y = x;
            break;

        default:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_ERROR, ErrorLevel, "  Unexpected rotation in RDCAdjustFrame\n");
            return;
    }
    
    base = rot_y * pRDC->VideoModeInfo.ScreenPitch + 
           rot_x * pRDC->VideoModeInfo.Bpp;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "  base = %x\n", base);
    vSetStartAddressCRT1(pRDC, base);

    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCAdjustFrame()== \n");
}

        
static Bool
RDCEnterVT(int scrnIndex, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    RDCRecPtr pRDC = RDCPTR(pScrn);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCEnterVT()== \n");
    if (!RDCModeInit(pScrn, pScrn->currentMode))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit1 RDCEnterVT() RDCModeInit Fail== return FALSE\n");
        return FALSE;
    }

    RDCAdjustFrame(scrnIndex, pScrn->frameX0, pScrn->frameY0, 0);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit2 RDCEnterVT() Normal Exit== return TRUE\n");
    return TRUE;
}


static void
RDCLeaveVT(int scrnIndex, int flags)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    RDCRecPtr pRDC = RDCPTR(pScrn);
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCLeaveVT();== \n");
    
#ifdef HWC
    if (!xf86ReturnOptValBool(pRDC->Options, OPTION_SW_CURSOR, FALSE))
    {
        pRDC->HWCInfoPtr->HideCursor(pScrn);
    }
#endif

#ifdef Accel_2D  
    pRDC->CMDQInfo.Disable2D(pScrn, pRDC);
#endif
      
    RDCRestore(pScrn);  
    vgaHWLock(hwp);
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCLeaveVT() Normal Exit== \n");
}

static void
RDCFreeScreen(int scrnIndex, int flags)
{
    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==Enter RDCFreeScreen()== \n");
    
    RDCFreeRec(xf86Screens[scrnIndex]);
    if (xf86LoaderCheckSymbol("vgaHWFreeHWRec"))
        vgaHWFreeHWRec(xf86Screens[scrnIndex]);

    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCFreeScreen()== \n");
}

static ModeStatus
RDCValidMode(int scrnIndex, DisplayModePtr mode, Bool verbose, int flags)
{
    Bool Flags = MODE_NOMODE;

    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    CBIOS_ARGUMENTS CBiosArguments;
    CBIOS_Extension CBiosExtension;
    USHORT wLCDHorSize, wLCDVerSize;
    USHORT wVESAModeHorSize, wVESAModeVerSize;

    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==Enter RDCValidMode() Verbose = %d, Flags = 0x%x==\n", 
               verbose, flags);
    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==Mode name=%s, Width=%d, Height=%d, Refresh reate=%f==\n", 
               mode->name, mode->HDisplay, mode->VDisplay, mode->VRefresh);

    CBiosExtension.pCBiosArguments = &CBiosArguments;
    CBiosExtension.IOAddress = (USHORT)(pRDC->RelocateIO);
    CBiosExtension.VideoVirtualAddress = (ULONG)(pRDC->FBVirtualAddr);
    CBiosArguments.reg.x.AX = OEMFunction;
    CBiosArguments.reg.x.BX = QueryDisplayPathInfo;
    CInt10(&CBiosExtension);
    pRDC->DeviceInfo.ucDeviceID = (CBiosArguments.reg.ex.EBX & 0xf0000) >> 16;
    

    if (mode->Flags & V_INTERLACE)
    {
        if (verbose)
        {
            xf86DrvMsgVerb(scrnIndex, X_PROBED, InfoLevel,
                       "==Removing interlaced mode \"%s\"\n==", mode->name);
        }
        xf86DrvMsgVerb(scrnIndex, X_INFO, ErrorLevel, "== RDCValidMode() Fail, Not Interlace Mode==\n");
        return MODE_NO_INTERLACE;
    }

    if (pRDC->DeviceInfo.ucDeviceID == TVINDEX) 
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
        if ((mode->HDisplay > 1680) || (mode->VDisplay > 1050))
        {
            return MODE_BAD;
        }

        if ((mode->HDisplay < 640) || (mode->VDisplay < 480))
        {
            return MODE_BAD;
        }

        if (mode->HDisplay ==720) 
        {
            return MODE_BAD;
        }
        
        if ((mode->HDisplay >= 1280) && 
            ((pScrn->bitsPerPixel > 16) || (mode->VRefresh > 62.0)))
        {
            return MODE_BAD;
        }
    }
    
    
    
    
    
    Flags = RDCFilterModeByBandWidth(pScrn, mode);

    return Flags;
}            




static Bool
RDCGetRec(ScrnInfoPtr pScrn)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCGetRec()== \n");

    if (pScrn->driverPrivate)
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==RDCGetRec() driverPrivate is TRUE== return TRUE\n");
        return TRUE;
    }

    pScrn->driverPrivate = xnfcalloc(sizeof(RDCRec), 1);
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCGetRec()== return TRUE\n");
    return TRUE;
}

static void
RDCFreeRec(ScrnInfoPtr pScrn)
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
    
    xfree(pScrn->driverPrivate);
    pScrn->driverPrivate = 0;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit3 RDCFreeRec()== \n");
}

static Bool
RDCSaveScreen(ScreenPtr pScreen, Bool unblack)
{
    Bool RetStatus;
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Enter RDCSaveScreen(unblack = %d)== \n", unblack);
    
    RetStatus = vgaHWSaveScreen(pScreen, unblack);
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==Exit1 RDCSaveScreen()== return RetStatus\n");
    return RetStatus;
}

static Bool
RDCCloseScreen(int scrnIndex, ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86Screens[scrnIndex];
    vgaHWPtr hwp = VGAHWPTR(pScrn);
    RDCRecPtr pRDC = RDCPTR(pScrn);
    Bool RetStatus;
    
    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==Enter RDCCloseScreen(); Screen Index = 0x%x == \n",scrnIndex);
    
    if (pScrn->vtSema == TRUE)
    {  
#ifdef HWC
        pRDC->HWCInfoPtr->HideCursor(pScrn);
#endif
          
#ifdef Accel_2D  
        pRDC->CMDQInfo.Disable2D(pScrn, pRDC);
#endif
         
        RDCRestore(pScrn);
        vgaHWLock(hwp);

        if (pRDC->DeviceInfo.ucDeviceID == TVINDEX) 
        {
            UnLockCR0ToCR7();
            LoadTVTiming(pRDC->DeviceInfo.ucDisplayPath, 0x3, pRDC->DeviceInfo.ucDeviceID); 
        }
    }

    RDCUnmapMem(pScrn);
    RDCUnmapMMIO(pScrn);
    RDCUnmapVBIOS(pScrn);
    
    vgaHWUnmapMem(pScrn);

    if(pRDC->AccelInfoPtr)
    {
        XAADestroyInfoRec(pRDC->AccelInfoPtr);
        pRDC->AccelInfoPtr = NULL;
    }
 
    if(pRDC->HWCInfoPtr)
    {
        xf86DestroyCursorInfoRec(pRDC->HWCInfoPtr);
        pRDC->HWCInfoPtr = NULL;
    }
 
    pScrn->vtSema = FALSE;
    pScreen->CloseScreen = pRDC->CloseScreen;
    RetStatus = (*pScreen->CloseScreen) (scrnIndex, pScreen);
    
    xf86DrvMsgVerb(scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCCloseScreen()== return(RetStatus=%X\n", RetStatus);
    return RetStatus;
}

static void
RDCSave(ScrnInfoPtr pScrn)
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

    
    GetIndexReg(CRTC_PORT, 0xC0, RDCReg->ucPLLValue[0]);
    GetIndexReg(CRTC_PORT, 0xC1, RDCReg->ucPLLValue[1]);
    GetIndexReg(CRTC_PORT, 0xCF, RDCReg->ucPLLValue[2]);

    GetIndexReg(CRTC_PORT, 0xA3, RDCReg->ucCRA3);
    GetIndexReg(SEQ_PORT, 0x58, RDCReg->ucSR58);
    GetIndexReg(SEQ_PORT, 0x70, RDCReg->ucSR70);
    GetIndexReg(SEQ_PORT, 0x74, RDCReg->ucSR74);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCSave()== \n");
}

static void
RDCRestore(ScrnInfoPtr pScrn)
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
    
       
    SetIndexReg(CRTC_PORT, 0xC0, RDCReg->ucPLLValue[0]);
    SetIndexReg(CRTC_PORT, 0xC1, RDCReg->ucPLLValue[1]);
    SetIndexReg(CRTC_PORT, 0xCF, RDCReg->ucPLLValue[2]);

    SetIndexRegMask(CRTC_PORT, 0xA3, ~0x20, RDCReg->ucCRA3 & 0x20);
    
    SetIndexRegMask(CRTC_PORT, 0xBB, 0xFF, 0);

    SetIndexReg(SEQ_PORT, 0x58, RDCReg->ucSR58);

    SetIndexReg(SEQ_PORT, 0x70, RDCReg->ucSR70);

    SetIndexReg(SEQ_PORT, 0x74, RDCReg->ucSR74);

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit1 RDCRestore()== \n");    
}

static void
RDCProbeDDC(ScrnInfoPtr pScrn, int index)
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

static xf86MonPtr
RDCDoDDC(ScrnInfoPtr pScrn, int index)
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
    
    if (pVbe)
    {
        MonInfo = vbeDoEDID(pVbe, NULL);
        xf86PrintEDID(MonInfo);
        xf86SetDDCproperties(pScrn, MonInfo);
    }
    else
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel,
          "this driver cannot do DDC without VBE\n");
    }
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit2 RDCDoDDC()== return (MonInfo)\n");
    return MonInfo;
}

static void
vFillRDCModeInfo (ScrnInfoPtr pScrn)
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

static Bool
RDCModeInit(ScrnInfoPtr pScrn, DisplayModePtr mode)
{
    vgaHWPtr hwp;
    RDCRecPtr pRDC;
    CBIOS_Extension CBiosExtension;
    CBIOS_ARGUMENTS CBiosArguments;

    hwp = VGAHWPTR(pScrn);
    pRDC = RDCPTR(pScrn);

    xorgRRConfig    config;
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Enter RDCModeInit()== \n");

#ifdef HWC
    pRDC->HWCInfoPtr->HideCursor(pScrn);
#endif

#ifdef Accel_2D 
    pRDC->CMDQInfo.Disable2D(pScrn, pRDC);
#endif

    CBiosExtension.pCBiosArguments = &CBiosArguments;
    CBiosExtension.IOAddress = (USHORT)(pRDC->RelocateIO);
    CBiosArguments.reg.x.AX = OEMFunction;
    CBiosArguments.reg.x.BX = QueryDisplayPathInfo;
    CInt10(&CBiosExtension);
    pRDC->DeviceInfo.ucNewDeviceID = ((CBiosArguments.reg.ex.EBX >> 22) & 0xF);
    
    if (pRDC->DeviceInfo.ucNewDeviceID != LCDINDEX) 
    {
        if (mode->PrivFlags & LCD_TIMING)
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==RDCModeInit() Fail== LCD timing is not supported!!\n");
            return FALSE;
        }
    }
    
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

    
    CBiosArguments.reg.x.AX = OEMFunction;
    CBiosArguments.reg.x.BX = QueryDisplayPathInfo;
    CInt10(&CBiosExtension);
    if (CBiosArguments.reg.x.AX == VBEFunctionCallSuccessful)
    {
        pRDC->DeviceInfo.ucDeviceID = (CBiosArguments.reg.ex.EBX & 0x000F0000) >> 16;
        pRDC->DeviceInfo.ucDisplayPath = 1;
        pRDC->DeviceInfo.ScalerConfig.EnableHorScaler = ((CBiosArguments.reg.ex.EBX & 0x00200000) ? true : false);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "H scaler enable = %d\n", pRDC->DeviceInfo.ScalerConfig.EnableHorScaler);
        pRDC->DeviceInfo.ScalerConfig.EnableVerScaler = ((CBiosArguments.reg.ex.EBX & 0x00100000) ? true : false);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "V scaler enable = %d\n", pRDC->DeviceInfo.ScalerConfig.EnableVerScaler);
    }
    else
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_ERROR, ErrorLevel, "Query Display Path Info Fail(%04X)\n", CBiosArguments.reg.x.AX);
    }

    CBiosArguments.reg.x.AX = OEMFunction;
    CBiosArguments.reg.x.BX = QueryLCDPanelSizeMode;
    CBiosArguments.reg.lh.CL = 0;
    CInt10(&CBiosExtension);
    if (CBiosArguments.reg.x.AX == VBEFunctionCallSuccessful)
    {
        pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution = CBiosArguments.reg.x.DX;
        pRDC->DeviceInfo.ScalerConfig.ulHorScalingFactor = (pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution << 12) / mode->HDisplay;
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "LCD H Size = %d\n", pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "Mode H Size = %d\n", mode->HDisplay);

        pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution = CBiosArguments.reg.ex.EDX >> 16;
        pRDC->DeviceInfo.ScalerConfig.ulVerScalingFactor = (pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution << 11) / mode->VDisplay;
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "LCD V Size = %d\n", pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution);
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InternalLevel, "Mode V Size = %d\n", mode->VDisplay);
    }
    else
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_ERROR, ErrorLevel, "Query LCD Panel Size Fail(%04X)\n", CBiosArguments.reg.x.AX);
    }

    
    pRDC->HWCInfo.iScreenOffset_x = 0;
    pRDC->HWCInfo.iScreenOffset_y = 0;
    RDCAdjustFrame(pScrn->scrnIndex, 0, 0, 0);
 
    vgaHWProtect(pScrn, FALSE);

#ifdef Accel_2D
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 7, "==RDCModeInit() Enable 2D== \n");
    if (!pRDC->noAccel) 
    {
        if (!pRDC->CMDQInfo.Enable2D(pScrn, pRDC)) 
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Enable 2D failed\n");  
            pRDC->noAccel = TRUE;                        
        }           
    }
#endif

#ifdef HWC
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 7, "==RDCModeInit() Enable Cursor== \n");
    if (!pRDC->noHWC) 
    {
        if (!bInitHWC(pScrn, pRDC))
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Init HWC failed\n");
            pRDC->noHWC = TRUE;                         
        }           
    }
#endif       

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "==Exit RDCModeInit()== return TRUE\n");
    return TRUE;
}

static void RDCSetHWCaps(RDCRecPtr pRDC)
{
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCSetHWCaps() Entry==\n");

    switch(DEVICE_ID(pRDC->PciInfo))
    {
        case PCI_CHIP_M2010_A0:
            break;
        case PCI_CHIP_M2010:
            pRDC->ENGCaps |= ENG_CAP_VIDEO_DISP;
            pRDC->ENGCaps |= ENG_CAP_HWC_MMIO;
            break;
        case PCI_CHIP_M2012:
            pRDC->ENGCaps |= ENG_CAP_VIDEO_DISP;
            pRDC->ENGCaps |= ENG_CAP_VIDEO_POST;
            pRDC->ENGCaps |= ENG_CAP_HWC_MMIO;
            pRDC->ENGCaps |= ENG_CAP_CR_SUPPORT;
            break;
        case PCI_CHIP_M2011:
            break;
        default:
            break;
    }
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCSetHWCaps() Exit Caps = 0x%x==\n",pRDC->ENGCaps);
}


static Bool
RDCRandRGetInfo(ScrnInfoPtr pScrn, Rotation *rotations)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "RDCRandRGetInfo\n");
    
    *rotations = (RR_Rotate_0|RR_Rotate_90|RR_Rotate_180|RR_Rotate_270);

    return TRUE;
}

static Bool
RDCRandRSetConfig(ScrnInfoPtr pScrn, xorgRRConfig *config)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, DefaultLevel, "RDCRandRSetConfig\n");
    RDCRecPtr   pRDC = RDCPTR(pScrn);
    ULONG       ulROTAP_CTL0 = 0;

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
            pRDC->rotate = config->rotation;
            *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x5;
            *ROTAP_END_ADDR0 = pRDC->VideoModeInfo.ScreenPitch * pScrn->displayWidth;
            *ROTAP_PITCH0 = (pScrn->displayWidth << 16) | pScrn->displayWidth;
            *ROTAP_SRC_PITCH_RECI0 = ((1 << 30) + (pScrn->displayWidth >> 1)) / pScrn->displayWidth;
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
            pRDC->rotate = config->rotation;
            *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x6;
            *ROTAP_END_ADDR0 = pRDC->VideoModeInfo.ScreenPitch * pScrn->virtualY;
            *ROTAP_PITCH0 = (pScrn->displayWidth << 16) | pScrn->displayWidth;
            *ROTAP_SRC_PITCH_RECI0 = ((1 << 30) + (pScrn->displayWidth >> 1)) / pScrn->displayWidth;
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
            pRDC->rotate = config->rotation;
            *(ULONG *)(pRDC->MMIOVirtualAddr + 0x8094) = 0x7;
            *ROTAP_END_ADDR0 = pRDC->VideoModeInfo.ScreenPitch * pScrn->displayWidth;
            *ROTAP_PITCH0 = (pScrn->displayWidth << 16) | pScrn->displayWidth;
            *ROTAP_SRC_PITCH_RECI0 = ((1 << 30) + (pScrn->displayWidth >> 1)) / pScrn->displayWidth;
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
            pRDC->rotate = RR_Rotate_0;
            return FALSE;
    }

    *ROTAP_CTL0 = ulROTAP_CTL0;
            
    return TRUE;
}




static Bool
RDCDriverFunc(ScrnInfoPtr pScrn, xorgDriverFuncOp op, pointer data)
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

static void RDCApertureInit(ScrnInfoPtr pScrn)
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
