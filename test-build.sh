#!/bin/sh

build_it ()
{
    PROD=$1
    echo "---- Building $PROD -------------------" >> test-build.log
    make PRODUCT=$PROD clean
    make PRODUCT=$PROD >> test-build.log
}


echo "Start of Test ===============================================" > test-build.log
build_it HDAK1ST
build_it BLK1CAM
build_it RFIDLOCK
build_it WAAGE
build_it WHDTX
echo "Test Builds Done ===============================================" >> test-build.log
