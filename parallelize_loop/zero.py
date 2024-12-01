#!/usr/bin/env python3
import subprocess
import json
import matplotlib.pyplot as plt
import numpy as np
import os

def run_mpi_program(threads):
    os.environ['OMP_NUM_THREADS'] = f'{threads}' 

    cmd = ['./a.out']
    result = subprocess.run(cmd, capture_output=True, text=True)
    return float(result.stdout.split()[-1])

def main():
    cmd = ['g++', '-fopenmp', 'zero.cpp']
    subprocess.run(cmd, check=True)

    num_threads = range(1, 20)  # From 1 to 16 threads
    execution_times = []

    for threads in num_threads:
        total_time = 0
        for _ in range(3):  # Run three times for each thread count
            try:
                exec_time = run_mpi_program(threads)
                total_time += exec_time
                print(f"Completed run with {threads} threads. Time: {exec_time:.4f} seconds")
            except Exception as e:
                print(f"Error running with {threads} threads: {e}")
        
        avg_exec_time = total_time / 3
        execution_times.append(avg_exec_time)

    data = {'num_threads': list(num_threads), 'times': execution_times}

    # Plot the results
    plt.figure(figsize=(12, 6))
    plt.plot(data['num_threads'], data['times'])
    plt.xscale('linear')
    plt.yscale('log')
    plt.xlabel('Number of Threads')
    plt.ylabel('Average Execution Time (seconds)')
    plt.title('Execution Time vs Number of Threads')
    plt.grid(True)
    plt.savefig('zero_execution_time.png')
    plt.close()

if __name__ == '__main__':
    main()
