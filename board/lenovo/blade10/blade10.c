// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018 MediaTek Inc.
 * Copyright (C) 2025 akku
 */

#include <config.h>
#include <mmc.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

// MT7623
int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = CFG_SYS_SDRAM_BASE + 0x100;

	return 0;
}
