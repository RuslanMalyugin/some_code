#!/bin/bash
file="$1.s"
filec="$2.c"
fileexe="$2.exe"
arm-linux-gnueabi-gcc -marm ${filec} ${file} -o ${fileexe}
qemu-arm -L ~/Documents/acos/inf01-0/gcc-linaro-7.3.1-2018.05-i686_arm-linux-gnueabi/arm-linux-gnueabi/libc ${fileexe}
