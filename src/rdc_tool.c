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

#include <stdio.h>

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86cmap.h"
#include "compiler.h"
//#include "mibstore.h"
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


//#include "xaa.h"
#include "xaarop.h"


#include "xf86Cursor.h"


#include "rdc.h"


Bool RDCMapMem(ScrnInfoPtr pScrn);
Bool RDCUnmapMem(ScrnInfoPtr pScrn);
Bool RDCMapMMIO(ScrnInfoPtr pScrn);
void RDCUnmapMMIO(ScrnInfoPtr pScrn);
Bool RDCMapVBIOS(ScrnInfoPtr pScrn);
Bool RDCUnmapVBIOS(ScrnInfoPtr pScrn);
ULONG EC_ReadPortUchar(BYTE *port, BYTE *value);
void EC_WritePortUchar(BYTE *port, BYTE data);
void EC_DetectCaps(ScrnInfoPtr pScrn, ECINFO* pECChip);


// map, unmap frame buffer
Bool
RDCMapMem(ScrnInfoPtr pScrn)
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

Bool
RDCUnmapMem(ScrnInfoPtr pScrn)
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

// map, ummap MMIO
Bool
RDCMapMMIO(ScrnInfoPtr pScrn)
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

void
RDCUnmapMMIO(ScrnInfoPtr pScrn)
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

// map, unmap VBIOS
Bool RDCMapVBIOS(ScrnInfoPtr pScrn)
{
    RDCRecPtr   pRDC = RDCPTR(pScrn);
    FILE        *fpVBIOS;
    int         i;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, "==Enter RDCMapVBIOS()==\n");
    pRDC->ulROMType = 0;

    
    if (pRDC->ulROMType == 0)
    {
        pRDC->BIOSVirtualAddr = xf86MapVidMem(pScrn->scrnIndex, VIDMEM_READONLY, BIOS_ROM_PHY_BASE, BIOS_ROM_SIZE);
        if (pRDC->BIOSVirtualAddr)
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 0, "pRDC->BIOSVirtualAddr = 0x%08x\n", pRDC->BIOSVirtualAddr);
            pRDC->ulROMType = 1;
        }
        else
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "==BIOS ROM not found()==\n");
        }
    }

    
    if (pRDC->ulROMType == 0)
    {
        
        fpVBIOS = fopen(BIOS_ROM_PATH_FILE, "r");
        if (!fpVBIOS)
        {
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, InfoLevel, "BIOS ROM file \"/root/RDCVBIOS.ROM\" not found()==\n");
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
        return false; 
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
        return false; 
    }
}

Bool RDCUnmapVBIOS(ScrnInfoPtr pScrn)
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


ULONG EC_ReadPortUchar(BYTE *port, BYTE *value)
{
    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Enter EC_ReadPortUchar()\n");

    int i;
    for (i=0 ; i<10 ; i++)
    {
        usleep(700);
        
        
        
        if (inb(0x66) & BIT0)
        {
            *value = (BYTE)inb(port);
            return EC_ACCESS_SUCCESS;
        }
    };

    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Leave EC_ReadPortUchar()\n");
    
    return EC_ACCESS_FAIL;
};

void EC_WritePortUchar(BYTE *port, BYTE data)
{
    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Enter EC_WritePortUchar()\n");
    
    int i;
    for (i=0 ; i<10 ; i++)
    {
        usleep(700);
        
        
        
        if (!(inb(0x66) & BIT1)) 
        {
            outb(port, data);
            break;
        };
    };

    xf86DrvMsgVerb(0, X_INFO, ErrorLevel, "==Leave EC_WritePortUchar()\n");
};

void EC_DetectCaps(ScrnInfoPtr pScrn, ECINFO* pECChip)
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

