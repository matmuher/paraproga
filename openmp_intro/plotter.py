import re
import matplotlib.pyplot as plt

pattern_time = r"time: (\d+\.\d+)"
pattern_threads = r"OMP_NUM_THREADS=(\d+)"

results = []

with open('output.txt', 'r') as file:
    lines = file.readlines()
    for i in range(0, len(lines), 2):
        app_name = lines[i].strip().split(',')[0]
        omp_threads_match = re.search(pattern_threads, lines[i])
        if omp_threads_match:
            omp_threads = int(omp_threads_match.group(1))
            
            time_match = re.search(pattern_time, lines[i+1])
            if time_match:
                time = float(time_match.group(1))
                
                results.append({
                    'app_name': app_name,
                    'omp_threads': omp_threads,
                    'time': time
                })

x = [result['omp_threads'] for result in results]
y = [result['time'] for result in results]

print("App Names:", [result['app_name'] for result in results])
print("OMP Threads:", x)
print("Execution Time:", y)
plt.plot(x, y)
plt.xlabel('Number of Threads')
plt.ylabel('Execution Time (seconds)')
plt.title('Execution Time vs Number of Threads')
plt.show()
