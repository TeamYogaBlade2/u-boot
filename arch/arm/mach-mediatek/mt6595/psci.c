#include "psci.h"
#include <asm/io.h>
#include <config.h>
#include <asm/gic.h>
#include <asm/psci.h>
#include <asm/secure.h>

extern void psci_cpu_entry(void);

static const struct mtk_boot_info __secure_data mt6595_boot_info = {
	.jump_reg = 0x34,
	.core_keys = { 0x534c4131, 0x4c415332, 0x41534c33, 0x534c4134,
		       0x4c415335, 0x41534c36, 0x534c4137 },
	.core_regs = { 0x38, 0x38, 0x38, 0x3c, 0x3c, 0x3c, 0x3c },
};

bool __secure mt6595_is_supported(void)
{
	u32 sw_ver = readl(DEVINFO_BASE + DEVINFO_SW_VERSION);

	/* 1 actually means SW_VER_02 */
	return sw_ver == 1;
}

u32 __secure mpidr_to_linear_cpu(u32 mpidr)
{
	u32 cluster, cpu;

	cluster = (mpidr >> 8) & 0xf;
	cpu = (mpidr & 0x3);
	return (cluster << 2) | cpu;
}

void __secure psci_gpt_udelay(u32 usec) {
	// 13 MHz
	u32 ticks_to_wait = usec * 13;
	u32 start = readl(GPT_BASE + GPT_CNT_REG2);

	while ((readl(GPT_BASE + GPT_CNT_REG2) - start) < ticks_to_wait) {
		cpu_relax();
	}
}

s32 __secure psci_features(u32 function_id, u32 psci_fid)
{
	switch (psci_fid) {
	case ARM_PSCI_0_2_FN_PSCI_VERSION:
	case ARM_PSCI_0_2_FN_CPU_OFF:
	case ARM_PSCI_0_2_FN_CPU_ON:
	case ARM_PSCI_0_2_FN_SYSTEM_OFF:
	case ARM_PSCI_0_2_FN_SYSTEM_RESET:
		return 0x0;
	default:
		return ARM_PSCI_RET_NI;
	}
}

u32 __secure psci_version(void)
{
	return ARM_PSCI_VER_1_0;
}

int __secure mtk_spm_topaxi_prot_disable(u32 bit)
{
	u32 status;

	clrbits_le32(INFRACFG_AO_BASE + INFRACFG_TOPAXI_PROT_EN, bit);

	do {
		status = readl(INFRACFG_AO_BASE + INFRACFG_TOPAXI_PROT_STA1);
	} while (status & bit);

	return 0;
}

int __secure mtk_big_cluster_on(void)
{
	u32 status, status2nd;
	int ret;

	if (readl(SPM_BASE + MT6595_SPM_SLEEP_DUAL_VCORE_PWR_CON) &
	    MT6595_SPM_SLEEP_BIG_PWR_ISO) {
		clrbits_le32(SPM_BASE + MT6595_SPM_SLEEP_DUAL_VCORE_PWR_CON,
			     MT6595_SPM_SLEEP_BIG_PWR_ISO);

		/* Disable DCM */
		setbits_le32(INFRACFG_AO_BASE + INFRACFG_AO_DCMCTL, 1);
		clrbits_le32(INFRACFG_AO_BASE + INFRACFG_AO_DCMCTL, 0x770);
	}

	/* Enable L2 hardware reset */
	clrbits_le32(CA15L_CONFIG_BASE + CA15L_RST_CTL, CA15L_L2RSTDISABLE);

	setbits_le32(SPM_BASE + MT6595_SPM_BIG_CPUTOP_PWR_CON, PWR_ON_BIT);
	psci_gpt_udelay(1);

	setbits_le32(SPM_BASE + MT6595_SPM_BIG_CPUTOP_PWR_CON, PWR_ON_2ND_BIT);

	do {
		status = readl(SPM_BASE + SPM_PWR_STATUS);
		status2nd = readl(SPM_BASE + SPM_PWR_STATUS_2ND);
	} while ((status & MT6595_SPM_BIG_CPUTOP) != MT6595_SPM_BIG_CPUTOP ||
		 (status2nd & MT6595_SPM_BIG_CPUTOP) != MT6595_SPM_BIG_CPUTOP);

	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_CPUTOP_PWR_CON, PWR_CLK_DIS_BIT);

	clrbits_le32(CA15L_CONFIG_BASE + 0xc, BIT(4) | BIT(0));

	/* Enable shared L2 */
	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_L2_PWR_CON,
		     MT6595_SPM_BIG_L2_PDN_ISO);
	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_L2_PWR_CON,
		     MT6595_SPM_BIG_L2_PDN);

	do {
		status = readl(SPM_BASE + MT6595_SPM_BIG_L2_PWR_CON);
	} while (status & MT6595_SPM_BIG_L2_PDN_ACK);

	setbits_le32(SPM_BASE + MT6595_SPM_BIG_CPUTOP_PWR_CON,
		     PWR_SRAM_ISOINT_B_BIT);
	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_CPUTOP_PWR_CON,
		     PWR_SRAM_CLKISO_BIT);

	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_CPUTOP_PWR_CON, PWR_ISO_BIT);

	setbits_le32(SPM_BASE + MT6595_SPM_BIG_CPUTOP_PWR_CON, PWR_RST_B_BIT);

	ret = mtk_spm_topaxi_prot_disable(INFRACFG_TOPAXI_PROT_CA15);
	if (ret)
		return ret;

	return 0;
}

int __secure mtk_big_on(u32 cpu)
{
	u32 status, status2nd;

	status = readl(SPM_BASE + SPM_PWR_STATUS);
	status2nd = readl(SPM_BASE + SPM_PWR_STATUS_2ND);

	if (!(status & MT6595_SPM_BIG_CPUTOP) &&
	    !(status2nd & MT6595_SPM_BIG_CPUTOP))
		mtk_big_cluster_on();

	setbits_le32(SPM_BASE + MT6595_SPM_BIG_CPU_PWR_CON(cpu), PWR_ON_BIT);
	psci_gpt_udelay(1);

	setbits_le32(SPM_BASE + MT6595_SPM_BIG_CPU_PWR_CON(cpu),
		     PWR_ON_2ND_BIT);

	writel(0x160, INFRACFG_AO_BASE + INFRACFG_AO_TOPCKDIV1);

	do {
		status = readl(SPM_BASE + SPM_PWR_STATUS);
		status2nd = readl(SPM_BASE + SPM_PWR_STATUS_2ND);
	} while ((status & MT6595_SPM_BIG_CPU(cpu)) !=
			 MT6595_SPM_BIG_CPU(cpu) ||
		 (status2nd & MT6595_SPM_BIG_CPU(cpu)) !=
			 MT6595_SPM_BIG_CPU(cpu));

	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_CPU_PWR_CON(cpu),
		     PWR_CLK_DIS_BIT);

	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_CPU_L1_PWR_CON,
		     MT6595_SPM_BIG_CPU_L1_PWR_PDN_ISO(cpu));
	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_CPU_L1_PWR_CON,
		     MT6595_SPM_BIG_CPU_L1_PWR_PDN(cpu));
	do {
		status = readl(SPM_BASE + MT6595_SPM_BIG_CPU_L1_PWR_CON);
	} while (status & MT6595_SPM_BIG_CPU_L1_PWR_PDN_ACK(cpu));
	psci_gpt_udelay(20);

	setbits_le32(SPM_BASE + MT6595_SPM_BIG_CPU_PWR_CON(cpu),
		     PWR_SRAM_ISOINT_B_BIT);
	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_CPU_PWR_CON(cpu),
		     PWR_SRAM_CLKISO_BIT);

	clrbits_le32(SPM_BASE + MT6595_SPM_BIG_CPU_PWR_CON(cpu), PWR_ISO_BIT);
	setbits_le32(SPM_BASE + MT6595_SPM_BIG_CPU_PWR_CON(cpu), PWR_RST_B_BIT);

	writel(0x0, INFRACFG_AO_BASE + INFRACFG_AO_TOPCKDIV1);

	/* Enable CCI400 SI3 */
	setbits_le32(CCI400_SI3_BASE + CCI400_SI_SNOOP_CONTROL,
		     SNOOP_REQ | DVM_MSG_REQ);
	do {
		status = readl(CCI400_BASE + CCI400_STATUS);
	} while (status & CCI400_STATUS_CHANGE_PENDING);

	return ARM_PSCI_RET_SUCCESS;
}

int __secure mtk_little_on(u32 cpu)
{
	u32 status, status2nd;

	setbits_le32(SPM_BASE + MT6595_SPM_LITTLE_CPU_PWR_CON(cpu), PWR_ON_BIT);
	psci_gpt_udelay(1);

	setbits_le32(SPM_BASE + MT6595_SPM_LITTLE_CPU_PWR_CON(cpu),
		     PWR_ON_2ND_BIT);

	do {
		status = readl(SPM_BASE + SPM_PWR_STATUS);
		status2nd = readl(SPM_BASE + SPM_PWR_STATUS_2ND);
	} while ((status & MT6595_SPM_LITTLE_CPU(cpu)) !=
			 MT6595_SPM_LITTLE_CPU(cpu) ||
		 (status2nd & MT6595_SPM_LITTLE_CPU(cpu)) !=
			 MT6595_SPM_LITTLE_CPU(cpu));

	clrbits_le32(SPM_BASE + MT6595_SPM_LITTLE_CPU_PWR_CON(cpu),
		     PWR_CLK_DIS_BIT);
	clrbits_le32(SPM_BASE + MT6595_SPM_LITTLE_CPU_PWR_CON(cpu),
		     PWR_ISO_BIT);

	clrbits_le32(SPM_BASE + MT6595_SPM_LITTLE_CPU_L1_PDN(cpu), L1_PDN);

	do {
		status = readl(SPM_BASE + MT6595_SPM_LITTLE_CPU_L1_PDN(cpu));
	} while (status & L1_PDN_ACK);

	setbits_le32(SPM_BASE + MT6595_SPM_LITTLE_CPU_PWR_CON(cpu),
		     PWR_SRAM_ISOINT_B_BIT);
	clrbits_le32(SPM_BASE + MT6595_SPM_LITTLE_CPU_PWR_CON(cpu),
		     PWR_SRAM_CLKISO_BIT);
	setbits_le32(SPM_BASE + MT6595_SPM_LITTLE_CPU_PWR_CON(cpu),
		     PWR_RST_B_BIT);

	return ARM_PSCI_RET_SUCCESS;
}

int __secure psci_cpu_on(u32 __always_unused unused, u32 mpidr, u32 pc,
			 u32 context_id)
{
	u32 cpu = mpidr_to_linear_cpu(mpidr);
	int ret;

	if (!mt6595_is_supported()) {
		return ARM_PSCI_RET_NI;
	}

	if (cpu == 0)
		/* CPU0 shutdown is not allowed */
		return ARM_PSCI_RET_DENIED;

	if (!mt6595_boot_info.core_keys[cpu - 1])
		return ARM_PSCI_RET_NOT_PRESENT;

	/* Save context for monitor */
	psci_save(cpu, pc, context_id);

	/* Prepare entry */
	writel((u32)mtk_psci_entry,
	       SRAMROM_BASE + mt6595_boot_info.jump_reg);

	/* Tell the CPU to jump */
	writel(mt6595_boot_info.core_keys[cpu - 1],
	       SRAMROM_BASE + mt6595_boot_info.core_regs[cpu - 1]);

	if (cpu < 4)
		ret = mtk_little_on(cpu);
	else {
		if (cpu == 5)
			mtk_big_cluster_on();
		ret = mtk_big_on(cpu - 4);
	}

	if (ret)
		return ret;

	/* Send IPI to wake up the core */
	writel((BIT(cpu) << 16), GIC_DIST_BASE + GICD_SGIR);

	return ARM_PSCI_RET_SUCCESS;
}

s32 __secure psci_cpu_off(void)
{
	return ARM_PSCI_RET_DISABLED;
}

void __secure psci_system_reset(void)
{
	writel(TOPRGU_SWRST_UNLOCK_KEY, TOPRGU_BASE + TOPRGU_SWRST);
	while (1);
}

void psci_board_init(void)
{
	u32 val;

	/* Allow booting secondary CPUs */
	clrbits_le32(INFRACFG_AO_BASE + INFRACFG_SRAMROM_SEC_CTRL,
		     INFRACFG_SRAMROM_SEC_CTRL_PDN);

	/* mt_smp_init_cpus(): */
	/* Enable little cluster snoop */
	val = readl(CA7MCUCFG_BASE + CA7MCUCFG_BUS_CONFIG);
	writel(val & ~CA7_ACINACTM, CA7MCUCFG_BASE + CA7MCUCFG_BUS_CONFIG);

	/* Enables DVM */
	val = 0;
	asm volatile("MRC p15, 0, %0, c1, c0, 1\n"
		     "BIC %0, %0, #1 << 15\n"
		     "MCR p15, 0, %0, c1, c0, 1\n"
		     : "+r"(val)
		     :
		     : "cc");

	/* Enable snoop requests and DVM message requests */
	setbits_le32(CCI400_SI4_BASE + CCI400_SI_SNOOP_CONTROL,
		     SNOOP_REQ | DVM_MSG_REQ);
	do {
		val = readl(CCI400_BASE + CCI400_STATUS);
	} while (val & CCI400_STATUS_CHANGE_PENDING);

	/* Enable cross trigger */
	setbits_le32(MCUCFG_BASE + MCUCFG_CROSS_TRIGGER,
		     MCUCFG_CROSS_TRIGER_CA7 | MCUCFG_CROSS_TRIGER_CA15);

	/* Set CONFIG_RES[31:0]=32h000F_FFFF to disable rguX reset wait for cpuX L1 pdn ack */
	writel(0x000fffff, CA15L_CONFIG_BASE + CA15L_CONFIG_RES);

	/* bigcore_power_on(): */
	/* Make sure big cluster buck is on */
	// TODO: move init into u-boot
	setbits_le32(0x10005060, BIT(7)); // output
	setbits_le32(0x10005460, BIT(7)); // mode
}


void __secure psci_arch_init(void)
{
	if (!mt6595_is_supported())
		return;

	writel(0xffffffff, GIC_DIST_BASE + GIC_DIST_IGROUP);
}

u32 __secure psci_get_cpu_id(void)
{
	u32 mpidr;

	asm volatile("mrc p15, 0, %0, c0, c0, 5" : "=r"(mpidr));
	return mpidr_to_linear_cpu(mpidr);
}
