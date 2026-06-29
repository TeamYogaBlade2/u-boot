// SPDX-License-Identifier: GPL-2.0
#include <clk.h>
#include <config.h>
#include <dm.h>
#include <init.h>
#include <log.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <linux/printk.h>
#include <dt-bindings/clock/mediatek,mt6595-clk.h>

DECLARE_GLOBAL_DATA_PTR;

int board_init(void)
{
	return 0;
}

int board_late_init(void)
{
	struct udevice *dev;
	int ret;

	ret = uclass_get_device_by_name(UCLASS_CLK, "mt6595-apmixedsys", &dev);
	if (ret) {
		ret = uclass_get_device(UCLASS_CLK, 0, &dev);
		if (ret) {
			printf("%s:%d: %d\n", __func__, __LINE__, ret);
			return ret;
		}
	}

	struct clk clk = { .id = CLK_APMIXED_ARMCA17PLL, .dev = dev };

	ret = clk_enable(&clk);
	if (ret) {
		printf("%s:%d: %d\n", __func__, __LINE__, ret);
		return ret;
	}

	printf("PLL value (lsb MUST BE 1): 0x%x\n", readl(0x10209200));
	printf("PLL pwr con value (lsb MUST BE 1): 0x%x\n", readl(0x1020920c));

	printf("enabled Cortex A17 PLL\n");

	return 0;
}
