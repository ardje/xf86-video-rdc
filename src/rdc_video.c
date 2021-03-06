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
#include <stdlib.h>
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
#include "cursorstr.h"


#include "fourcc.h"


#include "rdc.h"



static XF86VideoAdaptorPtr RDCSetupImageVideoOverlay(ScreenPtr);
static void RDCStopVideo(ScrnInfoPtr,pointer,Bool);
static int RDCSetPortAttribute(ScrnInfoPtr,Atom,INT32,pointer);
static int RDCGetPortAttribute(ScrnInfoPtr,Atom,INT32*,pointer);
static void RDCQueryBestSize(ScrnInfoPtr,Bool,short,short,short,short,unsigned int*,unsigned int*,pointer);
static int RDCPutImage(ScrnInfoPtr,short,short,short,short,short,short,short,short,
		               int,unsigned char*,short,short,Bool,RegionPtr,pointer,DrawablePtr);
static int RDCQueryImageAttributesOverlay(ScrnInfoPtr,int,unsigned short*,unsigned short*,int*,int*);
static FBLinearPtr RDCAllocateMemory(ScrnInfoPtr, FBLinearPtr, int);
void RDCUpdateVID(RDCRecPtr, RDCPortPrivPtr, BYTE);
void RDCCopyFOURCC(RDCRecPtr, unsigned char*, RDCPortPrivPtr, unsigned long, 
                   unsigned long, int, short, short);

    
static void RDCStopVideoPost(ScrnInfoPtr,pointer,Bool);
static int RDCPutImageVPOST(ScrnInfoPtr,short,short,short,short,short,short,short,short,
		               int,unsigned char*,short,short,Bool,RegionPtr,pointer,DrawablePtr);
void RDCAllocateVPOSTMem(ScrnInfoPtr, RDCPortPrivPtr, short, short);
void RDCCopyFourCCVPOST(RDCRecPtr, RDCPortPrivPtr, unsigned char*, short, short, short, short);
void RDCUpdateVideo(RDCRecPtr, RDCPortPrivPtr, BYTE);


#define IMAGE_MAX_WIDTH		1280
#define IMAGE_MAX_HEIGHT	1024

#define NUM_FORMATS 2
static XF86VideoFormatRec Formats[NUM_FORMATS] = 
{
    {16, TrueColor}, 
    {24, TrueColor}
};

#define NUM_ATTRIBUTES 4
static XF86AttributeRec Attributes[NUM_ATTRIBUTES] =
{
   {XvSettable | XvGettable, 0, (1 << 24) - 1, "XV_COLORKEY"},
   {XvSettable | XvGettable, 0, 10000, "XV_BRIGHTNESS"},
   {XvSettable | XvGettable, 0, 20000, "XV_CONTRAST"},
   {XvSettable | XvGettable, 0, 20000, "XV_SATURATION"},
};

#define NUM_IMAGES_VDISP 1
static XF86ImageRec Images_VDisp[NUM_IMAGES_VDISP] = 
{
    XVIMAGE_YUY2
};

#define NUM_IMAGES_VPOST 3
static XF86ImageRec Images_VPost[NUM_IMAGES_VPOST] = 
{
    XVIMAGE_YUY2,
    XVIMAGE_YV12,
    XVIMAGE_I420
};

#define MAKE_ATOM(a) MakeAtom(a, sizeof(a) - 1, TRUE)

static Atom xvBrightness, xvContrast, xvSaturation, xvColorKey;

static XF86VideoEncodingRec DummyEncoding[1] = 
{
    {0,"XV_IMAGE",IMAGE_MAX_WIDTH, IMAGE_MAX_HEIGHT,{1, 1}}
};


void RDCVideoInit(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    XF86VideoAdaptorPtr *adaptors, *newAdaptors = NULL;
    XF86VideoAdaptorPtr overlayAdaptor = NULL;
    int num_adaptors;
    Bool bTrue = FALSE;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCVideoInit()  entry==\n");
    
    num_adaptors = xf86XVListGenericAdaptors(pScrn, &adaptors);
    xf86DrvMsgVerb(0,X_INFO,4,"==RDCVideoInit() Adaptor number %d==\n",num_adaptors);
    
    newAdaptors = calloc((num_adaptors + 1) , sizeof(XF86VideoAdaptorPtr *));

    if (newAdaptors == NULL)
	    return;
    memcpy(newAdaptors, adaptors, num_adaptors * sizeof(XF86VideoAdaptorPtr));
    adaptors = newAdaptors;
   
    overlayAdaptor = RDCSetupImageVideoOverlay(pScreen);

    if (overlayAdaptor != NULL)
    {
        xf86DrvMsgVerb(0,X_INFO,DefaultLevel,"==RDCVideoInit() Get Overlay Adapter==\n");
        adaptors[num_adaptors++] = overlayAdaptor;
    }
    
    if (num_adaptors)
    {
        xf86DrvMsgVerb(0,X_INFO,DefaultLevel,"==RDCVideoInit() Adapter number %d==\n",num_adaptors);
        bTrue = xf86XVScreenInit(pScreen, adaptors, num_adaptors);
        if (!bTrue)
	    xf86DrvMsgVerb(0,X_INFO,DefaultLevel,"==RDCVideoInit() XVScreenInit Fail==\n");
    }

    
    pRDC->OverlayStatus.VidColorEnhance.ulScaleBrightness = BRIGHTNESS_DEFAULT;
    pRDC->OverlayStatus.VidColorEnhance.ulScaleContrast = CONTRAST_DEFAULT;
    pRDC->OverlayStatus.VidColorEnhance.ulScaleHue = HUE_DEFAULT; 
    pRDC->OverlayStatus.VidColorEnhance.ulScaleSaturation = SATURATION_DEFAULT;
	    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCVideoInit()  Exit==\n");
    free(adaptors);
}


static XF86VideoAdaptorPtr
RDCSetupImageVideoOverlay(ScreenPtr pScreen)
{
    ScrnInfoPtr pScrn = xf86Screens[pScreen->myNum];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    RDCPortPrivPtr pXVPriv = NULL;
    XF86VideoAdaptorPtr adapt = NULL;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCSetupImageVideoOverlay()  entry==\n");
    
    if (!(adapt = calloc(1, sizeof(XF86VideoAdaptorRec) + 
                             sizeof(RDCPortPrivRec) + 
                             sizeof(DevUnion))))
    {
        xf86DrvMsgVerb(0, X_INFO, 0, "==RDCSetupImageVideoOverlay() allocate fail==\n");
        return NULL;            
    }
    
    adapt->type = XvWindowMask | XvInputMask | XvImageMask;
    adapt->flags = VIDEO_OVERLAID_IMAGES ;
    adapt->name = "RDC Video Overlay";
    adapt->nEncodings = 1;
    adapt->pEncodings = DummyEncoding;
    
    adapt->nFormats = NUM_FORMATS;
    adapt->pFormats = Formats;
    adapt->nPorts = 1;
    adapt->pPortPrivates = (DevUnion *) (&adapt[1]);

    pXVPriv = (RDCPortPrivPtr)(&adapt->pPortPrivates[1]);

    
    pXVPriv->colorkey = (1 << pScrn->offset.red) |
                        (1 << pScrn->offset.green) |
                        (((pScrn->mask.blue >> pScrn->offset.blue) - 1) << pScrn->offset.blue);
    
    adapt->pPortPrivates[0].ptr = (pointer) (pXVPriv);
    
    adapt->nAttributes = NUM_ATTRIBUTES;
    adapt->pAttributes = Attributes;

    if (pRDC->ENGCaps & ENG_CAP_VIDEO_POST)
    {
        adapt->nImages = NUM_IMAGES_VPOST;
        adapt->pImages = Images_VPost;
        adapt->PutImage = RDCPutImageVPOST;
        adapt->StopVideo = RDCStopVideoPost;
    }
    else
    {
        adapt->nImages = NUM_IMAGES_VDISP;
        adapt->pImages = Images_VDisp;
        adapt->PutImage = RDCPutImage;
        adapt->StopVideo = RDCStopVideo;
    }
    
    adapt->PutVideo = NULL;
    adapt->PutStill = NULL;
    adapt->GetVideo = NULL;
    adapt->GetStill = NULL;
    adapt->SetPortAttribute = RDCSetPortAttribute;
    adapt->GetPortAttribute = RDCGetPortAttribute;
    adapt->QueryBestSize = RDCQueryBestSize;
    adapt->QueryImageAttributes = RDCQueryImageAttributesOverlay;

    
    pXVPriv->VidhwInfo.dwVidCtl         = 0;
    pXVPriv->VidhwInfo.dwVideoSrcOffset = 0;
    pXVPriv->VidhwInfo.dwFetch          = 0;
    pXVPriv->VidhwInfo.dwVDisplayCount  = 0;
    pXVPriv->VidhwInfo.dwVSrcPitch      = 0;
    pXVPriv->VidhwInfo.dwWinStartXY     = 0;
    pXVPriv->VidhwInfo.dwWinEndXY       = 0;
    pXVPriv->VidhwInfo.dwzoomCtl        = 0;
    pXVPriv->VidhwInfo.dwVIDBuffer[0]   = 0;
    pXVPriv->VidhwInfo.dwVIDBuffer[1]   = 0;
    pXVPriv->VidhwInfo.dwVIDBuffer[2]   = 0;
    pXVPriv->VidhwInfo.dwColorKey       = (pXVPriv->colorkey & 0x00FFFFFF) | VDPS_ENABLE_CK;
    REGION_NULL(pScreen, &pXVPriv->clip);

    
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_CTL)                      = VDPS_ENABLE;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_START_OFFSET)             = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FETCH_COUNT)              = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_SRC_DISP_COUNT_ON_SCR)    = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FB_STRIDE)                = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_START_LOCATION)           = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_END_LOCATION)             = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_ZOOM_CTL)                 = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FB0)                      = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FIRST_COLORKEY)           = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_SECOND_COLORKEY)          = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FIFO_THRESHOLD) = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_CHROMA_KEY_LOW) = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_CHROMA_KEY_UPPER) = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_COLOR_ENHANCE_CTL1) = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_COLOR_ENHANCE_CTL2) = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_COLOR_ENHANCE_CTL3) = 0;
    *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_COLOR_ENHANCE_CTL4) = 0;    

    xvColorKey = MAKE_ATOM("XV_COLORKEY");
    xvBrightness = MAKE_ATOM("XV_BRIGHTNESS");
    xvContrast = MAKE_ATOM("XV_CONTRAST");
    xvSaturation = MAKE_ATOM("XV_SATURATION");

    pRDC->OverlayStatus.bOverlayEnable = FALSE;
    pRDC->OverlayStatus.bPanningOverlayEnable = TRUE;
    pRDC->OverlayStatus.ulVidDispCtrl = 0;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCSetupImageVideoOverlay()  exit==\n");
    return adapt;
}

static void
RDCStopVideo(ScrnInfoPtr pScrn, pointer data, Bool shutdown)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    RDCPortPrivPtr pXVPriv = (RDCPortPrivPtr)data;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCStopVideo()  Entry==\n");

    REGION_EMPTY(pScrn->pScreen, &pXVPriv->clip);

    
    if (shutdown)
    {
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCStopVideo()  Stop Overlay==\n");

        pRDC->OverlayStatus.bOverlayEnable = false;

        while (!(GetReg(INPUT_STATUS1_READ) & 0x08))
        {
            xf86DrvMsgVerb(0, X_INFO, InternalLevel, "wait v sync\n");
        }

        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_CTL) = 0;

        
        pXVPriv->VidhwInfo.dwVidCtl = 0;
        pXVPriv->VidhwInfo.dwFetch = 0;
        pXVPriv->VidhwInfo.dwWinStartXY =0;
        pXVPriv->VidhwInfo.dwWinEndXY = 0;
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCStopVideo()  Exit==\n");
}

static int
RDCSetPortAttribute(
        ScrnInfoPtr pScrn,
		Atom attribute, 
		INT32 value, 
		pointer data)
{
    RDCRecPtr  pRDC = RDCPTR(pScrn);
    RDCPortPrivPtr pXVPriv = (RDCPortPrivPtr)data;
    int ReturnValue = Success;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCSetPortAttribute()  Entry==\n");

    if (attribute == xvBrightness)
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==SetPort Attribute Brightness %d==\n",value);
        pXVPriv->brightness = value;
    }
    else if (attribute == xvContrast)
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==SetPort Attribute Contrast %d==\n",value);
        pXVPriv->contrast = value;
    }
    else if (attribute == xvSaturation)
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==SetPort Attribute Saturation %d==\n", value);
        pXVPriv->saturation = value;
    }
    else if (attribute == xvColorKey)
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==SetPort Attribute ColorKey 0x%x==\n", value);
        pXVPriv->colorkey = value;

        pXVPriv->VidhwInfo.dwColorKey = (value & 0x00FFFFFF) | VDPS_ENABLE_CK;
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FIRST_COLORKEY) = pXVPriv->VidhwInfo.dwColorKey;
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_SECOND_COLORKEY) = pXVPriv->VidhwInfo.dwColorKey;
    }
    else
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==SetPort Attribute not Support==\n");
        ReturnValue = BadValue;        
    }
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCSetPortAttribute()  Exit==\n");
    
    return ReturnValue;
}

static int
RDCGetPortAttribute(
        ScrnInfoPtr pScrn,
		Atom attribute, 
		INT32 * value, 
		pointer data)
{
    RDCRecPtr  pRDC = RDCPTR(pScrn);
    RDCPortPrivPtr pXVPriv = (RDCPortPrivPtr)data;
    int ReturnValue = Success;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCGetPortAttribute()  Entry==\n");
    
    if (attribute == xvBrightness)
    {
        *value = pXVPriv->brightness;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==GetPort Attribute Brightness %d==\n",value);
    }
    else if (attribute == xvContrast)
    {
        *value = pXVPriv->contrast;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==GetPort Attribute Contrast %d==\n",value);
    }
    else if (attribute == xvSaturation)
    {
        *value = pXVPriv->saturation;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==GetPort Attribute Saturation %d==\n", value);
    }
    else if (attribute == xvColorKey)
    {
        *value = pXVPriv->colorkey;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==GetPort Attribute ColorKey 0x%x==\n", value);
    }
    else
    {
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==GetPort Attribute not Support==\n");
        ReturnValue = BadValue;        
    }
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCGetPortAttribute()  Exit==\n");

    return ReturnValue;
}

static void
RDCQueryBestSize(ScrnInfoPtr pScrn,
		  Bool motion,
		  short vid_w, short vid_h,
		  short drw_w, short drw_h,
		  unsigned int *p_w, unsigned int *p_h, pointer data)
{
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCQueryBestSize()  Entry==\n");

    *p_w = drw_w;
    *p_h = drw_h;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCQueryBestSize()  Exit==\n");
}


static int
RDCPutImage(ScrnInfoPtr pScrn,
	     short src_x, short src_y,
	     short drw_x, short drw_y,
	     short src_w, short src_h,
	     short drw_w, short drw_h,
	     int id, unsigned char *buf,
	     short width, short height,
	     Bool sync, RegionPtr clipBoxes, pointer data,
	     DrawablePtr pDraw)
{
    RDCRecPtr  pRDC = RDCPTR(pScrn);
    RDCPortPrivPtr  pRDCPortPriv = (RDCPortPrivPtr)data;
    unsigned long   SrcPitch, DstPitch;
    unsigned long   DstWidth, DstHeight, ScreenWidth, ScreenHeight, ViewWidth, ViewHeight;
    HWCINFO         *pHWCInfo = &pRDC->HWCInfo;
    OVERLAY_STATUS  *pOverlayStatus = &pRDC->OverlayStatus;
    int   size = 0;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCPutImage()  Entry==\n");
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==FOURCC ID 0x%x, Width = 0x%x, Height = 0x%x==\n", id, width, height);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Src position X = %d, Y = %d==\n",src_x, src_y);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Src size width = %d, Height = %d==\n",src_w, src_h);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Dst position X = %d, Y = %d==\n",drw_x, drw_y);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Dst size width = %d, Height = %d==\n",drw_w, drw_h);

    pOverlayStatus->iDstLeft = (int)drw_x;
    pOverlayStatus->iDstTop = (int)drw_y;
    pOverlayStatus->iDstWidth = (int)drw_w;
    pOverlayStatus->iDstHeight = (int)drw_h;
    pOverlayStatus->iSrcWidth = (int)src_w;
    pOverlayStatus->iSrcHeight = (int)src_h;

    ScreenWidth  = pRDC->ModePtr->HDisplay;
    ScreenHeight = pRDC->ModePtr->VDisplay;
    
    if (pRDC->DeviceInfo.ucDeviceID == LCDINDEX) 
    {
        ViewWidth    = pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution;
        ViewHeight   = pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution;
    }
    else
    {
        ViewWidth    = ScreenWidth;
        ViewHeight   = ScreenHeight;
    }

    
    if (pRDC->DeviceInfo.ucDeviceID == LCDINDEX)
    {
	    if (pScrn->bitsPerPixel == 16)
        {
            drw_x -= (pHWCInfo->iScreenOffset_x & ~0x3);
        }
        else if((pScrn->bitsPerPixel == 24) || (pScrn->bitsPerPixel == 32))
        {
            drw_x -= (pHWCInfo->iScreenOffset_x & ~0x1);
        }
        drw_y -= pHWCInfo->iScreenOffset_y;
    }

    switch(id)
    {
    case FOURCC_YV12:
    case FOURCC_I420:
        SrcPitch = (width + 3) & ~3;
        break;
    case FOURCC_YUY2:
        SrcPitch = (width << 1);
        break;
    }

    
    DstPitch = (width << 1);
    DstPitch = (DstPitch + 7) & ~7;     
    size = DstPitch * height;
    
    pRDCPortPriv->SrcPitch = DstPitch;
    
    
    if(!(pRDCPortPriv->PackedBuf[0] = RDCAllocateMemory(pScrn, pRDCPortPriv->PackedBuf[0], 
		(pScrn->bitsPerPixel == 16) ? size : (size >> 1))))
	    return BadAlloc;
	    
    
    RDCCopyFOURCC(pRDC, buf, pRDCPortPriv, SrcPitch, DstPitch, id, width, height);
    
    
    if(!REGION_EQUAL(pScrn->pScreen, &pRDCPortPriv->clip, clipBoxes)) 
    {
	    REGION_COPY(pScrn->pScreen, &pRDCPortPriv->clip, clipBoxes);
	    
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Color Key 0x%x==\n",pRDCPortPriv->colorkey);
        
	    xf86XVFillKeyHelper(pScrn->pScreen, pRDCPortPriv->colorkey, clipBoxes);
    }

    
    if(0 == drw_w)
        DstWidth = 1;
    else 
        DstWidth = drw_w;
        
    if(0 == drw_h)
        DstHeight = 1;
    else 
        DstHeight = drw_h;

    pRDCPortPriv->rSrc.left = src_x;
    pRDCPortPriv->rDst.left = drw_x;
    if (drw_x < 0)
    {
        pRDCPortPriv->rSrc.left  += (((-drw_x) * src_w) + (DstWidth >> 1)) / DstWidth;
        pRDCPortPriv->rDst.left = 0;
    }

    pRDCPortPriv->rSrc.right = src_x + src_w; 
    pRDCPortPriv->rDst.right = drw_x + drw_w;
    if (pRDC->DeviceInfo.ScalerConfig.EnableHorScaler) 
    {
        if ((drw_x + drw_w) > ScreenWidth)
        {
            pRDCPortPriv->rSrc.right -= (((drw_x + drw_w - ScreenWidth) * src_w) + (DstWidth >> 1)) / DstWidth;
            pRDCPortPriv->rDst.right = ScreenWidth;
        }
        pRDCPortPriv->rDst.left = pRDCPortPriv->rDst.left * ViewWidth / ScreenWidth;
        pRDCPortPriv->rDst.right = pRDCPortPriv->rDst.right * ViewWidth / ScreenWidth;
    }
    else
    {
        if ((drw_x + drw_w) > ViewWidth)
        {
            pRDCPortPriv->rSrc.right -= (((drw_x + drw_w - ViewWidth) * src_w) + (DstWidth >> 1)) / DstWidth;
            pRDCPortPriv->rDst.right = ViewWidth;
        }
    }

    pRDCPortPriv->rSrc.top = src_y;
    pRDCPortPriv->rDst.top = drw_y;
    if (drw_y < 0)
    {
        pRDCPortPriv->rSrc.top +=  ((-drw_y) * height + (DstHeight >> 1)) / DstHeight;
        pRDCPortPriv->rDst.top = 0;
    }

    pRDCPortPriv->rSrc.bottom = src_y + src_h;
    pRDCPortPriv->rDst.bottom = drw_y + drw_h;
    if (pRDC->DeviceInfo.ScalerConfig.EnableVerScaler) 
    {
        if ((drw_y + drw_h) > ScreenHeight)
        {
            pRDCPortPriv->rSrc.bottom -= (((drw_y + drw_h - ScreenHeight) * src_h) + (DstHeight >> 1)) / DstHeight;
            pRDCPortPriv->rDst.bottom = ScreenHeight;
        }
        pRDCPortPriv->rDst.top = pRDCPortPriv->rDst.top * ViewHeight/ ScreenHeight;
        pRDCPortPriv->rDst.bottom= pRDCPortPriv->rDst.bottom* ViewHeight/ ScreenHeight;
    }
    else
    {
        if ((drw_y + drw_h) > ViewHeight)
        {
            pRDCPortPriv->rSrc.bottom -= (((drw_y + drw_h - ViewHeight) * src_h) + (DstHeight >> 1)) / DstHeight;
            pRDCPortPriv->rDst.bottom = ViewHeight;
        }
    }
    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Src left = 0x%x, Src right  = 0x%x==\n",pRDCPortPriv->rSrc.left,pRDCPortPriv->rSrc.right);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Src top  = 0x%x, Src bottom = 0x%x==\n",pRDCPortPriv->rSrc.top, pRDCPortPriv->rSrc.bottom);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Dst left = 0x%x, Dst right  = 0x%x==\n",pRDCPortPriv->rDst.left,pRDCPortPriv->rDst.right);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Dst top  = 0x%x, Dst bottom = 0x%x==\n",pRDCPortPriv->rDst.top, pRDCPortPriv->rDst.bottom);

    RDCUpdateVID(pRDC, pRDCPortPriv, VID_Enable);
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCPutImage()  Exit==\n");
    return Success;
}

static int
RDCQueryImageAttributesOverlay(ScrnInfoPtr pScrn,int id,
				               unsigned short *w, unsigned short *h,
				               int *pitches, int *offsets)
{
    int size = 0, chromasize = 0;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, 
                   "==RDCQueryImageAttributes()  Entry; Width = %d, Height = %d==\n", 
                   *w, *h);

    if ((!w) || (!h))
    {
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, 
                       "==RDCQueryImageAttributesOverlay() W or H fail==\n");
        return 0;
    }
        
    if (offsets)
	    offsets[0] = 0;

    switch (id)
    {
    case FOURCC_YV12:
    case FOURCC_I420:
        
        *w = (*w + 3) & ~3;
        *h = (*h + 1) & ~1;

        size = *w;

        if (pitches)
            pitches[0] = size;

        size *= *h;
        chromasize = ((*h >> 1) * (*w >> 1)); 

        if (offsets)
            offsets[1] = size;

        if (pitches)
            pitches[1] = pitches[2] = (*w >> 1);

        size += chromasize;

        if (offsets)
            offsets[2] = size;

        size += chromasize;         
        break;
    case FOURCC_YUY2:
        
        *w = (*w + 1) & ~1;
        
        size = (*w << 1);

        if (pitches)
            pitches[0] = size;

        size *= *h; 
        break;
    default:
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, 
                       "==RDCQueryImageAttributesOverlay() Format is not support");
        return 0;
        break;
    }

    if (pitches)
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, 
                       "==pitche0 = 0x%x; pitche1 = 0x%x; pitche2 = 0x%x==\n", 
                       pitches[0],pitches[1],pitches[2]);
    if (offsets)
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, 
                       "==offset0 = 0x%x;offset1 = 0x%x;offset2 = 0x%x;==\n", 
                       offsets[0], offsets[1], offsets[2]);
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCQueryImageAttributesOverlay()  Exit; Size = %d==\n",size);

    return size;
}


static FBLinearPtr
RDCAllocateMemory(
    ScrnInfoPtr pScrn,
    FBLinearPtr PackedBuf,
    int size)
{
    ScreenPtr pScreen;
    FBLinearPtr newBuf;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCAllocateMemory()  Entry==\n");
    xf86DrvMsgVerb(0, X_INFO, 8, "==RDCAllocateMemory()  Total size %d==\n",size);
    
    if(PackedBuf) 
    {
        if(PackedBuf->size >= size)
        {
            return PackedBuf;
        }
        
        if(xf86ResizeOffscreenLinear(PackedBuf, size))
        {
            return PackedBuf;
        }
        xf86FreeOffscreenLinear(PackedBuf);
    }

    pScreen = screenInfo.screens[pScrn->scrnIndex];

    newBuf = xf86AllocateOffscreenLinear(pScreen, 
                                         size, 
                                         8,
                                         NULL, NULL, NULL);

    if(!newBuf) 
    {
        int max_size;

        xf86QueryLargestOffscreenLinear(pScreen, 
                                        &max_size, 
                                        8, 
                                        PRIORITY_EXTREME);
                                        
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==RDCAllocateMemory()  Max size %d==\n",max_size);
        
        if(max_size < size) 
            return NULL;

        xf86PurgeUnlockedOffscreenAreas(pScreen);
        newBuf = xf86AllocateOffscreenLinear(pScreen, 
                                             size, 
                                             8, 
                                             NULL, NULL, NULL);
    } 
    
    xf86DrvMsgVerb(0, X_INFO, 8, "==Buffer size %d==\n",newBuf->size);
    xf86DrvMsgVerb(0, X_INFO, 8, "==Buffer offset %d==\n",newBuf->offset);
    xf86DrvMsgVerb(0, X_INFO, 8, "==Buffer granularity %d==\n",newBuf->granularity);
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCAllocateMemory()  Exit==\n");
    return newBuf;
}


void RDCUpdateVID(RDCRecPtr  pRDC, RDCPortPrivPtr pRDCPortPriv, BYTE bEnable)
{
    unsigned long   SrcWidth, DstWidth, SrcHeight, DstHeight;
    VIDHWINFOPtr pVIDHwInfo = &pRDCPortPriv->VidhwInfo;
    OV_RECTLPtr  pRSrc      = &pRDCPortPriv->rSrc;
    OV_RECTLPtr  pRDst      = &pRDCPortPriv->rDst;

    ULONG   dwVideoSrcOffset;                             
    ULONG   dwFetch;                  
    ULONG   dwVDisplayCount;          
    ULONG   dwVSrcPitch;              
    ULONG   dwWinStartXY;
    ULONG   dwWinEndXY;
    ULONG   dwzoomCtl;
    ULONG   dwVIDBuffer[3];
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCUpdateVID()  Entry==\n");

    
    pRDC->OverlayStatus.ulVidDispCtrl = VDPS_ENABLE|
                                        VDPS_SW_FLIP|
                                        VDPS_DISP_REQ_EXPIRE|
                                        VDPS_DISP_PRE_FETCH|
                                        VDPS_DISP1;

       
    
    SrcWidth = pRSrc->right - pRSrc->left;
    SrcHeight = pRSrc->bottom - pRSrc->top;

    DstWidth = pRDst->right - pRDst->left;
    DstHeight = pRDst->bottom - pRDst->top;
    
    dwVSrcPitch = pRDCPortPriv->SrcPitch;
    dwVIDBuffer[0] = pRDCPortPriv->PackedBufOffset[0];

    dwVDisplayCount = ((pRSrc->bottom - 1) << 16) | (pRSrc->right - 1);
    dwVideoSrcOffset = (pRSrc->left << 16) | pRSrc->top;
    dwWinStartXY = (pRDst->left << 16) | pRDst->top;
    dwWinEndXY = ((pRDst->right - 1) << 16) | (pRDst->bottom - 1);

    
    dwFetch = ((SrcWidth << 1) + 7) >> 3;

    if (SrcWidth > DstWidth)
        dwFetch = ((DstWidth << 1) + 7) >> 3;
        
    if (dwFetch < 4)
        dwFetch = 4;
    
    dwzoomCtl &= 0x0000FFFF;
    if (SrcWidth < DstWidth)
    {
        dwzoomCtl |=((ULONG)(((SrcWidth*0x1000)/DstWidth)&0x0FFF)<<16)|
                                VDPS_H_ZOOM_ENABLE|VDPS_H_INTERP;        
    }

    dwzoomCtl &= 0xFFFF0000;
    if (SrcHeight < DstHeight)
    {
        dwzoomCtl |=(ULONG)(((SrcHeight*0x1000)/DstHeight)&0x0FFF)|
                                VDPS_V_ZOOM_ENABLE|VDPS_V_INTERP;        
    }
    
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Color Key 0x%x==\n",pVIDHwInfo->dwColorKey);

    if (!(((int)pRDst->right < (int)pRDst->left) || ((int)pRDst->bottom < (int)pRDst->top)))
        pRDC->OverlayStatus.bPanningOverlayEnable = true;
    
    
    
    if (!pRDC->OverlayStatus.bPanningOverlayEnable)
    {
        pRDC->OverlayStatus.ulVidDispCtrl &= ~VDPS_ENABLE;
    }
    
    if (!pRDC->OverlayStatus.bOverlayEnable)
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_CTL) = 
            VDPS_ENABLE;
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FIRST_COLORKEY) = 
            pVIDHwInfo->dwColorKey | VDPS_ENABLE_CK;
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FIFO_THRESHOLD) = 
            VDPS_FIFO_THRESHOLD;
        pRDC->OverlayStatus.bOverlayEnable = TRUE;
    }

    if (pVIDHwInfo->dwVidCtl != pRDC->OverlayStatus.ulVidDispCtrl)
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_CTL) = pRDC->OverlayStatus.ulVidDispCtrl;
        pVIDHwInfo->dwVidCtl = pRDC->OverlayStatus.ulVidDispCtrl;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==VID CTRL Setting 0x%x==\n",pVIDHwInfo->dwVidCtl);
    }
    
    if (pVIDHwInfo->dwFetch != dwFetch)
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FETCH_COUNT) = dwFetch;
        pVIDHwInfo->dwFetch = dwFetch;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Fetch count 0x%x==\n",pVIDHwInfo->dwFetch);
    }
    
    if (pVIDHwInfo->dwVideoSrcOffset != dwVideoSrcOffset)
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_START_OFFSET) = dwVideoSrcOffset;
        pVIDHwInfo->dwVideoSrcOffset = dwVideoSrcOffset;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Video Src Offset 0x%x==\n",pVIDHwInfo->dwVideoSrcOffset);
    }
    
    if (pVIDHwInfo->dwVDisplayCount != dwVDisplayCount)
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_SRC_DISP_COUNT_ON_SCR) = dwVDisplayCount;
        pVIDHwInfo->dwVDisplayCount = dwVDisplayCount;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Display count 0x%x==\n",pVIDHwInfo->dwVDisplayCount);
    }
    
    if (pVIDHwInfo->dwWinStartXY != dwWinStartXY)
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_START_LOCATION) = dwWinStartXY;
        pVIDHwInfo->dwWinStartXY = dwWinStartXY;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Start Location 0x%x==\n",pVIDHwInfo->dwWinStartXY);
    }
    
    if (pVIDHwInfo->dwWinEndXY != dwWinEndXY)
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_END_LOCATION) = dwWinEndXY;
        pVIDHwInfo->dwWinEndXY = dwWinEndXY;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==End Location 0x%x==\n",pVIDHwInfo->dwWinEndXY);
    }
    
    if (pVIDHwInfo->dwzoomCtl != dwzoomCtl)
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_ZOOM_CTL) = dwzoomCtl;
        pVIDHwInfo->dwzoomCtl = dwzoomCtl;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Zoom Setting 0x%x==\n",pVIDHwInfo->dwzoomCtl);
    }
    
    if (pVIDHwInfo->dwVIDBuffer[0] != dwVIDBuffer[0])
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FB0) = dwVIDBuffer[0];
        pVIDHwInfo->dwVIDBuffer[0] = dwVIDBuffer[0];
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Video Display Buffer 0x%x==\n",pVIDHwInfo->dwVIDBuffer[0]);
    }
    
    if (pVIDHwInfo->dwVSrcPitch != dwVSrcPitch)
    {
        *(ULONG*)(pRDC->MMIOVirtualAddr + VDP_FB_STRIDE) = dwVSrcPitch;
        pVIDHwInfo->dwVSrcPitch = dwVSrcPitch;
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Buffer stride 0x%x==\n",pVIDHwInfo->dwVSrcPitch);
    }

    *(unsigned long*)(pRDC->MMIOVirtualAddr + VDP_REG_UPDATE_MOD_SELECT) = VDPS_FIRE;

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCUpdateVID()  Exit==\n");
}


void RDCCopyFOURCC(RDCRecPtr  pRDC, unsigned char* pSrcStart, RDCPortPrivPtr pRDCPortPriv,
                   unsigned long SrcPitch, unsigned long DstPitch, int FourCC, 
                   short width, short height)
{
    unsigned char* pDst = NULL;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCCopyFOURCC()  Entry==\n");
    xf86DrvMsgVerb(0, X_INFO, 8, "==Packed1 Offset  0x%x==\n", pRDCPortPriv->PackedBuf[0]->offset);
    xf86DrvMsgVerb(0, X_INFO, 8, "==Per Pixel Bits 0x%x==\n",pRDC->VideoModeInfo.Bpp);

    pRDCPortPriv->PackedBufOffset[0] = 
        (pRDCPortPriv->PackedBuf[0]->offset * pRDC->VideoModeInfo.Bpp);

    pDst = pRDC->FBVirtualAddr + pRDCPortPriv->PackedBufOffset[0];
    
    xf86DrvMsgVerb(0, X_INFO, 8, "==FB  Base  0x%x==\n", (unsigned long)((unsigned long*)pRDC->FBVirtualAddr));
    xf86DrvMsgVerb(0, X_INFO, 6, "==Dst Start 0x%x==\n", (unsigned long)((unsigned long*)pDst));
    xf86DrvMsgVerb(0, X_INFO, 6, "==Src Start 0x%x==\n", (unsigned long)((unsigned long*)pSrcStart));
    xf86DrvMsgVerb(0, X_INFO, 6, "==Src Pitch 0x%x==\n", SrcPitch);
    xf86DrvMsgVerb(0, X_INFO, 8, "==Dst Pitch 0x%x==\n", DstPitch);
    xf86DrvMsgVerb(0, X_INFO, 8, "==Width 0x%x==\n", width);
    xf86DrvMsgVerb(0, X_INFO, 8, "==Height 0x%x==\n", height);
    
    switch (FourCC)
    {
    case FOURCC_YV12:
        {
            int i,j;
            CARD32 *pDst1 = (CARD32*)pDst;
            unsigned long SrcYPitch = SrcPitch;
            unsigned long SrcVPitch = SrcPitch >> 1;
            unsigned long SrcUPitch = SrcPitch >> 1;
            unsigned char* pSrcY = pSrcStart;
            unsigned char* pSrcV = pSrcStart + (width * height);
            unsigned char* pSrcU = pSrcStart + ((width << 1) * (height << 1));

            xf86DrvMsgVerb(0, X_INFO, 8, "==Y Start 0x%x==\n", (unsigned long)((unsigned long*)pSrcY));
            xf86DrvMsgVerb(0, X_INFO, 8, "==V Start 0x%x==\n", (unsigned long)((unsigned long*)pSrcV));
            xf86DrvMsgVerb(0, X_INFO, 8, "==U Start 0x%x==\n", (unsigned long)((unsigned long*)pSrcU));
            xf86DrvMsgVerb(0, X_INFO, 8, "==Y Pitch 0x%x==\n", SrcYPitch);
            xf86DrvMsgVerb(0, X_INFO, 8, "==V,U Pitch 0x%x==\n", SrcVPitch);
            
            for(j = 0; j < height; j++)
            {
                for(i = 0; i < (width >> 1); i++) 
                {
                    pDst1[i] = pSrcY[i << 1] | (pSrcY[(i << 1) + 1] << 16) |
                              (pSrcU[i] << 8) | (pSrcV[i] << 24);
                }
                
                pDst1 += DstPitch;
                pSrcY += SrcYPitch;

                if(j & 1)
                {
                    pSrcV += SrcVPitch;
                    pSrcU += SrcUPitch;
                }
            }
        }
        break;
    case FOURCC_YUY2:
        while(height--)
        {
            memcpy(pDst, pSrcStart, (width << 1));
            pSrcStart += SrcPitch;
            pDst += DstPitch;
        }
        break;
    }
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCCopyFOURCC()  Exit==\n");
}

static void
RDCStopVideoPost(ScrnInfoPtr pScrn, pointer data, Bool shutdown)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    RDCPortPrivPtr pXVPriv = (RDCPortPrivPtr)data;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCStopVideo()  Entry==\n");

    REGION_EMPTY(pScrn->pScreen, &pXVPriv->clip);

    
    if (shutdown)
    {
        PKT_SC *pSingleCMD;
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCStopVideo()  Stop Overlay==\n");

        pSingleCMD = (PPKT_SC)pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*7);
       
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) CR_LOCK_ID; 
        pSingleCMD->PKT_SC_dwData[0] = CR_LOCK_VIDEO;  
        pSingleCMD++; 
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_START_LOCATION_I;
        pSingleCMD->PKT_SC_dwData[0] = 0x0;  
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_END_LOCATION_I;
        pSingleCMD->PKT_SC_dwData[0] = 0x0;  
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_ZOOM_CTL_I;
        pSingleCMD->PKT_SC_dwData[0] = 0x0;  
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_CTL_I;
        pSingleCMD->PKT_SC_dwData[0] = 0x0;  
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_REG_UPDATE_MOD_SELECT_I;
        pSingleCMD->PKT_SC_dwData[0] = VDPS_FIRE;  
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_STREAM_CTL_I;
        pSingleCMD->PKT_SC_dwData[0] = 0x0;  
        pSingleCMD++;
        
        mUpdateWritePointer;
    }

    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCStopVideo()  Exit==\n");
}

void RDCCopyFourCCVPOST(RDCRecPtr  pRDC, RDCPortPrivPtr pRDCPortPriv, unsigned char* pSrc,
                        short SrcX, short SrcY, short Width, short Height)
{
    int             i,j;
    unsigned long   ulSrcPitch, ulDestPitch;
    unsigned char*  pDest = NULL;
    VIDHWINFOPtr    pVIDHwInfo = &pRDCPortPriv->VidhwInfo;
    VPOSTHWINFOPtr  pVPHwInfo = &pRDCPortPriv->VPosthwInfo;

    switch (pRDCPortPriv->YUVFormat)
    {
    case FOURCC_YV12:
        {
            ULONG   ulShift = 0;
            ULONG   ulTempH;
            
            for (i = 0; i < 3; i++)
            {
                ulSrcPitch = ALIGN_TO_4(Width);
                if ( i > 0)
                {
                    ulShift = 1;
                    ulSrcPitch = (Width >> ulShift);
                    ulTempH = Height >> ulShift;
                }    
                ulDestPitch = pRDCPortPriv->PackedBufStride[i];
                pDest = pRDC->FBVirtualAddr + pRDCPortPriv->PackedBufOffset[i];
                while(ulTempH--)
                {
                    memcpy(pDest, pSrc, (Width >> ulShift));
                    pSrc += ulSrcPitch;
                    pDest += ulDestPitch;
                }

            }            
        }
        break;
    case FOURCC_YUY2:
        {
            ulDestPitch = pRDCPortPriv->PackedBufStride[0];
            ulSrcPitch = ALIGN_TO_2(Width << 1);
            pDest = pRDC->FBVirtualAddr + pRDCPortPriv->PackedBufOffset[0];
            
            while(Height--)
            {
                memcpy(pDest, pSrc, (Width << 1));
                pSrc += ulSrcPitch;
                pDest += ulDestPitch;
            }
        }
        break;
    }
}


void RDCUpdateVideo(RDCRecPtr pRDC, RDCPortPrivPtr pRDCPortPriv, BYTE bEnable)
{
    unsigned long   SrcWidth, DstWidth, SrcHeight, DstHeight;
    PKT_SC          *pSingleCMD;
    double          dbXfactor, dbYfactor;
    ULONG           ulTempTop, ulTempLeft;
    int             i = 0;
    OVERLAY_STATUS  *pOverlayStatus = &pRDC->OverlayStatus;
    VIDHWINFOPtr    pVIDHwInfo = &pRDCPortPriv->VidhwInfo;
    VPOSTHWINFOPtr  pVPHwInfo = &pRDCPortPriv->VPosthwInfo;
    OV_RECTLPtr     pRSrc      = &pRDCPortPriv->rSrc;
    OV_RECTLPtr     pRDst      = &pRDCPortPriv->rDst;

    dbXfactor = dbYfactor = 1.0;
    
    
    ULONG           dwVideoSrcOffset;                             
    ULONG           dwFetch;                  
    ULONG           dwVDisplayCount;          
    ULONG           dwVSrcPitch;              
    ULONG           dwWinStartXY;
    ULONG           dwWinEndXY;
    ULONG           dwzoomCtl;
    ULONG           dwVIDBuffer[3];           

    
    ULONG           dwVPVideoSize;
    ULONG           dwVPStreamCtrl;
    ULONG           dwVPOST_H_Scale;
    ULONG           dwVPOST_V_Scale;
    ULONG           dwVPLumaPitch;
    ULONG           dwVPChromaPitch;
    ULONG           dwVPBuffer[3];            
    
    SrcWidth = pRSrc->right - pRSrc->left;
    SrcHeight = pRSrc->bottom - pRSrc->top;
    DstWidth = pRDst->right - pRDst->left;
    DstHeight = pRDst->bottom - pRDst->top;

    
    switch (pRDCPortPriv->YUVFormat)
    {
    case FOURCC_YUY2:
        dwVPStreamCtrl = VPS_STREAM_YUY2;
        break;
    case FOURCC_YV12:
    case FOURCC_I420:
        dwVPStreamCtrl = VPS_STREAM_YV12;
        break;
    }
    
    dwVPStreamCtrl |=  VPS_ENABLE|
                       VPS_TRIPLE_BUFFER|
                       VPS_SRC_CAPTURE|
                       VPS_INT_ENABLE;

    pOverlayStatus->ulVidDispCtrl = VDPS_ENABLE|
                                    VDPS_SW_FLIP|
                                    VDPS_DISP_REQ_EXPIRE|
                                    VDPS_DISP_PRE_FETCH|
                                    VDPS_DISP1;    
    
    dwVPVideoSize = ((SrcWidth - 1) << 16) |
                    (SrcHeight -1);
    
    dwVDisplayCount = ((pRSrc->bottom - 1) << 16) | (pRSrc->right - 1);
    dwVideoSrcOffset = (pRSrc->left << 16) | pRSrc->top;
    dwWinStartXY = (pRDst->left << 16) | pRDst->top;
    dwWinEndXY = ((pRDst->right - 1) << 16) | (pRDst->bottom - 1);

    
    dwFetch = ((SrcWidth << 1) + 7) >> 3;
    if (dwFetch < 4)
        dwFetch = 4;

        
    if (SrcWidth > DstWidth)
    {
        double dbXfactor = SrcWidth / DstWidth;   
        
        dwVPOST_H_Scale = VPS_SCALE_ENABLE | VPS_SCALE_DOWN;
        dwVPOST_H_Scale |= ((SrcWidth<<12)/DstWidth);
        dwFetch = ((DstWidth << 1) + 7) >> 3;
    }
    else if (SrcWidth == DstWidth)
    {
        
        dwzoomCtl &= 0x0000FFFF;
        dwVPOST_H_Scale |= 0x0000FFFF;
        dwFetch = ((SrcWidth << 1) + 7) >> 3;
    }
    else
    {
        
        dwzoomCtl |=((ULONG)(((SrcWidth*0x1000)/DstWidth)&0x0FFF)<<16)|
                                VDPS_H_ZOOM_ENABLE|VDPS_H_INTERP;   
        dwFetch = ((SrcWidth << 1) + 7) >> 3;
    }
    
    dwVDisplayCount = ((((ULONG)(SrcWidth/dbXfactor))-1) & 0x0000FFFF);
    dwVideoSrcOffset = (((ULONG)((double)pRSrc->left/dbXfactor))<<16);  
        
    
    if (SrcHeight > DstHeight)
    {
        double dbYfactor = SrcHeight / DstHeight;
        
        dwVPOST_V_Scale = VPS_SCALE_ENABLE | VPS_SCALE_DOWN;
        dwVPOST_V_Scale |= ((SrcHeight<<12) / DstHeight);
    }
    else if (SrcHeight == DstWidth) 
    {
        
        dwzoomCtl &= 0xFFFF0000;
        dwVPOST_V_Scale |= 0x0000FFFF;
    }
    else
    {
        
        dwzoomCtl |=(ULONG)(((SrcHeight*0x1000)/DstHeight)&0x0FFF)|
                                VDPS_V_ZOOM_ENABLE|VDPS_V_INTERP; 
    }
    
    dwVDisplayCount |= ((((ULONG)(SrcHeight/dbYfactor)<<16)-1) & 0xFFFF0000);
    dwVideoSrcOffset |= ((ULONG)((double)pRSrc->top /dbYfactor));   


    
    pVIDHwInfo->dwVSrcPitch = dwVSrcPitch = pRDCPortPriv->SrcPitch;
    dwVPLumaPitch = pRDCPortPriv->PackedBufStride[0];
    dwVPChromaPitch = pRDCPortPriv->PackedBufStride[1];
    
    
    for (i = 0; i++; i < 3)
    {
        dwVPBuffer[i] = pRDCPortPriv->PackedBufOffset[i];
        dwVIDBuffer[i] = pVPHwInfo->ulVP_DestBuf_Offset[i];
    }
    
    
    pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*1);
    pSingleCMD->PKT_SC_dwHeader  = (ULONG)(CR_LOCK_ID); 
    pSingleCMD->PKT_SC_dwData[0] = (ULONG)(CR_LOCK_VIDEO); 
    pSingleCMD++;
    
    if ((dwVPStreamCtrl != pVPHwInfo->ulVP_StreamCtrl) ||
        (pOverlayStatus->ulVidDispCtrl != pVIDHwInfo->dwVidCtl))
    {
        pVPHwInfo->ulVP_StreamCtrl = dwVPStreamCtrl;
        pVIDHwInfo->dwVidCtl = pOverlayStatus->ulVidDispCtrl;
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*4);
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_STREAM_CTL_I;
        pSingleCMD->PKT_SC_dwData[0] = dwVPStreamCtrl;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_CTL_I;
        pSingleCMD->PKT_SC_dwData[0] = pVIDHwInfo->dwVidCtl;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_FIFO_THRESHOLD_I;
        pSingleCMD->PKT_SC_dwData[0] = (ULONG)(VDPS_FIFO_THRESHOLD);
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_FIRST_COLORKEY_I;
        pSingleCMD->PKT_SC_dwData[0] = (ULONG)(pVIDHwInfo->dwColorKey | VDPS_ENABLE_CK);
        pSingleCMD++;
    }
      

    if (dwVPVideoSize != pVPHwInfo->ulVP_VideoSize)
    {
        
        pVPHwInfo->ulVP_VideoSize = dwVPVideoSize;
        pVPHwInfo->ulVP_SrcLumaStride = dwVPLumaPitch;
        pVPHwInfo->ulVP_Dest_SrcChroma_Stride = (dwVPChromaPitch << 19) |
                                                (dwVSrcPitch);
                                                
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*10);
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_VIDEO_SIZE_I;
        pSingleCMD->PKT_SC_dwData[0] = pVPHwInfo->ulVP_VideoSize;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_SRC_BUF_STRIDE_I;
        pSingleCMD->PKT_SC_dwData[0] = pVPHwInfo->ulVP_SrcLumaStride;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_DEST_STRIDE_I;
        pSingleCMD->PKT_SC_dwData[0] = pVPHwInfo->ulVP_Dest_SrcChroma_Stride;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_SW_SRC_BUF_LUMA_START_I;
        pSingleCMD->PKT_SC_dwData[0] = pVPHwInfo->ulVP_SrcBuf[0];
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_SW_SRC_BUF_CHROMA_START_I;
        pSingleCMD->PKT_SC_dwData[0] = pVPHwInfo->ulVP_SrcBuf[1];
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VP_YV12_V_BASE_I;
        pSingleCMD->PKT_SC_dwData[0] = pVPHwInfo->ulVP_SrcBuf[2];
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_FB0_I;
        pSingleCMD->PKT_SC_dwData[0] = dwVIDBuffer[0];
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_FB1_I;
        pSingleCMD->PKT_SC_dwData[0] = dwVIDBuffer[1];
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_FB2_I;
        pSingleCMD->PKT_SC_dwData[0] = dwVIDBuffer[2];
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader  = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_FB_STRIDE_I;
        pSingleCMD->PKT_SC_dwData[0] = pVIDHwInfo->dwVSrcPitch;
        pSingleCMD++;
    }

    if ((dwVDisplayCount != pVIDHwInfo->dwVDisplayCount) ||
        (dwVideoSrcOffset != pVIDHwInfo->dwVideoSrcOffset) ||
        (dwWinStartXY != pVIDHwInfo->dwWinStartXY)  ||
        (dwWinEndXY != pVIDHwInfo->dwWinEndXY))
    {
        
        pVIDHwInfo->dwVDisplayCount = dwVDisplayCount;
        pVIDHwInfo->dwVideoSrcOffset = dwVideoSrcOffset;
        pVIDHwInfo->dwzoomCtl = dwzoomCtl;
        pVIDHwInfo->dwFetch = dwFetch;
        pVIDHwInfo->dwWinStartXY = dwWinStartXY;
        pVIDHwInfo->dwWinEndXY = dwWinEndXY;
        pVPHwInfo->ulVP_HScale = dwVPOST_H_Scale;
        pVPHwInfo->ulVP_VScale = dwVPOST_V_Scale;
        
        pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*9);
        pSingleCMD->PKT_SC_dwHeader = (ULONG) PKT_VIDEO_CMD_HEADER | VP_H_SCALE_I;
        pSingleCMD->PKT_SC_dwData[0] = pVPHwInfo->ulVP_HScale;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader = (ULONG) PKT_VIDEO_CMD_HEADER | VP_V_SCALE_I;
        pSingleCMD->PKT_SC_dwData[0] = pVPHwInfo->ulVP_VScale;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_ZOOM_CTL_I;
        pSingleCMD->PKT_SC_dwData[0] = pVIDHwInfo->dwzoomCtl;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_SRC_DISP_COUNT_ON_SCR_I;
        pSingleCMD->PKT_SC_dwData[0] = pVIDHwInfo->dwVDisplayCount;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_START_OFFSET_I;
        pSingleCMD->PKT_SC_dwData[0] = pVIDHwInfo->dwVideoSrcOffset;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_START_OFFSET_I;
        pSingleCMD->PKT_SC_dwData[0] = pVIDHwInfo->dwVideoSrcOffset;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_FETCH_COUNT_I;
        pSingleCMD->PKT_SC_dwData[0] = pVIDHwInfo->dwFetch;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_START_LOCATION_I;
        pSingleCMD->PKT_SC_dwData[0] = pVIDHwInfo->dwWinStartXY;
        pSingleCMD++;
        pSingleCMD->PKT_SC_dwHeader = (ULONG) PKT_VIDEO_CMD_HEADER | VDP_END_LOCATION_I;
        pSingleCMD->PKT_SC_dwData[0] = pVIDHwInfo->dwWinEndXY;
        pSingleCMD++;
    }
    
    pSingleCMD = (PKT_SC *) pjRequestCMDQ(pRDC, PKT_SINGLE_LENGTH*1);
    pSingleCMD->PKT_SC_dwHeader  = (ULONG)(PKT_VIDEO_CMD_HEADER | VDP_REG_UPDATE_MOD_SELECT_I); 
    pSingleCMD->PKT_SC_dwData[0] = (ULONG)(VDPS_FIRE); 
    pSingleCMD++;
    mUpdateWritePointer;
}



void RDCAllocateVPOSTMem(ScrnInfoPtr pScrn, RDCPortPrivPtr pRDCPortPriv, short width, short height)
{
    int             size, i, shift;
    ULONG           ulPitch;
    RDCRecPtr       pRDC = RDCPTR(pScrn);
    VIDHWINFOPtr    pVIDHwInfo = &pRDCPortPriv->VidhwInfo;
    VPOSTHWINFOPtr  pVPHwInfo = &pRDCPortPriv->VPosthwInfo;
    
    
    
    ulPitch = ALIGN_TO_8(width << 1);
    
    
    pRDCPortPriv->SrcPitch = ulPitch;
    
    size = ulPitch * height;

    for (i = 0; i < 3; i++)
    {
        pVPHwInfo->pVPDestBuffer[i] = 
            RDCAllocateMemory(pScrn, 
                              pVPHwInfo->pVPDestBuffer[i],
                              (pScrn->bitsPerPixel == 16) ? size:(size >> 1));
        pVPHwInfo->ulVP_DestBuf_Offset[i] = 
            pVPHwInfo->pVPDestBuffer[i]->offset * pRDC->VideoModeInfo.Bpp;
    }

    
    
    switch (pRDCPortPriv->YUVFormat)
    {
    case FOURCC_YV12:
    case FOURCC_I420:
        
        
        for (i = 0; i < 3; i++)
        {
            if (i > 0)
                shift = 1;
                
            ulPitch = ALIGN_TO_8(width >> shift);
            pRDCPortPriv->PackedBufStride[i] = ulPitch;
            size = ulPitch * (height >> shift);
            pRDCPortPriv->PackedBuf[i] = 
                RDCAllocateMemory(pScrn,
                                  pRDCPortPriv->PackedBuf[i],
                                  (pScrn->bitsPerPixel == 16) ? size:(size >> 1));
            pRDCPortPriv->PackedBufOffset[i] = 
                pRDCPortPriv->PackedBuf[i]->offset * pRDC->VideoModeInfo.Bpp;
        }    
        break;
    case FOURCC_YUY2:
        
        ulPitch = ALIGN_TO_8(width << 1);
        pRDCPortPriv->PackedBufStride[0] = ulPitch;
        size = ulPitch * height;
        pRDCPortPriv->PackedBuf[0] = 
            RDCAllocateMemory(pScrn,
                              pRDCPortPriv->PackedBuf[0],
                              (pScrn->bitsPerPixel == 16) ? size:(size >> 1));
        pRDCPortPriv->PackedBufOffset[0] = 
            pRDCPortPriv->PackedBuf[0]->offset * pRDC->VideoModeInfo.Bpp;
        break;
    }

    for (i = 0; i < 3; i++)
        pVPHwInfo->ulVP_SrcBuf[i] = pRDCPortPriv->PackedBufOffset[i];    
}

static int
RDCPutImageVPOST(ScrnInfoPtr pScrn,
	     short src_x, short src_y,
	     short drw_x, short drw_y,
	     short src_w, short src_h,
	     short drw_w, short drw_h,
	     int id, unsigned char *buf,
	     short width, short height,
	     Bool sync, RegionPtr clipBoxes, pointer data,
	     DrawablePtr pDraw)
{
    RDCRecPtr       pRDC = RDCPTR(pScrn);
    RDCPortPrivPtr  pRDCPortPriv = (RDCPortPrivPtr)data;
    unsigned long   SrcPitch, DstPitch;
    unsigned long   DstWidth, DstHeight, ScreenWidth, ScreenHeight, ViewWidth, ViewHeight;
    OVERLAY_STATUS  *pOverlayStatus = &pRDC->OverlayStatus;
    
    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "==RDCPutImageVPOST()  Entry==\n");
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==FOURCC ID 0x%x, Width = 0x%x, Height = 0x%x==\n", id, width, height);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Src position X = %d, Y = %d==\n",src_x, src_y);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Src size width = %d, Height = %d==\n",src_w, src_h);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Dst position X = %d, Y = %d==\n",drw_x, drw_y);
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Dst size width = %d, Height = %d==\n",drw_w, drw_h);

    
    pOverlayStatus->iDstLeft = (int)drw_x;
    pOverlayStatus->iDstTop = (int)drw_y;
    pOverlayStatus->iDstWidth = (int)drw_w;
    pOverlayStatus->iDstHeight = (int)drw_h;
    pOverlayStatus->iSrcWidth = (int)src_w;
    pOverlayStatus->iSrcHeight = (int)src_h;
    
    pRDCPortPriv->YUVFormat = id;
    
    
    RDCAllocateVPOSTMem(pScrn, pRDCPortPriv, src_w, src_h);

    
    RDCCopyFourCCVPOST(pRDC, pRDCPortPriv, buf, src_x, src_y, width, height);
    
    
    if(!REGION_EQUAL(pScrn->pScreen, &pRDCPortPriv->clip, clipBoxes)) 
    {
	    REGION_COPY(pScrn->pScreen, &pRDCPortPriv->clip, clipBoxes);
	    
        xf86DrvMsgVerb(0, X_INFO, InfoLevel, "==Color Key 0x%x==\n",pRDCPortPriv->colorkey);
        
	    xf86XVFillKeyHelper(pScrn->pScreen, pRDCPortPriv->colorkey, clipBoxes);
    }

    
    
    ScreenWidth  = pRDC->ModePtr->HDisplay;
    ScreenHeight = pRDC->ModePtr->VDisplay;
    
    if (pRDC->DeviceInfo.ucDeviceID == LCDINDEX) 
    {
        
        ViewWidth    = pRDC->DeviceInfo.MonitorSize.ulHorMaxResolution;
        ViewHeight   = pRDC->DeviceInfo.MonitorSize.ulVerMaxResolution;
    }
    else
    {
        
        
        ViewWidth    = ScreenWidth;
        ViewHeight   = ScreenHeight;
    }   

    
    if(0 == drw_w)
        DstWidth = 1;
    else 
        DstWidth = drw_w;
        
        
    if(0 == drw_h)
        DstHeight = 1;
    else 
        DstHeight = drw_h;

    pRDCPortPriv->rSrc.left = src_x;
    pRDCPortPriv->rDst.left = drw_x;

    
    if (drw_x < 0)
    {
        pRDCPortPriv->rSrc.left  += (((-drw_x) * src_w) + (DstWidth >> 1)) / DstWidth;
        pRDCPortPriv->rDst.left = 0;
    }

    pRDCPortPriv->rSrc.right = src_x + src_w; 
    pRDCPortPriv->rDst.right = drw_x + drw_w;

    
    if ((drw_x + drw_w) > ScreenWidth)
    {
        pRDCPortPriv->rSrc.right -= (((drw_x + drw_w - ScreenWidth) * src_w) + (DstWidth >> 1)) / DstWidth;
        pRDCPortPriv->rDst.right = ScreenWidth;
    }
    
    
    pRDCPortPriv->rDst.left = pRDCPortPriv->rDst.left * ViewWidth / ScreenWidth;
    pRDCPortPriv->rDst.right = pRDCPortPriv->rDst.right * ViewWidth / ScreenWidth;

    
    pRDCPortPriv->rSrc.top = src_y;
    pRDCPortPriv->rDst.top = drw_y;

    
    if (drw_y < 0)
    {
        pRDCPortPriv->rSrc.top +=  ((-drw_y) * height + (DstHeight >> 1)) / DstHeight;
        pRDCPortPriv->rDst.top = 0;
    }

    pRDCPortPriv->rSrc.bottom = src_y + src_h;
    pRDCPortPriv->rDst.bottom = drw_y + drw_h;
    
    if ((drw_y + drw_h) > ScreenHeight)
    {
        pRDCPortPriv->rSrc.bottom -= (((drw_y + drw_h - ScreenHeight) * src_h) + (DstHeight >> 1)) / DstHeight;
        pRDCPortPriv->rDst.bottom = ScreenHeight;
    }
    pRDCPortPriv->rDst.top = pRDCPortPriv->rDst.top * ViewHeight/ ScreenHeight;
    pRDCPortPriv->rDst.bottom= pRDCPortPriv->rDst.bottom* ViewHeight/ ScreenHeight;

    RDCUpdateVideo(pRDC, pRDCPortPriv, 1);
    
    return Success;
}
