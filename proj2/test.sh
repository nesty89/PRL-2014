#!/bin/bash


if [ $# -lt 1 ];then 
    numbers=4
else
    numbers=$1
fi;


proc=$[ $numbers *2 - 1 ]
#-np ... number of processors
mpirun --prefix /usr/local/share/OpenMPI -np $proc  mm

rm -f mm numbers *.exe
