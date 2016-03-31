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

#include <sys/ipc.h>
#include <sys/shm.h>

#include "xf86.h"
#include "xf86_OSproc.h"
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


#ifdef HAVE_XAA
#include "xaa.h"
#endif
#include "xaarop.h"


#include "xf86Cursor.h"


#include "rdc.h"

#include "X11/Xproto.h"     
#include "extnsionst.h" 
#include "dixstruct.h"  

#include "rdc_extension.h"




#if GET_ABI_MAJOR(ABI_VIDEODRV_VERSION) < 12
#define _swapl(x, n)	swapl(x, n)
#define _swaps(x, n)	swaps(x, n)
#else
#define _swapl(x, n)	swapl(x)
#define _swaps(x, n)	swaps(x)
#endif

ULONG *inBufPtr, *outBufPtr ; 


int g_ScreenNumber;


static int ProcRDCGFXQueryVersion (ClientPtr client)
{        
    xRDCGFXQueryVersionReply rep;
    register int n;

    REQUEST_SIZE_MATCH(xRDCGFXQueryVersionReply);
    rep.type = X_Reply;
    rep.length = 0;
    rep.sequenceNumber = client->sequence;
    rep.majorVersion = 2;
    rep.minorVersion = 0;
    
    if(client->swapped)
    {
		_swaps(&rep.sequenceNumber, n);
        _swapl(&rep.length, n);
        _swaps(&rep.majorVersion, n);
        _swaps(&rep.minorVersion, n);
    }
    
    WriteToClient(client, sizeof(xRDCGFXQueryVersionReply), (char *)&rep);
    return (client->noClientException);
}



static int ProcRDCGFXCommand(ClientPtr client)
{
    xRDCGFXCommandReq *req = (xRDCGFXCommandReq*)client->requestBuffer;    register int     n;
    int          i, ret;    

    REQUEST_SIZE_MATCH(*req);
	

    
    if(!CheckExtension(RDC_GFX_UT_EXTENSION_NAME)) return BadMatch;       

    
    RDCGFXUtilityProc(req);
    
    
    
    xGenericReply *reply = client->requestBuffer;
    reply->type = X_Reply;
    reply->length = 0;
#if 0
    if(client->swapped)
    {
       swapl(&req->length, n);       
       swapl(&req->util_cmd_id, n);
       swapl(&req->result_header, n);
       
       for(i = 0; i < req->outBufferSize; i++)
       {
          swapl(&req->outBuffer[i], n);
       }
    }
#endif
    
    WriteToClient(client, sizeof(xGenericReply), (char *)reply);
    return client->noClientException;
}

static void RDCGFXResetProc(ExtensionEntry* extEntry)
{    
}

static int ProcRDCGFXDispatch(ClientPtr client)
{   
    REQUEST(xReq);
    switch(stuff->data)
    {
        case X_RDCGFXQueryVersion:
            return ProcRDCGFXQueryVersion(client);
        case X_RDCGFXCommand:
            return ProcRDCGFXCommand(client);
    }
    
    return BadRequest;
}

void RDCDisplayExtensionInit(ScrnInfoPtr pScrn)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Enter RDCDisplayExtensionInit()== \n");

    
    int shm_in_id, shm_out_id ;
    key_t in_key, out_key ;
    char inBufName[32] = "/RDC/GFX/shm/inBuf" ;
    char outBufName[32] = "/RDC/GFX/shm/outBuf" ;

    in_key = ftok(inBufName, 4) ;
    out_key = ftok(outBufName, 4) ;
    shm_in_id = shmget(in_key, 4096, IPC_CREAT) ;
    shm_out_id = shmget(out_key, 4096, IPC_CREAT) ;
    inBufPtr = (ULONG *) shmat(shm_in_id, NULL, 0) ;
    outBufPtr = (ULONG *) shmat(shm_out_id, NULL, 0) ;
    
    if(NULL == CheckExtension(RDC_GFX_UT_EXTENSION_NAME))
    {
	xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 6, "== check point 1== \n");
        
        if (!AddExtension(RDC_GFX_UT_EXTENSION_NAME, 0, 0,
                                     ProcRDCGFXDispatch,
                                     ProcRDCGFXDispatch,
                                     RDCGFXResetProc,
                                     StandardMinorOpcode))
        {
            xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "Failed to add RDCGFX extension\n");
            return;
        }
    }
    else
    {
        
        xf86DrvMsg(pScrn->scrnIndex, X_ERROR, "error: RDC GFX Extension Exists!\n");
    }
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Exit RDCDisplayExtensionInit()== \n");    
}

int RDCGFXUtilityProc(xRDCGFXCommandReq* req)
{
    ScrnInfoPtr pScrn = xf86Screens[g_ScreenNumber];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    CBIOS_ARGUMENTS *pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
    UCHAR ucNewDeviceID;

    
            
    ULONG   ulResult;

    switch(req->util_cmd_id)
    {
        
        
        
        case UT_QUERY_SUPPORT_DEVICE: 
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Query Device Support\n");
            
            if (req->outBufferSize != sizeof(ULONG))
                return UT_FAIL;

            
            memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
            pCBiosArguments->AX = OEMFunction;
            pCBiosArguments->BX = QueryBiosInfo;
            
            
            CInt10(pRDC->pCBIOSExtension);
            if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
            {
                ((UTDEVICELIST *) outBufPtr)->MERGE = (USHORT)pCBiosArguments->SI;
                return (UT_SUCCESS);
            }
            else
            {
                return(UT_FAIL);
            }
            break;

        case UT_QUERY_CONNECT_DEVICE: 
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Query Device Connect\n");
            
            if (req->outBufferSize != sizeof(ULONG))
                return UT_FAIL;
            
            memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
            pCBiosArguments->AX = OEMFunction;
            pCBiosArguments->BX = QueryDeviceConnectStatus;
            
            
            CInt10(pRDC->pCBIOSExtension);
            if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
            {
                ((UTDEVICELIST *) outBufPtr)->MERGE = (USHORT)pCBiosArguments->BX;
                return (UT_SUCCESS);
            }
            else
            {
                return(UT_FAIL);
            }
            break;

        case UT_SET_DEVICE_ACT:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Set Active Device\n");
            
            if (req->inBufferSize != sizeof(UTDEVICELIST))
                return UT_FAIL;

             
            memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
            pCBiosArguments->AX = OEMFunction;
            pCBiosArguments->BX = SetActiveDisplayDevice;
         
            switch (inBufPtr[0] & 0xFFFF)
            {
                case 0x04: 
                    pCBiosArguments->CL = 3; 
                    break;
                case 0x01: 
                    pCBiosArguments->CL = 1; 
                    break;
                case 0x02: 
                    pCBiosArguments->CL = 2; 
                    break;
                case 0x08: 
                    pCBiosArguments->CL = 4; 
                    break;
                case 0x20: 
                    pCBiosArguments->CL = 6; 
                    break;
                default:
                    pCBiosArguments->CL = 0;
                    break;
            }
            
            ucNewDeviceID = pCBiosArguments->CL;
            
            CInt10(pRDC->pCBIOSExtension);
            if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
            {
                pRDC->DeviceInfo.ucNewDeviceID = ucNewDeviceID;
                return(UT_SUCCESS | UT_MODESET);
            }
            else
            {
                return(UT_FAIL);
            }
            break;

        case UT_QUERY_ACT_DEVICE:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Query active device\n");
            
            if (req->outBufferSize != sizeof(UCHAR) * 2)
                return UT_FAIL;

             
            memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
            pCBiosArguments->AX = OEMFunction;
            pCBiosArguments->BX = QueryDisplayPathInfo;
            
            CInt10(pRDC->pCBIOSExtension);
            if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
            {
                outBufPtr[0] = 0;
                switch ((pCBiosArguments->Ebx & 0x000F0000) >> 16)
                {
                    case 3: 
                        outBufPtr[0] |= 0x0004; 
                        break;
                    case 1: 
                        outBufPtr[0] |= 0x0001; 
                        break;
                    case 2: 
                        outBufPtr[0] |= 0x0002; 
                        break;
                    case 4: 
                        outBufPtr[0] |= 0x0008; 
                        break;
                    case 6: 
                        outBufPtr[0] |= 0x0020; 
                        break;
                    default:
                        break;
                }
                switch ((pCBiosArguments->Ecx & 0x000F0000) >> 16)
                {
                    case 3: 
                        outBufPtr[0] |= 0x0004; 
                        break;
                    case 1: 
                        outBufPtr[0] |= 0x0001; 
                        break;
                    case 2: 
                        outBufPtr[0] |= 0x0002; 
                        break;
                    case 4: 
                        outBufPtr[0] |= 0x0008; 
                        break;
                    case 6: 
                        outBufPtr[0] |= 0x0020; 
                        break;
                    default:
                        break;
                }

                return (UT_SUCCESS);
            }
            else
            {
                return (UT_NOT_IMPLEMENT);
            }
            break;

        
        case UT_QUERY_HW_INFO:
            if (req->outBufferSize != sizeof(UTHWINFO))
                return UT_FAIL;
            {
                UTHWINFO *pUTHWInfo = (UTHWINFO*)outBufPtr;
                pUTHWInfo->ulVenderID = (ULONG)VENDOR_ID(pRDC->PciInfo);
                pUTHWInfo->ulDeviceID = (ULONG)DEVICE_ID(pRDC->PciInfo);
                pUTHWInfo->ulSubSystemID = (ULONG)SUBSYS_ID(pRDC->PciInfo);
                pUTHWInfo->ulSubVenderID = (ULONG)SUBVENDOR_ID(pRDC->PciInfo);
                pUTHWInfo->ulRevision = (ULONG)CHIP_REVISION(pRDC->PciInfo);
                return (UT_SUCCESS);
                break;
            }
        case UT_QUERY_BIOS_VERSION:
            if (req->outBufferSize != sizeof(ULONG))
                return UT_FAIL;

            
            memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
            pCBiosArguments->AX = OEMFunction;
            pCBiosArguments->BX = QueryBiosInfo;
            
            CInt10(pRDC->pCBIOSExtension);
            if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
            {
                outBufPtr[0] = pCBiosArguments->Ebx;
                return(UT_SUCCESS);
            }
            else
            {
                return (UT_NOT_IMPLEMENT);
            }
            break;
            
        case UT_QUERY_MEM_INFO:
            if (req->outBufferSize != sizeof(ULONG))
                return UT_FAIL;

            outBufPtr[0] = pRDC->FbMapSize;
            return(UT_SUCCESS);
            break;
            
        case UT_QUERY_CLK_SPEED:
            if (req->outBufferSize != sizeof(CLKINFO))
                return UT_FAIL;
            {
                CLKINFO *pCLKInfo = (CLKINFO*)outBufPtr;
                
                memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
                pCBiosArguments->AX = OEMFunction;
                pCBiosArguments->BX = QueryBiosInfo;
                
                CInt10(pRDC->pCBIOSExtension);
                if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
                {
                    ULONG ulMCLK;
                    switch (pCBiosArguments->CX & 0x000F)
                    {
                        case 0:
                            ulMCLK = 266;
                            break;
                        case 1:
                            ulMCLK = 333;
                            break;
                        case 2:
                            ulMCLK = 400;
                            break;
                        case 3:
                            ulMCLK = 533;
                            break;
                    }
                    pCLKInfo->EngCLK = pCLKInfo->MemCLK = ulMCLK;
                    return(UT_SUCCESS);
                }
                else
                {
                    return (UT_NOT_IMPLEMENT);
                }
            }
            break;

        
            
        case UT_SET_GAMMA:
            {
                
                if (req->inBufferSize != sizeof(GammaTbl) ||
                    ((pGammaTbl)inBufPtr)->dwSize != MAX_CLUT_SIZE ||
                    ((1<<(pRDC->DeviceInfo.ucDeviceID-1)) != ((pGammaTbl)inBufPtr)->device))
                    return UT_FAIL;

                
                ((pGammaTbl)(inBufPtr))->pClut = (PVIDEO_CLUT) (((BYTE*)inBufPtr)+3*sizeof(DWORD));
                return VgaSetColorLookup(pScrn, ((pGammaTbl)(inBufPtr))->pClut, ((pGammaTbl)inBufPtr)->dwSize);
            }
            break;
            
        

        case UT_SET_LCD_PWM:
            if (req->inBufferSize != sizeof(UCHAR))
                return UT_FAIL;

            if (pRDC->ECChipInfo.bECExist)
            {
                return EC_SetLCDPWM(pRDC, (char *)(inBufPtr));
            };
            break;

        case UT_QUERY_LCD_PWM:
            if (req->outBufferSize != sizeof(UCHAR))
                return UT_FAIL;

            if(pRDC->ECChipInfo.bECExist)
            {
                return EC_QueryLCDPWM(pRDC, (char *)(outBufPtr));
            };
            break;

        case UT_QUERY_LCD_SIZE:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Query LCD Size\n");

            if (req->outBufferSize != sizeof(LCDINFO))
                return UT_FAIL;

            
            int iDepth = pScrn->depth;
            if (iDepth == 32) 
                iDepth = 24;
                
            
            memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
            pCBiosArguments->AX = OEMFunction;
            pCBiosArguments->BX =QueryLCDPanelSizeMode;
            pCBiosArguments->CL = (iDepth/8) - 1; 
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "Color depth is %d\n", pCBiosArguments->CL);
            
            
            CInt10(pRDC->pCBIOSExtension);
            if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
            {
                PLCDINFO pLcdPanelSize;
                pLcdPanelSize = (PLCDINFO) outBufPtr;
                pLcdPanelSize->wLCDWidth  = pCBiosArguments->Edx & 0x0000FFFF;
                pLcdPanelSize->wLCDHeight = pCBiosArguments->Edx >> 16;
                return(UT_SUCCESS);
            }
            break;
        case UT_SET_TV_INFO:
            {
                PTVINFO TVConfig;
                if (req->inBufferSize != sizeof(TVINFO))
                   return UT_FAIL;
                TVConfig = (PTVINFO)inBufPtr;

                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Set TV info ,type =%x , Connector = %x\n",TVConfig->wType,TVConfig->bConnector);
                if(!TVConfig->bConnector) 
                {
                    
                    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
                    pCBiosArguments->AX = OEMFunction;
                    pCBiosArguments->BX = QueryTVConfiguration;
                    
                    CInt10(pRDC->pCBIOSExtension);
                    TVConfig->bConnector = (pCBiosArguments->BL >> 4);
                    TVConfig->wType      = (WORD)(pCBiosArguments->BL & (0x0f));
                }
                
                memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
                pCBiosArguments->AX = OEMFunction;
                pCBiosArguments->BX = SetTVConfiguration;
                pCBiosArguments->CL= (TVConfig->bConnector<<4) | (UCHAR)(TVConfig->wType);
                
                
                
                CInt10(pRDC->pCBIOSExtension);
                if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
                {
                    return(UT_SUCCESS);
                }
                else
                {
                    return (UT_NOT_IMPLEMENT);
                }
                break;
            }
        case UT_QUERY_TV_INFO:
            if (req->outBufferSize != sizeof(TVINFO))
                return UT_FAIL;
            
            memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
            pCBiosArguments->AX = OEMFunction;
            pCBiosArguments->BX = QueryTVConfiguration;
            
            CInt10(pRDC->pCBIOSExtension);
            if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
            {
                PTVINFO TVConfig;
                TVConfig = (PTVINFO)outBufPtr;
                TVConfig->wType      = (WORD)(pCBiosArguments->BL & (0x0f));
                TVConfig->bConnector = (pCBiosArguments->BL >> 4);
                xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Query TV info Type=%x, Connector =%x\n",TVConfig->wType,TVConfig->bConnector);
                return(UT_SUCCESS);
            }else
            {
                return (UT_NOT_IMPLEMENT);
            }
            break;
            
        case UT_SET_TV_CCRS_LEVEL:
            {
                TV_Disp_Info TVDispInfo;
                if (req->inBufferSize != sizeof(UCHAR))
                    return UT_FAIL;
                TVDispInfo.ucCCRSLevel = *(UCHAR *)(inBufPtr);
                CBIOS_SetTVEncDispRegModify(pRDC, &TVDispInfo, DiffCCRS);
                return UT_SUCCESS;
            }
            break;
                
        case UT_QUERY_TV_CCRS_LEVEL:            
            if (req->outBufferSize != sizeof(UCHAR))
                return UT_FAIL;
            ucGetTV_CVBS_CCRSLevel((UCHAR *)(outBufPtr));
            return UT_SUCCESS;
            break;

        case UT_SET_TV_H_POS:
            {
                TV_Disp_Info TVDispInfo;
                if (req->inBufferSize != sizeof(UCHAR))
                    return UT_INVALID;
                TVDispInfo.ucHPosition = *((UCHAR *)inBufPtr);
                CBIOS_SetTVEncDispRegModify(pRDC, &TVDispInfo, DiffHPos);
            }
            return UT_SUCCESS;

        case UT_SET_TV_V_POS:
            {
                TV_Disp_Info TVDispInfo;
                if (req->inBufferSize != sizeof(UCHAR))
                    return UT_INVALID;
                TVDispInfo.ucVPosition = *((UCHAR *)inBufPtr);
                CBIOS_SetTVEncDispRegModify(pRDC, &TVDispInfo, DiffVPos);
            }
            return UT_SUCCESS;

        case UT_SET_TV_H_SCALER:
            {
                TV_Disp_Info TVDispInfo;
                
                int iScaler = *((int *)inBufPtr);
                if (req->inBufferSize != sizeof(int))
                    return UT_INVALID;
                if(iScaler>10)
                {
                    iScaler = 10;
                    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "RDC: IOCTL TV H Scaler level is out of range\n");
                }
                else if((iScaler<-10))
                {
                    iScaler = -10;
                    xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "RDC: IOCTL TV H Scaler level is out of range\n");
                }
                TVDispInfo.iHScaler = iScaler;
                CBIOS_SetTVEncDispRegModify(pRDC, &TVDispInfo, DiffHScaler);
            }
            return UT_SUCCESS;

        case UT_SET_TV_V_SCALER:
            {
                TV_Disp_Info TVDispInfo;
                if (req->inBufferSize != sizeof(UCHAR))
                    return UT_INVALID;
                TVDispInfo.ucVScaler = *((UCHAR *)inBufPtr);
                CBIOS_SetTVEncDispRegModify(pRDC, &TVDispInfo, DiffVScaler);
            }
            return UT_SUCCESS;

        case UT_QUERY_VIDEO_CONTRAST:            
            if (req->outBufferSize != sizeof(ULONG))
                return UT_INVALID;

            *(ULONG*)outBufPtr = pRDC->OverlayStatus.VidColorEnhance.ulScaleContrast;
            return UT_SUCCESS;            

        case UT_SET_VIDEO_CONTRAST:            
            if (req->inBufferSize != sizeof(ULONG))
                return UT_INVALID;

            pRDC->OverlayStatus.VidColorEnhance.ulScaleContrast = *(ULONG*)inBufPtr;
            SetVIDColor(pRDC);
            return UT_SUCCESS;


        case UT_QUERY_VIDEO_BRIGHT:            
            if (req->outBufferSize != sizeof(ULONG))
                return UT_INVALID;

            *(ULONG*)outBufPtr = pRDC->OverlayStatus.VidColorEnhance.ulScaleBrightness;
            return UT_SUCCESS;            

        case UT_SET_VIDEO_BRIGHT:            
            if (req->inBufferSize != sizeof(ULONG))
                return UT_INVALID;
                
            pRDC->OverlayStatus.VidColorEnhance.ulScaleBrightness = *(ULONG*)inBufPtr;
            SetVIDColor(pRDC);
            return UT_SUCCESS;

        case UT_QUERY_VIDEO_HUE:            
            if (req->outBufferSize != sizeof(ULONG))
                return UT_INVALID;

            *(ULONG*)outBufPtr = pRDC->OverlayStatus.VidColorEnhance.ulScaleHue;
            return UT_SUCCESS;            

        case UT_SET_VIDEO_HUE:            
            if (req->inBufferSize != sizeof(ULONG))
                return UT_INVALID;
                
            pRDC->OverlayStatus.VidColorEnhance.ulScaleHue = *(ULONG*)inBufPtr;
            SetVIDColor(pRDC);
            return UT_SUCCESS;


        case UT_QUERY_VIDEO_SATURATION:            
            if (req->outBufferSize != sizeof(ULONG))
                return UT_INVALID;

            *(ULONG*)outBufPtr = pRDC->OverlayStatus.VidColorEnhance.ulScaleSaturation;
            return UT_SUCCESS;            

        case UT_SET_VIDEO_SATURATION:            
            if (req->inBufferSize != sizeof(ULONG))
                return UT_INVALID;
                
            pRDC->OverlayStatus.VidColorEnhance.ulScaleSaturation = *(ULONG*)inBufPtr;
            SetVIDColor(pRDC);
            return UT_SUCCESS;

      

        case UT_QUERY_ROTATION_CAP:
            if (req->outBufferSize != sizeof(BOOL))
                return UT_FAIL;

            BOOL *bStatus;
            bStatus = (BOOL *) outBufPtr;
            *bStatus = pRDC->bRandRRotation;
            
            return UT_SUCCESS;

        case UT_SET_HDMI_UNDERSCAN_PERSENT:
            if (req->inBufferSize != sizeof(WORD))
                return UT_FAIL;

            pRDC->bHRatio = *((WORD *)inBufPtr) & 0x00FF;
            pRDC->bVRatio = (*((WORD *)inBufPtr)>>8) & 0xFF;
            return UT_SUCCESS;

        case UT_QUERY_HDMI_UNDERSCAN_PERSENT:
            if (req->outBufferSize != sizeof(WORD))
                return UT_FAIL;

            *(WORD *) outBufPtr = (pRDC->bVRatio<<8) | pRDC->bHRatio;
            return UT_SUCCESS;

        case UT_QUERY_TV_ENC_ID:
            if (req->outBufferSize != sizeof(BYTE))
                return UT_FAIL;

            *((BYTE *) outBufPtr) = Get_TVEnc_TX_ID();
            return UT_SUCCESS;

        case UT_SET_TV_PANNING_ENABLE:
            if (req->inBufferSize != sizeof(BOOL))
                return UT_FAIL;

            pRDC->bEnableTVPanning = *((BOOL *)inBufPtr);
            return UT_SUCCESS;

        case UT_QUERY_TV_PANNING_ENABLE:
            if (req->outBufferSize != sizeof(BOOL))
                return UT_FAIL;

            *(BOOL *) outBufPtr = pRDC->bEnableTVPanning;
            return UT_SUCCESS;

        case UT_QUERY_TV_DEFAULT_VALUE:
            {
                PTV_Disp_Info pTVDispInfo;
                if (req->outBufferSize != sizeof(TV_Disp_Info))
                    return UT_FAIL;
                GetFS473PositionFromVBIOS(pRDC);
                pTVDispInfo              = (PTV_Disp_Info) outBufPtr;
                pTVDispInfo->ucCCRSLevel = 0;
                pTVDispInfo->ucHPosition = pRDC->TVEncoderInfo[0].ucHPosition;
                pTVDispInfo->ucVPosition = pRDC->TVEncoderInfo[0].ucVPosition;
                pTVDispInfo->iHScaler    = 0;
                pTVDispInfo->ucVScaler   = 0;
                return UT_SUCCESS;
            }
#if 0

#endif
        default:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "RDCGFXUtilityProc: unknown command 0x%x", req->util_cmd_id);
            break;
    }

    return UT_INVALID;
}


int EC_SetLCDPWM(RDCRecPtr pRDC, char *level)
{
    xf86DrvMsgVerb(0, X_INFO, 5, "==Enter EC_SetLCDPWM() level == \n");

    
    
    
    
    if (!pRDC->ECChipInfo.bECExist)
        return FALSE;

    
    if (*level == 0)
        *level = 1;

    EC_WritePortUchar((BYTE*)0x66, 0x40);
    EC_WritePortUchar((BYTE*)0x62, (BYTE)*level);
        
    return UT_SUCCESS;
}


int EC_QueryLCDPWM(RDCRecPtr pRDC, char *level)
{
    xf86DrvMsgVerb(0, X_INFO, 5, "==Enter EC_QueryLCDPWM()== \n");

    int bSuccess = EC_ACCESS_FAIL;
    BYTE bLevel;

    if (!pRDC->ECChipInfo.bECExist)
        return FALSE;

    if(pRDC->ECChipInfo.bNewEC)
    {
        EC_WritePortUchar((BYTE*)0x66, 0x41);
        bSuccess = (int)EC_ReadPortUchar((BYTE*)0x62,&bLevel);
    }
    else
    {
        EC_WritePortUchar((BYTE*)0x66, 0x80);
        EC_WritePortUchar((BYTE*)0x62, 0x95);
        bSuccess = (int)EC_ReadPortUchar((BYTE*)0x62,&bLevel);
    }
    
    if(bSuccess == EC_ACCESS_FAIL)
    {
        *level = 0xff;
        return FALSE;
    }
    else
    {
        *level = bLevel;
        xf86DrvMsgVerb(0, X_INFO, 5, "==EC PWM Level=%d == \n", bLevel);
        return UT_SUCCESS;
    }
}

void GetSAA7105CCRSLevel(UCHAR ucI2Cport, UCHAR ucDevAddress, UCHAR *Level)
{
    UCHAR bTmpData;
    
    *Level = (bTmpData & 0x3F)>>6;
    xf86DrvMsgVerb(0, 0, 4, "==CCRS Level Get Data =%x==\n",*Level);
}

UCHAR ucGetTV_CVBS_CCRSLevel(UCHAR *Level)
{
    ScrnInfoPtr pScrn = xf86Screens[g_ScreenNumber];
    RDCRecPtr pRDC = RDCPTR(pScrn);
    BYTE bI2CPort;
    CBIOS_ARGUMENTS *pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;

    
    pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
    
    
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, ErrorLevel, " Query External Device Info \n");

    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->AX = OEMFunction;
    pCBiosArguments->BX = QueryExternalDeviceInfo;
    
    CInt10(pRDC->pCBIOSExtension);
    if(pCBiosArguments->AX == VBEFunctionCallSuccessful)
    {
        bI2CPort = (BYTE)(pCBiosArguments->Edx >> 16);
        if((pCBiosArguments->BL>>4) == TVEnc_SAA7105)
            GetSAA7105CCRSLevel(bI2CPort,0x88, Level);
    }
    xf86DrvMsgVerb(0, X_INFO, InfoLevel, "CCRSLevel Get = %x\n",*Level);
    return TRUE;
}

void GetFS473PositionFromVBIOS(RDCRecPtr pRDC)
{
    
    USHORT   wHres;
    VIDEOMODE *pVidMode = &(pRDC->VideoModeInfo);
    
    if(pRDC->bEnableTVPanning)
        wHres = pRDC->TVEncoderInfo[0].TVOut_HSize;
    else
        wHres = pVidMode->ScreenWidth;
        
    switch(wHres)    
    {
        case 640:
            if(pRDC->TVEncoderInfo[0].TVType) 
            {
                pRDC->TVEncoderInfo[0].ucHPosition = 0x1a;
                pRDC->TVEncoderInfo[0].ucVPosition = 0x2c;
            }else 
            {
                pRDC->TVEncoderInfo[0].ucHPosition = 0x5b;
                pRDC->TVEncoderInfo[0].ucVPosition = 0x18;
            }
        break;
        case 720:
            if(pRDC->TVEncoderInfo[0].TVType) 
            {
                pRDC->TVEncoderInfo[0].ucHPosition = 0x30;
                pRDC->TVEncoderInfo[0].ucVPosition = 0x20;
            }else 
            {
                pRDC->TVEncoderInfo[0].ucHPosition = 0x20;
                pRDC->TVEncoderInfo[0].ucVPosition = 0x16;
            }
        break;
        case 800:        
            if(pRDC->TVEncoderInfo[0].TVType) 
            {
                pRDC->TVEncoderInfo[0].ucHPosition = 0x41;
                pRDC->TVEncoderInfo[0].ucVPosition = 0x43;
            }else 
            {
                pRDC->TVEncoderInfo[0].ucHPosition = 0x8b;
                pRDC->TVEncoderInfo[0].ucVPosition = 0x1b;
            }
        break;
        case 1024:        
            if(pRDC->TVEncoderInfo[0].TVType) 
            {
                pRDC->TVEncoderInfo[0].ucHPosition = 0x42;
                pRDC->TVEncoderInfo[0].ucVPosition = 0x57;
            }else 
            {
                pRDC->TVEncoderInfo[0].ucHPosition = 0x32;
                pRDC->TVEncoderInfo[0].ucVPosition = 0x26;
            }
        break;
        default:
            pRDC->TVEncoderInfo[0].ucHPosition = 0xa0;
            pRDC->TVEncoderInfo[0].ucVPosition = 0x2a;
        break;
    }
    pRDC->TVEncoderInfo[0].iHScaler = 0;
}

void CBIOS_SetTVEncDispRegModify(RDCRecPtr pRDC, PTV_Disp_Info pTVDispInfo, BYTE bChange)
{
    CBIOS_ARGUMENTS *pCBiosArguments = pRDC->pCBIOSExtension->pCBiosArguments;
    CBTV_Disp_Info   CBTVDispInfo;

    memcpy(&CBTVDispInfo,pTVDispInfo,sizeof(TV_Disp_Info));
    CBTVDispInfo.bTVType           = pRDC->TVEncoderInfo[TV_1].TVType;
    CBTVDispInfo.bChange           = bChange;
    CBTVDispInfo.bEnableHPanning   = pRDC->TVEncoderInfo[TV_1].bEnableHPanning;
    CBTVDispInfo.wTVOut_HSize      = pRDC->TVEncoderInfo[TV_1].TVOut_HSize;
    CBTVDispInfo.wModeHres         = pRDC->TVEncoderInfo[TV_1].ModeHSize;    

    
    memset(pCBiosArguments, 0, sizeof(CBIOS_ARGUMENTS));
    pCBiosArguments->AX  = OEMFunction;
    pCBiosArguments->BX  = SetTVFunction;
    pCBiosArguments->Ecx = (DWORD)(&CBTVDispInfo);
    
    
    CInt10(pRDC->pCBIOSExtension);
    
    if(bChange & DiffCCRS)
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "TV CCRS Level =%x\n", CBTVDispInfo.ucCCRSLevel);
    if(bChange & DiffVScaler)
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "TV  V Scaler  =%x\n", CBTVDispInfo.ucVScaler);
    if(bChange & DiffHPos)
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "TV H Position =%x\n", CBTVDispInfo.ucHPosition);
    if(bChange & DiffVPos)
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "TV V Position =%x\n", CBTVDispInfo.ucVPosition);
    if(bChange & DiffHScaler)
        xf86DrvMsgVerb(0, X_INFO, DefaultLevel, "TV  H Scaler  =%d\n", CBTVDispInfo.iHScaler);
}

