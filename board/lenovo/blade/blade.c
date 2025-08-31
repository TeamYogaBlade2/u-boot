// SPDX-License-Identifier: GPL-2.0-or-later

#include <asm/global_data.h>
#include <config.h>
#include <linux/sizes.h>
#include <init.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)CFG_SYS_SDRAM_BASE, SZ_1G);
	return 0;
}
