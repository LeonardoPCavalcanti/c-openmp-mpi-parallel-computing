#!/bin/bash
#SBATCH --partition=gpu-8-v100 
#SBATCH --gpus-per-node=1
#SBATCH --cpus-per-task=32        
#SBATCH --time=0-1:00
export OMP_NUM_THREADS=$SLURM_CPUS_PER_TASK

./heat_target_map 32000 10