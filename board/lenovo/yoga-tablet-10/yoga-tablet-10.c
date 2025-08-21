// SPDX-License-Identifier: GPL-2.0-or-later

#include <asm/global_data.h>
#include <config.h>
#include <linux/sizes.h>
#include <init.h>

DECLARE_GLOBAL_DATA_PTR;

/* DeviceTreeから取得したフレームバッファ情報 */
#define FB_ADDR     0xbf600000
#define FB_WIDTH    1280
#define FB_HEIGHT   800
#define FB_STRIDE   (1280 * 2) /* r5g6b5形式のため、1ピクセルあたり2バイト */
#define FB_SIZE     (FB_STRIDE * FB_HEIGHT)

/*
 * フレームバッファを単色で塗りつぶす関数
 * color: 16-bit RGB565形式の色値
 */
void fill_framebuffer_with_color(unsigned short color) {
    /* フレームバッファの物理アドレスを仮想アドレスにマップ（必要に応じて） */
    unsigned short *fb_ptr = (unsigned short *)FB_ADDR;

    /* 画面全体を塗りつぶす */
    for (int y = 0; y < FB_HEIGHT; y++) {
        unsigned short *row_ptr = fb_ptr + (y * (FB_STRIDE / 2));
        for (int x = 0; x < FB_WIDTH; x++) {
            row_ptr[x] = color;
        }
    }
}

int board_init(void)
{
	fill_framebuffer_with_color(0x001F);
	return 0;
}

int dram_init(void)
{
	gd->ram_size = get_ram_size((long *)CFG_SYS_SDRAM_BASE, SZ_1G);
	return 0;
}
