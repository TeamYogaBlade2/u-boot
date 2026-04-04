// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * Copyright (C) 2026 Akari Tsuyukusa
 */

#include <linux/string.h>
#include <linux/io.h>

int board_init(void)
{
//	memset((void *)0xbf600000, 0x33, 0x400000);
	printf("Hello World!");

	return 0;
}
