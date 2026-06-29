TODO (sorted by priority):
- USB
- pinctrl
- MMC
- I2C
- flatten memory ranges

status:
- PSCI: ok

how to boot:
- `make ARCH=arm CROSS_COMPILE=arm-none-eabi- -j16 mt6595_defconfig`
- `make ARCH=arm CROSS_COMPILE=arm-none-eabi- -j16`
- `python mk6595img.py`
`./da-boot --lk ../mt6595/backup/lk.bin -p ../mt6595/backup/preloader.bin --kernel ../mt6595/u-boot/zImage-u-boot --input test.bin@0x42000000 --input ../mt6595/linux/arch/arm/boot/dts/mediatek/mt6595-lenovo-x2eu.dtb@0x49000000 --dram-size-per-rank 0x40000000 --dram-ranks 2 lk`
- flashing not tested
