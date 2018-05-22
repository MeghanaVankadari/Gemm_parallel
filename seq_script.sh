#!/bin/sh

max=1024
for((size=32;size<=$max;size=$size*2))
do
	./omp_gemm  $size 1000
done
