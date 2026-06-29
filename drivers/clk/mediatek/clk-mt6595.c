// SPDX-License-Identifier: GPL-2.0-only

#include <asm/io.h>
#include <dm.h>
#include <dt-bindings/clock/mediatek,mt6595-clk.h>
#include <linux/bitops.h>

#include "clk-mtk.h"

enum {
	CLK_PAD_CLK32K,
	CLK_PAD_CLK26M,
};

static const ulong ext_clock_rates[] = {
	[CLK_PAD_CLK32K] = 32000,
	[CLK_PAD_CLK26M] = 26000000,
};

#define MT6595_PLL_FMAX		(180UL * MHZ)
#define MT6595_PLL_FMIN		(2200UL * MHZ)
#define CON0_MT6595_RST_BAR	BIT(24)
#define PLL_AO			BIT(1)

#define PLL(_id, _reg, _pwr_reg, _en_mask, _flags, _pcwbits,		\
	      _pd_reg, _pd_shift, _pcw_reg, _pcw_shift) {	\
		.id = _id,						\
		.reg = _reg,						\
		.pwr_reg = _pwr_reg,					\
		.en_mask = _en_mask,					\
		.flags = _flags,					\
		.rst_bar_mask = CON0_MT6595_RST_BAR,			\
		.fmin = MT6595_PLL_FMIN,				\
		.fmax = MT6595_PLL_FMAX,				\
		.pcwbits = _pcwbits,					\
		.pd_reg = _pd_reg,					\
		.pd_shift = _pd_shift,					\
		.pcw_reg = _pcw_reg,					\
		.pcw_shift = _pcw_shift,				\
	}

static const struct mtk_pll_data apmixed_plls[] = {
	PLL(CLK_APMIXED_ARMCA17PLL, 0x200, 0x20c, 0, PLL_AO,
	    21, 0x204, 24, 0x204, 0),
	PLL(CLK_APMIXED_ARMCA7PLL, 0x210, 0x21c, 0, PLL_AO,
	    21, 0x214, 24, 0x214, 0),
	PLL(CLK_APMIXED_MAINPLL, 0x220, 0x22c, 0xf0000100, CLK_PLL_HAVE_RST_BAR, 21,
	    0x220, 4, 0x224, 0),
	PLL(CLK_APMIXED_UNIVPLL, 0x230, 0x23c, 0xfe000000, CLK_PLL_HAVE_RST_BAR, 7,
	    0x230, 4, 0x234, 14),
	PLL(CLK_APMIXED_MMPLL, 0x240, 0x24c, 0, 0, 21, 0x244, 24, 0x244, 0),
	PLL(CLK_APMIXED_MSDCPLL, 0x250, 0x25c, 0, 0, 21, 0x250, 4, 0x254, 0),
	PLL(CLK_APMIXED_VENCPLL, 0x260, 0x26c, 0, 0, 21, 0x260, 4, 0x264, 0),
	PLL(CLK_APMIXED_TVDPLL, 0x270, 0x27c, 0, 0, 21, 0x270, 4, 0x274, 0),
	PLL(CLK_APMIXED_MPLL, 0x280, 0x28c, 0, 0, 21, 0x280, 4, 0x284, 0),
	PLL(CLK_APMIXED_VCODECPLL, 0x290, 0x29c, 0, 0, 21, 0x290, 4, 0x294, 0),
	PLL(CLK_APMIXED_APLL1, 0x2a0, 0x2b0, 0, 0, 31, 0x2a0, 4, 0x2a4, 0),
	PLL(CLK_APMIXED_APLL2, 0x2b4, 0x2c4, 0, 0, 31, 0x2b4, 4, 0x2b8, 0),
};

static const struct mtk_clk_tree mt6595_apmixedsys_clk_tree = {
	.pll_parent = EXT_PARENT(CLK_PAD_CLK26M),
	.ext_clk_rates = ext_clock_rates,
	.num_ext_clks = ARRAY_SIZE(ext_clock_rates),
	.plls = apmixed_plls,
	.num_plls = ARRAY_SIZE(apmixed_plls),
};

#define FIXED_CLK_CLK26M(_id, _rate)			\
	FIXED_CLK(_id, CLK_PAD_CLK26M, CLK_PARENT_EXT, _rate)

static const struct mtk_fixed_clk top_fixed_clks[] = {
	FIXED_CLK_CLK26M(CLK_TOP_DMPLL, 0),
	FIXED_CLK_CLK26M(CLK_TOP_USB_SYSPLL_125M, 125 * MHZ),
};

#define FACTOR_PLL(_id, _name, _parent, _mult, _div)	\
	FACTOR(_id, _parent, _mult, _div, CLK_PARENT_APMIXED)

#define FACTOR_TOP(_id, _name, _parent, _mult, _div)	\
	FACTOR(_id, _parent, _mult, _div, CLK_PARENT_TOPCKGEN)

static const struct mtk_fixed_factor top_divs[] = {
	FACTOR_PLL(CLK_TOP_TVDPLL_D4, "tvdpll_d4", CLK_APMIXED_TVDPLL, 1, 4),
	FACTOR_PLL(CLK_TOP_TVDPLL_D3, "tvdpll_d3", CLK_APMIXED_TVDPLL, 1, 3),

	FACTOR_PLL(CLK_TOP_ARMCA7PLL_D2, "armca7pll_d2", CLK_APMIXED_ARMCA7PLL, 1, 2),
	FACTOR_PLL(CLK_TOP_ARMCA7PLL_D3, "armca7pll_d3", CLK_APMIXED_ARMCA7PLL, 1, 3),

	FACTOR_TOP(CLK_TOP_DMPLL_D2, "dmpll_d2", CLK_TOP_DMPLL, 1, 2),
	FACTOR_TOP(CLK_TOP_DMPLL_D4, "dmpll_d4", CLK_TOP_DMPLL, 1, 4),
	FACTOR_TOP(CLK_TOP_DMPLL_D8, "dmpll_d8", CLK_TOP_DMPLL, 1, 8),
	FACTOR_TOP(CLK_TOP_DMPLL_D16, "dmpll_d16", CLK_TOP_DMPLL, 1, 16),

	FACTOR_PLL(CLK_TOP_MMPLL_D2, "mmpll_d2", CLK_APMIXED_MMPLL, 1, 2),

	FACTOR_PLL(CLK_TOP_MSDCPLL_D2, "msdcpll_d2", CLK_APMIXED_MSDCPLL, 1, 2),
	FACTOR_PLL(CLK_TOP_MSDCPLL_D4, "msdcpll_d4", CLK_APMIXED_MSDCPLL, 1, 4),

	FACTOR_PLL(CLK_TOP_SYSPLL_D2, "syspll_d2", CLK_APMIXED_MAINPLL, 1, 2),
	FACTOR_TOP(CLK_TOP_SYSPLL1_D2, "syspll1_d2", CLK_TOP_SYSPLL_D2, 1, 2),
	FACTOR_TOP(CLK_TOP_SYSPLL1_D4, "syspll1_d4", CLK_TOP_SYSPLL_D2, 1, 4),
	FACTOR_TOP(CLK_TOP_SYSPLL1_D8, "syspll1_d8", CLK_TOP_SYSPLL_D2, 1, 8),
	FACTOR_TOP(CLK_TOP_SYSPLL1_D16, "syspll1_d16", CLK_TOP_SYSPLL_D2, 1, 16),
	FACTOR_PLL(CLK_TOP_SYSPLL_D3, "syspll_d3", CLK_APMIXED_MAINPLL, 1, 3),
	FACTOR_PLL(CLK_TOP_SYSPLL2_D2, "syspll2_d2", CLK_TOP_SYSPLL_D3, 1, 2),
	FACTOR_PLL(CLK_TOP_SYSPLL2_D4, "syspll2_d4", CLK_TOP_SYSPLL_D3, 1, 4),
	FACTOR_PLL(CLK_TOP_SYSPLL_D5, "syspll_d5", CLK_APMIXED_MAINPLL, 1, 5),
	FACTOR_TOP(CLK_TOP_SYSPLL3_D2, "syspll3_d2", CLK_TOP_SYSPLL_D5, 1, 2),
	FACTOR_TOP(CLK_TOP_SYSPLL3_D4, "syspll3_d4", CLK_TOP_SYSPLL_D5, 1, 4),
	FACTOR_PLL(CLK_TOP_SYSPLL_D7, "syspll_d7", CLK_APMIXED_MAINPLL, 1, 7),
	FACTOR_TOP(CLK_TOP_SYSPLL4_D2, "syspll4_d2", CLK_TOP_SYSPLL_D7, 1, 2),
	FACTOR_TOP(CLK_TOP_SYSPLL4_D4, "syspll4_d4", CLK_TOP_SYSPLL_D7, 1, 4),

	FACTOR_TOP(CLK_TOP_TVDPLL1_D2, "tvdpll1_d2", CLK_TOP_TVDPLL_D3, 1, 2),
	FACTOR_TOP(CLK_TOP_TVDPLL1_D4, "tvdpll1_d4", CLK_TOP_TVDPLL_D3, 1, 4),
	FACTOR_TOP(CLK_TOP_TVDPLL1_D8, "tvdpll1_d8", CLK_TOP_TVDPLL_D3, 1, 8),
	FACTOR_TOP(CLK_TOP_TVDPLL1_D16, "tvdpll1_d16", CLK_TOP_TVDPLL_D3, 1, 16),

	FACTOR_PLL(CLK_TOP_UNIVPLL_D2, "univpll_d2", CLK_APMIXED_UNIVPLL, 1, 2),
	FACTOR_TOP(CLK_TOP_UNIVPLL1_D2, "univpll1_d2", CLK_TOP_UNIVPLL_D2, 1, 2),
	FACTOR_TOP(CLK_TOP_UNIVPLL1_D4, "univpll1_d4", CLK_TOP_UNIVPLL_D2, 1, 4),
	FACTOR_TOP(CLK_TOP_UNIVPLL1_D8, "univpll1_d8", CLK_TOP_UNIVPLL_D2, 1, 8),
	FACTOR_PLL(CLK_TOP_UNIVPLL_D3, "univpll_d3", CLK_APMIXED_UNIVPLL, 1, 3),
	FACTOR_TOP(CLK_TOP_UNIVPLL2_D2, "univpll2_d2", CLK_TOP_UNIVPLL_D3, 1, 2),
	FACTOR_TOP(CLK_TOP_UNIVPLL2_D4, "univpll2_d4", CLK_TOP_UNIVPLL_D3, 1, 4),
	FACTOR_TOP(CLK_TOP_UNIVPLL2_D8, "univpll2_d8", CLK_TOP_UNIVPLL_D3, 1, 8),
	FACTOR_PLL(CLK_TOP_UNIVPLL_D5, "univpll_d5", CLK_APMIXED_UNIVPLL, 1, 5),
	FACTOR_TOP(CLK_TOP_UNIVPLL3_D2, "univpll3_d2", CLK_TOP_UNIVPLL_D5, 1, 2),
	FACTOR_TOP(CLK_TOP_UNIVPLL3_D4, "univpll3_d4", CLK_TOP_UNIVPLL_D5, 1, 4),
	FACTOR_TOP(CLK_TOP_UNIVPLL3_D8, "univpll3_d8", CLK_TOP_UNIVPLL_D5, 1, 8),
	FACTOR_PLL(CLK_TOP_UNIVPLL_D7, "univpll_d7", CLK_APMIXED_UNIVPLL, 1, 7),
	FACTOR_PLL(CLK_TOP_UNIVPLL_D26, "univpll_d26", CLK_APMIXED_UNIVPLL, 1, 26),
	FACTOR_PLL(CLK_TOP_UNIVPLL_D52, "univpll_d52", CLK_APMIXED_UNIVPLL, 1, 52),

	FACTOR_PLL(CLK_TOP_VENCPLL_D2, "vencpll_d2", CLK_APMIXED_VENCPLL, 1, 2),
	FACTOR_PLL(CLK_TOP_VENCPLL_D4, "vencpll_d4", CLK_APMIXED_VENCPLL, 1, 4),
};

static const struct mtk_parent axi_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL1_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D5),
	TOP_PARENT(CLK_TOP_SYSPLL1_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL_D5),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D2),
	TOP_PARENT(CLK_TOP_DMPLL_D2),
	TOP_PARENT(CLK_TOP_DMPLL_D4)
};

static const struct mtk_parent mem_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_DMPLL),
};

static const struct mtk_parent ddrphycfg_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL1_D8)
};

static const struct mtk_parent mm_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_VENCPLL_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D3),
	TOP_PARENT(CLK_TOP_SYSPLL1_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D5),
	TOP_PARENT(CLK_TOP_SYSPLL1_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D2),
	TOP_PARENT(CLK_TOP_DMPLL_D2)
};

static const struct mtk_parent pwm_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL3_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D4),
};

static const struct mtk_parent vdec_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	APMIXED_PARENT(CLK_APMIXED_VCODECPLL),
	TOP_PARENT(CLK_TOP_TVDPLL_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL_D3),
	TOP_PARENT(CLK_TOP_VENCPLL_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D3),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D2),
	TOP_PARENT(CLK_TOP_MMPLL_D2),
	TOP_PARENT(CLK_TOP_DMPLL_D2),
	TOP_PARENT(CLK_TOP_DMPLL_D4)
};

static const struct mtk_parent venc_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	APMIXED_PARENT(CLK_APMIXED_VCODECPLL),
	TOP_PARENT(CLK_TOP_TVDPLL_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL_D3),
	TOP_PARENT(CLK_TOP_VENCPLL_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D3),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D2),
	TOP_PARENT(CLK_TOP_DMPLL_D2),
	TOP_PARENT(CLK_TOP_DMPLL_D4)
};

static const struct mtk_parent mfg_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	APMIXED_PARENT(CLK_APMIXED_MMPLL),
	TOP_PARENT(CLK_TOP_DMPLL),
	EXT_PARENT(CLK_PAD_CLK26M),
	EXT_PARENT(CLK_PAD_CLK26M),
	EXT_PARENT(CLK_PAD_CLK26M),
	EXT_PARENT(CLK_PAD_CLK26M),
	EXT_PARENT(CLK_PAD_CLK26M),
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL_D3),
	TOP_PARENT(CLK_TOP_SYSPLL1_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D5),
	TOP_PARENT(CLK_TOP_UNIVPLL_D3),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL_D5),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D2)
};

static const struct mtk_parent camtg_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL_D26),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D2),
	TOP_PARENT(CLK_TOP_SYSPLL3_D2),
	TOP_PARENT(CLK_TOP_SYSPLL3_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D4),
	TOP_PARENT(CLK_TOP_DMPLL_D8),
};

static const struct mtk_parent uart_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D8)
};

static const struct mtk_parent spi_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL3_D2),
	TOP_PARENT(CLK_TOP_SYSPLL1_D4),
	TOP_PARENT(CLK_TOP_SYSPLL4_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL3_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D8)
};

static const struct mtk_parent usb20_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D8),
	TOP_PARENT(CLK_TOP_UNIVPLL3_D4)
};

static const struct mtk_parent usb30_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL3_D2),
	TOP_PARENT(CLK_TOP_USB_SYSPLL_125M),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D4)
};

static const struct mtk_parent msdc50_0_h_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL1_D2),
	TOP_PARENT(CLK_TOP_SYSPLL2_D2),
	TOP_PARENT(CLK_TOP_SYSPLL4_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL_D5),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D4),
};

static const struct mtk_parent msdc50_0_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	APMIXED_PARENT(CLK_APMIXED_MSDCPLL),
	TOP_PARENT(CLK_TOP_MSDCPLL_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D4),
	TOP_PARENT(CLK_TOP_SYSPLL2_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D7),
	TOP_PARENT(CLK_TOP_MSDCPLL_D4),
	TOP_PARENT(CLK_TOP_VENCPLL_D4),
	TOP_PARENT(CLK_TOP_TVDPLL_D3),
	TOP_PARENT(CLK_TOP_UNIVPLL_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D2),
	APMIXED_PARENT(CLK_APMIXED_MMPLL)
};

static const struct mtk_parent msdc30_1_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D2),
	TOP_PARENT(CLK_TOP_MSDCPLL_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D4),
	TOP_PARENT(CLK_TOP_SYSPLL2_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D7),
	TOP_PARENT(CLK_TOP_UNIVPLL_D7),
	TOP_PARENT(CLK_TOP_VENCPLL_D4)
};

static const struct mtk_parent msdc30_2_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D2),
	TOP_PARENT(CLK_TOP_MSDCPLL_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D4),
	TOP_PARENT(CLK_TOP_SYSPLL2_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D7),
	TOP_PARENT(CLK_TOP_UNIVPLL_D7),
	TOP_PARENT(CLK_TOP_VENCPLL_D2)
};

static const struct mtk_parent msdc30_3_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D2),
	TOP_PARENT(CLK_TOP_MSDCPLL_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D4),
	TOP_PARENT(CLK_TOP_SYSPLL2_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D7),
	TOP_PARENT(CLK_TOP_UNIVPLL_D7),
	TOP_PARENT(CLK_TOP_VENCPLL_D4)
};

static const struct mtk_parent audio_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL3_D4),
	TOP_PARENT(CLK_TOP_SYSPLL4_D4),
	TOP_PARENT(CLK_TOP_SYSPLL1_D16)
};

static const struct mtk_parent aud_intbus_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL1_D4),
	TOP_PARENT(CLK_TOP_SYSPLL4_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL3_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D8),
	TOP_PARENT(CLK_TOP_DMPLL_D4),
	TOP_PARENT(CLK_TOP_DMPLL_D8)
};

static const struct mtk_parent pmicspi_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL1_D8),
	TOP_PARENT(CLK_TOP_SYSPLL3_D4),
	TOP_PARENT(CLK_TOP_SYSPLL1_D16),
	TOP_PARENT(CLK_TOP_UNIVPLL3_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL_D26),
	TOP_PARENT(CLK_TOP_DMPLL_D8),
	TOP_PARENT(CLK_TOP_DMPLL_D16)
};

static const struct mtk_parent scp_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL1_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL_D5),
	TOP_PARENT(CLK_TOP_SYSPLL_D5),
	TOP_PARENT(CLK_TOP_DMPLL_D2),
	TOP_PARENT(CLK_TOP_DMPLL_D4)
};

static const struct mtk_parent mjc_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL_D3),
	APMIXED_PARENT(CLK_APMIXED_VCODECPLL),
	TOP_PARENT(CLK_TOP_TVDPLL_D4),
	TOP_PARENT(CLK_TOP_VENCPLL_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D3),
	TOP_PARENT(CLK_TOP_UNIVPLL1_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D5),
	TOP_PARENT(CLK_TOP_SYSPLL1_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL_D5),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D2),
	TOP_PARENT(CLK_TOP_DMPLL),
};

static const struct mtk_parent dpi0_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_TVDPLL1_D2),
	TOP_PARENT(CLK_TOP_TVDPLL1_D4),
	EXT_PARENT(CLK_PAD_CLK26M),
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_TVDPLL1_D8),
	TOP_PARENT(CLK_TOP_TVDPLL1_D16)
};

static const struct mtk_parent irda_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D4),
	TOP_PARENT(CLK_TOP_SYSPLL2_D4),
	TOP_PARENT(CLK_TOP_DMPLL_D8)
};

static const struct mtk_parent cci400_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	APMIXED_PARENT(CLK_TOP_VENCPLL),
	TOP_PARENT(CLK_TOP_ARMCA7PLL_D2),
	TOP_PARENT(CLK_TOP_ARMCA7PLL_D3),
	TOP_PARENT(CLK_TOP_UNIVPLL_D2),
	TOP_PARENT(CLK_TOP_SYSPLL_D2),
	APMIXED_PARENT(CLK_APMIXED_MSDCPLL),
	TOP_PARENT(CLK_TOP_DMPLL),
};

static const struct mtk_parent aud_1_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	APMIXED_PARENT(CLK_APMIXED_APLL1),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D8)
};

static const struct mtk_parent aud_2_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	APMIXED_PARENT(CLK_APMIXED_APLL2),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D4),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D8)
};

static const struct mtk_parent mem_mfg_in_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	APMIXED_PARENT(CLK_APMIXED_MMPLL),
	TOP_PARENT(CLK_TOP_DMPLL),
};

static const struct mtk_parent axi_mfg_in_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	EXT_PARENT(CLK_PAD_CLK26M), /* "axi_sel" */
	TOP_PARENT(CLK_TOP_DMPLL_D2)
};

static const struct mtk_parent scam_parents[] = {
	EXT_PARENT(CLK_PAD_CLK26M),
	TOP_PARENT(CLK_TOP_SYSPLL3_D2),
	TOP_PARENT(CLK_TOP_UNIVPLL2_D4),
	TOP_PARENT(CLK_TOP_DMPLL_D4)
};

#define MUX_GATE0(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x040, _shift, _width, _gate)

#define MUX_GATE1(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x050, _shift, _width, _gate)

#define MUX_GATE2(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x060, _shift, _width, _gate)

#define MUX_GATE3(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x070, _shift, _width, _gate)
#define MUX_GATE3_NOSR(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x070, _shift, _width, _gate)

#define MUX_GATE4(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x080, _shift, _width, _gate)
#define MUX_GATE4_NOSR(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x080, _shift, _width, _gate)

#define MUX_GATE5(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x090, _shift, _width, _gate)

#define MUX_GATE6(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x0a0, _shift, _width, _gate)
#define MUX_GATE6_NOSR(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x0a0, _shift, _width, _gate)

#define MUX_GATE7(_id, _name, _parents, _shift, _width, _gate) \
	MUX_GATE(_id, _parents, 0x0b0, _shift, _width, _gate)

static const struct mtk_composite top_muxes[] = {
	MUX_GATE0(CLK_TOP_AXI_SEL, "axi_sel", axi_parents, 0, 3, 7),
	MUX_GATE0(CLK_TOP_MEM_SEL, "mem_sel", mem_parents, 8, 1, 15),
	MUX_GATE0(CLK_TOP_DDRPHYCFG_SEL, "ddrphycfg_sel", ddrphycfg_parents, 16, 1, 23),
	MUX_GATE0(CLK_TOP_MM_SEL, "mm_sel", mm_parents, 24, 4, 31),

	MUX_GATE1(CLK_TOP_PWM_SEL, "pwm_sel", pwm_parents, 0, 2, 7),
	MUX_GATE1(CLK_TOP_VDEC_SEL, "vdec_sel", vdec_parents, 8, 4, 15),
	MUX_GATE1(CLK_TOP_VENC_SEL, "venc_sel", venc_parents, 16, 4, 23),
	MUX_GATE1(CLK_TOP_MFG_SEL, "mfg_sel", mfg_parents, 24, 4, 31),

	MUX_GATE2(CLK_TOP_CAMTG_SEL, "camtg_sel", camtg_parents, 0, 3, 7),
	MUX_GATE2(CLK_TOP_UART_SEL, "uart_sel", uart_parents, 8, 1, 15),
	MUX_GATE2(CLK_TOP_SPI_SEL, "spi_sel", spi_parents, 16, 3, 23),
	MUX_GATE2(CLK_TOP_USB20_SEL, "usb20_sel", usb20_parents, 24, 2, 31),

	MUX_GATE3(CLK_TOP_USB30_SEL, "usb30_sel", usb30_parents, 0, 2, 7),
	MUX_GATE3_NOSR(CLK_TOP_MSDC50_0_H_SEL, "msdc50_0_h_sel", msdc50_0_h_parents, 8, 3, 15),
	MUX_GATE3_NOSR(CLK_TOP_MSDC50_0_SEL, "msdc50_0_sel", msdc50_0_parents, 16, 4, 23),
	MUX_GATE3_NOSR(CLK_TOP_MSDC30_1_SEL, "msdc30_1_sel", msdc30_1_parents, 24, 3, 31),

	MUX_GATE4_NOSR(CLK_TOP_MSDC30_2_SEL, "msdc30_2_sel", msdc30_2_parents, 0, 3, 7),
	MUX_GATE4_NOSR(CLK_TOP_MSDC30_3_SEL, "msdc30_3_sel", msdc30_3_parents, 8, 4, 15),
	MUX_GATE4(CLK_TOP_AUDIO_SEL, "audio_sel", audio_parents, 16, 2, 23),
	MUX_GATE4(CLK_TOP_AUD_INTBUS_SEL, "aud_intbus_sel", aud_intbus_parents, 24, 3, 31),

	MUX_GATE5(CLK_TOP_PMICSPI_SEL, "pmicspi_sel", pmicspi_parents, 0, 3, 7),
	MUX_GATE5(CLK_TOP_SCP_SEL, "scp_sel", scp_parents, 8, 3, 15),
	MUX_GATE5(CLK_TOP_MJC_SEL, "mjc_sel", mjc_parents, 24, 4, 31),

	MUX_GATE6_NOSR(CLK_TOP_DPI0_SEL, "dpi0_sel", dpi0_parents, 0, 3, 7),
	MUX_GATE6(CLK_TOP_IRDA_SEL, "irda_sel", irda_parents, 8, 2, 15),
	MUX_GATE6(CLK_TOP_CCI400_SEL, "cci400_sel", cci400_parents, 16, 3, 23),
	MUX_GATE6(CLK_TOP_AUD_1_SEL, "aud_1_sel", aud_1_parents, 24, 2, 31),

	MUX_GATE7(CLK_TOP_AUD_2_SEL, "aud_2_sel", aud_2_parents, 0, 2, 7),
	MUX_GATE7(CLK_TOP_MEM_MFG_IN_SEL, "mem_mfg_in_sel", mem_mfg_in_parents, 8, 2, 15),
	MUX_GATE7(CLK_TOP_AXI_MFG_IN_SEL, "axi_mfg_in_sel", axi_mfg_in_parents, 16, 2, 23),
	MUX_GATE7(CLK_TOP_SCAM_SEL, "scam_sel", scam_parents, 24, 2, 31)
};

static const struct mtk_clk_tree mt6595_topckgen_clk_tree = {
	.ext_clk_rates = ext_clock_rates,
	.num_ext_clks = ARRAY_SIZE(ext_clock_rates),
	.fclks = top_fixed_clks,
	.num_fclks = ARRAY_SIZE(top_fixed_clks),
	.fdivs_offs = CLK_TOP_TVDPLL_D4,
	.fdivs = top_divs,
	.num_fdivs = ARRAY_SIZE(top_divs),
	.muxes_offs = CLK_TOP_AXI_SEL,
	.muxes = top_muxes,
	.num_muxes = ARRAY_SIZE(top_muxes),
};

static const struct mtk_gate_regs peri_cg_regs = {
	.set_ofs = 0x08,
	.clr_ofs = 0x10,
	.sta_ofs = 0x18,
};

#define GATE_PERI0(_id, _name, _parent, _shift) \
	GATE_FLAGS(_id, _parent, &peri_cg_regs, _shift, \
		CLK_GATE_SETCLR | CLK_PARENT_TOPCKGEN)

#define GATE_PERI0_EXT(_id, _name, _parent, _shift)		\
	GATE_FLAGS(_id, _parent, &peri_cg_regs, _shift,	\
		CLK_GATE_SETCLR | CLK_PARENT_EXT)

static const struct mtk_gate peri_gates[] = {
	GATE_PERI0(CLK_PERI_NFI, "peri_nfi", CLK_TOP_AXI_SEL, 0),
	GATE_PERI0(CLK_PERI_THERM, "peri_therm", CLK_TOP_AXI_SEL, 1),
	GATE_PERI0(CLK_PERI_PWM1, "peri_pwm1", CLK_TOP_AXI_SEL, 2),
	GATE_PERI0(CLK_PERI_PWM2, "peri_pwm2", CLK_TOP_AXI_SEL, 3),
	GATE_PERI0(CLK_PERI_PWM3, "peri_pwm3", CLK_TOP_AXI_SEL, 4),
	GATE_PERI0(CLK_PERI_PWM4, "peri_pwm4", CLK_TOP_AXI_SEL, 5),
	GATE_PERI0(CLK_PERI_PWM5, "peri_pwm5", CLK_TOP_AXI_SEL, 6),
	GATE_PERI0(CLK_PERI_PWM6, "peri_pwm6", CLK_TOP_AXI_SEL, 7),
	GATE_PERI0(CLK_PERI_PWM7, "peri_pwm7", CLK_TOP_AXI_SEL, 8),
	GATE_PERI0(CLK_PERI_PWM, "peri_pwm", CLK_TOP_AXI_SEL, 9),
	GATE_PERI0(CLK_PERI_USB0, "peri_usb0", CLK_TOP_USB20_SEL, 10),
	GATE_PERI0(CLK_PERI_USB1, "peri_usb1", CLK_TOP_USB20_SEL, 11),
	GATE_PERI0(CLK_PERI_AP_DMA, "peri_ap_dma", CLK_TOP_AXI_SEL, 12),
	GATE_PERI0(CLK_PERI_MSDC30_0, "peri_msdc30_0", CLK_TOP_MSDC50_0_SEL, 13),
	GATE_PERI0(CLK_PERI_MSDC30_1, "peri_msdc30_1", CLK_TOP_MSDC30_1_SEL, 14),
	GATE_PERI0(CLK_PERI_MSDC30_2, "peri_msdc30_2", CLK_TOP_MSDC30_2_SEL, 15),
	GATE_PERI0(CLK_PERI_MSDC30_3, "peri_msdc30_3", CLK_TOP_MSDC30_3_SEL, 16),
	GATE_PERI0(CLK_PERI_NLI_ARB, "peri_nli_arb", CLK_TOP_AXI_SEL, 17),
	GATE_PERI0(CLK_PERI_IRDA, "peri_irda", CLK_TOP_IRDA_SEL, 18),
	GATE_PERI0(CLK_PERI_UART0, "peri_uart0", CLK_TOP_AXI_SEL, 19),
	GATE_PERI0(CLK_PERI_UART1, "peri_uart1", CLK_TOP_AXI_SEL, 20),
	GATE_PERI0(CLK_PERI_UART2, "peri_uart2", CLK_TOP_AXI_SEL, 21),
	GATE_PERI0(CLK_PERI_UART3, "peri_uart3", CLK_TOP_AXI_SEL, 22),
	GATE_PERI0(CLK_PERI_I2C0, "peri_i2c0", CLK_TOP_AXI_SEL, 23),
	GATE_PERI0(CLK_PERI_I2C1, "peri_i2c1", CLK_TOP_AXI_SEL, 24),
	GATE_PERI0(CLK_PERI_I2C2, "peri_i2c2", CLK_TOP_AXI_SEL, 25),
	GATE_PERI0(CLK_PERI_I2C3, "peri_i2c3", CLK_TOP_AXI_SEL, 26),
	GATE_PERI0(CLK_PERI_I2C4, "peri_i2c4", CLK_TOP_AXI_SEL, 27),
	GATE_PERI0_EXT(CLK_PERI_AUXADC, "peri_auxadc", CLK_PAD_CLK26M, 28),
	GATE_PERI0(CLK_PERI_SPI0, "peri_spi0", CLK_TOP_SPI_SEL, 29)
};

static const struct mtk_clk_tree mt6595_pericfg_clk_tree = {
	.ext_clk_rates = ext_clock_rates,
	.num_ext_clks = ARRAY_SIZE(ext_clock_rates),
};

static int mt6595_apmixedsys_probe(struct udevice *dev)
{
	return mtk_common_clk_init(dev, &mt6595_apmixedsys_clk_tree);
}

static int mt6595_topckgen_probe(struct udevice *dev)
{
	return mtk_common_clk_init(dev, &mt6595_topckgen_clk_tree);
}

static int mt6595_pericfg_probe(struct udevice *dev)
{
	return mtk_common_clk_gate_init(dev, &mt6595_pericfg_clk_tree, peri_gates,
					ARRAY_SIZE(peri_gates), 0);
}

static const struct udevice_id mt6595_apmixed[] = {
	{ .compatible = "mediatek,mt6595-apmixedsys" },
	{ }
};

static const struct udevice_id mt6595_topckgen_compat[] = {
	{ .compatible = "mediatek,mt6595-topckgen" },
	{ }
};

static const struct udevice_id mt6595_pericfg_compat[] = {
	{ .compatible = "mediatek,mt6595-pericfg" },
	{ }
};

U_BOOT_DRIVER(mtk_clk_apmixedsys) = {
	.name = "mt6595-apmixedsys",
	.id = UCLASS_CLK,
	.of_match = mt6595_apmixed,
	.probe = mt6595_apmixedsys_probe,
	.priv_auto = sizeof(struct mtk_clk_priv),
	.ops = &mtk_clk_apmixedsys_ops,
	.flags = DM_FLAG_PRE_RELOC,
};

U_BOOT_DRIVER(mtk_clk_topckgen) = {
	.name = "mt6595-topckgen",
	.id = UCLASS_CLK,
	.of_match = mt6595_topckgen_compat,
	.probe = mt6595_topckgen_probe,
	.priv_auto = sizeof(struct mtk_clk_priv),
	.ops = &mtk_clk_topckgen_ops,
	.flags = DM_FLAG_PRE_RELOC,
};

U_BOOT_DRIVER(mtk_clk_pericfg) = {
	.name = "mt6595-pericfg",
	.id = UCLASS_CLK,
	.of_match = mt6595_pericfg_compat,
	.probe = mt6595_pericfg_probe,
	.priv_auto = sizeof(struct mtk_cg_priv),
	.ops = &mtk_clk_gate_ops,
	.flags = DM_FLAG_PRE_RELOC,
};
