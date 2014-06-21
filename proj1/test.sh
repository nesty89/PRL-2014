#!/bin/bash


if [ $# -lt 1 ];then 
    numbers=10
else
    numbers=$1
fi;
# convert and copy file
# if= input file
# bs= pocet r/w Byte
# count= pocet cisel
# of= output file
dd if=/dev/random bs=1 count=$numbers of=numbers

#-np ... number of processors
mpirun --prefix /usr/local/share/OpenMPI -np $[ $numbers + 1 ] es

rm -f es numbers *.exe