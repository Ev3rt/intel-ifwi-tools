/*
 * mkboot.c
 *
 * This utility packs the command line, bootstub, kernel image and initrd into a
 * single, unsigned boot image.
 *
 * Buffer Layout (maximum 16MB):
 * 0x0000                                   : Kernel command line
 * 0x0400 - 0x0403                          : Kernel size
 * 0x0404 - 0x0407                          : Initrd size
 * 0x040c - 0x417                           : Magic values
 * 0x1000                                   : bootstub (either 4KB or 8KB)
 * 0x1000 + [bootstub size]                 : Kernel image
 * 0x1000 + [bootstub size] + [kernel size] : initrd.cpio.gz
 */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAXLEN 16384 * 1024 // The boot image is limited to 16MB maximum

// Write 'len' bytes from 'ptr' to file 'name'
void wrt(char *name, char *ptr, int len) {
  int ofd = open(name, O_WRONLY | O_CREAT | O_TRUNC, 0660);
  write(ofd, ptr, len);
  close(ofd);
}

// Read file 'name' into buffer 'ptr'. Returns number of bytes read.
int rrd(char *name, char *ptr) {
  int ifd = open(name, O_RDONLY);
  if (ifd < 0) {
    printf("cannot open %s\n", name);
    return 1;
  }
  int len = read(ifd, ptr, MAXLEN);
  printf("read len=%x\n", len);
  close(ifd);
  return len;
}

int main(int argc, char *argv[]) {
  int bootstub_size = 8192; // Default a bootstub of 8KB
  int opt;

  // Allow specifying bootstub size with -s
  while ((opt = getopt(argc, argv, "s:")) != -1) {
    switch (opt) {
    case 's':
      bootstub_size = atoi(optarg);
      break;
    default:
      fprintf(stderr, "Usage: %s [-s bootstub_size]\n", argv[0]);
      return 1;
    }
  }

  // Allocate a buffer for constructing the new boot image
  char *buf = (char *)malloc(MAXLEN);
  memset(buf, 0, MAXLEN);

  // 0x0000: Kernel command line
  rrd("cmdline", buf);
  // 0x1000: Bootstub
  rrd("bootstub", buf + 0x1000);
  // 0x2000 or 0x3000 (depending on bootstub size): Kernel image
  int klen = rrd("bzImage", buf + 0x1000 + bootstub_size);

  // Append initrd immediately after the kernel
  int rlen = rrd("initrd.cpio.gz", buf + 0x1000 + bootstub_size + klen);

  // Write kernel and initrd size to 0x400, 0x404
  *(int *)(buf + 0x400) = klen;
  *(int *)(buf + 0x404) = rlen;

  // Magic values
  // These are only used in the 'new' format with also has the larger bootstub.
  if (bootstub_size == 8192) {
    buf[0x40c] = 0xff;
    buf[0x410] = 0xbd;
    buf[0x411] = 0x02;
    buf[0x412] = 0xbd;
    buf[0x413] = 0x02;
    buf[0x414] = 0xbd;
    buf[0x415] = 0x12;
    buf[0x416] = 0xbd;
    buf[0x417] = 0x12;
  }

  // Write the resulting image
  wrt("unsigned.bin", buf, 0x1000 + bootstub_size + klen + rlen);

  return 0;
}
