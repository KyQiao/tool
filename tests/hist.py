import numpy as np
import matplotlib.pyplot as plt

# data = np.loadtxt('./1000ql.csv')
# data = np.loadtxt('./0ql.csv')
# data = np.loadtxt('./0qw.csv',skiprows=1)
data = np.loadtxt('./1000qw.csv',skiprows=1)
q6=data[:,0]
q4=data[:,1]
w6=data[:,2]
w4=data[:,3]
# data = np.loadtxt('./3970q6_2.csv')
# plt.hist(data,bins=100,density=True)
# data = np.loadtxt('./3000q6.csv')
# plt.hist(data,bins=100,density=True)

# plt.scatter(q4, q6, s=10, label='fcc', color='#FFB300')
plt.scatter(w4, w6, s=10, label='fcc', color='#FFB300')
plt.xlabel("$w_4$", fontsize=20)
plt.ylabel("$w_6$", fontsize=20)
plt.legend(loc=4, fontsize=15)

plt.show()
