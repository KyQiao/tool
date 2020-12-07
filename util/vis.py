import matplotlib.pyplot as plt
import numpy as np

# q6 q4 w6 w4
data1 = np.loadtxt('wq.1934.xyz')
data2 = np.loadtxt('wq.3970.xyz')

h1, _, _ = np.histogram2d(data1[:, 1], data1[:, 0], bins=(50, 50))

# h2, _, _ = np.histogram2d(data2[:, 1], data2[:, 0], bins=(50, 50))


# plt.imshow(h2-h1)
plt.imshow(h1)
plt.show()


# plt.subplot(121)
# plt.xlabel("q4")
# plt.ylabel("q6")
# plt.hist2d(data[:, 1], data[:, 0], bins=(50, 50))

# plt.subplot(122)
# plt.xlabel("w4")
# plt.ylabel("w6")
# plt.hist2d(data[:, 3], data[:, 2], bins=(50, 50))

# plt.show()
