import numpy as np
import matplotlib.pyplot as plt
import struct
from scipy.optimize import curve_fit

'''
size distribution of time-bin avalanches
'''

with open("timing.dat", mode='rb') as file: # b is important -> binary
    spike = file.read()
file.close()

spike_list = []
for i in struct.iter_unpack('i', spike):
    spike_list.append(i[0])


length = len(spike_list)
num_list = []
counter = 1
pre = spike_list[0]
timeava = []
for niter in range(1,length):
    num = spike_list[niter]
    if num-pre <= 1:
        counter += 1
    else:
        num_list.append(int(counter))
        counter = 1
    pre = num


cut = 500
size = [0]*cut
for item in num_list:
    if item <= cut:
        size[item-1] +=1

size = np.array(size, dtype=np.float)
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
print("the fitted exponent is ", a)

y = a*np.log(xlist)+b
y = np.exp(y)


xlist = np.array(xlist)
ylist = np.array(ylist)
plt.figure(figsize=(10, 8))
plt.scatter(np.log(xlist), np.log(ylist), s=10, marker='o')
plt.plot(np.log(xlist),np.log(y),'r')
plt.show()
