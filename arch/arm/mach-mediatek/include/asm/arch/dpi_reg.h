/* Copyright Statement:
 *
 * This software/firmware and related documentation ("MediaTek Software") are
 * protected under relevant copyright laws. The information contained herein
 * is confidential and proprietary to MediaTek Inc. and/or its licensors.
 * Without the prior written permission of MediaTek inc. and/or its licensors,
 * any reproduction, modification, use or disclosure of MediaTek Software,
 * and information contained herein, in whole or in part, shall be strictly prohibited.
 */
/* MediaTek Inc. (C) 2010. All rights reserved.
 *
 * BY OPENING THIS FILE, RECEIVER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
 * THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
 * RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO RECEIVER ON
 * AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
 * NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
 * SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
 * SUPPLIED WITH THE MEDIATEK SOFTWARE, AND RECEIVER AGREES TO LOOK ONLY TO SUCH
 * THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. RECEIVER EXPRESSLY ACKNOWLEDGES
 * THAT IT IS RECEIVER'S SOLE RESPONSIBILITY TO OBTAIN FROM ANY THIRD PARTY ALL PROPER LICENSES
 * CONTAINED IN MEDIATEK SOFTWARE. MEDIATEK SHALL ALSO NOT BE RESPONSIBLE FOR ANY MEDIATEK
 * SOFTWARE RELEASES MADE TO RECEIVER'S SPECIFICATION OR TO CONFORM TO A PARTICULAR
 * STANDARD OR OPEN FORUM. RECEIVER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND
 * CUMULATIVE LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
 * AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
 * OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY RECEIVER TO
 * MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
 *
 * The following software/firmware and/or related documentation ("MediaTek Software")
 * have been modified by MediaTek Inc. All revisions are subject to any receiver's
 * applicable license agreements with MediaTek Inc.
 */

/*****************************************************************************
*  Copyright Statement:
*  --------------------
*  This software is protected by Copyright and the information contained
*  herein is confidential. The software may not be copied and the information
*  contained herein may not be used or disclosed except with the written
*  permission of MediaTek Inc. (C) 2008
*
*  BY OPENING THIS FILE, BUYER HEREBY UNEQUIVOCALLY ACKNOWLEDGES AND AGREES
*  THAT THE SOFTWARE/FIRMWARE AND ITS DOCUMENTATIONS ("MEDIATEK SOFTWARE")
*  RECEIVED FROM MEDIATEK AND/OR ITS REPRESENTATIVES ARE PROVIDED TO BUYER ON
*  AN "AS-IS" BASIS ONLY. MEDIATEK EXPRESSLY DISCLAIMS ANY AND ALL WARRANTIES,
*  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF
*  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE OR NONINFRINGEMENT.
*  NEITHER DOES MEDIATEK PROVIDE ANY WARRANTY WHATSOEVER WITH RESPECT TO THE
*  SOFTWARE OF ANY THIRD PARTY WHICH MAY BE USED BY, INCORPORATED IN, OR
*  SUPPLIED WITH THE MEDIATEK SOFTWARE, AND BUYER AGREES TO LOOK ONLY TO SUCH
*  THIRD PARTY FOR ANY WARRANTY CLAIM RELATING THERETO. MEDIATEK SHALL ALSO
*  NOT BE RESPONSIBLE FOR ANY MEDIATEK SOFTWARE RELEASES MADE TO BUYER'S
*  SPECIFICATION OR TO CONFORM TO A PARTICULAR STANDARD OR OPEN FORUM.
*
*  BUYER'S SOLE AND EXCLUSIVE REMEDY AND MEDIATEK'S ENTIRE AND CUMULATIVE
*  LIABILITY WITH RESPECT TO THE MEDIATEK SOFTWARE RELEASED HEREUNDER WILL BE,
*  AT MEDIATEK'S OPTION, TO REVISE OR REPLACE THE MEDIATEK SOFTWARE AT ISSUE,
*  OR REFUND ANY SOFTWARE LICENSE FEES OR SERVICE CHARGE PAID BY BUYER TO
*  MEDIATEK FOR SUCH MEDIATEK SOFTWARE AT ISSUE.
*
*  THE TRANSACTION CONTEMPLATED HEREUNDER SHALL BE CONSTRUED IN ACCORDANCE
*  WITH THE LAWS OF THE STATE OF CALIFORNIA, USA, EXCLUDING ITS CONFLICT OF
*  LAWS PRINCIPLES.  ANY DISPUTES, CONTROVERSIES OR CLAIMS ARISING THEREOF AND
*  RELATED THERETO SHALL BE SETTLED BY ARBITRATION IN SAN FRANCISCO, CA, UNDER
*  THE RULES OF THE INTERNATIONAL CHAMBER OF COMMERCE (ICC).
*
*****************************************************************************/

#ifndef __DPI_REG_H__
#define __DPI_REG_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
    unsigned EN         : 1;
	unsigned rsv_1      : 31;
}DPI_REG_EN, *PDPI_REG_EN;

typedef struct
{
    unsigned DPI_OEN_OFF    : 1;
    unsigned DPI_BG_EN      : 1;
	unsigned INTL_EN		: 1;
	unsigned TDFP_EN		: 1;
	unsigned CLPF_EN		: 1;
	unsigned YUV422_EN		: 1;
	unsigned YCMUX_EN		: 1;
	unsigned CSC_EN			: 1;
	unsigned EMBSYNC_EN		: 1;
	unsigned rsv_9          : 7;
    unsigned INRBSWAP		: 1;
	unsigned rsv_17         :15;
} DPI_REG_CNTL, *PDPI_REG_CNTL;


typedef struct
{
    unsigned VSYNC          : 1;
    unsigned rsv_1          : 31;
} DPI_REG_INTERRUPT, *PDPI_REG_INTERRUPT;


typedef struct
{
    UINT16 WIDTH;
    UINT16 HEIGHT;
} DPI_REG_SIZE, *PDPI_REG_SIZE;


typedef struct
{
    unsigned DIVISOR  : 5;
    unsigned rsv_5    : 3;
    unsigned DUTY     : 5;
    unsigned rsv_13   : 18;
    unsigned POLARITY : 1;
} DPI_REG_CLKCNTL, *PDPI_REG_CLKCNTL;

typedef struct
{
    unsigned DP_START      : 1;
    unsigned TGEN_START    : 1;
    unsigned OUT_EN        : 1;
    unsigned rsv_3         : 29;
} DPI_REG_STATUS, *PDPI_REG_STATUS;


typedef struct
{
	unsigned OFIX_EN         : 1;
    unsigned rsv_1           : 30;
	unsigned DPI_OEN_ON          : 1;
} DPI_REG_TMODE, *PDPI_REG_TMODE;

typedef struct
{
    unsigned HPW       : 8;
    unsigned HBP       : 8;
    unsigned HFP       : 8;
    unsigned HSYNC_POL : 1;
    unsigned DE_POL    : 1;
    unsigned rsv_26    : 6;
} DPI_REG_TGEN_HCNTL, *PDPI_REG_TGEN_HCNTL;

typedef struct
{
    unsigned HBP       : 12;
	unsigned rsv_12    : 4;
    unsigned HFP       : 12;
    unsigned rsv_28    : 4;
} DPI_REG_TGEN_HPORCH, *PDPI_REG_TGEN_HPORCH;

typedef struct
{
    unsigned VPW_LODD  : 8;
	unsigned VPW_HALF_LODD  : 1;
    unsigned rsv_9    : 23;
} DPI_REG_TGEN_VWIDTH_LODD, *PDPI_REG_TGEN_VWIDTH_LODD;

typedef struct
{
    unsigned VBP_LODD  : 8;
	unsigned VBP_HALF_LODD  : 1;
    unsigned rsv_9    : 7;
    unsigned VFP_LODD  : 8;
	unsigned VFP_HALF_LODD  : 1;
    unsigned rsv_25    : 7;
} DPI_REG_TGEN_VPORCH_LODD, *PDPI_REG_TGEN_VPORCH_LODD;

typedef struct
{
    unsigned VPW       : 8;
    unsigned VBP       : 8;
    unsigned VFP       : 8;
    unsigned VSYNC_POL : 1;
    unsigned rsv_25    : 7;
} DPI_REG_TGEN_VCNTL, *PDPI_REG_TGEN_VCNTL;

typedef struct
{
    unsigned BG_RIGHT  : 11;
	unsigned rsv_11    : 5;
    unsigned BG_LEFT   : 11;
    unsigned rsv_27    : 5;
} DPI_REG_BG_HCNTL, *PDPI_REG_BG_HCNTL;


typedef struct
{
    unsigned BG_BOT   : 10;
	unsigned rsv_10   : 6;
    unsigned BG_TOP   : 10;
    unsigned rsv_26   : 6;
} DPI_REG_BG_VCNTL, *PDPI_REG_BG_VCNTL;


typedef struct
{
    unsigned BG_B     : 8;
	unsigned BG_G     : 8;
    unsigned BG_R     : 8;
    unsigned rsv_24   : 8;
} DPI_REG_BG_COLOR, *PDPI_REG_BG_COLOR;


typedef struct
{
    DPI_REG_EN        DPI_EN;           // 0000
    UINT32   		  DPI_RST;			//0004
    DPI_REG_INTERRUPT INT_ENABLE;       // 0008
    DPI_REG_INTERRUPT INT_STATUS;       // 000C
    DPI_REG_CNTL      CNTL;             // 0010

	DPI_REG_CLKCNTL   CLK_CNTL; 		// 0014

    DPI_REG_SIZE      SIZE;             // 0018

	UINT32            TGEN_HWIDTH;      // 001C
    DPI_REG_TGEN_HPORCH TGEN_HPORCH;    // 0020

	DPI_REG_TGEN_VWIDTH_LODD TGEN_VWIDTH_LODD;	// 0024
    DPI_REG_TGEN_VPORCH_LODD TGEN_VPORCH_LODD;	// 0028

	UINT32 			   rsv_002C[9];
	
    DPI_REG_BG_HCNTL   BG_HCNTL;        // 0050  
    DPI_REG_BG_VCNTL   BG_VCNTL;        // 0054  
    DPI_REG_BG_COLOR   BG_COLOR;        // 0058  
} volatile DPI_REGS, *PDPI_REGS;

#ifdef __cplusplus
}
#endif

#endif // __DPI_REG_H__

