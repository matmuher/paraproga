import matplotlib.pyplot as plt
import numpy as np
import subprocess

bs = [1.45, 1.50, 1.55]
print(bs)

for b in bs:

    cmd = [f'./a.out', '8', '1', f'{b}']
    result = subprocess.run(cmd, capture_output=True, text=True)

    with open("results.txt") as file:

        xs = file.readline()
        ys = file.readline()

    xs = [float(x) for x in xs[:-2].split(' ')]
    ys = [float(y) for y in ys[:-2].split(' ')]

    step = 1
    plt.scatter(xs[::step], ys[::step], label = f"b = {b}")


plt.legend()
plt.show()
# plt.savefig("B2.svg")
