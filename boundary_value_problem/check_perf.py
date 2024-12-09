#!/usr/bin/env python3
import subprocess
import json
import matplotlib.pyplot as plt
import numpy as np
import re

name = 'bvp'

def parse_output(text):
    pattern = r'\[TIME\] total: (\d+\.\d+) ms'
    match = re.search(pattern, text)
    
    if match:
        total_time_ms = float(match.group(1))
        total_time_s = total_time_ms
        
        return total_time_s
    else:
        return None

def run_program(threads):
    cmd = [f'./{name}.out', f'{threads}']
    result = subprocess.run(cmd, capture_output=True, text=True)
    return parse_output(result.stdout)

def main():
    cmd = ['g++', f'{name}.cpp', '-fopenmp', '-O3', '-ftree-vectorize', '-o', f'{name}.out']
    subprocess.run(cmd, check=True)

    num_threads = [1, 2, 4, 8]  # From 1 to 16 threads
    execution_times = []

    for threads in num_threads:
        total_time = 0
        samples_num = 3
        for _ in range(samples_num):  # Run three times for each thread count
            try:
                exec_time = run_program(threads)
                total_time += exec_time
                print(f"Completed run with {threads} threads. Time: {exec_time:.4f} seconds")
            except Exception as e:
                print(f"Error running with {threads} threads: {e}")
        
        avg_exec_time = total_time / samples_num
        execution_times.append(avg_exec_time)

    # Save execution times to a JSON file
    with open('execution_times.json', 'w') as f:
        json.dump({'num_threads': list(num_threads), 'times': execution_times}, f)

    # Load execution times from the JSON file
    with open('execution_times.json', 'r') as f:
        data = json.load(f)

    # Plot the results
    fig = plt.figure(figsize=(12, 6))
    plt.plot(data['num_threads'], data['times'][0]/np.array(data['times']))
    plt.xscale('linear')
    plt.yscale('linear')
    plt.xlabel('Number of Threads')
    plt.ylabel('Boost factor')
    plt.title('Boost factor vs Number of Threads')
    plt.grid(True)
    plt.savefig(f'{name}_perfomance.png')
    
    plt.figure(figsize=(12, 6))
    plt.plot(data['num_threads'], np.array(data['times']))
    plt.xscale('linear')
    plt.yscale('linear')
    plt.xlabel('Number of Threads')
    plt.ylabel('Average Execution Time (ms)')
    plt.title('Execution Time vs Number of Threads')
    plt.grid(True)
    plt.savefig(f'{name}_time.png')
    plt.close()

if __name__ == '__main__':
    main()
