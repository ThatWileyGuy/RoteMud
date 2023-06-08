#!/bin/bash

set -e
#set -x

NUM_CORES=`nproc`

for threads in 1 2 4 $(($NUM_CORES - 1))
do
	echo $threads threads
	for i in 1 2 3
	do
		echo pass $i
		make clean
		sleep 20
		time -p bash -c "make -j${threads} >> /dev/null 2>&1"
	done
done
