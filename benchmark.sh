#!/bin/bash

CORES=`nproc --all`

for THREADS in 1 2 4 $(($CORES - 1)); do
	echo $THREADS threads

	for TEST in 1 2 3; do
		make clean
		sleep 5
		time -p bash -c "make -j$THREADS >> /dev/null 2>&1"

		RETURN=$?

		if [ $RETURN -ne 0 ]; then
			echo "FAILED"
			exit 1
		fi
		echo

	done

	echo
	echo
	echo
done
