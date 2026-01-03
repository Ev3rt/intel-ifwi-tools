#!/bin/sh

BOOTSTUB_SIZE=""

# Parse command line option for bootstub size, if specified
while getopts "s:" opt; do
  case $opt in
    s) BOOTSTUB_SIZE="-s $OPTARG" ;;
    \?) echo "Invalid option -$OPTARG" >&2; exit 1 ;;
  esac
done

shift $((OPTIND-1))

if [ -z "$1" ]; then
    echo "Usage: $0 [-s bootstub_size] directory"
    exit 1
fi

cd $1.unp

# Repack initrd
cd initrd && find . | cpio -o -H newc | gzip -c -9 >../initrd.cpio.gz && cd ..

# Create an unsigned boot image
../mkboot $BOOTSTUB_SIZE

# Sign the boot image using the Intel Signing Utility
../isu -i unsigned.bin -o signed.bin -t 4 -l ../key.pem

# Prepend the header to the signed boot image
../mkbootfinal

# Copy result to parent directory
cp final.bin ../patched.$1
