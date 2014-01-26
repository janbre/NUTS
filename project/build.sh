#!/bin/bash
if [ $# -lt 1 ] ; then
    echo No output directory set
    echo Example usage: ./build.sh /home/\<user\>/csp
    exit 0
else
    ./waf clean configure --prefix=$1 --install-csp --enable-promisc --enable-if-kiss --with-driver-usart=linux --enable-crc32 --enable-qos --enable-rdp --with-max-connections=20 --enable-hmac --enable-examples --enable-seqnr --with-os=posix --with-loglevel=info build install
fi
