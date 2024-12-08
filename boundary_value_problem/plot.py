import matplotlib.pyplot as plt
import numpy as np

with open("results.txt") as file:

    xs = file.readline()
    ys = file.readline()

xs = [float(x) for x in xs[:-2].split(' ')]

ys = [float(y) for y in ys[:-2].split(' ')]

orig = 0.1 * np.exp(np.array(xs)) - 0.2 * np.exp(-np.array(xs))
err = np.abs(orig - ys)

plt.plot(xs, ys, label = "computed")
# plt.plot(xs, orig, label = "true")

# plt.plot(xs, err, label = 'error')

plt.legend()
plt.show()
