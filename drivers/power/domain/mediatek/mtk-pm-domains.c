// SPDX-License-Identifier: GPL-2.0+

#include <clk.h>
#include <dm.h>
#include <dm/device_compat.h>
#include <linux/bitops.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/iopoll.h>
#include <power-domain-uclass.h>
#include <regmap.h>
#include <syscon.h>

#define SPM_VDE_PWR_CON			0x0210
#define SPM_MFG_PWR_CON			0x0214
#define SPM_VEN_PWR_CON			0x0230
#define SPM_ISP_PWR_CON			0x0238
#define SPM_DIS_PWR_CON			0x023c
#define SPM_CONN_PWR_CON		0x0280
#define SPM_MD1_PWR_CON			0x0284

#define SPM_PWR_STATUS			0x060c
#define SPM_PWR_STATUS_2ND		0x0610

#define PWR_STATUS_MD1			BIT(0)
#define PWR_STATUS_CONN			BIT(1)
#define PWR_STATUS_DISP			BIT(3)
#define PWR_STATUS_MFG			BIT(4)

#define PWR_RST_B_BIT			BIT(0)
#define PWR_ISO_BIT			BIT(1)
#define PWR_ON_BIT			BIT(2)
#define PWR_ON_2ND_BIT			BIT(3)
#define PWR_CLK_DIS_BIT			BIT(4)
#define PWR_SRAM_CLKISO_BIT		BIT(5)
#define PWR_SRAM_ISOINT_B_BIT		BIT(6)

#define INFRA_TOPAXI_PROTECTEN		0x0220
#define INFRA_TOPAXI_PROTECTSTA1	0x0228

#define MT6572_TOP_AXI_PROT_EN_MD1	(BIT(3) | BIT(4))
#define MT6572_TOP_AXI_PROT_EN_CONN	(BIT(1) | BIT(2))
#define MT6572_TOP_AXI_PROT_EN_DIS	(BIT(10) | BIT(11))

#define MT6572_POWER_DOMAIN_MD1		0
#define MT6572_POWER_DOMAIN_CONN	1
#define MT6572_POWER_DOMAIN_DIS		2
#define MT6572_POWER_DOMAIN_MFG		3

#define SPM_MAX_BUS_PROT_DATA		4
#define MTK_POLL_TIMEOUT_US		1000000

enum scpsys_bus_prot_block {
	BUS_PROT_BLOCK_INFRA,
	BUS_PROT_BLOCK_SMI,
	BUS_PROT_BLOCK_COUNT,
};

enum scpsys_bus_prot_flags {
	BUS_PROT_REG_UPDATE = BIT(1),
	BUS_PROT_IGNORE_CLR_ACK = BIT(2),
};

struct scpsys_bus_prot_data {
	u8 bus_prot_block;
	u32 bus_prot_set_clr_mask;
	u32 bus_prot_set;
	u32 bus_prot_clr;
	u32 bus_prot_sta_mask;
	u32 bus_prot_sta;
	u8 flags;
};

#define BUS_PROT_INFRA_UPDATE_TOPAXI(_mask)               \
	{                                                 \
		.bus_prot_block = BUS_PROT_BLOCK_INFRA,   \
		.bus_prot_set_clr_mask = (_mask),         \
		.bus_prot_set = INFRA_TOPAXI_PROTECTEN,   \
		.bus_prot_clr = INFRA_TOPAXI_PROTECTEN,   \
		.bus_prot_sta_mask = (_mask),             \
		.bus_prot_sta = INFRA_TOPAXI_PROTECTSTA1, \
		.flags = BUS_PROT_REG_UPDATE,             \
	}

struct scpsys_domain_data {
	const char *name;
	u32 sta_mask;
	u32 sta2nd_mask;
	int ctl_offs;
	u32 sram_pdn_bits;
	u32 sram_pdn_ack_bits;
	const struct scpsys_bus_prot_data bp_cfg[SPM_MAX_BUS_PROT_DATA];
};

struct scpsys_soc_data {
	const struct scpsys_domain_data *domains_data;
	int num_domains;
};

struct mtk_power_domain_priv {
	struct regmap *base;
	struct regmap *infracfg;
	const struct scpsys_soc_data *soc_data;
};

static bool scpsys_domain_is_on(struct mtk_power_domain_priv *priv,
				const struct scpsys_domain_data *data)
{
	u32 status, status2, mask2;

	mask2 = data->sta2nd_mask ? data->sta2nd_mask : data->sta_mask;

	regmap_read(priv->base, SPM_PWR_STATUS, &status);
	regmap_read(priv->base, SPM_PWR_STATUS_2ND, &status2);

	return (status & data->sta_mask) && (status2 & mask2);
}

static int scpsys_bus_protect_enable(struct mtk_power_domain_priv *priv,
				     const struct scpsys_domain_data *data)
{
	int i;

	for (i = 0; i < SPM_MAX_BUS_PROT_DATA; i++) {
		const struct scpsys_bus_prot_data *bpd = &data->bp_cfg[i];
		u32 val;

		if (!bpd->bus_prot_set_clr_mask)
			break;

		if (bpd->flags & BUS_PROT_REG_UPDATE)
			regmap_set_bits(priv->infracfg, bpd->bus_prot_set,
					bpd->bus_prot_set_clr_mask);
		else
			regmap_write(priv->infracfg, bpd->bus_prot_set,
				     bpd->bus_prot_set_clr_mask);

		regmap_read_poll_timeout(priv->infracfg, bpd->bus_prot_sta, val,
					 (val & bpd->bus_prot_sta_mask) == bpd->bus_prot_sta_mask,
					 10, MTK_POLL_TIMEOUT_US);
	}

	return 0;
}

static int scpsys_bus_protect_disable(struct mtk_power_domain_priv *priv,
				      const struct scpsys_domain_data *data)
{
	int i;

	for (i = SPM_MAX_BUS_PROT_DATA - 1; i >= 0; i--) {
		const struct scpsys_bus_prot_data *bpd = &data->bp_cfg[i];
		u32 val;

		if (!bpd->bus_prot_set_clr_mask)
			continue;

		if (bpd->flags & BUS_PROT_REG_UPDATE)
			regmap_clear_bits(priv->infracfg, bpd->bus_prot_clr,
					  bpd->bus_prot_set_clr_mask);
		else
			regmap_write(priv->infracfg, bpd->bus_prot_clr,
				     bpd->bus_prot_set_clr_mask);

		if (bpd->flags & BUS_PROT_IGNORE_CLR_ACK)
			continue;

		regmap_read_poll_timeout(priv->infracfg, bpd->bus_prot_sta, val,
					 !(val & bpd->bus_prot_sta_mask), 10,
					 MTK_POLL_TIMEOUT_US);
	}

	return 0;
}

static int scpsys_sram_enable(struct mtk_power_domain_priv *priv,
			      const struct scpsys_domain_data *data)
{
	u32 val, pdn_ack = data->sram_pdn_ack_bits;

	if (!data->sram_pdn_bits)
		return 0;

	regmap_clear_bits(priv->base, data->ctl_offs, data->sram_pdn_bits);

	if (pdn_ack) {
		return regmap_read_poll_timeout(priv->base, data->ctl_offs, val,
						!(val & pdn_ack), 10,
						MTK_POLL_TIMEOUT_US);
	}

	return 0;
}

static int scpsys_sram_disable(struct mtk_power_domain_priv *priv,
			       const struct scpsys_domain_data *data)
{
	u32 val, pdn_ack = data->sram_pdn_ack_bits;

	if (!data->sram_pdn_bits)
		return 0;

	regmap_set_bits(priv->base, data->ctl_offs, data->sram_pdn_bits);

	if (pdn_ack) {
		return regmap_read_poll_timeout(priv->base, data->ctl_offs, val,
						(val & pdn_ack) == pdn_ack, 10,
						MTK_POLL_TIMEOUT_US);
	}

	return 0;
}

static int mtk_power_domain_on(struct power_domain *power_domain)
{
	struct mtk_power_domain_priv *priv = dev_get_priv(power_domain->dev);
	const struct scpsys_domain_data *data;
	u32 val;
	int ret;

	if (power_domain->id >= priv->soc_data->num_domains)
		return -EINVAL;

	data = &priv->soc_data->domains_data[power_domain->id];

	regmap_set_bits(priv->base, data->ctl_offs, PWR_ON_BIT);
	regmap_set_bits(priv->base, data->ctl_offs, PWR_ON_2ND_BIT);

	ret = regmap_read_poll_timeout(priv->base, SPM_PWR_STATUS, val,
				       scpsys_domain_is_on(priv, data), 10,
				       MTK_POLL_TIMEOUT_US);
	if (ret)
		return ret;

	regmap_clear_bits(priv->base, data->ctl_offs, PWR_CLK_DIS_BIT);
	regmap_clear_bits(priv->base, data->ctl_offs, PWR_ISO_BIT);
	regmap_set_bits(priv->base, data->ctl_offs, PWR_RST_B_BIT);

	ret = scpsys_sram_enable(priv, data);
	if (ret)
		return ret;

	return scpsys_bus_protect_disable(priv, data);
}

static int mtk_power_domain_off(struct power_domain *power_domain)
{
	struct mtk_power_domain_priv *priv = dev_get_priv(power_domain->dev);
	const struct scpsys_domain_data *data;
	u32 val;
	int ret;

	if (power_domain->id >= priv->soc_data->num_domains)
		return -EINVAL;

	data = &priv->soc_data->domains_data[power_domain->id];

	ret = scpsys_bus_protect_enable(priv, data);
	if (ret)
		return ret;

	ret = scpsys_sram_disable(priv, data);
	if (ret)
		return ret;

	regmap_set_bits(priv->base, data->ctl_offs, PWR_ISO_BIT);
	regmap_set_bits(priv->base, data->ctl_offs, PWR_CLK_DIS_BIT);
	regmap_clear_bits(priv->base, data->ctl_offs, PWR_RST_B_BIT);
	regmap_clear_bits(priv->base, data->ctl_offs, PWR_ON_2ND_BIT);
	regmap_clear_bits(priv->base, data->ctl_offs, PWR_ON_BIT);

	return regmap_read_poll_timeout(priv->base, SPM_PWR_STATUS, val,
					!scpsys_domain_is_on(priv, data), 10,
					MTK_POLL_TIMEOUT_US);
}

static int mtk_power_domain_request(struct power_domain *power_domain)
{
	struct mtk_power_domain_priv *priv = dev_get_priv(power_domain->dev);

	if (power_domain->id >= priv->soc_data->num_domains)
		return -EINVAL;

	return 0;
}

static int mtk_power_domain_free(struct power_domain *power_domain)
{
	return 0;
}

static const struct power_domain_ops mtk_power_domain_ops = {
	.request = mtk_power_domain_request,
	.rfree = mtk_power_domain_free,
	.on = mtk_power_domain_on,
	.off = mtk_power_domain_off,
};

static int mtk_power_domain_probe(struct udevice *dev)
{
	struct mtk_power_domain_priv *priv = dev_get_priv(dev);
	struct udevice *parent = dev_get_parent(dev);

	priv->soc_data = (const struct scpsys_soc_data *)dev_get_driver_data(dev);

	priv->base = syscon_get_regmap(parent);
	if (IS_ERR(priv->base)) {
		dev_err(dev, "Failed to get parent syscon regmap: %ld\n", PTR_ERR(priv->base));
		return PTR_ERR(priv->base);
	}

	priv->infracfg = syscon_regmap_lookup_by_phandle(dev, "access-controllers");
	if (IS_ERR(priv->infracfg))
		priv->infracfg = syscon_regmap_lookup_by_phandle(dev, "mediatek,infracfg");

	if (IS_ERR(priv->infracfg))
		return PTR_ERR(priv->infracfg);

	return 0;
}

/* MT6572 SoC Data Table */
static const struct scpsys_domain_data mt6572_domain_data[] = {
	[MT6572_POWER_DOMAIN_MD1] = {
		.name = "md1",
		.sta_mask = PWR_STATUS_MD1,
		.ctl_offs = SPM_MD1_PWR_CON,
		.bp_cfg = {
			BUS_PROT_INFRA_UPDATE_TOPAXI(MT6572_TOP_AXI_PROT_EN_MD1),
		},
	},
	[MT6572_POWER_DOMAIN_CONN] = {
		.name = "conn",
		.sta_mask = PWR_STATUS_CONN,
		.ctl_offs = SPM_CONN_PWR_CON,
		.bp_cfg = {
			BUS_PROT_INFRA_UPDATE_TOPAXI(MT6572_TOP_AXI_PROT_EN_CONN),
		},
	},
	[MT6572_POWER_DOMAIN_DIS] = {
		.name = "dis",
		.sta_mask = PWR_STATUS_DISP,
		.ctl_offs = SPM_DIS_PWR_CON,
		.sram_pdn_bits = GENMASK(11, 8),
		.sram_pdn_ack_bits = GENMASK(15, 12),
		.bp_cfg = {
			BUS_PROT_INFRA_UPDATE_TOPAXI(MT6572_TOP_AXI_PROT_EN_DIS),
		},
	},
	[MT6572_POWER_DOMAIN_MFG] = {
		.name = "mfg",
		.sta_mask = PWR_STATUS_MFG,
		.ctl_offs = SPM_MFG_PWR_CON,
		.sram_pdn_bits = BIT(8),
		.sram_pdn_ack_bits = BIT(12),
	},
};

static const struct scpsys_soc_data mt6572_scpsys_data = {
	.domains_data = mt6572_domain_data,
	.num_domains = ARRAY_SIZE(mt6572_domain_data),
};

static const struct udevice_id mtk_power_domain_ids[] = {
	{ .compatible = "mediatek,mt6572-power-controller", .data = (ulong)&mt6572_scpsys_data },
	{ }
};

U_BOOT_DRIVER(mtk_power_domain) = {
	.name = "mtk_power_domain",
	.id = UCLASS_POWER_DOMAIN,
	.of_match = mtk_power_domain_ids,
	.probe = mtk_power_domain_probe,
	.ops = &mtk_power_domain_ops,
	.priv_auto = sizeof(struct mtk_power_domain_priv),
};
