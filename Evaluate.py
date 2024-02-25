import subprocess

# Testing Parameters
num_threads_options = [1, 2, 4, 8]
scheduling_strategies = ['static', 'dynamic', 'guided', 'auto']
chunk_sizes = [1, 10, 100, 1000]

# Function to call the C program with given parameters
def run(num_threads, scheduling_strategy, chunk_size):
    # Construct command to run C program with arguments
    command = ['./SumTot', str(num_threads), scheduling_strategy, str(chunk_size)]

    # Run the command and capture output
    result = subprocess.run(command, capture_output=True, text=True)

    # Return the output or any part of it you're interested in
    return result.stdout