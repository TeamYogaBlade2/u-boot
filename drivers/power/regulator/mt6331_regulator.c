// SPDX-License-Identifier: GPL-2.0-only
/*
 * MT6331 regulator driver
 *
 * Based on Linux mt6331-regulator.c
 */

#include <dm.h>
#include <linux/bitops.h>
#include <power/regulator.h>
#include <power/mt6331.h>
#include <power/pmic.h>

enum mt6331_regulator_type {
	MT6331_REG_TYPE_RANGE,
	MT6331_REG_TYPE_TABLE,
	MT6331_REG_TYPE_FIXED,
};

struct mt6331_linear_range {
	unsigned int min;
	unsigned int min_sel;
	unsigned int max_sel;
	unsigned int step;
};

struct mt6331_regulator_info {
	const char *name;
	const char *of_match;
	enum mt6331_regulator_type type;
	int id;
	unsigned int n_voltages;
	const unsigned int *volt_table;
	const struct mt6331_linear_range *linear_ranges;
	unsigned int min_uV;
	unsigned int vsel_reg;
	unsigned int vsel_mask;
	unsigned int enable_reg;
	unsigned int enable_mask;
	u32 qi;
	u32 vselon_reg;
	u32 vselctrl_reg;
	u32 vselctrl_mask;
	u32 modeset_reg;
	u32 modeset_mask;
	u32 status_reg;
	u32 status_mask;
};

/* Initialize struct mt6331_linear_range for regulators */
#define REGULATOR_LINEAR_RANGE(_min_uV, _min_sel, _max_sel, _step_uV)	\
{									\
	.min        = _min_uV,						\
	.min_sel    = _min_sel,						\
	.max_sel    = _max_sel,						\
	.step       = _step_uV,						\
}

#define MT6331_BUCK(match, vreg, min, max, step, volt_ranges, enreg,	\
		vosel, vosel_mask, voselon, vosel_ctrl)			\
[MT6331_ID_##vreg] = {							\
	.name = #vreg,							\
	.of_match = match,						\
	.type = MT6331_REG_TYPE_RANGE,					\
	.id = MT6331_ID_##vreg,						\
	.n_voltages = (max - min)/step + 1,				\
	.linear_ranges = volt_ranges,					\
	.vsel_reg = vosel,						\
	.vsel_mask = vosel_mask,					\
	.enable_reg = enreg,						\
	.enable_mask = BIT(0),						\
	.qi = BIT(13),							\
	.vselon_reg = voselon,						\
	.vselctrl_reg = vosel_ctrl,					\
	.vselctrl_mask = BIT(1),					\
	.status_mask = 0,						\
}

#define MT6331_LDO_AO(match, vreg, ldo_volt_table, vosel, vosel_mask)	\
[MT6331_ID_##vreg] = {							\
	.name = #vreg,							\
	.of_match = match,						\
	.type = MT6331_REG_TYPE_TABLE,					\
	.id = MT6331_ID_##vreg,						\
	.n_voltages = ARRAY_SIZE(ldo_volt_table),			\
	.volt_table = ldo_volt_table,					\
	.vsel_reg = vosel,						\
	.vsel_mask = vosel_mask,					\
}

#define MT6331_LDO_S(match, vreg, ldo_volt_table, enreg, enbit, vosel,	\
		     vosel_mask, _modeset_reg, _modeset_mask,		\
		     _status_reg, _status_mask)				\
[MT6331_ID_##vreg] = {							\
	.name = #vreg,							\
	.of_match = match,						\
	.type = MT6331_REG_TYPE_TABLE,					\
	.id = MT6331_ID_##vreg,						\
	.n_voltages = ARRAY_SIZE(ldo_volt_table),			\
	.volt_table = ldo_volt_table,					\
	.vsel_reg = vosel,						\
	.vsel_mask = vosel_mask,					\
	.enable_reg = enreg,						\
	.enable_mask = BIT(enbit),					\
	.modeset_reg = _modeset_reg,					\
	.modeset_mask = _modeset_mask,					\
	.status_reg = _status_reg,					\
	.status_mask = _status_mask,					\
}

#define MT6331_LDO(match, vreg, ldo_volt_table, enreg, enbit, vosel,	\
		   vosel_mask, _modeset_reg, _modeset_mask)		\
[MT6331_ID_##vreg] = {							\
	.name = #vreg,							\
	.of_match = match,						\
	.type = MT6331_REG_TYPE_TABLE,					\
	.id = MT6331_ID_##vreg,						\
	.n_voltages = ARRAY_SIZE(ldo_volt_table),			\
	.volt_table = ldo_volt_table,					\
	.vsel_reg = vosel,						\
	.vsel_mask = vosel_mask,					\
	.enable_reg = enreg,						\
	.enable_mask = BIT(enbit),					\
	.qi = BIT(15),							\
	.modeset_reg = _modeset_reg,					\
	.modeset_mask = _modeset_mask,					\
}

#define MT6331_REG_FIXED(match, vreg, enreg, enbit, qibit, volt,	\
			 _modeset_reg, _modeset_mask)			\
[MT6331_ID_##vreg] = {							\
	.name = #vreg,							\
	.of_match = match,						\
	.type = MT6331_REG_TYPE_FIXED,					\
	.id = MT6331_ID_##vreg,						\
	.n_voltages = 1,						\
	.enable_reg = enreg,						\
	.enable_mask = BIT(enbit),					\
	.min_uV = volt,							\
	.qi = BIT(qibit),						\
	.modeset_reg = _modeset_reg,					\
	.modeset_mask = _modeset_mask,					\
}

static int mt6331_range_find_value(const struct mt6331_linear_range *r,
				   unsigned int sel, unsigned int *val)
{
	if (!val || sel < r->min_sel || sel > r->max_sel)
		return -EINVAL;

	*val = r->min + r->step * (sel - r->min_sel);

	return 0;
}

static int mt6331_range_find_selector(const struct mt6331_linear_range *r,
				      int val, unsigned int *sel)
{
	if (val < r->min)
		return -EINVAL;

	if (r->step == 0) {
		*sel = r->min_sel;
		return 0;
	}

	*sel = r->min_sel + ((val - r->min) / r->step);

	if (*sel > r->max_sel)
		return -EINVAL;

	return 0;
}

static int mt6331_get_enable(struct udevice *dev)
{
	struct mt6331_regulator_info *info = dev_get_priv(dev);
	int ret;

	if (!info->enable_reg)
		/* always on */
		return true;

	ret = pmic_reg_read(dev->parent, info->enable_reg);
	if (ret < 0)
		return ret;

	return (ret & info->enable_mask) ? true : false;
}

static int mt6331_set_enable(struct udevice *dev, bool enable)
{
	struct mt6331_regulator_info *info = dev_get_priv(dev);

	if (!info->enable_reg)
		return enable ? 0 : -EINVAL;

	return pmic_clrsetbits(dev->parent, info->enable_reg,
			       info->enable_mask,
			       enable ? info->enable_mask : 0);
}

static int mt6331_get_value(struct udevice *dev)
{
	struct mt6331_regulator_info *info = dev_get_priv(dev);
	unsigned int val_uV;
	int selector, shift, ret;

	switch (info->type) {
	case MT6331_REG_TYPE_RANGE:
		ret = pmic_reg_read(dev->parent, info->vsel_reg);
		if (ret < 0)
			return ret;

		shift = info->vsel_mask ? (ffs(info->vsel_mask) - 1) : 0;
		selector = (ret & info->vsel_mask) >> shift;

		ret = mt6331_range_find_value(info->linear_ranges, selector, &val_uV);
		if (ret < 0)
			return ret;

		return val_uV;

	case MT6331_REG_TYPE_TABLE:
		if (!info->vsel_reg)
			return -EINVAL;

		ret = pmic_reg_read(dev->parent, info->vsel_reg);
		if (ret < 0)
			return ret;

		shift = info->vsel_mask ? (ffs(info->vsel_mask) - 1) : 0;
		selector = (ret & info->vsel_mask) >> shift;

		if (selector >= info->n_voltages)
			return -EINVAL;

		return info->volt_table[selector];

	case MT6331_REG_TYPE_FIXED:
		return info->min_uV;

	default:
		return -EINVAL;
	}
}

static int mt6331_set_value(struct udevice *dev, int uvolt)
{
	struct mt6331_regulator_info *info = dev_get_priv(dev);
	unsigned int selector;
	int idx, shift, ret;

	switch (info->type) {
	case MT6331_REG_TYPE_RANGE:
		ret = mt6331_range_find_selector(info->linear_ranges, uvolt, &selector);
		if (ret < 0)
			return ret;

		shift = info->vsel_mask ? (ffs(info->vsel_mask) - 1) : 0;
		return pmic_clrsetbits(dev->parent, info->vsel_reg,
				       info->vsel_mask, selector << shift);

	case MT6331_REG_TYPE_TABLE:
		for (idx = 0; idx < info->n_voltages; idx++) {
			if (info->volt_table[idx] == uvolt) {
				shift = info->vsel_mask ? (ffs(info->vsel_mask) - 1) : 0;
				return pmic_clrsetbits(dev->parent, info->vsel_reg,
						       info->vsel_mask, idx << shift);
			}
		}
		return -EINVAL;

	default:
		return -EINVAL;
	}
}

static const struct mt6331_linear_range buck_volt_range[] = {
	REGULATOR_LINEAR_RANGE(700000, 0, 0x7f, 6250),
};

static const unsigned int ldo_volt_table1[] = {
	2800000, 3000000, 0, 3200000
};

static const unsigned int ldo_volt_table2[] = {
	1500000, 1800000, 2500000, 2800000,
};

static const unsigned int ldo_volt_table3[] = {
	1200000, 1300000, 1500000, 1800000, 2000000, 2800000, 3000000, 3300000,
};

static const unsigned int ldo_volt_table4[] = {
	0, 0, 1700000, 1800000, 1860000, 2760000, 3000000, 3100000,
};

static const unsigned int ldo_volt_table5[] = {
	1800000, 3300000, 1800000, 3300000,
};

static const unsigned int ldo_volt_table6[] = {
	3000000, 3300000,
};

static const unsigned int ldo_volt_table7[] = {
	1200000, 1600000, 1700000, 1800000, 1900000, 2000000, 2100000, 2200000,
};

static const unsigned int ldo_volt_table8[] = {
	900000, 1000000, 1100000, 1220000, 1300000, 1500000, 1500000, 1500000,
};

static const unsigned int ldo_volt_table9[] = {
	1000000, 1050000, 1100000, 1150000, 1200000, 1250000, 1300000, 1300000,
};

static const unsigned int ldo_volt_table10[] = {
	1200000, 1300000, 1500000, 1800000,
};

static const unsigned int ldo_volt_table11[] = {
	1200000, 1300000, 1400000, 1500000, 1600000, 1700000, 1800000, 1800000,
};

static const struct mt6331_regulator_info mt6331_regulators[] = {
	MT6331_BUCK("buck-vdvfs11", VDVFS11, 700000, 1493750, 6250,
		    buck_volt_range, MT6331_VDVFS11_CON9,
		    MT6331_VDVFS11_CON11, GENMASK(6, 0),
		    MT6331_VDVFS11_CON12, MT6331_VDVFS11_CON7),
	MT6331_BUCK("buck-vdvfs12", VDVFS12, 700000, 1493750, 6250,
		    buck_volt_range, MT6331_VDVFS12_CON9,
		    MT6331_VDVFS12_CON11, GENMASK(6, 0),
		    MT6331_VDVFS12_CON12, MT6331_VDVFS12_CON7),
	MT6331_BUCK("buck-vdvfs13", VDVFS13, 700000, 1493750, 6250,
		    buck_volt_range, MT6331_VDVFS13_CON9,
		    MT6331_VDVFS13_CON11, GENMASK(6, 0),
		    MT6331_VDVFS13_CON12, MT6331_VDVFS13_CON7),
	MT6331_BUCK("buck-vdvfs14", VDVFS14, 700000, 1493750, 6250,
		    buck_volt_range, MT6331_VDVFS14_CON9,
		    MT6331_VDVFS14_CON11, GENMASK(6, 0),
		    MT6331_VDVFS14_CON12, MT6331_VDVFS14_CON7),
	MT6331_BUCK("buck-vcore2", VCORE2, 700000, 1493750, 6250,
		    buck_volt_range, MT6331_VCORE2_CON9,
		    MT6331_VCORE2_CON11, GENMASK(6, 0),
		    MT6331_VCORE2_CON12, MT6331_VCORE2_CON7),
	MT6331_REG_FIXED("buck-vio18", VIO18, MT6331_VIO18_CON9, 0, 13, 1800000, 0, 0),
	MT6331_REG_FIXED("ldo-vrtc", VRTC, MT6331_DIGLDO_CON11, 8, 15, 2800000, 0, 0),
	MT6331_REG_FIXED("ldo-vtcxo1", VTCXO1, MT6331_ANALDO_CON1, 10, 15, 2800000,
			 MT6331_ANALDO_CON1, GENMASK(1, 0)),
	MT6331_REG_FIXED("ldo-vtcxo2", VTCXO2, MT6331_ANALDO_CON2, 10, 15, 2800000,
			 MT6331_ANALDO_CON2, GENMASK(1, 0)),
	MT6331_REG_FIXED("ldo-vsram", VSRAM_DVFS1, MT6331_SYSLDO_CON4, 10, 15, 1012500,
			 MT6331_SYSLDO_CON4, GENMASK(1, 0)),
	MT6331_REG_FIXED("ldo-vio28", VIO28, MT6331_DIGLDO_CON1, 10, 15, 2800000,
			 MT6331_DIGLDO_CON1, GENMASK(1, 0)),
	MT6331_LDO("ldo-avdd32aud", AVDD32_AUD, ldo_volt_table1, MT6331_ANALDO_CON3, 10,
		   MT6331_ANALDO_CON10, GENMASK(6, 5), MT6331_ANALDO_CON3, GENMASK(1, 0)),
	MT6331_LDO("ldo-vauxa32", VAUXA32, ldo_volt_table1, MT6331_ANALDO_CON4, 10,
		   MT6331_ANALDO_CON6, GENMASK(6, 5), MT6331_ANALDO_CON4, GENMASK(1, 0)),
	MT6331_LDO("ldo-vemc33", VEMC33, ldo_volt_table6, MT6331_DIGLDO_CON5, 10,
		   MT6331_DIGLDO_CON17, BIT(6), MT6331_DIGLDO_CON5, GENMASK(1, 0)),
	MT6331_LDO("ldo-vibr", VIBR, ldo_volt_table3, MT6331_DIGLDO_CON12, 10,
		   MT6331_DIGLDO_CON20, GENMASK(6, 4), MT6331_DIGLDO_CON12, GENMASK(1, 0)),
	MT6331_LDO("ldo-vmc", VMC, ldo_volt_table5, MT6331_DIGLDO_CON3, 10,
		   MT6331_DIGLDO_CON15, GENMASK(5, 4), MT6331_DIGLDO_CON3, GENMASK(1, 0)),
	MT6331_LDO("ldo-vmch", VMCH, ldo_volt_table6, MT6331_DIGLDO_CON4, 10,
		   MT6331_DIGLDO_CON16, BIT(6), MT6331_DIGLDO_CON4, GENMASK(1, 0)),
	MT6331_LDO("ldo-vmipi", VMIPI, ldo_volt_table3, MT6331_SYSLDO_CON5, 10,
		   MT6331_SYSLDO_CON13, GENMASK(5, 3), MT6331_SYSLDO_CON5, GENMASK(1, 0)),
	MT6331_LDO("ldo-vsim1", VSIM1, ldo_volt_table4, MT6331_DIGLDO_CON8, 10,
		   MT6331_DIGLDO_CON21, GENMASK(6, 4), MT6331_DIGLDO_CON8, GENMASK(1, 0)),
	MT6331_LDO("ldo-vsim2", VSIM2, ldo_volt_table4, MT6331_DIGLDO_CON9, 10,
		   MT6331_DIGLDO_CON22, GENMASK(6, 4), MT6331_DIGLDO_CON9, GENMASK(1, 0)),
	MT6331_LDO("ldo-vusb10", VUSB10, ldo_volt_table9, MT6331_SYSLDO_CON2, 10,
		   MT6331_SYSLDO_CON10, GENMASK(5, 3), MT6331_SYSLDO_CON2, GENMASK(1, 0)),
	MT6331_LDO("ldo-vcama", VCAMA, ldo_volt_table2, MT6331_ANALDO_CON5, 15,
		   MT6331_ANALDO_CON9, GENMASK(5, 4), 0, 0),
	MT6331_LDO_S("ldo-vcamaf", VCAM_AF, ldo_volt_table3, MT6331_DIGLDO_CON2, 10,
		     MT6331_DIGLDO_CON14, GENMASK(6, 4), MT6331_DIGLDO_CON2, GENMASK(1, 0),
		     MT6331_EN_STATUS1, BIT(0)),
	MT6331_LDO_S("ldo-vcamd", VCAMD, ldo_volt_table8, MT6331_SYSLDO_CON1, 15,
		     MT6331_SYSLDO_CON9, GENMASK(6, 4), MT6331_SYSLDO_CON1, GENMASK(1, 0),
		     MT6331_EN_STATUS1, BIT(11)),
	MT6331_LDO_S("ldo-vcamio", VCAM_IO,  ldo_volt_table10, MT6331_SYSLDO_CON3, 10,
		     MT6331_SYSLDO_CON11, GENMASK(4, 3), MT6331_SYSLDO_CON3, GENMASK(1, 0),
		     MT6331_EN_STATUS1, BIT(13)),
	MT6331_LDO_S("ldo-vgp1", VGP1, ldo_volt_table3, MT6331_DIGLDO_CON6, 10,
		     MT6331_DIGLDO_CON19, GENMASK(6, 4), MT6331_DIGLDO_CON6, GENMASK(1, 0),
		     MT6331_EN_STATUS1, BIT(4)),
	MT6331_LDO_S("ldo-vgp2", VGP2, ldo_volt_table10, MT6331_SYSLDO_CON6, 10,
		     MT6331_SYSLDO_CON14, GENMASK(4, 3), MT6331_SYSLDO_CON6, GENMASK(1, 0),
		     MT6331_EN_STATUS1, BIT(15)),
	MT6331_LDO_S("ldo-vgp3", VGP3, ldo_volt_table10, MT6331_SYSLDO_CON7, 10,
		     MT6331_SYSLDO_CON15, GENMASK(4, 3), MT6331_SYSLDO_CON7, GENMASK(1, 0),
		     MT6331_EN_STATUS2, BIT(0)),
	MT6331_LDO_S("ldo-vgp4", VGP4, ldo_volt_table7, MT6331_DIGLDO_CON7, 10,
		     MT6331_DIGLDO_CON18, GENMASK(6, 4), MT6331_DIGLDO_CON7, GENMASK(1, 0),
		     MT6331_EN_STATUS1, BIT(5)),
	MT6331_LDO_AO("ldo-vdig18", VDIG18, ldo_volt_table11,
		      MT6331_DIGLDO_CON28, GENMASK(14, 12)),
};

static int mt6331_regulator_probe(struct udevice *dev)
{
	struct mt6331_regulator_info *priv = dev_get_priv(dev);
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(mt6331_regulators); i++) {
		if (!mt6331_regulators[i].of_match)
			continue;

		if (!strcmp(dev->name, mt6331_regulators[i].of_match)) {
			*priv = mt6331_regulators[i];

			/* mt6331_set_buck_vosel_reg */
			if (priv->vselctrl_reg) {
				ret = pmic_reg_read(dev->parent, priv->vselctrl_reg);
				if (ret >= 0 && (ret & priv->vselctrl_mask)) {
					priv->vsel_reg = priv->vselon_reg;
				}
			}
			return 0;
		}
	}

	return -ENOENT;
}

static const struct dm_regulator_ops mt6331_regulator_ops = {
	.get_value  = mt6331_get_value,
	.set_value  = mt6331_set_value,
	.get_enable = mt6331_get_enable,
	.set_enable = mt6331_set_enable,
};

U_BOOT_DRIVER(mt6331_regulator) = {
	.name      = "mt6331_regulator",
	.id        = UCLASS_REGULATOR,
	.ops       = &mt6331_regulator_ops,
	.probe     = mt6331_regulator_probe,
	.priv_auto = sizeof(struct mt6331_regulator_info),
};
