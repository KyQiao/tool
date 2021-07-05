import numpy as np
import matplotlib.pyplot as plt

import pickle

data = pickle.load(open("q6data",'rb'))
edge = np.load('edge.npy')

for index,i in enumerate(range(84800,121600,400)):
  # print(index,i)
  plt.plot(edge[:-1],data[index]/np.max(data[index]))

plt.show()