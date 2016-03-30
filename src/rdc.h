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


#include "xf86Cursor.h"
 
#if XSERVER_LIBPCIACCESS
#include <pciaccess.h>
#endif

#ifdef USE_XAA
#include "xaa.h"
#include "xaarop.h"
#endif
#include "xf86.h"


#include "exa.h"

#include "vbe.h"

#define PCI_DEV_MAP_FLAG_WRITABLE       (1U<<0)
#define PCI_DEV_MAP_FLAG_WRITE_COMBINE  (1U<<1)
#define PCI_DEV_MAP_FLAG_CACHABLE       (1U<<2)

#if XSERVER_LIBPCIACCESS
#define RDC_MEMBASE(p,n)  (p)->regions[(n)].base_addr
#define VENDOR_ID(p)      (p)->vendor_id
#define DEVICE_ID(p)      (p)->device_id
#define SUBVENDOR_ID(p)   (p)->subvendor_id
#define SUBSYS_ID(p)      (p)->subdevice_id
#define CHIP_REVISION(p)  (p)->revision
#else
#define RDC_MEMBASE(p,n)  (p)->memBase[n]
#define VENDOR_ID(p)      (p)->vendor
#define DEVICE_ID(p)      (p)->chipType
#define SUBVENDOR_ID(p)   (p)->subsysVendor
#define SUBSYS_ID(p)      (p)->subsysCard
#define CHIP_REVISION(p)  (p)->chipRev
#endif


//#define     Accel_2D
//#define     Accel_2D_DEBUG          0


#define     HWC
#define     HWC_DEBUG               0


#define NTSC                 0
#define PAL                  1


#ifndef PCI_VENDOR_RDC
#define     PCI_VENDOR_RDC          0x17F3
#endif

#ifndef PCI_VENDOR_RDC_M2010_A0
#define     PCI_VENDOR_RDC_M2010_A0 0x25F0
#endif

#ifndef PCI_CHIP_M2010
#define     PCI_CHIP_M2010          0x2010
#endif

#ifndef PCI_CHIP_M2010_A0
#define     PCI_CHIP_M2010_A0       0x17F3
#endif

#ifndef PCI_CHIP_M2011
#define     PCI_CHIP_M2011          0x2011
#endif

#ifndef PCI_CHIP_M2012
#define     PCI_CHIP_M2012          0x2012
#endif

typedef enum _CHIP_ID {
    VGALegacy,
    M2010,
    M2011,
    M2012
} CHIP_ID;


#define RDC_NAME                "RDC"
#define RDC_DRIVER_NAME         "rdc"
#define RDC_MAJOR_VERSION       0
#define RDC_MINOR_VERSION       0
#define RDC_PATCH_VERSION       9
#define RDC_VERSION    \
        ((RDC_MAJOR_VERSION << 20) | (RDC_MINOR_VERSION << 10) | RDC_PATCH_VERSION)


#define DEFAULT_VIDEOMEM_SIZE   0x00800000
#define DEFAULT_MMIO_SIZE       0x00020000
#define DEFAULT_CMDQ_SIZE       CMD_QUEUE_SIZE_1M
#define MIN_CMDQ_SIZE           0x00040000
#define CMD_QUEUE_GUARD_BAND    0x00000020
#define DEFAULT_HWC_NUM         0x00000002
#define CAPTURE_BUFFER_SIZE     0x00300000


typedef INT32       LONG;
typedef CARD8       UCHAR;
typedef CARD16      USHORT;
typedef CARD32      ULONG;


#define ErrorLevel             0    
#define DefaultLevel           4    
#define InfoLevel              5    
#define InternalLevel          6    


#define BIT0                    0x00000001
#define BIT1                    0x00000002
#define BIT2                    0x00000004
#define BIT3                    0x00000008
#define BIT4                    0x00000010
#define BIT5                    0x00000020
#define BIT6                    0x00000040
#define BIT7                    0x00000080
#define BIT8                    0x00000100
#define BIT9                    0x00000200
#define BIT10                   0x00000400
#define BIT11                   0x00000800
#define BIT12                   0x00001000
#define BIT13                   0x00002000
#define BIT14                   0x00004000
#define BIT15                   0x00008000
#define BIT16                   0x00010000
#define BIT17                   0x00020000
#define BIT18                   0x00040000
#define BIT19                   0x00080000
#define BIT20                   0x00100000
#define BIT21                   0x00200000
#define BIT22                   0x00400000
#define BIT23                   0x00800000
#define BIT24                   0x01000000
#define BIT25                   0x02000000
#define BIT26                   0x04000000
#define BIT27                   0x08000000
#define BIT28                   0x10000000
#define BIT29                   0x20000000
#define BIT30                   0x40000000
#define BIT31                   0x80000000

#define BIOS_ROM_PATH_FILE      "//root//RDCVBIOS.ROM"
#define BIOS_ROM_SIZE           32*1024
#define BIOS_ROM_PHY_BASE       0xC0000


 
#define ALIGN_TO_2(f)             (((f) + 1) & ~1)
#define ALIGN_TO_4(f)             (((f) + 3) & ~3)
#define ALIGN_TO_8(f)             (((f) + 7) & ~7)
#define ALIGN_TO_16(f)            (((f) + 15) & ~15)
#define ALIGN_TO_32(f)            (((f) + 31) & ~31)
#define ALIGN_TO_64(f)            (((f) + 63) & ~63)
#define ALIGN_TO_128(f)           (((f) + 127) & ~127)
#define ALIGN_TO_256(f)           (((f) + 255) & ~255)


//CR Lock Control Header
#define CR_LOCK_ID                  0x00009570
#define CR_JUMP_ID                  0x00009571
#define CR_LOCK_2DDMA_ID            BIT31
//CR Lock Control Engine
#define CR_LOCK_2D                  BIT24
#define CR_LOCK_DMA                 BIT25
#define CR_LOCK_VIDEO               BIT26
#define CR_LOCK_SUB_PIC             BIT27
#define CR_LOCK_DSP                 BIT28

typedef struct _RDCRec* RDCRecPtr;
typedef struct _RDCRec RDCRec;


typedef struct _RDCRegRec {

    UCHAR   ucPLLValue[3];
    UCHAR   ucCRA3;
    UCHAR   ucSR58;
    UCHAR   ucSR70;
    UCHAR   ucSR74;
} RDCRegRec, *RDCRegPtr;

typedef struct _VIDEOMODE {

    int     ScreenWidth;
    int     ScreenHeight;
    int     bitsPerPixel;
    int     Bpp;
    int     ScreenPitch;
        
} VIDEOMODE, *PVIDEOMODE;

typedef struct {

    ULONG   ulCMDQSize;
    ULONG   ulCMDQType;
    
    ULONG   ulCMDQOffsetAddr;
    UCHAR   *pjCMDQVirtualAddr;
    
    UCHAR   *pjCmdQCtrlPort;    
    UCHAR   *pjCmdQBasePort;
    UCHAR   *pjWritePort;
    UCHAR   *pjReadPort;
    UCHAR   *pjCmdQEndPort;     
    UCHAR   *pjEngStatePort;
          
    ULONG   ulCMDQMask;
    ULONG   ulCurCMDQueueLen;
                
    ULONG   ulWritePointer;
    ULONG   ulWritePointerMask;

    ULONG   ulReadPointerMask;
    
    ULONG   ulReadPointer_OK;       

    void (*Disable2D)(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
    Bool (*Enable2D)(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
    void (*WaitEngIdle)(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
} CMDQINFO, *PCMDQINFO;

typedef struct {

    ULONG   fg;
    ULONG   bg;
    UCHAR   ucXorBitmap[64*64/8];
    UCHAR   ucAndBitmap[64*64/8];
        
} MONOHWC, *PMONOHWC;

typedef struct {

    int     HWC_NUM;
    int     HWC_NUM_Next;

    ULONG   ulHWCOffsetAddr;
    UCHAR   *pjHWCVirtualAddr;
    
    USHORT  cursortype;
    USHORT  width;
    USHORT  height;
    USHORT  xhot;
    USHORT  yhot;
    USHORT  offset_x;
    USHORT  offset_y;
    int     iScreenOffset_x;
    int     iScreenOffset_y;
    MONOHWC MonoHWC;

} HWCINFO, *PHWCINFO;

typedef struct {
    ULONG   ulHorMaxResolution;
    ULONG   ulVerMaxResolution;
} MONITORSIZE;

typedef struct {
    Bool    EnableHorScaler;
    Bool    EnableVerScaler;
    int     ulHorScalingFactor;
    int     ulVerScalingFactor;
} SCALER;

typedef struct {
    UCHAR       ucDeviceID;     
    UCHAR       ucDisplayPath;
    MONITORSIZE MonitorSize;
    SCALER      ScalerConfig;
    UCHAR       ucNewDeviceID;  
    
} DEVICEINFO;


//Structure defination for Video function

#define     PI                      3.1415926535897932
#define     BRIGHTNESS_DEFAULT      10000
#define     CONTRAST_DEFAULT        10000
#define     HUE_DEFAULT             0
#define     SATURATION_DEFAULT      10000

// OVERLAYRECORD define
#define VIDEO_SHOW              0x80000000  
#define VIDEO_HIDE              0x00000000  
#define VIDEO_POST_INUSE        0x08000000  
#define VIDEO_CAP_SUBPIC_INUSE  0x00000008  

typedef struct _Cofe
{
	float A1;
	float A2;
	float A3;
	float B1;
	float B2;
	float B3;
	float C1;
	float C2;
	float C3;
	float D1;
	float D2;
	float D3;

	CARD32 dwIA1;
	CARD32 dwIB1;
	CARD32 dwIC1;
	CARD32 dwID1;
	CARD32 dwIA2;
	CARD32 dwIB2;
	CARD32 dwIC2;
	CARD32 dwID2;
	CARD32 dwIA3;
	CARD32 dwIB3;
	CARD32 dwIC3;
	CARD32 dwID3;

	CARD32 dwPA1;
	CARD32 dwPB1;
	CARD32 dwPC1;
	CARD32 dwPD1;
	CARD32 dwPA2;
	CARD32 dwPB2;
	CARD32 dwPC2;
	CARD32 dwPD2;
	CARD32 dwPA3;
	CARD32 dwPB3;
	CARD32 dwPC3;
	CARD32 dwPD3;
}Cofe;

typedef struct _VIDCOLORENHANCE
{
    ULONG    ulScaleBrightness;         
    ULONG    ulScaleContrast;           
    ULONG    ulScaleHue;                
    ULONG    ulScaleSaturation;         
} VIDCOLORENHANCE, *LPVIDCOLORENHANCE;

typedef struct _COLORKEY    
{
    CARD32 dwColorKeyOn;
    CARD32 dwChromaKeyOn;
    CARD32 dwKeyLow;
    CARD32 dwKeyHigh;
    CARD32 dwChromaLow;
    CARD32 dwChromaHigh;
}COLORKEY, *LPCOLORKEY;

typedef struct {
    Bool    	bOverlayEnable;
    int         iSrcWidth;
    int         iSrcHeight;
    int         iDstLeft;
    int         iDstTop;
    int         iDstWidth;
    int         iDstHeight;
    ULONG       ulVidDispCtrl;
    Bool        bPanningOverlayEnable;

    
    VIDCOLORENHANCE VidColorEnhance;
} OVERLAY_STATUS, *POVERLAY_STATUS;

typedef struct{
    ULONG       ulSrcBase;
    ULONG       ulSrcPitch;
    ULONG       ulDstBase;
    ULONG       ulDstPitch;
    
    union
    {
       ULONG    ulDstX;
       ULONG    ulDrawLineStartX;
    };
    
    union
    {
       ULONG    ulDstY;
       ULONG    ulDrawLineStartY;
    };
    
    union
    {
       ULONG    ulSrcX;
       ULONG    ulDrawLineEndX;
    };
    
    union
    {
       ULONG    ulSrcY;
       ULONG    ulDrawLineEndY;
    };

    ULONG    ulDrawWidth;

    union
    {
       ULONG    ulDrawHeight;
       ULONG    ulNumLinePoint;
    };
    
    union
    {
       ULONG    ulForeColorPat;
       ULONG    ulHighColorSrc;
    };
    
    union
    {
       ULONG    ulBackColorPat;
       ULONG    ulLowColorSrc;
    };
    
    union
    {
       ULONG    ulForeColorSrc;
       ULONG    ulHighColorDst;
    };
    
    union
    {
       ULONG    ulBackColorSrc;
       ULONG    ulLowColorDst;
    };
    
    union
    {
       ULONG    ulMonoMaskPat0;
       ULONG    ulPatLineStyle0;
    };
    
    union
    {
       ULONG    ulMonoMaskPat1;
       ULONG    ulPatLineStyle1;
    };
    
    ULONG       ulClipTop;
    ULONG       ulCLipLeft;
    ULONG       ulClipBottom;
    ULONG       ulClipRight;
    
    ULONG       ulFireCMD;
}HW2DINFO;


typedef struct _ECINFO
{
    BOOL bECExist;
    BOOL bNewEC;
} ECINFO;

#define EC_ACCESS_SUCCESS   0x0
#define EC_ACCESS_FAIL      0xFFFFFFFF

struct _RDCRec
{
    vbeInfoPtr pVbe;
    EntityInfoPtr       pEnt;
#if XSERVER_LIBPCIACCESS
    struct pci_device   *PciInfo;
#else
    pciVideoPtr         PciInfo;
    PCITAG              PciTag;
#endif


    OptionInfoPtr       Options;
    DisplayModePtr      ModePtr;        
#ifdef USE_XAA
    XAAInfoRecPtr       AccelInfoPtr;
#else
    void *	AccelInfoPtr; // Will be NULL
#endif
    xf86CursorInfoPtr   HWCInfoPtr;
    
    VIDEOMODE           VideoModeInfo;  
    RDCRegRec           SavedReg;
    CMDQINFO            CMDQInfo;
    HWCINFO             HWCInfo;
    OVERLAY_STATUS      OverlayStatus;
    DEVICEINFO          DeviceInfo;
    ULONG               ulCMDReg;
    HW2DINFO            Hw2Dinfo;
    Bool                EnableClip;

    
    ULONG               ENGCaps;
    ULONG               FBPhysAddr;             
    UCHAR               *FBVirtualAddr;         
    unsigned long       FbMapSize;
    unsigned long       AvailableFBsize;
    
    ULONG               MMIOPhysAddr;           
    UCHAR               *MMIOVirtualAddr;       
    unsigned long       MMIOMapSize;
    
    UCHAR               *BIOSVirtualAddr;       
    ULONG               ulROMType;              

    ULONG               MemoryBandwidth;         

    IOADDRESS           IODBase;                
    IOADDRESS           PIOOffset;
    IOADDRESS           RelocateIO;

    USHORT              usSupportDevice;

    Bool                noAccel;
    Bool                useEXA;
    Bool                noHWC;
    Bool                MMIO2D;
    
    ExaDriverPtr        exaDriverPtr;
    unsigned int        curMaker;
    unsigned int        lastMaker;
    CloseScreenProcPtr  CloseScreen;

    
    ECINFO ECChipInfo;

    
    BOOL bEnableGamma;
    UCHAR GammaRampR[256];
    UCHAR GammaRampG[256];
    UCHAR GammaRampB[256];

    
    Bool                bRandRRotation;
    Rotation            rotate; 
};


#define RDCPTR(p) ((RDCRecPtr)((p)->driverPrivate))
#define MODE_PRIVATE_PTR(p) ((MODE_PRIVATE*)(p->Private))


#include "rdc_mode.h"
#include "rdc_vgatool.h"
#include "rdc_2dtool.h"
#include "rdc_cursor.h"
#include "CInt10.h"
#include "rdc_video.h"
#include "rdc_rotation.h"

