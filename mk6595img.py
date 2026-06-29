import struct


def main():
    with open("u-boot.bin", "rb") as f:
        uboot_data = f.read()
    with open("arch/arm/dts/mt6595-lenovo-x2eu.dtb", "rb") as f:
        dtb_data = f.read()

    header_size = 64
    header = bytearray(header_size)

    # B 0x40
    struct.pack_into("<I", header, 0x00, 0xEA00000E)

    # zImage magic
    struct.pack_into("<I", header, 0x24, 0x016F2818)

    # zImage offset
    struct.pack_into("<I", header, 0x28, 0x00000000)

    # zImage end
    zimage_end = header_size + len(uboot_data)
    struct.pack_into("<I", header, 0x2C, zimage_end)

    payload = header + uboot_data + dtb_data
    with open("zImage-u-boot", "wb") as f:
        f.write(payload)

    print(f"zImage-u-boot is {len(payload)} bytes")


if __name__ == "__main__":
    main()
