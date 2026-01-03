# intel-ifwi-study

This repository contains several utilities for modifying Intel MID (Mobile Internet Devices) boot images, specifically for Clover Trail (Atom Z25xx/Z27xx) devices.

## Scripts

### mkboot.sh

A wrapper script that rebuilds the initrd, packs the boot image, signs it, and creates the final, flashable boot image.

```bash
./mkboot.sh [-s bootstub_size] <directory_name>
```

### unpboot.sh

A wrapper script that unpacks a boot image and subsequently unpacks the initrd.

```bash
./unpboot.sh [-s bootstub_size] <boot_image>
```

## Programs

### mkboot

Packs boot components into a single unsigned binary image.

```bash
./mkboot [-s bootstub_size]
```

**Options:**

- `-s`: Size of the bootstub in bytes (default: 8192). Use 4096 for some older Android devices/versions.

Run in a directory containing `cmdline`, `bootstub`, `bzImage`, and `initrd.cpio.gz`. `unsigned.bin` will be created.

### mkbootfinal

Combines a header and a signed binary payload into a final bootable image.

```bash
./mkbootfinal
```

Run in a directory containing `header.bin` and `signed.bin`. `final.bin` will be created.

### pack

Repacks an Intel IFWI (Integrated Firmware Image).

```bash
./pack
```

Run in a directory containing the following files:

- `1-mip.part`
- `2-ifwi.part`
- `3-psfw1.part`
- `4-ssfw.part`
- `5-psfw2.part`
- `6-vfw.part`
- `7-sucp.part`

`newifwi.bin` will be created.

### unp

Unpacks an Intel IFWI binary.

```bash
./unp <ifwi_file>
```

Files

- `1-mip.part`
- `2-ifwi.part`
- `3-psfw1.part`
- `4-ssfw.part`
- `5-psfw2.part`
- `6-vfw.part`
- `7-sucp.part`

will be created.

### unpboot

Unpacks a boot image.

```bash
./unpboot [-s bootstub_size] <boot_image>
```

**Options:**

- `-s`: Size of the bootstub in bytes (default: 8192). Use 4096 for some Android versions.

`header.bin`, `cmdline`, `bootstub`, `bzImage`, and `initrd.cpio.gz` will be created.
