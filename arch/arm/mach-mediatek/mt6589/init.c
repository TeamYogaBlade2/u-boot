// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2026 Akari Tsuyukusa
 */

#include <linux/sizes.h>
#include <linux/io.h>
#include <config.h>
#include <init.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

int dram_init(void)
{
	/* TODO */
	gd->ram_size = get_ram_size((long *)CFG_SYS_SDRAM_BASE, SZ_1G);
	return 0;
}

int print_cpuinfo(void)
{
	printf("CPU:   MediaTek MT6589\n");
	return 0;
}

void reset_cpu(void)
{
	/* TODO */
	return;
}
