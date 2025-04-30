// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2018 MediaTek Inc.
 * Copyright (C) 2025 akku
 */

#include <config.h>
#include <init.h>
#include <asm/global_data.h>
#include <linux/io.h>
#include <linux/sizes.h>
#include <asm/arch/misc.h>

int print_cpuinfo(void)
{
	void __iomem *chipid;
	u32 swver;

	chipid = ioremap(VER_BASE, VER_SIZE);
	swver = readl(chipid + APSW_VER);

	printf("CPU:   MediaTek MT6589 E%d\n", (swver & 0xf) + 1);

	return 0;
}
