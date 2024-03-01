#!/bin/bash
#SBATCH --job-name=seq_benchmark
#SBATCH --partition=intel-longq
#SBATCH --output=seq_benchmark_%A_%a.out
#SBATCH --ntasks=1

# Define the ranges
DATASETS=(15000 13000 100000)
ITERATIONS=5

# Sequential Flag
gcd_version="--seq"

# Select GCD version based on SLURM_ARRAY_TASK_ID, It is Euclidean if no flag is specified
if [ "${SLURM_ARRAY_TASK_ID}" -eq 2 ]; then
    gcd_version="--binary"
fi

# Loop through each dataset
for ds in "${!DATASETS[@]}"; do
  for ITER in $(seq 1 $ITERATIONS); do
    srun -c1 ./program 1 "${DATASETS[$ds]}" 1 static 1 $gcd_version --seq --filename intel_seq_metrics_ds"${ds}".csv
  done
done

