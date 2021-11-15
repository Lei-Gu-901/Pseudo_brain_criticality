import numpy as np
import matplotlib.pyplot as plt
import struct
from scipy.optimize import curve_fit

'''
The duration distribution of causal avalanches using the starting and ending times.
'''

with open("start.dat", mode='rb') as file: # b is important -> binary
    start = file.read()
file.close()
start_list = []
for i in struct.iter_unpack('i', start):
    start_list.append(i[0])


with open("end.dat", mode='rb') as file: # b is important -> binary
    end = file.read()
file.close()
end_list = []
for i in struct.iter_unpack('i', end):
    end_list.append(i[0])

end = np.array(end_list)
start = np.array(start_list)

dur = end - start + 1
dis = np.zeros(500,dtype=np.float)                    # limit to durations < 500

leng = len(start_list)
for iter in range(leng):
    tdur = dur[iter]
    if tdur < 501: dis[tdur-1] += 1

size = dis
norm = np.sum(size)
size = size/norm
xlist = []
ylist = []
for iter in range(len(size)):
    xlist.append(iter + 1)
    if size[iter]>0.0:
        ylist.append(size[iter])
    else: ylist.append(np.NaN)

'''
set the duration range for fitting, with the c++/python indexing convention: 0 indexes the first element 
'''
start = 1
end = 10
fxlist = np.log(xlist[start:end])
fylist = np.log(ylist[start:end])


def func(x, a, b):
    return a*x+b


popt, pcov = curve_fit(func, fxlist, fylist)

a = popt[0]
b = popt[1]

print("the fitted exponent is ", a)     # the fitted exponent

y = a*np.log(xlist)+b
y = np.exp(y)

xlist = np.array(xlist)
ylist = np.array(ylist)
plt.figure(figsize=(10, 8))
plt.scatter(np.log(xlist), np.log(ylist), s=10, marker='o')
plt.plot(np.log(xlist),np.log(y),'r')
plt.show()
