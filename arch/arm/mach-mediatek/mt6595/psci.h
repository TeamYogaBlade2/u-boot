#include <asm/io.h>
#include <asm/processor.h>
#include <asm/secure.h>

/* Chip ID */
#define DEVINFO_BASE			0x8000000
/* Chip software version */
#define DEVINFO_SW_VERSION		0xc

/* INFRACFG Always On domain */
#define INFRACFG_AO_BASE		0x10001000
#define INFRACFG_AO_TOPCKDIV1		0x008

#define INFRACFG_AO_DCMCTL		0x10

#define INFRACFG_TOPAXI_PROT_EN		0x220
#define INFRACFG_TOPAXI_PROT_STA1	0x228
#define INFRACFG_TOPAXI_PROT_CA15	BIT(30)

/* INFRACFG CPU software power down */
#define INFRACFG_SRAMROM_SEC_CTRL	0x804
/* Power down */
#define INFRACFG_SRAMROM_SEC_CTRL_PDN	BIT(31)

#define MCUCFG_BASE			0x10200000
#define MCUCFG_CROSS_TRIGGER		0x1c
#define MCUCFG_CROSS_TRIGER_CA7		BIT(14)
#define MCUCFG_CROSS_TRIGER_CA15	BIT(15)

#define MCUCFG_XBAR_CTL			0x74
#define MCUCFG_XBAR_HAZARD_DISABLE	BIT(6)
#define MCUCFG_XBAR_AW_ENABLE		BIT(4)

/* Power domain MMIO */
#define SPM_BASE			0x10006000

/* Common SPM offsets */
#define SPM_PWR_STATUS			0x060c
#define SPM_PWR_STATUS_2ND		0x0610

/* Common SPM status */
#define PWR_RST_B_BIT			BIT(0)
#define PWR_ISO_BIT			BIT(1)
#define PWR_ON_BIT			BIT(2)
#define PWR_ON_2ND_BIT			BIT(3)
#define PWR_CLK_DIS_BIT			BIT(4)
#define PWR_SRAM_CLKISO_BIT		BIT(5)
#define PWR_SRAM_ISOINT_B_BIT		BIT(6)

/* CPU SPM bits (may be shared across MT65xx) */
#define L1_PDN				BIT(0)
#define L1_PDN_ACK			BIT(8)

/* Little cores power domain */
#define MT6595_SPM_LITTLE_CPU_PWR_CON(n) \
	((n) == 0 ? 0x200 : (0x218 + (((n) - 1) * 4)))
/* Little cores cache power domain */
#define MT6595_SPM_LITTLE_CPU_L1_PDN(n)		(0x25c + (n * 8))

/* Big cores power domain */
#define MT6595_SPM_BIG_CPU_PWR_CON(n)		(0x2a0 + (n * 4))
/* Big cores cache power domain */
#define MT6595_SPM_BIG_CPU_L1_PWR_CON		0x2b4
#define MT6595_SPM_BIG_CPU_L1_PWR_PDN(n)	BIT(0 + n)
#define MT6595_SPM_BIG_CPU_L1_PWR_PDN_ISO(n)	BIT(4 + n)
#define MT6595_SPM_BIG_CPU_L1_PWR_PDN_ACK(n)	BIT(8 + n)

/* Little cluster status */
#define MT6595_SPM_LITTLE_CPUTOP		BIT(8)
/* Little cores status: 9~12 */
#define MT6595_SPM_LITTLE_CPU(n)		BIT(9 + n)
/* Big cluster status */
#define MT6595_SPM_BIG_CPUTOP			BIT(15)
/* Big cores status status: 16~19 */
#define MT6595_SPM_BIG_CPU(n)			BIT(16 + n)

/* Big cluster */
#define MT6595_SPM_BIG_CPUTOP_PWR_CON		0x2b0

#define MT6595_SPM_BIG_L2_PWR_CON		0x2b8
#define MT6595_SPM_BIG_L2_PDN_ACK		BIT(8)
#define MT6595_SPM_BIG_L2_PDN_ISO		BIT(2)
#define MT6595_SPM_BIG_L2_PDN			BIT(0)

#define MT6595_SPM_SLEEP_DUAL_VCORE_PWR_CON	0x404
#define MT6595_SPM_SLEEP_BIG_PWR_ISO		BIT(13)
#define MT6595_SPM_SLEEP_LITTLE_PWR_ISO		BIT(12)

/* Reset controller */
#define TOPRGU_BASE				0x10007000
/* Software reset */
#define TOPRGU_SWRST				0x14
#define TOPRGU_SWRST_UNLOCK_KEY			0x1209

/* Timer */
#define GPT_BASE				0x10008000
#define GPT_CNT_REG2				(0x08 + (0x10 * 2))

/* Little cluster configuration */
#define CA7MCUCFG_BASE	 		0x10200100
/* Little cluster bus configuration */
#define CA7MCUCFG_BUS_CONFIG		0x1c
/* Cortex A7 snoop function */
#define CA7_ACINACTM			BIT(4)

/* Used for SMP jump address */
#define SRAMROM_BASE			0x10202000
/* CPU jump address offset */
#define SRAMROM_JUMP_ADDRESS		0x34

#define CA15L_CONFIG_BASE		0x10200200

#define CA15L_RST_CTL			0x44
#define CA15L_L2RSTDISABLE		BIT(14)

#define CA15L_CONFIG_RES		0x68

/* GIC distributor base */
#define GIC_DIST_BASE			0x10221000
#define GIC_DIST_IGROUP			0x080

/* CCI400 */
#define CCI400_BASE			0x10390000
#define CCI400_STATUS			0xc
#define CCI400_STATUS_CHANGE_PENDING	BIT(0)

/* CCI400 Cortex A17 port */
#define CCI400_SI3_BASE			(CCI400_BASE + 0x4000)

/* CCI400 Cortex A7 port */
#define CCI400_SI4_BASE			(CCI400_BASE + 0x5000)

#define CCI400_SI_SNOOP_CONTROL		0x0
#define DVM_MSG_REQ			BIT(1)
#define SNOOP_REQ			BIT(0)

#define MTK_MAX_CPU	8

struct mtk_boot_info {
	u32 jump_reg;
	u32 core_keys[MTK_MAX_CPU - 1];
	u32 core_regs[MTK_MAX_CPU - 1];
};

void __secure psci_gpt_udelay(u32 usec);

void __noreturn __secure mtk_psci_entry(void);
