#!/bin/bash -e
# based on https://github.com/catfish03/u-boot-mt65xx/blob/44dd1d7268b970ec30ce7aece87f2c57c8771e49/build_mt6572.sh

export ARCH=arm
export CROSS_COMPILE=arm-none-eabi-

rm_prev_file() {
  prev_file="u-boot-mtk.img"
  if [ -f $prev_file ]; then
    echo "previous u-boot image found, removing..."
    rm $prev_file
  else
    echo "previous u-boot image not found..."
  fi
}

build_uboot() {
  echo "building u-boot..."
  make lenovo-blade_defconfig
  make -j$(nproc --all)
  echo "u-boot build is done!"
}

make_android_bootimg() {
  echo "creating a dummy ramdisk..."
  dd if=/dev/random of=/tmp/ramdisk-dummy bs=2048 count=9
  echo "prepending the mtk rootfs header to dummy ramdisk..."
  tools/mtk-mkimage ROOTFS /tmp/ramdisk-dummy /tmp/ramdisk-dummy.mtk

  echo "prepending the mtk kernel header to u-boot image..."
  tools/mtk-mkimage KERNEL u-boot.bin /tmp/u-boot.bin.mtk

  echo "creating an android boot.img..."
  # mkbootimg-osm0sis --kernel /tmp/u-boot.bin.mtk --ramdisk /tmp/ramdisk-dummy.mtk -o u-boot-mtk.img
  mkbootimg --kernel /tmp/u-boot.bin.mtk --ramdisk /tmp/ramdisk-dummy.mtk -o u-boot-mtk.img
  echo "done!"
  echo "filename: u-boot-mtk.img"
}

tmp_cleanup() {
  echo "cleaning up..."
  rm /tmp/ramdisk-dummy
  rm /tmp/ramdisk-dummy.mtk
  rm /tmp/u-boot.bin.mtk
  echo "done!"
}

main() {
  rm_prev_file
  build_uboot
  make_android_bootimg
  tmp_cleanup
}

main
