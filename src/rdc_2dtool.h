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




#define BITBLT                          0x01      
#define TRANSPARENTBLT                  0x02      
#define TEXTOUT                         0x03      
#define LINE_TO                         0x04      
#define FILL_PATH                       0x05      
#define STROKE_PATH                     0x06      
#define DDBLT                           0x07      
#define GRADIENT_FILL                   0x08      
#define ALPHA_BLENDING                  0x09      
#define STRETCH_BLT                     0x0A      
#define SOURCE_IN_QUEUE                 0x80      


#define    ENG_CAP_Sync                         0x00000001
#define    ENG_CAP_ScreenToScreenCopy           0x00000002
#define    ENG_CAP_SolidFill                    0x00000004
#define    ENG_CAP_SolidLine                    0x00000008
#define    ENG_CAP_DashedLine                   0x00000010
#define    ENG_CAP_Mono8x8PatternFill           0x00000020
#define    ENG_CAP_Color8x8PatternFill          0x00000040
#define    ENG_CAP_CPUToScreenColorExpand       0x00000080
#define    ENG_CAP_ScreenToScreenColorExpand    0x00000100
#define    ENG_CAP_Clipping                     0x00000200    
#define    ENG_CAP_2D   (ENG_CAP_Sync | ENG_CAP_ScreenToScreenCopy | ENG_CAP_SolidFill |    \
                        ENG_CAP_SolidLine | ENG_CAP_DashedLine |                \
                        ENG_CAP_Mono8x8PatternFill | ENG_CAP_Color8x8PatternFill |        \
                        ENG_CAP_Clipping)

#define    ENG_CAP_VIDEO_DISP                   0x00000400
#define    ENG_CAP_VIDEO_POST                   0x00000800
#define    ENG_CAP_HWC_MMIO                     0x00001000
#define    ENG_CAP_CR_SUPPORT                   0x00002000
#define    ENG_CAP_HWROTATE                     0x00004000
#define    ENG_CAP_EXTENDFLIPADDRS              0x00008000
#define    ENG_CAPS_ADV_ROT                     0x00010000


#define    VM_CMD_QUEUE             0
#define    VM_CMD_MMIO              2

#define    CMD_QUEUE_SIZE_256K      0x00040000
#define    CMD_QUEUE_SIZE_512K      0x00080000
#define    CMD_QUEUE_SIZE_1M        0x00100000
#define    CMD_QUEUE_SIZE_2M        0x00200000
#define    CMD_QUEUE_SIZE_4M        0x00400000


#define     CR_CTRL                     0x400
#define     CRCTRL_ENABLE               BIT0
#define     CRCTRL_RESET                BIT1
#define     CRCTRL_VPOSTMMIO            BIT2
#define     CRCTRL_DMAMMIO              BIT3
#define     CRCTRL_VDISPMMIO            BIT4
#define     CRCTRL_READPORT_DATABACK    BIT7
#define     CRCTRL_THRESHOLD            0x20
#define     CR_BUFFER_START             0x404
#define     CR_BUFFER_WRITEPORT         0x408
#define     CR_BUFFER_READPORT          0x40C
#define     CR_BUFFER_END               0x410
#define     CR_ENG_STATUS               0x414
#define     CRENG_CRRESET               BIT11
#define     CRENG_2DIDLE                BIT12
#define     CRENG_DMAIDLE               BIT13
#define     CRENG_VPOSTIDEL             BIT14
#define     CRENG_CRIDLE                BIT15



#define    PKT_NULL_CMD             0x00009561

#define    PKT_SINGLE_LENGTH        8
#define    PKT_SINGLE_CMD_HEADER    0x00009562
#define    PKT_DMA_CMD_HEADER       0x0000956E
#define    PKT_VIDEO_CMD_HEADER     0x0000956F

typedef struct  _PKT_SC
{
    ULONG    PKT_SC_dwHeader;
    ULONG    PKT_SC_dwData[1];
    
} PKT_SC, *PPKT_SC;


#define    MAX_SRC_X                0x7FF
#define    MAX_SRC_Y                0x7FF
#define    MAX_DST_X                0x7FF
#define    MAX_DST_Y                0x7FF

#define    MASK_SRC_PITCH           0x1FFF
#define    MASK_DST_PITCH           0x1FFF
#define    MASK_DST_HEIGHT          0x7FF
#define    MASK_SRC_X               0xFFF
#define    MASK_SRC_Y               0xFFF
#define    MASK_DST_X               0xFFF
#define    MASK_DST_Y               0xFFF
#define    MASK_RECT_WIDTH          0x7FF
#define    MASK_RECT_HEIGHT         0x7FF
#define MASK_CLIP                   0xFFF
#define MASK_1ST_FLIP_BASE          0x1FFFFFF8


#define MASK_LINE_X                 0xFFF   
#define MASK_LINE_Y                 0xFFF
#define MASK_LINE_ERR               0x3FFFFF        
#define MASK_LINE_WIDTH             0x7FF        
#define MASK_LINE_K1                0x3FFFFF           
#define MASK_LINE_K2                0x3FFFFF
#define MASK_M2010LINE_X            0xFFF   
#define MASK_M2010LINE_Y            0xFFF

#define MAX_PATReg_Size             256

  

#define MMIOREG_SRC_BASE        (pRDC->MMIOVirtualAddr + 0x8000)                
#define MMIOREG_SRC_PITCH       (pRDC->MMIOVirtualAddr + 0x8004)
#define MMIOREG_DST_BASE        (pRDC->MMIOVirtualAddr + 0x8008)
#define MMIOREG_DST_PITCH       (pRDC->MMIOVirtualAddr + 0x800C)
#define MMIOREG_DST_XY          (pRDC->MMIOVirtualAddr + 0x8010)
#define MMIOREG_SRC_XY          (pRDC->MMIOVirtualAddr + 0x8014)
#define MMIOREG_RECT_XY         (pRDC->MMIOVirtualAddr + 0x8018)
#define MMIOREG_FG              (pRDC->MMIOVirtualAddr + 0x801C)
#define MMIOREG_BG              (pRDC->MMIOVirtualAddr + 0x8020)
#define MMIOREG_FG_SRC          (pRDC->MMIOVirtualAddr + 0x8024)
#define MMIOREG_BG_SRC          (pRDC->MMIOVirtualAddr + 0x8028)
#define MMIOREG_MONO1           (pRDC->MMIOVirtualAddr + 0x802C)
#define MMIOREG_MONO2           (pRDC->MMIOVirtualAddr + 0x8030)
#define MMIOREG_CLIP1           (pRDC->MMIOVirtualAddr + 0x8034)
#define MMIOREG_CLIP2           (pRDC->MMIOVirtualAddr + 0x8038)
#define MMIOREG_CMD             (pRDC->MMIOVirtualAddr + 0x803C)
#define MMIOREG_1ST_FLIP        (pRDC->MMIOVirtualAddr + 0x8040)
#define MMIOREG_1ST_ADV_FLIP    (pRDC->MMIOVirtualAddr + 0x8048)
#define MMIOREG_CMDQ_SET        (pRDC->MMIOVirtualAddr + 0x8044)
#define MMIOREG_PAT             (pRDC->MMIOVirtualAddr + 0x8100)    
#define MMIOREG_2ST_FLIP        (pRDC->MMIOVirtualAddr + 0x8054)


#define MMIOREG_LINE_XY     (pRDC->MMIOVirtualAddr + 0x8010)  
#define MMIOREG_LINE_Err    (pRDC->MMIOVirtualAddr + 0x8014)  
#define MMIOREG_LINE_WIDTH  (pRDC->MMIOVirtualAddr + 0x8018)  
#define MMIOREG_LINE_K1     (pRDC->MMIOVirtualAddr + 0x8024)  
#define MMIOREG_LINE_K2     (pRDC->MMIOVirtualAddr + 0x8028)  
#define MMIOREG_LINE_STYLE1 (pRDC->MMIOVirtualAddr + 0x802C)  
#define MMIOREG_LINE_STYLE2 (pRDC->MMIOVirtualAddr + 0x8030)  
#define MMIOREG_LINE_XY2    (pRDC->MMIOVirtualAddr + 0x8014)
#define MMIOREG_LINE_NUMBER (pRDC->MMIOVirtualAddr + 0x8018)  
#define MMIOREG_2D_FENCE    (pRDC->MMIOVirtualAddr + 0x8058)
#define MMIOREG_STRETCH_CTRL1   (pRDC->MMIOVirtualAddr + 0x8064)
#define MMIOREG_STRETCH_CTRL2   (pRDC->MMIOVirtualAddr + 0x8068)
#define MMIOREG_STRETCH_CTRL3   (pRDC->MMIOVirtualAddr + 0x806C)
#define MMIOREG_FUNCTION_ENABLE (pRDC->MMIOVirtualAddr + 0x8088)


#define CMDQREG_SRC_BASE    (0x00 << 24)                       
#define CMDQREG_SRC_PITCH   (0x01 << 24)
#define CMDQREG_DST_BASE    (0x02 << 24)
#define CMDQREG_DST_PITCH   (0x03 << 24)
#define CMDQREG_DST_XY      (0x04 << 24)
#define CMDQREG_SRC_XY      (0x05 << 24)
#define CMDQREG_RECT_XY     (0x06 << 24)
#define CMDQREG_FG          (0x07 << 24)
#define CMDQREG_BG          (0x08 << 24)
#define CMDQREG_FG_SRC      (0x09 << 24)
#define CMDQREG_BG_SRC      (0x0A << 24)
#define CMDQREG_MONO1       (0x0B << 24)
#define CMDQREG_MONO2       (0x0C << 24)
#define CMDQREG_CLIP1       (0x0D << 24)
#define CMDQREG_CLIP2       (0x0E << 24)
#define CMDQREG_CMD         (0x0F << 24)
#define CMDQREG_2D_IDLE_COUNTER   (0x24 << 24)
#define CMDQREG_PAT         (0x40 << 24)
#define CMDQREQ_2DFENCE     (0x16 << 24)
#define CMDQREG_STRETCH_CTRL1   (0x19 << 24)
#define CMDQREG_STRETCH_CTRL2   (0x1A << 24)
#define CMDQREG_STRETCH_CTRL3   (0x1B << 24)
#define CMDQREG_FUNCTION_ENABLE (0x22 << 24)

#define CMDQREG_LINE_XY     (0x04 << 24)
#define CMDQREG_LINE_Err    (0x05 << 24)  
#define CMDQREG_LINE_WIDTH  (0x06 << 24)    
#define CMDQREG_LINE_K1     (0x09 << 24) 
#define CMDQREG_LINE_K2     (0x0A << 24)
#define CMDQREG_LINE_STYLE1 (0x0B << 24) 
#define CMDQREG_LINE_STYLE2 (0x0C << 24)
#define CMDQREG_LINE_XY2    (0x05 << 24)
#define CMDQREG_LINE_NUMBER (0x06 << 24)    


#define CMD_BITBLT              0x00000000
#define CMD_LINEDRAW            0x00000001
#define CMD_COLOREXP            0x00000002
#define CMD_ENHCOLOREXP         0x00000003
#define CMD_TRANSPARENTBLT      0x00000004
#define CMD_MASK                0x00000007
                  
#define CMD_DISABLE_CLIP        0x00000000   
#define CMD_ENABLE_CLIP         0x00000008

#define CMD_COLOR_08            0x00000000
#define CMD_COLOR_16            0x00000010
#define CMD_COLOR_32            0x00000020

#define CMD_SRC_SIQ             0x00000040

#define CMD_TRANSPARENT         0x00000080

#define CMD_PAT_FGCOLOR         0x00000000
#define CMD_PAT_MONOMASK        0x00010000
#define CMD_PAT_PATREG          0x00020000

#define CMD_OPAQUE              0x00000000
#define CMD_FONT_TRANSPARENT    0x00040000

#define CMD_X_INC               0x00000000    
#define CMD_X_DEC               0x00200000

#define CMD_Y_INC               0x00000000    
#define CMD_Y_DEC               0x00100000

#define CMD_NT_LINE             0x00000000
#define CMD_NORMAL_LINE         0x00400000

#define CMD_DRAW_LAST_PIXEL     0x00000000
#define CMD_NOT_DRAW_LAST_PIXEL 0x00800000

#define CMD_DISABLE_LINE_STYLE  0x00000000
#define CMD_ENABLE_LINE_STYLE   0x40000000

#define CMD_RESET_STYLE_COUNTER 0x80000000
#define CMD_NOT_RESET_STYLE_COUNTER 0x00000000

#define BURST_FORCE_CMD         0x80000000


#define CMD_ENABLE_1STFLIP      0x80000000


#define LINEPARAM_XM            0x00000001
#define LINEPARAM_X_DEC         0x00000002
#define LINEPARAM_Y_DEC         0x00000004

typedef struct _LINEPARAM {
    USHORT  dsLineX;
    USHORT  dsLineY;
    USHORT  dsLineWidth;
    ULONG   dwErrorTerm;
    ULONG   dwK1Term;
    ULONG   dwK2Term;
    ULONG   dwLineAttributes;    
} LINEPARAM, *PLINEPARAM;

typedef struct {
    LONG    X1;
    LONG    Y1;
    LONG    X2;
    LONG    Y2;    
} _LINEInfo;



#define RDCSetupSRCBase_MMIO(base) \
      { \
        do { \
           *(ULONG *)(MMIOREG_SRC_BASE) = (ULONG) (base); \
        } while (*(volatile ULONG *)(MMIOREG_SRC_BASE) != (ULONG) (base)); \
      }
#define RDCSetupSRCPitch_MMIO(pitch) \
      { \
        do { \
           *(ULONG *)(MMIOREG_SRC_PITCH) = (ULONG)(pitch << 16); \
        } while (*(volatile ULONG *)(MMIOREG_SRC_PITCH) != (ULONG)(pitch << 16)); \
      }
#define RDCSetupDSTBase_MMIO(base) \
      { \
        do { \
           *(ULONG *)(MMIOREG_DST_BASE) = (ULONG)(base); \
        } while (*(volatile ULONG *)(MMIOREG_DST_BASE) != (ULONG)(base)); \
      }      
#define RDCSetupDSTPitchHeight_MMIO(pitch, height) \
      { \
        ULONG dstpitch; \
        dstpitch = (ULONG)((pitch << 16) + ((height) & MASK_DST_HEIGHT)); \
        do { \
           *(ULONG *)(MMIOREG_DST_PITCH) = dstpitch; \
        } while (*(volatile ULONG *)(MMIOREG_DST_PITCH) != dstpitch); \
      }      
#define RDCSetupDSTXY_MMIO(x, y) \
      { \
        ULONG dstxy; \
        dstxy = (ULONG)(((x & MASK_DST_X) << 16) + (y & MASK_DST_Y)); \
        do { \
           *(ULONG *)(MMIOREG_DST_XY) = dstxy; \
        } while (*(volatile ULONG *)(MMIOREG_DST_XY) != dstxy); \
      }           
#define RDCSetupSRCXY_MMIO(x, y) \
      { \
        ULONG srcxy; \
        srcxy = (ULONG)(((x & MASK_SRC_X) << 16) + (y & MASK_SRC_Y)); \
        do { \
           *(ULONG *)(MMIOREG_SRC_XY) = srcxy; \
        } while (*(volatile ULONG *)(MMIOREG_SRC_XY) != srcxy); \
      }             
#define RDCSetupRECTXY_MMIO(x, y) \
      { \
        ULONG rectxy; \
        rectxy = (ULONG)(((x & MASK_RECT_WIDTH) << 16) + (y & MASK_RECT_WIDTH)); \
        do { \
           *(ULONG *)(MMIOREG_RECT_XY) = rectxy; \
        } while (*(volatile ULONG *)(MMIOREG_RECT_XY) != rectxy); \
      }  
#define RDCSetupFG_MMIO(color) \
      { \
        do { \
           *(ULONG *)(MMIOREG_FG) = (ULONG)(color); \
        } while (*(volatile ULONG *)(MMIOREG_FG) != (ULONG)(color)); \
      } 
#define RDCSetupBG_MMIO(color) \
      { \
        do { \
           *(ULONG *)(MMIOREG_BG) = (ULONG)(color); \
        } while (*(volatile ULONG *)(MMIOREG_BG) != (ULONG)(color)); \
      }
#define RDCSetupMONO1_MMIO(pat) \
      { \
        do { \
          *(ULONG *)(MMIOREG_MONO1) = (ULONG)(pat); \
        } while (*(volatile ULONG *)(MMIOREG_MONO1) != (ULONG)(pat)); \
      } 
#define RDCSetupMONO2_MMIO(pat) \
      { \
        do { \
          *(ULONG *)(MMIOREG_MONO2) = (ULONG)(pat); \
        } while (*(volatile ULONG *)(MMIOREG_MONO2) != (ULONG)(pat)); \
      }
#define RDCSetupCLIP1_MMIO(left, top) \
      { \
       ULONG clip1; \
       clip1 = (ULONG)(((left & MASK_CLIP) << 16) + (top & MASK_CLIP)); \
       do { \
          *(ULONG *)(MMIOREG_CLIP1) = clip1; \
       } while (*(volatile ULONG *)(MMIOREG_CLIP1) != clip1); \
      } 
#define RDCSetupCLIP2_MMIO(right, bottom) \
      { \
       ULONG clip2; \
       clip2 = (ULONG)(((right & MASK_CLIP) << 16) + (bottom & MASK_CLIP)); \
       do { \
          *(ULONG *)(MMIOREG_CLIP2) = clip2; \
       } while (*(volatile ULONG *)(MMIOREG_CLIP2) != clip2); \
      }                                                                                               
#define RDCSetupCMDReg_MMIO(reg) \
      { \
        *(ULONG *)(MMIOREG_CMD) = (ULONG)(reg);    \
      }
#define RDCSetupPatReg_MMIO(patreg, pat) \
      { \
       do { \
          *(ULONG *)(MMIOREG_PAT + patreg*4) = (ULONG)(pat); \
       } while (*(volatile ULONG *)(MMIOREG_PAT + patreg*4) != (ULONG)(pat)); \
      }      
                                

#define RDCSetupLineXMErrTerm_MMIO(xm, err) \
      { \
        ULONG lineerr; \
        lineerr = (ULONG)((xm << 24) + (err & MASK_LINE_ERR)); \
        do { \
           *(ULONG *)(MMIOREG_LINE_Err) = lineerr; \
        } while (*(volatile ULONG *)(MMIOREG_LINE_Err) != lineerr); \
      }      
#define RDCSetupLineWidth_MMIO(width) \
      { \
        ULONG linewidth; \
        linewidth = (ULONG)((width & MASK_LINE_WIDTH) << 16); \
        do { \
          *(ULONG *)(MMIOREG_LINE_WIDTH) = linewidth; \
        } while (*(volatile ULONG *)(MMIOREG_LINE_WIDTH) != linewidth); \
      }
#define RDCSetupLineK1Term_MMIO(err) \
      { \
        do { \
          *(ULONG *)(MMIOREG_LINE_K1) = (ULONG)(err & MASK_LINE_K1); \
        } while (*(volatile ULONG *)(MMIOREG_LINE_K1) != (ULONG)(err & MASK_LINE_K1)); \
      }            
#define RDCSetupLineK2Term_MMIO(err) \
      { \
        do { \
           *(ULONG *)(MMIOREG_LINE_K2) = (ULONG)(err & MASK_LINE_K2); \
        } while (*(volatile ULONG *)(MMIOREG_LINE_K2) != (ULONG)(err & MASK_LINE_K2)); \
      }
#define RDCSetupLineStyle1_MMIO(pat) \
      { \
        do { \
           *(ULONG *)(MMIOREG_LINE_STYLE1) = (ULONG)(pat); \
        } while (*(volatile ULONG *)(MMIOREG_LINE_STYLE1) != (ULONG)(pat)); \
      } 
#define RDCSetupLineStyle2_MMIO(pat) \
      { \
        do { \
          *(ULONG *)(MMIOREG_LINE_STYLE2) = (ULONG)(pat); \
        } while (*(volatile ULONG *)(MMIOREG_LINE_STYLE2) != (ULONG)(pat)); \
      }

#define RDCSetupLineXY_MMIO(x, y) \
      { \
        ULONG linexy; \
        linexy = (ULONG)(((x & MASK_M2010LINE_X) << 16) + (y & MASK_M2010LINE_Y)); \
        do { \
           *(ULONG *)(MMIOREG_LINE_XY) = linexy; \
        } while (*(volatile ULONG *)(MMIOREG_LINE_XY) != linexy); \
      }
#define RDCSetupLineXY2_MMIO(x, y) \
      { \
        ULONG linexy; \
        linexy = (ULONG)(((x & MASK_M2010LINE_X) << 16) + (y & MASK_M2010LINE_Y)); \
        do { \
           *(ULONG *)(MMIOREG_LINE_XY2) = linexy; \
        } while (*(volatile ULONG *)(MMIOREG_LINE_XY2) != linexy); \
      }
#define RDCSetupLineNumber_MMIO(no) \
      { \
        do { \
           *(ULONG *)(MMIOREG_LINE_NUMBER) = (ULONG) no; \
        } while (*(volatile ULONG *)(MMIOREG_LINE_NUMBER) != (ULONG) no); \
      }

 
#define mUpdateWritePointer *(ULONG *) (pRDC->CMDQInfo.pjWritePort) = (pRDC->CMDQInfo.ulWritePointer >>3)


#define RDCSetupSRCBase(addr, base) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_SRC_BASE);     \
        addr->PKT_SC_dwData[0] = (ULONG)(base);                    \
      }
#define RDCSetupSRCPitch(addr, pitch) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_SRC_PITCH);     \
        addr->PKT_SC_dwData[0] = (ULONG)(pitch << 16);                    \
      }
#define RDCSetupDSTBase(addr, base) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_DST_BASE);     \
        addr->PKT_SC_dwData[0] = (ULONG)(base);                    \
      }      
#define RDCSetupDSTPitchHeight(addr, pitch, height) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_DST_PITCH);     \
        addr->PKT_SC_dwData[0] = (ULONG)((pitch << 16) + ((height) & MASK_DST_HEIGHT));                    \
      }      
#define RDCSetupDSTXY(addr, x, y) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_DST_XY);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((x & MASK_DST_X) << 16) + (y & MASK_DST_Y));                    \
      }           
#define RDCSetupSRCXY(addr, x, y) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_SRC_XY);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((x & MASK_SRC_X) << 16) + (y & MASK_SRC_Y));                    \
      }             
#define RDCSetupRECTXY(addr, x, y) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_RECT_XY);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((x & MASK_RECT_WIDTH) << 16) + (y & MASK_RECT_WIDTH));                    \
      }  
#define RDCSetupFG(addr, color) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_FG);     \
        addr->PKT_SC_dwData[0] = (ULONG)(color);                    \
      }
#define RDCSetupBG(addr, color) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_BG);     \
        addr->PKT_SC_dwData[0] = (ULONG)(color);                    \
      }
#define RDCSetupMONO1(addr, pat) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_MONO1);     \
        addr->PKT_SC_dwData[0] = (ULONG)(pat);                \
      }            
#define RDCSetupMONO2(addr, pat) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_MONO2);     \
        addr->PKT_SC_dwData[0] = (ULONG)(pat);                \
      }     
#define RDCSetupCLIP1(addr, left, top) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_CLIP1);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((left & MASK_CLIP) << 16) + (top & MASK_CLIP));    \
      }            
#define RDCSetupCLIP2(addr, right, bottom) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_CLIP2);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((right & MASK_CLIP) << 16) + (bottom & MASK_CLIP));    \
      }                                                                                                    
#define RDCSetupCMDReg(addr, reg) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_CMD);     \
        addr->PKT_SC_dwData[0] = (ULONG)(reg);                    \
      }
#define RDCSetup2DIdleCounter(addr, reg) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_2D_IDLE_COUNTER);     \
        addr->PKT_SC_dwData[0] = (ULONG)(reg);                \
      } 
#define RDCSetupPatReg(addr, patreg, pat) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + (CMDQREG_PAT + (patreg << 24)));     \
        addr->PKT_SC_dwData[0] = (ULONG)(pat);                \
      }    
#define RDCSetupScaleCtrl1(addr, srcWidth, srcHeight) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_STRETCH_CTRL1);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((srcWidth) << 16) | (srcHeight));             \
      }    
#define RDCSetupScaleCtrl2(addr, horScaleFactor, horScaleFunc) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_STRETCH_CTRL2);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((horScaleFactor) << 4) | ((horScaleFunc) << 1));             \
      }    
#define RDCSetupScaleCtrl3(addr, verScaleFactor, verScaleFunc) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_STRETCH_CTRL3);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((verScaleFactor) << 4) | ((verScaleFunc) << 1));             \
      }    
#define RDCSetupFunctionEnable(addr, reg) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_FUNCTION_ENABLE);     \
        addr->PKT_SC_dwData[0] = (ULONG)(reg);                    \
      }


#define RDCSetupLineStyle1(addr, pat) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_LINE_STYLE1);     \
        addr->PKT_SC_dwData[0] = (ULONG)(pat);                \
      }            
#define RDCSetupLineStyle2(addr, pat) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_LINE_STYLE2);     \
        addr->PKT_SC_dwData[0] = (ULONG)(pat);                \
      }     

#define RDCSetupLineXY(addr, x, y) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_LINE_XY);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((x & MASK_M2010LINE_X) << 16) + (y & MASK_M2010LINE_Y));                    \
      }
#define RDCSetupLineXY2(addr, x, y) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_LINE_XY2);     \
        addr->PKT_SC_dwData[0] = (ULONG)(((x & MASK_M2010LINE_X) << 16) + (y & MASK_M2010LINE_Y));                    \
      }
#define RDCSetupLineNumber(addr, no) \
      { \
        addr->PKT_SC_dwHeader  = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREG_LINE_NUMBER);     \
        addr->PKT_SC_dwData[0] = (ULONG)(no);                    \
      }
#define RDCSetup2DFenceID(addr, no) \
      { \
        addr->PKT_SC_dwHeader   = (ULONG)(PKT_SINGLE_CMD_HEADER + CMDQREQ_2DFENCE); \
        addr->PKT_SC_dwData[0] = (ULONG)(no);   \
      }        

#include "rdc_2dtool_proto.h"
