#!/bin/sh

BOOTSTUB_SIZE=""

# Parse command line options
while getopts "s:" opt; do
  case $opt in
    s) BOOTSTUB_SIZE="-s $OPTARG" ;;
    \?) echo "Invalid option -$OPTARG" >&2; exit 1 ;;
  esac
done

shift $((OPTIND-1))

if [ -z "$1" ]; then
    echo "Usage: $0 [-s bootstub_size] boot.img"
    exit 1
fi

# Create unpack directory
mkdir $1.unp
cd $1.unp

# Unpack boot image components
../unpboot $BOOTSTUB_SIZE ../$1

# Unpack initrd
mkdir initrd
cd initrd
gzip -c -d ../initrd.cpio.gz | cpio -iv
cd ../..

