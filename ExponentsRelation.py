import numpy as np
import matplotlib.pyplot as plt
import struct
from scipy.optimize import curve_fit

with open("timing.dat", mode='rb') as file: # b is important -> binary
    spike = file.read()
file.close()

spike_list = []
for i in struct.iter_unpack('i', spike):
    spike_list.append(i[0])

length = len(spike_list)

num_list = []
size_list = []
pre = spike_list[0]
diff = 1
count = 1
for niter in range(1,length):
    num = spike_list[niter]
    temp = num-pre
    if temp <= 1:
        diff += temp
        count += 1
    else:
        num_list.append(int(diff))
        size_list.append(int(count))
        diff = 1
        count = 1
    pre = num

print("-------------")
print(num_list)

cut = 200
dur = np.linspace(1,cut,cut)
sizecount = np.zeros(cut, dtype=np.int)
sizesum = np.zeros(cut,dtype=np.int)

leng = len(num_list)
for iter in range(leng):
     temp = num_list[iter]
     if temp <= cut:
         sizesum[temp-1] += size_list[iter]
         sizecount[temp-1] += 1

sizeaverge = sizesum/sizecount

'''
set the duration range for fitting, with the c++/python indexing convention: 0 indexes the first element 
'''
start = 0
end = 50
fxlist = np.log(dur[start:end])
fylist = np.log(sizeaverge[start:end])


def func(x, a, b):
    return a*x+b


popt, pcov = curve_fit(func, fxlist, fylist)


a = popt[0]
print('the fitted exponent is', a)
b = popt[1]

y = a*np.log(dur)+b
y = np.exp(y)

plt.figure(figsize=(10, 8))
plt.scatter(np.log(dur), np.log(sizeaverge), s=10, marker='o')
plt.plot(np.log(dur),np.log(y),'r')
plt.show()
