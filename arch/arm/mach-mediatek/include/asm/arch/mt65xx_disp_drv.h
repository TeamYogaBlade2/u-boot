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

#ifndef __mt6573_DISP_DRV_H__
#define __mt6573_DISP_DRV_H__

#include <asm/arch/mt65xx_typedefs.h>
#include <asm/arch/disp_drv.h>
#include "lcm_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

// ---------------------------------------------------------------------------
//  UBoot Display Utility Macros
// ---------------------------------------------------------------------------

#define AS_UINT32(x)    (*(unsigned int *)(x))
#define AS_UINT16(x)    (*(unsigned short *)(x))

#define mt6573_DBG  1

#if mt6573_DBG

#define ASSERT(expr)                                                    \
    do {                                                                \
        if(!(expr)) {                                                   \
    		printf("<ASSERT> %s:line %d %dx\n",                          \
    		    __FILE__,__LINE__,(int)(#expr));                        \
    		while (1);                                                  \
        }                                                               \
    } while(0);

#define NOT_IMPLEMENTED()                                               \
    do {                                                                \
        printf("<NOT_IMPLEMENTED> %s:line %d\n", __FILE__, __LINE__);   \
        while (1);                                                      \
    } while(0);

#else   // !mt6573_DBG

    #define ASSERT(expr)
    #define NOT_IMPLEMENTED()

#endif  // end of mt6573_DBG

// ---------------------------------------------------------------------------

#define NOT_REFERENCED(x) {(x) = (x);}
#define msleep(x)    mdelay(x)

// ---------------------------------------------------------------------------
typedef enum {
   DISP_VERTICAL_PROG_BAR = 0,
   DISP_HORIZONTAL_PROG_BAR,
} DISP_PROG_BAR_DIRECT;



   

// ---------------------------------------------------------------------------
//  UBoot Display Export Functions
// ---------------------------------------------------------------------------

UINT32 mt65xx_disp_get_vram_size(void);
void   mt65xx_disp_init(void *lcdbase);
void   mt65xx_disp_power(BOOL on);
void   mt65xx_disp_update(UINT32 x, UINT32 y, UINT32 width, UINT32 height);
void   mt65xx_disp_wait_idle(void);
UINT32 mt65xx_disp_get_lcd_time(void);

BOOL DISP_DetectDevice(void);

// -- Utility Functions for Customization --

void*  mt65xx_get_logo_db_addr(void);
void*  mt65xx_get_fb_addr(void);
UINT32 mt65xx_get_fb_size(void);

void mt65xx_disp_fill_rect(UINT32 left, UINT32 top,
                           UINT32 right, UINT32 bottom,
                           UINT32 color);

void mt65xx_disp_draw_prog_bar(DISP_PROG_BAR_DIRECT direct,
                               UINT32 left, UINT32 top,
                               UINT32 right, UINT32 bottom,
                               UINT32 fgColor, UINT32 bgColor,
                               UINT32 start_div, UINT32 total_div,
                               UINT32 occupied_div);

#ifdef __cplusplus
}
#endif

#endif // __mt65xx_DISP_DRV_H__
