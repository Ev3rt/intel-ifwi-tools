/*
 * mkbootfinal.c
 *
 * This utility packs a header file and a signed boot image into a
 * final, flashable boot image.
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
  // Allocate a buffer for constructing the new boot image
  char *buf = (char *)malloc(MAXLEN);

  // Initialize the buffer with 0xff bytes
  memset(buf, 0xff, MAXLEN);

  // Read header into the buffer
  rrd("header.bin", buf);

  // Read the signed boot image into the buffer
  int klen = rrd("signed.bin", buf + 512);

  // Align the file to a 512-byte boundary
  klen += 511;
  klen -= klen % 512;

  // Store the number of 512-byte blocks at offset 0x30 in the header
  *(int *)(buf + 0x30) = klen / 512;

  // Write the final boot image
  wrt("final.bin", buf, 512 + klen);

  return 0;
}
