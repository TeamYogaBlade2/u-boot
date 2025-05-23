/*
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <stdio.h>
#include <asm/arch/mt65xx_typedefs.h>
#include <asm/arch/mt_rtc.h>
#include <asm/arch/boot_mode.h>
#include <asm/arch/mt_pmic_wrap_uboot.h>

#define RTC_RELPWR_WHEN_XRST	1	/* BBPU = 0 when xreset_rstb goes low */

static U16 RTC_Read(U16 addr)
{
	U32 rdata=0;
	pwrap_read((U32)addr, &rdata);
	return (U16)rdata;
}

static void RTC_Write(U16 addr, U16 data)
{
	pwrap_write((U32)addr, (U32)data);
}


#define rtc_busy_wait()					\
do {							\
	while (RTC_Read(RTC_BBPU) & RTC_BBPU_CBUSY);	\
} while (0)

static unsigned long rtc_mktime(int yea, int mth, int dom, int hou, int min, int sec)
{
	unsigned long d1, d2, d3;

	mth -= 2;
	if (mth <= 0) {
		mth += 12;
		yea -= 1;
	}

	d1 = (yea - 1) * 365 + (yea / 4 - yea / 100 + yea / 400);
	d2 = (367 * mth / 12 - 30) + 59;
	d3 = d1 + d2 + (dom - 1) - 719162;

	return ((d3 * 24 + hou) * 60 + min) * 60 + sec;
}

static void rtc_write_trigger(void)
{
	RTC_Write(RTC_WRTGR, 1);
	rtc_busy_wait();
}

void rtc_writeif_unlock(void)
{
	RTC_Write(RTC_PROT, 0x586a);
	rtc_write_trigger();
	RTC_Write(RTC_PROT, 0x9136);
	rtc_write_trigger();
}

void rtc_writeif_lock(void)
{
	RTC_Write(RTC_PROT, 0);
	rtc_write_trigger();
}

void rtc_bbpu_power_down(void)
{
	U16 bbpu;

	/* pull PWRBB low */
	bbpu = RTC_BBPU_KEY | RTC_BBPU_AUTO | RTC_BBPU_PWREN;
	rtc_writeif_unlock();
	RTC_Write(RTC_BBPU, bbpu);
	rtc_write_trigger();
}

U16 rtc_rdwr_uart_bits(U16 *val)
{
	U16 pdn2;

	if (val) {
		pdn2 = RTC_Read(RTC_PDN2) & ~0x0060;
		pdn2 |= (*val & 0x0003) << 5;
		RTC_Write(RTC_PDN2, pdn2);
		rtc_write_trigger();
	}

	return (RTC_Read(RTC_PDN2) & 0x0060) >> 5;
}

bool rtc_boot_check(bool can_alarm_boot)
{
	U16 irqsta, pdn1, pdn2, spar0, spar1;

	irqsta = RTC_Read(RTC_IRQ_STA);	/* read clear */
	pdn1 = RTC_Read(RTC_PDN1);
	pdn2 = RTC_Read(RTC_PDN2);
	spar0 = RTC_Read(RTC_SPAR0);
	spar1 = RTC_Read(RTC_SPAR1);
	/*printf("irqsta = 0x%x, pdn1 = 0x%x, pdn2 = 0x%x, spar0 = 0x%x, spar1 = 0x%x\n",
	       irqsta, pdn1, pdn2, spar0, spar1);*/

	if (irqsta & RTC_IRQ_STA_AL) {
#if RTC_RELPWR_WHEN_XRST
		/* set AUTO bit because AUTO = 0 when PWREN = 1 and alarm occurs */
		U16 bbpu = RTC_Read(RTC_BBPU) | RTC_BBPU_KEY | RTC_BBPU_AUTO;
		RTC_Write(RTC_BBPU, bbpu);
		rtc_write_trigger();
#endif

		if (pdn1 & 0x0080) {	/* power-on time is available */
			U16 now_sec, now_min, now_hou, now_dom, now_mth, now_yea;
			U16 irqen, sec, min, hou, dom, mth, yea;
			unsigned long now_time, time;

			now_sec = RTC_Read(RTC_TC_SEC);
			now_min = RTC_Read(RTC_TC_MIN);
			now_hou = RTC_Read(RTC_TC_HOU);
			now_dom = RTC_Read(RTC_TC_DOM);
			now_mth = RTC_Read(RTC_TC_MTH);
			now_yea = RTC_Read(RTC_TC_YEA) + RTC_MIN_YEAR;
			if (RTC_Read(RTC_TC_SEC) < now_sec) {	/* SEC has carried */
				now_sec = RTC_Read(RTC_TC_SEC);
				now_min = RTC_Read(RTC_TC_MIN);
				now_hou = RTC_Read(RTC_TC_HOU);
				now_dom = RTC_Read(RTC_TC_DOM);
				now_mth = RTC_Read(RTC_TC_MTH);
				now_yea = RTC_Read(RTC_TC_YEA) + RTC_MIN_YEAR;
			}

			sec = spar0 & 0x003f;
			min = spar1 & 0x003f;
			hou = (spar1 & 0x07c0) >> 6;
			dom = (spar1 & 0xf800) >> 11;
			mth = pdn2 & 0x000f;
			yea = ((pdn2 & 0x7f00) >> 8) + RTC_MIN_YEAR;

			now_time = rtc_mktime(now_yea, now_mth, now_dom, now_hou, now_min, now_sec);
			time = rtc_mktime(yea, mth, dom, hou, min, sec);

			printf("now = %d/%d/%d %d:%d:%d (%lu)\n",
			       now_yea, now_mth, now_dom, now_hou, now_min, now_sec, now_time);
			printf("power-on = %d/%d/%d %d:%d:%d (%lu)\n",
			       yea, mth, dom, hou, min, sec, time);

			if (now_time >= time - 1 && now_time <= time + 4) {	/* power on */
				pdn1 &= ~(0x0080 | 0x0010 | 0x0040);
				RTC_Write(RTC_PDN1, pdn1);
				RTC_Write(RTC_PDN2, pdn2 | 0x0010);
				rtc_write_trigger();
				if (can_alarm_boot &&
				    !(pdn2 & 0x8000)) {		/* no logo means ALARM_BOOT */
					g_boot_mode = ALARM_BOOT;
				}
				return true;
			} else if (now_time < time) {	/* set power-on alarm */
				RTC_Write(RTC_AL_YEA, yea - RTC_MIN_YEAR);
				RTC_Write(RTC_AL_MTH, mth);
				RTC_Write(RTC_AL_DOM, dom);
				RTC_Write(RTC_AL_HOU, hou);
				RTC_Write(RTC_AL_MIN, min);
				RTC_Write(RTC_AL_SEC, sec);
				RTC_Write(RTC_AL_MASK, 0x0010);	/* mask DOW */
				rtc_write_trigger();
				irqen = RTC_Read(RTC_IRQ_EN) | RTC_IRQ_EN_ONESHOT_AL;
				RTC_Write(RTC_IRQ_EN, irqen);
				rtc_write_trigger();
			}
		}
	}

	if ((pdn1 & 0x0030) == 0x0010) {	/* factory data reset */
		RTC_Write(RTC_PDN1, pdn1 & ~0x0010);
		rtc_write_trigger();
		return true;
	}

	if (pdn1 & 0x0040) {	/* bypass power key detection */
		RTC_Write(RTC_PDN1, pdn1 & ~0x0040);
		rtc_write_trigger();
		return true;
	}

	return false;
}

//MODULE_AUTHOR("Terry Chang <terry.chang@mediatek.com>");
//MODULE_DESCRIPTION("MT6575 RTC U-Boot Driver v1.9");
