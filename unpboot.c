/*
 * unpboot.c
 *
 * This utility unpacks a boot image into its individual components.
 *
 * It assumes the following layout:
 * - 0x0000           : Header
 * - 0x0200           : Command line arguments
 * - 0x07E0           : Kernel size (4 bytes)
 * - 0x07E4           : Initrd size (4 bytes)
 * - 0x13E0           : Bootstub (either 4KB or 8KB)
 * - [directly after] : bzImage (Kernel)
 * - [directly after] : initrd.cpio.gz (Ramdisk)
 */

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXLEN 16384 * 1024 // The boot image is limited to 16MB maximum

// Write 'len' bytes from 'ptr' to file 'name'
void wrt(char *name, char *ptr, int len) {
  int ofd = open(name, O_WRONLY | O_CREAT, 0660);
  write(ofd, ptr, len);
  close(ofd);
}

int main(int argc, char *argv[]) {
  int bootstub_size = 8192; // Default to a bootstub of 8KB
  int opt;

  // Allow specifying bootstub size with -s
  while ((opt = getopt(argc, argv, "s:")) != -1) {
    switch (opt) {
    case 's':
      bootstub_size = atoi(optarg);
      break;
    default:
      fprintf(stderr, "Usage: %s [-s bootstub_size] boot.img\n", argv[0]);
      return 1;
    }
  }

  if (optind >= argc) {
    fprintf(stderr, "Usage: %s [-s bootstub_size] boot.img\n", argv[0]);
    return 1;
  }

  // Allocate a buffer for loading the boot image
  char *buf = (char *)malloc(MAXLEN);

  int ifd = open(argv[optind], O_RDONLY);
  if (ifd < 0) {
    printf("cannot open %s\n", argv[optind]);
    return 1;
  }
  int len = read(ifd, buf, MAXLEN);
  close(ifd);

  // Extract header (first 512 bytes)
  wrt("header.bin", buf, 512);

  // Extract cmdline (starts at 0x200 + 480 offset?)
  wrt("cmdline", buf + 512 + 480, strlen(buf + 512 + 480));

  // Read kernel and initrd sizes from fixed offsets
  int klen = *(int *)(buf + 0x7e0);
  int rlen = *(int *)(buf + 0x7e4);

  // Extract bootstub
  buf += 0x13e0;
  wrt("bootstub", buf, bootstub_size);
  buf += bootstub_size;

  // Extract kernel
  wrt("bzImage", buf, klen);

  // Extract initrd
  wrt("initrd.cpio.gz", buf + klen, rlen);

  return 0;
}
