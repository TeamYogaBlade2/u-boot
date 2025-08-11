// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2025 akku
 * Author: akku <akkun11.open@gmail.com>
 */

#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

void reset_cpu(void)
{
	/* TODO */
}

int dram_init(void)
{
	/* TODO */
	return 0;
}

int print_cpuinfo(void)
{
	printf("CPU:   MediaTek MT6589");

	return 0;
}
