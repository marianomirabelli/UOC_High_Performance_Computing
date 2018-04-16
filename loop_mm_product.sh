#!/bin/bash
schedule="$1"
for ((c=1;c<=4;c++))
do
  	size=$((c*100))
  	for ((d=1;d<=4;d++))
	do
		qsub mm_parallel_omp.sge "$d" "$schedule" "$size"
	done
done
