import matplotlib.pyplot as plt
import numpy as np

# r gr
# data = np.loadtxt('atom.0.gr.xyz')
# data = np.loadtxt('atom.400000.gr.xyz')
# data2 = np.loadtxt('atom.400000.gr.xyz.right')

data = np.loadtxt('gr.10.xyz.t')

# data = np.loadtxt('gr.3970.xyz')
# data = np.loadtxt('atom.9000.gr.xyz')
# data2 = np.loadtxt('atom.9000.gr.xyz.right')
r,gr = data[:,0],data[:,1]
# r2,gr2 = data2[:,0],data2[:,1]

# plt.plot(r-r2,gr-gr2,'-*')
plt.plot(r,gr,'-*')
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
