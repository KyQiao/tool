import os
import numpy as np
import matplotlib.pyplot as plt

import pickle

path = "./result/select/"
filelist = os.listdir(path)


arr=[]
# s 84800
# end 121600
out=[]

for i in range(84800,121600,400):
    print(i)
    data = np.loadtxt("./result/select/atom."+str(i)+".xyz.gz.xyzOnly.xyz",skiprows=1)
    h,edge = np.histogram(data[:,2],bins=100,range=(0,0.58),density=True)
    out.append(h)
    # plt.plot(edge[:-1],h)

with open("q6data",'wb') as f:
    pickle.dump(out,f)

np.save("edge.npy",edge)

# plt.show()