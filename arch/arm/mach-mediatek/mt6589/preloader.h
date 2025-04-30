// mediatek/platform/mt6589/uboot/inc/asm/arch/boot_mode.h

/* MT6577 boot type definitions */
typedef enum
{
	NORMAL_BOOT = 0,
	META_BOOT = 1,
	RECOVERY_BOOT = 2,
	SW_REBOOT = 3,
	FACTORY_BOOT = 4,
	ADVMETA_BOOT = 5,
	ATE_FACTORY_BOOT = 6,
	ALARM_BOOT = 7,
	UNKNOWN_BOOT
} BOOTMODE;
