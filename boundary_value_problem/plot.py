import matplotlib.pyplot as plt
xs = "0.0 0.6 1.2 1.9 2.5 3.1 3.8 4.4 5.0 5.6 6.2 6.9 7.5 8.1 8.8 9.4 10.0"
xs = [float(x) for x in xs.split(' ')]

ys = "0.0 0.4 1.6 3.5 6.3 9.8 14.1 19.1 25.0 31.6 39.1 47.3 56.3 66.0 76.6 87.9 100.0"
ys = [float(y) for y in ys.split(' ')]
plt.plot(xs, ys)
plt.show()
