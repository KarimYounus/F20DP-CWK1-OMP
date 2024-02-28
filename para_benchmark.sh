#!/bin/bash

# Slurm script template
read -r -d '' SLURM_SCRIPT_TEMPLATE << 'EOF'
#!/bin/bash
#SBATCH --job-name=<JOB_NAME>
#SBATCH --ntasks=1
#SBATCH --partition=amd-longq
#SBATCH --output=/dev/null
#SBATCH -c<CORES>

./program 1 <DS> <CORES> <SCHED> <CHUNK> --filename <JOB_NAME>.csv
EOF

# Parameters to vary
CORE_COUNTS=(2 4 8 16 32 64)
SCHED_STRATEGIES=("static" "dynamic" "guided")
CHUNK_SIZES=(1 10 20 40 60 80 100 120 140 160)
DATASETS=("15000" "30000" "100000")
ITERATIONS=5

# Directory to store Slurm scripts
SLURM_SCRIPTS_DIR="./slurm_scripts"
mkdir -p "${SLURM_SCRIPTS_DIR}"

# Loop through datasets
for DS in "${DATASETS[@]}"; do
  # Loop through core counts
  for CORES in "${CORE_COUNTS[@]}"; do
    # Loop through scheduling strategies
    for SCHED in "${SCHED_STRATEGIES[@]}"; do
      # Loop through chunk sizes
      for CHUNK in "${CHUNK_SIZES[@]}"; do
        # Construct job name
        JOB_NAME="para_benchmark_${DS}"
        # Create a Slurm script for the current configuration
        SLURM_SCRIPT="${SLURM_SCRIPTS_DIR}/${JOB_NAME}.slurm"
        echo "${SLURM_SCRIPT_TEMPLATE}" > "${SLURM_SCRIPT}"
        sed -i "s/<JOB_NAME>/${JOB_NAME}/g" "${SLURM_SCRIPT}"
        sed -i "s/<CORES>/${CORES}/g" "${SLURM_SCRIPT}"
        sed -i "s/<DS>/${DS}/g" "${SLURM_SCRIPT}"
        sed -i "s/<SCHED>/${SCHED}/g" "${SLURM_SCRIPT}"
        sed -i "s/<CHUNK>/${CHUNK}/g" "${SLURM_SCRIPT}"
        # Submit the job using sbatch
        echo "Submitting: sbatch ${SLURM_SCRIPT}"
        for ITER in $(seq 1 $ITERATIONS); do
          sbatch "${SLURM_SCRIPT}"
        done
      done
    done
  done
done
