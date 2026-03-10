#!/bin/bash
#SBATCH --job-name=multithreading_example
#SBATCH --time=0-0:10
#SBATCH --partition=amd-512
#SBATCH --cpus-per-task=32
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

./vadd_par
