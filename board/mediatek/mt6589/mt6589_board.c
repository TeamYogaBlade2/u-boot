// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2026 Akari Tsuyukusa
 */

#include <cpu_func.h>
#include <linux/string.h>
#include <linux/io.h>

int board_init(void)
{
	memset((void *)0xbf600000, 0x33, 0x400000);
	flush_dcache_range(0xbf600000, 0xbfa00000);

	printf("Hello World!");
	flush_dcache_range(0xbf600000, 0xbfa00000);

	return 0;
}
