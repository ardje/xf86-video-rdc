#/bin/sh
#
# Utility for making xorg driver
# Author : Jason Lin
# Date   : 2009/8/21
#
# Functions :
#
# *2009/8/21
#   1. Build driver for xorg-server 1.2.0 ~ 1.6.0 at once.

#
# Path setting
#
XORG_LIB=/home/pub/Xorg
DRV_OUT_DIR=./
DRV_NAME=rdcm12_drv.so

#
# Global variables
#
VER_LST=`ls $XORG_LIB`


echo ================================
echo Driver make tool
echo type MakDriver.sh -h for usage
echo ================================

#
# Build all driver according the number of library in $XORG_LIB
#

VER_LST=1.7.5

for VER in $VER_LST; do
    echo ----------------------------------------------------
    echo Compiling driver for xorg-server $VER
    echo Making distclean
    make distclean
    echo Compiling
    export PKG_CONFIG_PATH=$XORG_LIB/$VER/lib/pkgconfig
    echo $PKG_CONFIG_PATH
    export ACLOCAL="aclocal -I $XORG_LIB/$VER/share/aclocal"
    echo $ACLOCAL
    ./autogen.sh
    make
    echo Compiled
    mkdir $DRV_OUT_DIR/$VER
    echo Copy $DRV_NAME to $DRV_OUT_DIR/$VER
    cp $DRV_OUT_DIR/src/.libs/$DRV_NAME $DRV_OUT_DIR/$VER/$DRV_NAME
    echo ----------------------------------------------------
done
