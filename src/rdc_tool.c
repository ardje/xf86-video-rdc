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

#include <stdio.h>

#include "xf86.h"
#include "xf86_OSproc.h"
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


#ifdef HAVE_XAA
#include "xaa.h"
#endif
#include "xaarop.h"


#include "xf86Cursor.h"


#define _rdc_tool_c_
#include "rdc.h"
#include "rdc_extension.h"

RDC_EXPORT Bool RDCMapMem(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    
#if XSERVER_LIBPCIACCESS
    struct pci_device *const device = pRDC->PciInfo;
    int err;
    
    err = pci_device_map_range(device, pRDC->FBPhysAddr, pRDC->FbMapSize, 
                                   PCI_DEV_MAP_FLAG_WRITABLE | PCI_DEV_MAP_FLAG_WRITE_COMBINE,
                                   (void **) &pRDC->FBVirtualAddr);
    
    if (err)
    {
        xf86DrvMsg (pScrn->scrnIndex, X_ERROR,
        "Unable to map frame buffer BAR. %s (%d)\n",
        strerror (err), err);
        return FALSE;
    }
#else
    pRDC->FBVirtualAddr = xf86MapPciMem(pScrn->scrnIndex, VIDMEM_FRAMEBUFFER,
                                        pRDC->PciTag,
                                        pRDC->FBPhysAddr, pRDC->FbMapSize);
#endif

    if (!pRDC->FBVirtualAddr)
        return FALSE;

    return TRUE;
}

RDC_EXPORT Bool RDCUnmapMem(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);

#if XSERVER_LIBPCIACCESS
       pci_device_unmap_range (pRDC->PciInfo, pRDC->FBVirtualAddr, pRDC->FbMapSize);
#else 
    xf86UnMapVidMem(pScrn->scrnIndex, (pointer) pRDC->FBVirtualAddr,
                    pRDC->FbMapSize);
#endif

    pRDC->FBVirtualAddr = 0;
   
    return TRUE;
}


RDC_EXPORT Bool RDCMapMMIO(ScrnInfoPtr pScrn)
{
   int mmioFlags;
    RDCRecPtr pRDC = RDCPTR(pScrn);

#if !defined(__alpha__)
    mmioFlags = VIDMEM_MMIO | VIDMEM_READSIDEEFFECT;
#else
    mmioFlags = VIDMEM_MMIO | VIDMEM_READSIDEEFFECT | VIDMEM_SPARSE;
#endif

#if XSERVER_LIBPCIACCESS
    struct pci_device *const device = pRDC->PciInfo;
        
    pci_device_map_range(device, pRDC->MMIOPhysAddr, pRDC->MMIOMapSize,
                         PCI_DEV_MAP_FLAG_WRITABLE | PCI_DEV_MAP_FLAG_WRITE_COMBINE,
                         (void **) &pRDC->MMIOVirtualAddr);
#else
    pRDC->MMIOVirtualAddr = xf86MapPciMem(pScrn->scrnIndex, mmioFlags,
                                          pRDC->PciTag,
                                          pRDC->MMIOPhysAddr, pRDC->MMIOMapSize);
#endif

    if (!pRDC->MMIOVirtualAddr)
        return FALSE;

    return TRUE;
}

RDC_EXPORT void RDCUnmapMMIO(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);

#if XSERVER_LIBPCIACCESS
        pci_device_unmap_range (pRDC->PciInfo, pRDC->MMIOVirtualAddr, pRDC->MMIOMapSize);
#else
    xf86UnMapVidMem(pScrn->scrnIndex, (pointer) pRDC->MMIOVirtualAddr,
                    pRDC->MMIOMapSize);
#endif

    pRDC->MMIOVirtualAddr = 0;
   
}


RDC_EXPORT Bool RDCMapVBIOS(ScrnInfoPtr pScrn)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);
    FILE        *fpVBIOS;
    int         i;
    Bool        bVBIOSExist = FALSE;
    WORD        VenID, DevID;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Enter RDCMapVBIOS()==\n");
    pRDC->ulROMType = 0;

    
    if (pRDC->ulROMType == 0)
    {
        pRDC->BIOSVirtualAddr = xf86MapVidMem(pScrn->scrnIndex, VIDMEM_READONLY, BIOS_ROM_PHY_BASE, BIOS_ROM_SIZE);

        
        VenID = *(USHORT*)(pRDC->BIOSVirtualAddr+0x40);
        DevID = *(USHORT*)(pRDC->BIOSVirtualAddr+0x42);

        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 0, "Vendor ID = %X, Device ID = %X\n", VenID, DevID);

        if (VenID == PCI_VENDOR_RDC)
        {
            if (DevID == PCI_CHIP_M2010 ||
                DevID == PCI_CHIP_M2011 ||
                DevID == PCI_CHIP_M2012 ||
                DevID == PCI_CHIP_M2013 ||
                DevID == PCI_CHIP_M2014 ||
                DevID == PCI_CHIP_M2015 ||
                DevID == PCI_CHIP_M2200 ||
                DevID == PCI_CHIP_M2010_A0)
            {
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 0, "VBIOS exist\n");
                bVBIOSExist = TRUE;
            };
        };
        
        if (bVBIOSExist)
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 0, "pRDC->BIOSVirtualAddr = 0x%08x\n", pRDC->BIOSVirtualAddr);
            pRDC->ulROMType = 1;
        }
        else
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "VBIOS not exist\n");
        }
    }

    
    if (pRDC->ulROMType == 0)
    {
        
        fpVBIOS = fopen(BIOS_ROM_PATH_FILE, "r");
        if (!fpVBIOS)
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "BIOS ROM file \"/usr/lib/xorg/modules/drivers/RDCVBIOS.ROM\" not found()==\n");
        }
    
        
        pRDC->BIOSVirtualAddr = xnfalloc(BIOS_ROM_SIZE);
        if (!pRDC->BIOSVirtualAddr)
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "Read BIOS ROM file: Mem Allocate Fail!!==\n");
            fclose (fpVBIOS);
        }

        
        if (fpVBIOS && pRDC->BIOSVirtualAddr)
        {
            pRDC->ulROMType = 2;
            for (i = 0; i < BIOS_ROM_SIZE; i++)
            {
                fscanf(fpVBIOS, "%c", pRDC->BIOSVirtualAddr+i);
            }
            fclose (fpVBIOS);
        }
    }

    if (pRDC->ulROMType == 0)
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit1 RDCMapVBIOS()== No VBIOS\n");
        return FALSE; 
    }

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "pRDC->ulROMType = %d\n", pRDC->ulROMType);

    if ((*(USHORT*)pRDC->BIOSVirtualAddr == 0xAA55) && (*(USHORT*)(pRDC->BIOSVirtualAddr+0x40) == PCI_VENDOR_RDC))
    {
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit RDCMapVBIOS()== return TRUE\n");
        return TRUE;
    }
    else
    {
        RDCUnmapVBIOS(pScrn);

        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Exit2 RDCMapVBIOS()== Not RDC VBIOS\n");
        return FALSE; 
    }
}

RDC_EXPORT Bool RDCUnmapVBIOS(ScrnInfoPtr pScrn)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);

    if (pRDC->ulROMType == 1)
    {
        xf86UnMapVidMem(pScrn->scrnIndex, (pointer) pRDC->BIOSVirtualAddr, BIOS_ROM_SIZE);
    }
    else if (pRDC->ulROMType == 2)
    {
        free(pRDC->BIOSVirtualAddr);
    }
    pRDC->BIOSVirtualAddr = 0;
   
    return TRUE;
}


RDC_EXPORT ULONG EC_ReadPortUchar(BYTE *port, BYTE *value)
{
    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Enter EC_ReadPortUchar()\n");

    int i;
    for (i=0 ; i<10 ; i++)
    {
        usleep(700);
        
        
        
        if (inb(0x66) & BIT0)
        {
            *value = (BYTE)inb((ULONG)port);
            return EC_ACCESS_SUCCESS;
        }
    };

    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Leave EC_ReadPortUchar()\n");
    
    return EC_ACCESS_FAIL;
};

RDC_EXPORT void EC_WritePortUchar(BYTE *port, BYTE data)
{
    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Enter EC_WritePortUchar()\n");
    
    int i;
    for (i=0 ; i<10 ; i++)
    {
        usleep(700);
        
        
        
        if (!(inb(0x66) & BIT1)) 
        {
            outb((ULONG)port, data);
            break;
        };
    };

    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Leave EC_WritePortUchar()\n");
};

RDC_EXPORT void EC_DetectCaps(ScrnInfoPtr pScrn, ECINFO* pECChip)
{
    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Enter EC_DetectCaps()\n");
    
    BYTE bP41Level = 0x0, bP80Level = 0x0, bP41temp = 0x0, bP80temp = 0x0;
    ULONG bSuccess = EC_ACCESS_FAIL;

    
    EC_WritePortUchar((BYTE*)0x66, 0x41);
    bSuccess = EC_ReadPortUchar((BYTE*)0x62,&bP41Level);

    
    EC_WritePortUchar((BYTE*)0x66, 0x80);
    EC_WritePortUchar((BYTE*)0x62, 0x95);
    bSuccess = EC_ReadPortUchar((BYTE*)0x62,&bP80Level);

    
    EC_WritePortUchar((BYTE*)0x66, 0x40);
    EC_WritePortUchar((BYTE*)0x62, 0xD);

    
    EC_WritePortUchar((BYTE*)0x66, 0x41);
    bSuccess = EC_ReadPortUchar((BYTE*)0x62,&bP41temp);

    
    EC_WritePortUchar((BYTE*)0x66, 0x80);
    EC_WritePortUchar((BYTE*)0x62, 0x95);
    bSuccess = EC_ReadPortUchar((BYTE*)0x62,&bP80temp);

    if(0xD == bP41temp)
    {
        EC_WritePortUchar((BYTE*)0x66, 0x40);
        EC_WritePortUchar((BYTE*)0x62, bP41Level);
        pECChip->bECExist = TRUE;
        pECChip->bNewEC = TRUE;
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "RDC: New EC has been detected.\n");
    }
    else if(0xD == bP80temp)
    {
        EC_WritePortUchar((BYTE*)0x66, 0x40);
        EC_WritePortUchar((BYTE*)0x62, bP80Level);
        pECChip->bECExist = TRUE;
        pECChip->bNewEC = FALSE;
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "RDC: Old EC has been detected.\n");
    }
    else
    {
        pECChip->bECExist = FALSE;
        pECChip->bNewEC = FALSE;
        xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "RDC: EC has been NOT detected.\n");
    }

    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Leave EC_DetectCaps()\n");            
    return;
}

