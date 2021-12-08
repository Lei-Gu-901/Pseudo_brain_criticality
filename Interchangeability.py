import numpy as np
import matplotlib.pyplot as plt

limit = 200
size = np.linspace(1,limit,limit)

exponent = -1.5
origin = size**exponent
origin = origin/np.sum(origin)

'''add probability of empty bins as avalanche of zeros size'''
withemp = np.zeros(limit+1, dtype=np.float)
emp = 0.1
nonemp = 1.0 - emp
withemp[0] = emp
withemp[1:limit+1] = nonemp*origin

summation = np.zeros(limit,dtype=np.float)
induce = np.zeros(limit,dtype=np.float)

'''prepare the normalization constant for each time-bin size'''
for iter in range(1,limit+1):
    tsum = 0.0
    for iiter in range(1,iter+1):
        tsum += iiter**exponent*origin[iter-iiter]/(1.0-origin[iter-iiter])
    summation[iter-1] = tsum


for iter in range(1, limit+1):
    fac = iter**exponent/summation[iter-1]
    for iiter in range(1, iter+1):
        induce[iiter-1] += iiter**exponent*fac*origin[iter-iiter]/(1.0-origin[iter-iiter])

induce = induce/np.sum(induce)

plt.figure(figsize=(10, 8))
plt.scatter(np.log(size), np.log(induce), s=10, marker='o')
plt.scatter(np.log(size), np.log(origin), s=10, marker='o', c='r')
plt.show()