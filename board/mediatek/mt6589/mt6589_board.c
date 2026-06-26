// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2026 Akari Tsuyukusa
 */

#include <linux/string.h>
#include <linux/io.h>
#include <asm/global_data.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
//	memset((void *)0xbf600000, 0x33, 0x400000);
	printf("Reloc Addr: %08lx\n", gd->relocaddr);
	puts("Hello World!\n");

	return 0;
}
