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

#include <sys/ipc.h>
#include <sys/shm.h>

#include "xf86.h"
#include "xf86_OSproc.h"
#include "xf86Resources.h"
#include "xf86RAC.h"
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


#include "xaa.h"
#include "xaarop.h"


#include "xf86Cursor.h"


#include "rdc.h"

#include "X11/Xproto.h"     
#include "extnsionst.h" 
#include "dixstruct.h"  

#include "rdc_extension.h"

//#include <sys/ipc.h>
//#include <sys/shm.h>

CARD32 *inBufPtr, *outBufPtr ; 


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
        swaps(&rep.sequenceNumber, n);
        swapl(&rep.length, n);
        swaps(&rep.majorVersion, n);
        swaps(&rep.minorVersion, n);
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
    inBufPtr = (CARD32 *) shmat(shm_in_id, NULL, 0) ;
    outBufPtr = (CARD32 *) shmat(shm_out_id, NULL, 0) ;
    
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

    CBIOS_Extension CBIOSExtension;
    CBIOS_ARGUMENTS CBiosArguments;

    CBIOSExtension.pCBiosArguments = &CBiosArguments;
    CBIOSExtension.IOAddress = (USHORT)(pRDC->RelocateIO) ;
    CBIOSExtension.VideoVirtualAddress = (ULONG)(pRDC->FBVirtualAddr);
    
            
    ULONG   ulResult;

    switch(req->util_cmd_id)
    {
        
        
        
        case UT_QUERY_SUPPORT_DEVICE: 
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Query Device Support");
            
            if (req->outBufferSize != sizeof(ULONG))
                return UT_FAIL;

            CBiosArguments.reg.x.AX = OEMFunction;
            CBiosArguments.reg.x.BX = QueryBiosInfo;
            if (CInt10(&CBIOSExtension))
            {
                ((UTDEVICELIST *) outBufPtr)->MERGE = (USHORT)CBiosArguments.reg.x.SI;
                return (UT_SUCCESS);
            }
            else
            {
                return(UT_FAIL);
            }
            break;

        case UT_QUERY_CONNECT_DEVICE: 
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Query Device Connect");
            
            if (req->outBufferSize != sizeof(ULONG))
                return UT_FAIL;
                
            CBiosArguments.reg.x.AX = OEMFunction;
            CBiosArguments.reg.x.BX = QueryDeviceConnectStatus;
            if (CInt10(&CBIOSExtension))
            {
                ((UTDEVICELIST *) outBufPtr)->MERGE = (USHORT)CBiosArguments.reg.x.BX;
                return (UT_SUCCESS);
            }
            else
            {
                return(UT_FAIL);
            }
            break;

        case UT_SET_DEVICE_ACT:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Set Active Device");
            
            if (req->inBufferSize != sizeof(UTDEVICELIST))
                return UT_FAIL;

            CBiosArguments.reg.x.AX = OEMFunction;
            CBiosArguments.reg.x.BX = SetActiveDisplayDevice;
         
            switch (inBufPtr[0] & 0xFFFF)
            {
                case 0x04: 
                    CBiosArguments.reg.lh.CL = 3; 
                    break;
                case 0x01: 
                    CBiosArguments.reg.lh.CL = 1; 
                    break;
                case 0x02: 
                    CBiosArguments.reg.lh.CL = 2; 
                    break;
                case 0x20: 
                    CBiosArguments.reg.lh.CL = 6; 
                    break;
                default:
                    CBiosArguments.reg.lh.CL = 0;
                    break;
            }

            if (CInt10(&CBIOSExtension))
            {
                return(UT_SUCCESS | UT_MODESET);
            }
            else
            {
                return(UT_FAIL);
            }
            break;

        case UT_QUERY_ACT_DEVICE:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Query active device");
            
            if (req->outBufferSize != sizeof(UCHAR) * 2)
                return UT_FAIL;

            CBiosArguments.reg.x.AX = OEMFunction;
            CBiosArguments.reg.x.BX = QueryDisplayPathInfo;
            if (CInt10(&CBIOSExtension))
            {
                outBufPtr[0] = 0;
                switch ((CBiosArguments.reg.ex.EBX & 0x000F0000) >> 16)
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
                    case 6: 
                        outBufPtr[0] |= 0x0020; 
                        break;
                    default:
                        break;
                }
                switch ((CBiosArguments.reg.ex.ECX & 0x000F0000) >> 16)
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

            CBiosArguments.reg.x.AX = OEMFunction;
            CBiosArguments.reg.x.BX = QueryBiosInfo;
            if (CInt10(&CBIOSExtension))
            {
                outBufPtr[0] = CBiosArguments.reg.ex.EBX;
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
                CBiosArguments.reg.x.AX = OEMFunction;
                CBiosArguments.reg.x.BX = QueryBiosInfo;
                if (CInt10(&CBIOSExtension))
                {
                    ULONG ulMCLK;
                    switch (CBiosArguments.reg.x.CX & 0x000F)
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
                    ((pGammaTbl)inBufPtr)->dwSize != MAX_CLUT_SIZE)
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
            }
            else
            {
               return BIOS_SetLCDPWM(&CBIOSExtension, (char *)(inBufPtr));
            };
            break;

        case UT_QUERY_LCD_PWM:
            if (req->outBufferSize != sizeof(UCHAR))
                return UT_FAIL;

            if(pRDC->ECChipInfo.bECExist)
            {
                return EC_QueryLCDPWM(pRDC, (char *)(outBufPtr));
            }
            else
            {
                return BIOS_QueryLCDPWM(&CBIOSExtension, (char *)(outBufPtr));
            };
            break;

        case UT_QUERY_LCD_SIZE:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "DrvEscape: Query LCD Size");

            if (req->outBufferSize != sizeof(LCDINFO))
                return UT_FAIL;

            
            int iDepth = pScrn->depth;
            if (iDepth == 32) 
                iDepth = 24;
                
            CBiosArguments.reg.x.AX = OEMFunction;
            CBiosArguments.reg.x.BX = QueryLCDPanelSizeMode;
            CBiosArguments.reg.lh.CL = (iDepth/8) - 1; 
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "Color depth is %d\n", CBiosArguments.reg.lh.CL);
            if (CInt10(&CBIOSExtension))
            {
                PLCDINFO pLcdPanelSize;
                pLcdPanelSize = (PLCDINFO) outBufPtr;
                pLcdPanelSize->wLCDWidth = CBiosArguments.reg.ex.EDX & 0x0000FFFF;
                pLcdPanelSize->wLCDHeight = CBiosArguments.reg.ex.EDX >> 16;
                return(UT_SUCCESS);
            }
            break;
#if 0

#endif
        default:
            xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 4, "RDCGFXUtilityProc: unknown command 0x%x", req->util_cmd_id);
            break;
    }

    return UT_INVALID;
}


int EC_SetLCDPWM(ScrnInfoPtr pScrn, char *level)
{
    RDCRecPtr pRDC = RDCPTR(pScrn);
    int iLevel;

    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Enter EC_SetLCDPWM()== \n");

    
    
    
    
    if (!pRDC->ECChipInfo.bECExist)
        return FALSE;

    iLevel = *level;

    
    if (iLevel == 0)
        iLevel = 1;
    
    EC_WritePortUchar((BYTE*)0x66, 0x40);
    EC_WritePortUchar((BYTE*)0x62, (BYTE)iLevel);
        
    return UT_SUCCESS;
}


int EC_QueryLCDPWM(ScrnInfoPtr pScrn, char *level)
{
    xf86DrvMsgVerb(pScrn->scrnIndex, X_INFO, 5, "==Enter EC_QueryLCDPWM()== \n");

    RDCRecPtr pRDC = RDCPTR(pScrn);
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
        return UT_SUCCESS;
    }
}


int BIOS_SetLCDPWM(CBIOS_Extension *CBIOSExtension, char *level)
{
    xf86DrvMsgVerb(0, X_INFO, 5, "==Enter BIOS_SetLCDPWM()== \n");

    CBIOS_ARGUMENTS *BiosArguments = CBIOSExtension->pCBiosArguments;
    ULONG   Status = UT_FAIL;
   
    BiosArguments->reg.x.AX = OEMFunction;
    BiosArguments->reg.x.BX = SetLCDPWMLevel;
    BiosArguments->reg.lh.CL = *level;
    if (CInt10(CBIOSExtension))
    {
        return(UT_SUCCESS);
    }

    xf86DrvMsgVerb(0, X_INFO, 5, "==Leave BIOS_SetLCDPWM()== \n");
    
    return  Status;
}


int BIOS_QueryLCDPWM(CBIOS_Extension *CBIOSExtension, char *level)
{
    xf86DrvMsgVerb(0, X_INFO, 5, "==Enter BIOS_QueryLCDPWM()== \n");

    CBIOS_ARGUMENTS *BiosArguments = CBIOSExtension->pCBiosArguments;
    ULONG Status = UT_FAIL;
    
    BiosArguments->reg.x.AX = OEMFunction;
    BiosArguments->reg.x.BX = QueryLCDPWMLevel;
    if (CInt10(CBIOSExtension))
    {
        *(UCHAR*)level = BiosArguments->reg.lh.BL;
        return(UT_SUCCESS);
    }

    xf86DrvMsgVerb(0, X_INFO, 5, "==Leave BIOS_QueryLCDPWM()== \n");
    
    return Status;
}
