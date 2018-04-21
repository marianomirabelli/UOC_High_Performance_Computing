#!/bin/bash
#$ -cwd
#$ -V
#$ -S /bin/bash
#$ -N original_barrier
#$ -o original_barrier_omp.out
#$ -e original_barrier_omp.err

export OMP_NUM_THREADS="$1"
SIZE="$2"
ITERS="$3"
IMBALANCES="$4"
IMBALANCES_BASE="$5"


mpirun ./barr_sync_with_openmp "$SIZE" "$ITERS" "$IMBALANCES" "$IMBALANCES_BASE"