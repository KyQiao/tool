import numpy as np
import matplotlib.pyplot as plt


def p(r):
    return (1/r)**14+1/2*(1-np.tanh(10*(r-1.35)))


def dp(r):
    return 14*(1/r)**15+5*(2/(np.exp(10*(r-1.35))+np.exp(-10*(r-1.35))))**2


def testConsistent():
    # data = np.loadtxt('potential.table',skiprows=3,usecols=(1,2,3))
    data = np.loadtxt('potential.table', skiprows=6, usecols=(1, 2, 3))
    plt.subplot(321)
    plt.plot(data[:, 0], data[:, 1], '*-')

    plt.subplot(322)
    plt.plot(data[:, 0], data[:, 2], '*-')
    plt.subplot(323)
    plt.plot(data[:, 0], p(data[:, 0]), '*-')

    plt.subplot(324)
    plt.plot(data[:, 0], dp(data[:, 0]), '*-')

    plt.subplot(325)
    plt.plot(data[:, 0], p(data[:, 0])-data[:, 1], '*-')

    plt.subplot(326)
    plt.plot(data[:, 0], dp(data[:, 0])-data[:, 2], '*-')

    plt.show()


def generateP():
    N = 4000
    start=0.7
    R = 2.7

    r = np.linspace(start, R, N)
    e = p(r)
    mf = dp(r)

    with open("table.txt", 'w') as f:
        f.write("SHOULDER\n")
        f.write("N {:d} R {:.3f} {:.3f}\n\n".format(N,start,R))
        for i in range(r.shape[0]):
            f.write("{:d} {:.12f} {:.12f} {:.12f}\n".format(
                i+1, r[i], e[i], mf[i]))


if __name__ == '__main__':
    # generateP()
    testConsistent()
