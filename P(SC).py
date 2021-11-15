import numpy as np
import matplotlib.pyplot as plt
import struct
from scipy.optimize import curve_fit

'''
The size distribution of causal avalanches.
'''

with open("avaind.dat", mode='rb') as file: # b is important -> binary
    avaind = file.read()
file.close()

avaind_list = []
for i in struct.iter_unpack('i', avaind):
    avaind_list.append(i[0])

length = len(avaind_list)
maxum = max(avaind_list)
count = np.zeros(maxum+1, dtype=np.int)

for niter in range(0,length):
    num = avaind_list[niter]
    count[num] += 1

cmax = np.max(count)

dis = np.zeros(cmax, dtype=np.int)
for iter in range(maxum+1):
    num = count[iter]-1
    dis[num] += 1

dis = dis[:500]                                  # limit to the sizes < 500
size = np.array(dis, dtype=np.float)
norm = np.sum(size)
size = size/norm
xlist = []
ylist = []
for iter in range(len(size)):
    xlist.append(iter + 1)
    if size[iter]>0.0:
        ylist.append(size[iter])
    else: ylist.append(np.NaN)


start = 0
end = 25
fxlist = np.log(xlist[start:end])
fylist = np.log(ylist[start:end])


def func(x, a, b):
    return a*x+b


popt, pcov = curve_fit(func, fxlist, fylist)

a = popt[0]
b = popt[1]
print("the fitted exponent is ", a)                    # the fitted exponent

y = a*np.log(xlist)+b
y = np.exp(y)


xlist = np.array(xlist)
ylist = np.array(ylist)
plt.figure(figsize=(10, 8))
plt.scatter(np.log(xlist), np.log(ylist), s=10, marker='o')
plt.plot(np.log(xlist),np.log(y),'r')
plt.show()

