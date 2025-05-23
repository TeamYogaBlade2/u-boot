#ifndef __MT6575_IRQ_H__
#define __MT6575_IRQ_H__

#define GIC_CPU_CTRL			0x00
#define GIC_CPU_PRIMASK			0x04
#define GIC_CPU_BINPOINT		0x08
#define GIC_CPU_INTACK			0x0c
#define GIC_CPU_EOI			0x10
#define GIC_CPU_RUNNINGPRI		0x14
#define GIC_CPU_HIGHPRI			0x18

#define GIC_DIST_CTRL			0x000
#define GIC_DIST_CTR			0x004
#define GIC_DIST_ENABLE_SET		0x100
#define GIC_DIST_ENABLE_CLEAR		0x180
#define GIC_DIST_PENDING_SET		0x200
#define GIC_DIST_PENDING_CLEAR		0x280
#define GIC_DIST_ACTIVE_BIT		0x300
#define GIC_DIST_PRI			0x400
#define GIC_DIST_TARGET			0x800
#define GIC_DIST_CONFIG			0xc00
#define GIC_DIST_SOFTINT		0xf00

enum {IRQ_MASK_HEADER = 0xF1F1F1F1, IRQ_MASK_FOOTER = 0xF2F2F2F2};

struct mtk_irq_mask
{
    unsigned int header;   /* for error checking */
    unsigned int mask0;
    unsigned int mask1;
    unsigned int mask2;
    unsigned int mask3;
    unsigned int mask4;
    unsigned int footer;   /* for error checking */
};


/*
 * Define hadware registers.
 */

/*
 * Define IRQ code.
 */

#define GIC_PRIVATE_SIGNALS (32)

#define GIC_PPI_OFFSET          (27)
#define GIC_PPI_GLOBAL_TIMER    (GIC_PPI_OFFSET + 0)
#define GIC_PPI_LEGACY_FIQ      (GIC_PPI_OFFSET + 1)
#define GIC_PPI_PRIVATE_TIMER   (GIC_PPI_OFFSET + 2)
#define GIC_PPI_WATCHDOG_TIMER  (GIC_PPI_OFFSET + 3)
#define GIC_PPI_LEGACY_IRQ      (GIC_PPI_OFFSET + 4)

#define MT6575_L2CCINTR_IRQ_ID              (GIC_PRIVATE_SIGNALS + 0)
#define MT6575_SCUEVABORT_IRQ_ID            (GIC_PRIVATE_SIGNALS + 1)
#define MT6575_NEON_EXCEPTION0_IRQ_ID       (GIC_PRIVATE_SIGNALS + 2)
#define MT6575_CA9MP_PARITYFAIL0_IRQ_ID     (GIC_PRIVATE_SIGNALS + 3)
// (GIC_PRIVATE_SIGNALS + 4) is NA
#define MT6575_PMUIRQ_IRQ_ID                (GIC_PRIVATE_SIGNALS + 5)
#define MT6575_NCTIIRQ_IRQ_ID               (GIC_PRIVATE_SIGNALS + 6)
// (GIC_PRIVATE_SIGNALS + 7) is NA
#define MT6575_USB0_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 8)
#define MT6575_USB1_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 9)
#define MT6575_DSI_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 10)
#define MT6575_DPI_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 11)
#define MT6575_TVE_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 12)
#define MT6575_TVC_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 13)
#define MT6575_CAM_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 14)
#define MT6575_CRZ_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 15)
#define MT6575_JPEG_CODEC_IRQ_ID            (GIC_PRIVATE_SIGNALS + 16)
#define MT6575_EIS_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 17)
#define MT6575_LCD_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 18)
#define MT6575_TV_ROT_IRQ_ID                (GIC_PRIVATE_SIGNALS + 19)
#define MT6575_ROT_DMA_IRQ_ID               (GIC_PRIVATE_SIGNALS + 20)
#define MT6575_OVL_JPG_DMA_IRQ_ID           (GIC_PRIVATE_SIGNALS + 21)
#define MT6575_SCAM_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 22)
#define MT6575_AFE_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 23)
#define MT6575_MD_WDT_DSP_IRQ_ID            (GIC_PRIVATE_SIGNALS + 24)
#define MT6575_MD_WDT_IRQ_ID                (GIC_PRIVATE_SIGNALS + 25)
#define MT6575_DMA_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 26)
#define MT6575_WDT_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 27)
#define MT6575_SMI_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 28)
#define MT6575_SLEEP_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 29)
#define MT6575_TS_IRQ_ID                    (GIC_PRIVATE_SIGNALS + 30)
#define MT6575_TS_BATCH_IRQ_ID              (GIC_PRIVATE_SIGNALS + 31)
#define MT6575_LOWBATTERY_IRQ_ID            (GIC_PRIVATE_SIGNALS + 32)
#define MT6575_DCC_APARM_IRQ_ID             (GIC_PRIVATE_SIGNALS + 33)
#define MT6575_APARM_CTI_IRQ_ID             (GIC_PRIVATE_SIGNALS + 34)
#define MT6575_APARM_DOMAIN_IRQ_ID          (GIC_PRIVATE_SIGNALS + 35)
#define MT6575_APARM_DECERR_IRQ_ID          (GIC_PRIVATE_SIGNALS + 36)
#define MT6575_ACCDET_IRQ_ID                (GIC_PRIVATE_SIGNALS + 37)
#define MT6575_RTC_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 38)
#define MT6575_SIM0_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 39)
#define MT6575_SIM1_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 40)
#define MT6575_PWM_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 41)
#define MT6575_THERM_CTR_IRQ_ID             (GIC_PRIVATE_SIGNALS + 42)
#define MT6575_AP_CCIF_IRQ_ID               (GIC_PRIVATE_SIGNALS + 43)
#define MT6575_MSDC1_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 44)
#define MT6575_MSDC3_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 45)
#define MT6575_MSDC2_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 46)
#define MT6575_MSDC0_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 47)
#define MT6575_AP_HIF_IRQ_ID                (GIC_PRIVATE_SIGNALS + 48)
#define MT6575_I2C0_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 49)
#define MT6575_I2C1_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 50)
#define MT6575_UART1_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 51)
#define MT6575_UART2_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 52)
#define MT6575_UART3_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 53)
#define MT6575_UART4_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 54)
#define MT6575_NFIECC_IRQ_ID                (GIC_PRIVATE_SIGNALS + 55)
#define MT6575_NFI_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 56)
#define MT6575_GPT_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 57)
#define MT6575_GDMA1_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 58)
#define MT6575_GDMA2_IRQ_ID                 (GIC_PRIVATE_SIGNALS + 59)
#define MT6575_DMA_AP_HIF_IRQ_ID            (GIC_PRIVATE_SIGNALS + 60)
#define MT6575_DMA_MD_HIF_IRQ_ID            (GIC_PRIVATE_SIGNALS + 61)
#define MT6575_DMA_SIM0_IRQ_ID              (GIC_PRIVATE_SIGNALS + 62)
#define MT6575_DMA_SIM1_IRQ_ID              (GIC_PRIVATE_SIGNALS + 63)
#define MT6575_DMA_IRDA_IRQ_ID              (GIC_PRIVATE_SIGNALS + 64)
#define MT6575_DMA_UART0_TX_IRQ_ID          (GIC_PRIVATE_SIGNALS + 65)
#define MT6575_DMA_UART0_RX_IRQ_ID          (GIC_PRIVATE_SIGNALS + 66)
#define MT6575_DMA_UART1_TX_IRQ_ID          (GIC_PRIVATE_SIGNALS + 67)
#define MT6575_DMA_UART1_RX_IRQ_ID          (GIC_PRIVATE_SIGNALS + 68)
#define MT6575_DMA_UART2_TX_IRQ_ID          (GIC_PRIVATE_SIGNALS + 69)
#define MT6575_DMA_UART2_RX_IRQ_ID          (GIC_PRIVATE_SIGNALS + 70)
#define MT6575_KP_IRQ_ID                    (GIC_PRIVATE_SIGNALS + 71)
#define MT6575_IRDA_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 72)
#define MT6575_I2C_DUAL_IRQ_ID              (GIC_PRIVATE_SIGNALS + 73)
#define MT6575_SMI_LARB0_VAMAU_IRQ_ID       (GIC_PRIVATE_SIGNALS + 74)
#define MT6575_SMI_LARB0_MMU_IRQ_ID         (GIC_PRIVATE_SIGNALS + 75)
#define MT6575_SMI_LARB1_VAMAU_IRQ_ID       (GIC_PRIVATE_SIGNALS + 76)
#define MT6575_SMI_LARB1_MMU_IRQ_ID         (GIC_PRIVATE_SIGNALS + 77)
#define MT6575_SMI_LARB2_VAMAU_IRQ_ID       (GIC_PRIVATE_SIGNALS + 78)
#define MT6575_SMI_LARB2_MMU_IRQ_ID         (GIC_PRIVATE_SIGNALS + 79)
#define MT6575_SPI_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 80)
#define MT6575_VENC_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 81)
#define MT6575_VDEC_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 82)
#define MT6575_CSI2_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 83)
#define MT6575_R_DMA0_IRQ_ID                (GIC_PRIVATE_SIGNALS + 84)
#define MT6575_RESZ_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 85)
#define MT6575_DISP_MDP_IRQ_ID              (GIC_PRIVATE_SIGNALS + 86)
#define MT6575_G2D_IRQ_ID                   (GIC_PRIVATE_SIGNALS + 87)
#define MT6575_MFG_THALIA_IRQ_ID            (GIC_PRIVATE_SIGNALS + 88)
#define MT6575_LARB3_VAMAU_IRQ_ID           (GIC_PRIVATE_SIGNALS + 89)
#define MT6575_LARB3_MMU_IRQ_ID             (GIC_PRIVATE_SIGNALS + 90)
#define MT6575_AFE_MCU_IRQ_ID               (GIC_PRIVATE_SIGNALS + 91)
#define MT6575_DMA_FIQ_IRQ_ID               (GIC_PRIVATE_SIGNALS + 92)
#define MT6575_MD_UART2_IRQ_ID              (GIC_PRIVATE_SIGNALS + 93)
#define MT6575_MD_UART1_IRQ_ID              (GIC_PRIVATE_SIGNALS + 94)
// (GIC_PRIVATE_SIGNALS + 95) is NA
#define MT6575_EINT_IRQ_ID                  (GIC_PRIVATE_SIGNALS + 96)
#define MT6575_EINT_DIRECT0_IRQ_ID          (GIC_PRIVATE_SIGNALS + 97)
#define MT6575_EINT_DIRECT1_IRQ_ID          (GIC_PRIVATE_SIGNALS + 98)
#define MT6575_EINT_DIRECT2_IRQ_ID          (GIC_PRIVATE_SIGNALS + 99)
#define MT6575_EINT_DIRECT3_IRQ_ID          (GIC_PRIVATE_SIGNALS + 100)
#define MT6575_EINT_DIRECT4_IRQ_ID          (GIC_PRIVATE_SIGNALS + 101)
#define MT6575_EINT_DIRECT5_IRQ_ID          (GIC_PRIVATE_SIGNALS + 102)
#define MT6575_EINT_DIRECT6_IRQ_ID          (GIC_PRIVATE_SIGNALS + 103)
#define MT6575_EINT_DIRECT7_IRQ_ID          (GIC_PRIVATE_SIGNALS + 104)

#define MT6575_APARM_GPTTIMER_IRQ_LINE  MT6575_GPT_IRQ_ID   // alias name for GPT
#define MT6575_CAM_IRQ_LINE             MT6575_CAM_IRQ_ID   // alias name for CAM

#define MT6575_NR_PPI   (5)
#define MT6575_NR_SPI   (128)
#define NR_MT6575_IRQ_LINE  (GIC_PPI_OFFSET + MT6575_NR_PPI + MT6575_NR_SPI)    // 5 PPIs and 128 SPIs

#define MT65xx_EDGE_SENSITIVE 0
#define MT65xx_LEVEL_SENSITIVE 1

#define MT65xx_POLARITY_LOW   0
#define MT65xx_POLARITY_HIGH  1

void mt6575_irq_set_sens(unsigned int irq, unsigned int sens);
void mt6575_irq_set_polarity(unsigned int irq, unsigned int polarity);

#endif  /* !__MT6575_IRQ_H__ */
