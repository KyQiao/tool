from mpl_toolkits.mplot3d import Axes3D
import matplotlib.pyplot as plt
import numpy as np


# data = np.loadtxt('atom.400000.sk.xyz')
# data = np.loadtxt('n.sk.xyz')
data = np.loadtxt('n100.sk.xyz')
# data = np.loadtxt('atom.0.sk.xyz')


kx,ky,kz,sk= data[:,0],data[:,1],data[:,2],data[:,3]

# peak = np.where(sk>0.5)
peak = np.where(sk>2*np.mean(sk[1:]))

fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

ax.scatter(kx[peak],ky[peak],kz[peak], c=sk[peak], marker='o')

ax.set_xlabel('X Label')
ax.set_ylabel('Y Label')
ax.set_zlabel('Z Label')

plt.show()