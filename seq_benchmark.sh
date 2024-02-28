#!/bin/bash
#SBATCH --job-name=seq_benchmark
#SBATCH --partition=amd-longq
#SBATCH --output=seq_benchmark_%A_%a.out
#SBATCH --array=1-2
#SBATCH --ntasks=1

# Define the ranges
declare -A datasets=( [1]=15000 [2]=30000 [3]=100000 )

# Sequential Flag
gcd_version="--seq"

# Select GCD version based on SLURM_ARRAY_TASK_ID, It is Euclidean if no flag is specified
if [ "${SLURM_ARRAY_TASK_ID}" -eq 2 ]; then
    gcd_version="--binary"
fi

# Loop through each dataset
for ds in "${!datasets[@]}"
do
    srun -c1 ./program 1 "${datasets[$ds]}" 1 static 1 $gcd_version --filename seq_metrics_ds"${ds}"_gcd"${SLURM_ARRAY_TASK_ID}".csv
done
