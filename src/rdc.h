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

#ifndef _RDC_H_
#define _RDC_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "xf86.h"
#include "xf86fbman.h"
#include "vgaHW.h"
#include "xf86Crtc.h"
#include "xf86RandR12.h"


#if GET_ABI_MAJOR(ABI_VIDEODRV_VERSION) < 13
#define HAVE_XAA
#endif

#if GET_ABI_MAJOR(ABI_VIDEODRV_VERSION) < 10
#define HAVE_DUAL
#endif

 
#if XSERVER_LIBPCIACCESS
#include <pciaccess.h>
#endif


#ifdef HAVE_XAA
#include "xaa.h"
#endif
#include "xaarop.h"
#include "xf86.h"


#include "exa.h"
#include "vbe.h"
#include "BiosDef.h"



#include "xf86Cursor.h"


#include "CInt10.h"

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


#define     Accel_2D_DEBUG          1
#define     HWC_DEBUG               1
#define     CR_FENCE_ID             0


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

#ifndef PCI_CHIP_M2013
#define     PCI_CHIP_M2013          0x2013
#endif

#ifndef PCI_CHIP_M2014
#define     PCI_CHIP_M2014          0x2014
#endif

#ifndef PCI_CHIP_M2015
#define     PCI_CHIP_M2015          0x2015
#endif

#ifndef PCI_CHIP_M2200
#define     PCI_CHIP_M2200          0x2200
#endif

typedef enum {
    OPTION_NOACCEL,
    OPTION_MMIO2D,   
    OPTION_SW_CURSOR,
    OPTION_HWC_NUM,
    OPTION_ENG_CAPS,   
    OPTION_DBG_SELECT,
    OPTION_NO_DDC,
    OPTION_ACCELMETHOD,
    OPTION_HRATIO,
    OPTION_VRATIO,
    OPTION_RANDRROTATION,    
    OPTION_DOWN_SCALE
} RDCOpts;

// What the fuck code...
#if defined( _rdc_driver_c_) || defined(_rdcdual_driver_c_)
static const OptionInfoRec RDCOptions[] = {
    {OPTION_NOACCEL,       "NoAccel",       OPTV_BOOLEAN,  {0}, TRUE},
    {OPTION_MMIO2D,        "MMIO2D",        OPTV_BOOLEAN,  {0}, FALSE},   
    {OPTION_SW_CURSOR,     "SWCursor",      OPTV_BOOLEAN,  {0}, TRUE},    
    {OPTION_HWC_NUM,       "HWCNumber",     OPTV_INTEGER,  {0}, FALSE},
    {OPTION_ENG_CAPS,      "ENGCaps",       OPTV_INTEGER,  {0}, FALSE},                    
    {OPTION_NO_DDC,        "NoDDC",         OPTV_BOOLEAN,  {0}, FALSE},
    {OPTION_ACCELMETHOD,   "AccelMethod",   OPTV_STRING,   {0}, FALSE},
    {OPTION_HRATIO,         "HRatio",         OPTV_INTEGER,    {0},    FALSE},
    {OPTION_VRATIO,         "VRatio",         OPTV_INTEGER,    {0},    FALSE},    
    {OPTION_RANDRROTATION, "RandRRotation", OPTV_BOOLEAN,  {0}, FALSE},
    {OPTION_DOWN_SCALE,    "DownScale",     OPTV_BOOLEAN,  {0}, FALSE},
    {-1,                   NULL,            OPTV_NONE,     {0}, FALSE}
};


static SymTabRec RDCChipsets[] = {
    {PCI_CHIP_M2010_A0, "M2010_A0"},
    {PCI_CHIP_M2010, "M2010"},
    {PCI_CHIP_M2011, "M2011"},
    {PCI_CHIP_M2012, "M2012"},
    {PCI_CHIP_M2013, "M2013"},
    {PCI_CHIP_M2014, "M2014"},
    {PCI_CHIP_M2015, "M2015"},
    {PCI_CHIP_M2200, "M2200"},
    {-1,              NULL}
};

static PciChipsets RDCPciChipsets[] = {
    {PCI_CHIP_M2010, PCI_CHIP_M2010, RES_SHARED_VGA},
    {PCI_CHIP_M2010_A0, PCI_CHIP_M2010_A0, RES_SHARED_VGA},
    {PCI_CHIP_M2011, PCI_CHIP_M2011, RES_SHARED_VGA},
    {PCI_CHIP_M2012, PCI_CHIP_M2012, RES_SHARED_VGA},
    {PCI_CHIP_M2013, PCI_CHIP_M2013, RES_SHARED_VGA},
    {PCI_CHIP_M2014, PCI_CHIP_M2014, RES_SHARED_VGA},
    {PCI_CHIP_M2015, PCI_CHIP_M2015, RES_SHARED_VGA},
    {PCI_CHIP_M2200, PCI_CHIP_M2200, RES_SHARED_VGA},
    {-1,              -1,              RES_UNDEFINED }
};
#else
extern const OptionInfoRec RDCOptions[];
#endif



#define DMPDX2                      0x34504D44
#define CIDOffset                   0x90


#define R3308NBID                   0x602217f3
#define DirectAccessFB              BIT24

typedef enum _CHIP_ID {
    VGALegacy,
    M2010,
    M2011,
    M2012,
    M2013,
    M2014,
    R2200
} CHIP_ID;


#define RDC_NAME                "RDC GFX R0.10.0"
#define RDC_DRIVER_NAME         "rdc"
#define RDC_MAJOR_VERSION       0
#define RDC_MINOR_VERSION       10
#define RDC_PATCH_VERSION       0
#define RDC_VERSION    \
        ((RDC_MAJOR_VERSION << 20) | (RDC_MINOR_VERSION << 10) | RDC_PATCH_VERSION)


#define DEFAULT_VIDEOMEM_SIZE   0x00800000
#define DEFAULT_MMIO_SIZE       0x00020000
#define DEFAULT_CMDQ_SIZE       CMD_QUEUE_SIZE_1M
#define MIN_CMDQ_SIZE           0x00040000
#define CMD_QUEUE_GUARD_BAND    0x00000020
#define DEFAULT_HWC_NUM         0x00000002
#define CAPTURE_BUFFER_SIZE     0x00700000


#define ErrorLevel             0    
#define DefaultLevel           4    
#define InfoVideo              5    
#define InfoLevel              6    
#define InternalLevel          7    


#define LCD_TIMING          0x00010000


#define  CR_HEADER_ERROR             BIT10
#define  CR_2D_IDLE                  BIT12
#define  CR_DMA_IDLE                 BIT13
#define  CR_VIDEO_IDLE               BIT14
#define  CR_CR_IDLE                  BIT15

#define BIOS_ROM_PATH_FILE      "//usr//lib//xorg//modules//drivers//RDCVBIOS.ROM"
#define BIOS_ROM_SIZE           32*1024
#define BIOS_ROM_PHY_BASE       0xC0000


 
#define ALIGN_TO_2(f)             (((f) + 1) & ~1)
#define ALIGN_TO_UB_2(f)           (((f) + 1) & ~1)
#define ALIGN_TO_LB_2(f)           ((f) & ~1)
#define ALIGN_TO_4(f)             (((f) + 3) & ~3)
#define ALIGN_TO_UB_4(f)           (((f) + 3) & ~3)
#define ALIGN_TO_LB_4(f)           ((f) & ~3)
#define ALIGN_TO_8(f)             (((f) + 7) & ~7)
#define ALIGN_TO_UB_16(f)           (((f) + 15) & ~15)
#define ALIGN_TO_LB_16(f)           ((f) & ~15)
#define ALIGN_TO_UB_32(f)           (((f) + 31) & ~31)
#define ALIGN_TO_LB_32(f)           ((f) & ~31)
#define ALIGN_TO_UB_64(f)           (((f) + 63) & ~63)
#define ALIGN_TO_LB_64(f)           ((f) & ~63)
#define ALIGN_TO_128(f)           (((f) + 127) & ~127)
#define ALIGN_TO_256(f)           (((f) + 255) & ~255)



#define CR_LOCK_ID                  0x00009570
#define CR_JUMP_ID                  0x00009571
#define CR_LOCK_2DDMA_ID            BIT31

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
    Bool    bInitialized;
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

    
    UCHAR   bFenceID;
    UCHAR   bFuncType;

    void (*Disable2D)(RDCRecPtr pRDC);
    Bool (*Enable2D)(RDCRecPtr pRDC);
    void (*WaitEngIdle)(RDCRecPtr pRDC);
    Bool (*InitCMDQInfo)(ScrnInfoPtr pScrn, RDCRecPtr pRDC);
    Bool (*EnableCMDQ)(RDCRecPtr pRDC);
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
    Bool    EnableDownScaling;
    Bool    EnableHorScaler;
    Bool    EnableVerScaler;
    Bool    EnableHorUpScaler;
    Bool    EnableVerUpScaler;
    Bool    EnableHorDownScaler;
    Bool    EnableVerDownScaler;
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




#define     PI                      3.1415926535897932
#define     BRIGHTNESS_DEFAULT      10000
#define     CONTRAST_DEFAULT        10000
#define     HUE_DEFAULT             0
#define     SATURATION_DEFAULT      10000


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

	ULONG dwIA1;
	ULONG dwIB1;
	ULONG dwIC1;
	ULONG dwID1;
	ULONG dwIA2;
	ULONG dwIB2;
	ULONG dwIC2;
	ULONG dwID2;
	ULONG dwIA3;
	ULONG dwIB3;
	ULONG dwIC3;
	ULONG dwID3;

	ULONG dwPA1;
	ULONG dwPB1;
	ULONG dwPC1;
	ULONG dwPD1;
	ULONG dwPA2;
	ULONG dwPB2;
	ULONG dwPC2;
	ULONG dwPD2;
	ULONG dwPA3;
	ULONG dwPB3;
	ULONG dwPC3;
	ULONG dwPD3;
}Cofe;

typedef struct _VIDCOLORENHANCE
{
    ULONG    ulScaleBrightness;         
    ULONG    ulScaleContrast;           
    ULONG    ulScaleHue;                
    ULONG    ulScaleSaturation;         

    ULONG    ulFact1;
    ULONG    ulFact2;
    ULONG    ulFact3;
    ULONG    ulFact4;
} VIDCOLORENHANCE, *LPVIDCOLORENHANCE;

typedef struct _COLORKEY    
{
    ULONG dwColorKeyOn;
    ULONG dwChromaKeyOn;
    ULONG dwKeyLow;
    ULONG dwKeyHigh;
    ULONG dwChromaLow;
    ULONG dwChromaHigh;
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



typedef struct _TVENCODERINFO
{
    unsigned char    bTV_HDTV;           
    unsigned char    bSupportHDTV;       
    unsigned char    EnCoder;            
    unsigned char    I2CPort;            
    unsigned char    SlaveAddr;          
    unsigned char    DisplayPath;        
    unsigned char    TVType;             
    unsigned char    TVConnect;          
    unsigned char    HDTVType;           
    unsigned char    HDTVConnect;
    unsigned char    CCRSLevel;          
    unsigned char    ucHPosition;        
    unsigned char    ucVPosition;        
    int              iHScaler;           
    unsigned char    ucVScaler;          
    unsigned short   TVOut_HSize;        
    unsigned short   TVOut_VSize;        
    unsigned char    bEnableHPanning;     
    unsigned char    bEnableVPanning;    
    unsigned short   wPreX;              
    unsigned short   wPreY;              
    unsigned short   wPanningViewX;      
    unsigned short   wPanningViewY;      
    unsigned long    dwPanningDispAddr;
    unsigned short   ModeHSize;          
    unsigned short   ModeVSize;          
    unsigned char    bTVModeSupport;     
} TVENCODERINFO, *PTVENCODERINFO;


typedef struct _ECINFO
{
    BOOL bECExist;
    BOOL bNewEC;
} ECINFO;


typedef struct _EDID_DETAILED_TIMING {
    UCHAR   bValid;                         
    USHORT  usPixelClock;                   

    USHORT  usHorDispEnd;                   
    USHORT  usHorBlankingTime;              
    USHORT  usHorSyncStart;                 
    USHORT  usHorSyncTime;                  
    UCHAR   ucHorBorder;                    
    
    USHORT  usVerDispEnd;                   
    USHORT  usVerBlankingTime;              
    USHORT  usVerSyncStart;                 
    USHORT  usVerSyncTime;                  
    UCHAR   ucVerBorder;                    

    UCHAR   ucFlags;                        
    
} EDID_DETAILED_TIMING, *PEDID_DETAILED_TIMING;

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
#ifdef HAVE_XAA
    XAAInfoRecPtr       AccelInfoPtr;
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
    BOOL                bEnableTVPanning;       
    TVENCODERINFO       TVEncoderInfo[2];       

    
    ULONG               ENGCaps;
    ULONG               FBPhysAddr;             
    UCHAR               *FBVirtualAddr;         
    unsigned long       FbMapSize;
    unsigned long       AvailableFBsize;   
    unsigned long       ulMaxPitch;
    unsigned long       ulMaxHeight;
    
    ULONG               MMIOPhysAddr;           
    UCHAR               *MMIOVirtualAddr;       
    unsigned long       MMIOMapSize;
    
    UCHAR               *BIOSVirtualAddr;       
    ULONG               ulROMType;              

    ULONG               MemoryBandwidth;         

    ADDRESS           IODBase;                
    ADDRESS           PIOOffset;
    ADDRESS           RelocateIO;

    USHORT              usSupportDevice;

    Bool                noAccel;
    Bool                useEXA;
    Bool                noHWC;
    Bool                MMIO2D;
    Bool                MMIOVPost;
    Bool                IoctlCR;
    Bool                SupportDualPath;
    
    ExaDriverPtr        exaDriverPtr;
    unsigned int        curMaker;
    unsigned int        lastMaker;
    CloseScreenProcPtr  CloseScreen;

    
    PCBIOS_Extension     pCBIOSExtension;

    
    ECINFO ECChipInfo;

    
    BOOL bEnableGamma;
    UCHAR GammaRampR[256];
    UCHAR GammaRampG[256];
    UCHAR GammaRampB[256];

    
    Bool                bRandRRotation;
    Rotation            rotate; 

    
    
    Bool                bDirectAccessFB;

    ULONG               ulVirtualDesktopOffset;

    Bool                bColorEnhanceOn;
    Bool                bVPColorEnhance;

    
    BYTE                bHRatio;
    BYTE                bVRatio;

    int                 iFBDev;
    
    
    DWORD               dwMaxVPOSTWidth;
    unsigned long       bufindex;
    DisplayModePtr	    modePool;		
    CreateScreenResourcesProcPtr    CreateScreenResources;
    ScreenBlockHandlerProcPtr       BlockHandler;
    unsigned int SaveGeneration;
    DWORD              SetVideoDisplay;
};


#define RDCPTR(p) ((RDCRecPtr)((p)->driverPrivate))
#define MODE_PRIVATE_PTR(p) ((MODE_PRIVATE*)(p->Private))


#define RDC_OUTPUT_UNUSED 0
#define RDC_OUTPUT_ANALOG 1
#define RDC_OUTPUT_DVO_TMDS 2
#define RDC_OUTPUT_DVO_LVDS 3
#define RDC_OUTPUT_DVO_TVOUT 4
#define RDC_OUTPUT_SDVO 5
#define RDC_OUTPUT_LVDS 6
#define RDC_OUTPUT_TVOUT 7
#define RDC_OUTPUT_HDMI 8

typedef struct _RDCCrtcPrivateRec {
    int			    pipe;
    int			    plane;

    Bool    		enabled;
    
    int			    dpms_mode;
    
    int			    x, y;

    
    unsigned long cursor_offset;
    unsigned long cursor_argb_offset;
    
    uint64_t cursor_addr;
    unsigned long cursor_argb_addr;
    Bool	cursor_is_argb;
} RDCCrtcPrivateRec, *RDCCrtcPrivatePtr;

#define RDCCrtcPrivate(c) ((RDCCrtcPrivatePtr) (c)->driver_private)

typedef struct _RDCOutputPrivateRec {
   int			    type;
   int              pipe_mask;
   int			    clone_mask;
   int              dpms_mode;
   void			    *dev_priv;
} RDCOutputPrivateRec, *RDCOutputPrivatePtr;



#define IOCTL_DSP_INIT                  _IOWR('R', 0x30, unsigned int)
#define IOCTL_DSP_CLOSE                 _IOWR('R', 0x31, unsigned int)
#define IOCTL_DSP_CMDQ                  _IOWR('R', 0x32, unsigned int)
#define IOCTL_DSP_IDLE_STATUS           _IOWR('R', 0x33, unsigned int)
#define IOCTL_HW_ASIC_INFO              _IOWR('R', 0x34, unsigned int)
#define IOCTL_QUERY_BUF_INDEX           _IOWR('R', 0x35, unsigned int)
#define IOCTL_QUERY_SHARED_BUFF_OFFSET  _IOWR('R', 0x36, unsigned int)
#define IOCTL_QUERY_RGB_BUF_INDEX       _IOWR('R', 0x37, unsigned int)
    
#define IOCTL_MM_INIT                   _IOWR('R', 0x50, unsigned int)
#define IOCTL_MM_CLOSE                  _IOWR('R', 0x51, unsigned int)
#define IOCTL_MM_MALLOC                 _IOWR('R', 0x52, unsigned int)
#define IOCTL_MM_REALLOC                _IOWR('R', 0x53, unsigned int)
#define IOCTL_MM_ZALLOC                 _IOWR('R', 0x54, unsigned int)
#define IOCTL_MM_FREE                   _IOWR('R', 0x55, unsigned int)
    
#define IOCTL_CR_IDLE_STATUS            _IOWR('R', 0x70, unsigned int)
#define IOCTL_ASIC_CR_IDLE_STATUS       _IOWR('R', 0x71, unsigned int)
#define IOCTL_CR_CMDQ                   _IOWR('R', 0x72, unsigned int)
#define IOCTL_IF_CAPS                   _IOWR('R', 0x73, unsigned int)
    
#define IOCTL_FOR_TEST          _IOWR('R', 0xFF, unsigned int)


#define IF_CAP_DSP_CR   0x00000001
#define IF_CAP_DSP      0x00000002
#define IF_CAP_MM       0x00000004
#define IF_CAP_CR       0x00000008

#define RDC_DEV         "/dev/fb0"

#include <fcntl.h>


#include "rdc_vgatool.h"
#include "rdc_2dtool.h"
#include "rdc_cursor.h"
#include "rdc_video.h"
#include "rdc_rotation.h"

 
#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)
#define RL(msg) xf86DrvMsgVerb(0, X_INFO, 0,__FILE__ "(" STRINGIZE(__LINE__) "): " msg)
#if Accel_2D_DEBUG 
#define RL2D(...) xf86DrvMsgVerb(0, X_INFO, 0,__FILE__ "(" STRINGIZE(__LINE__) "): " __VA_ARGS__)
#else
#define RL2D(...)
#endif
#include "rdc_driver_proto.h"
#include "rdc_accel_proto.h"
#include "rdc_driver_proto.h"
#include "HDMI_proto.h"
#include "TV_proto.h"
#include "rdc_mode_proto.h"

#define RDC_EXPORT

#endif
