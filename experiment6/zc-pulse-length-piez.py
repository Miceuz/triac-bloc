import math
import sys
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator
import numpy as np
import numpy.fft as ff
import scipy.signal as signal
import scipy as scipy

def differential(x):
	ret = []
	for i in range(0, x.shape[0]-1):
		if i < x.shape[0]-1:
			ret.append(x[i+1] - x[i])
		else:
			ret.append(x[i])
	return np.array(ret)

def magnitude(x):
	ret = []
	for sample in x:
		ret.append(math.sqrt(sample.real**2 + sample.imag**2))
	return np.array(ret)

x = np.loadtxt("piez")
N = x.shape[0]

#plt.plot(range(0, N-1), diff+np.mean(x), color="grey")
xvals = np.arange(0, x.shape[0], 1)
plt.plot(xvals, x, color="grey")
#plt.plot(xvals, x[:,1]/16, color="green")
#plt.plot(xvals, x[:,2]/16, color="red")
plt.xlabel("Time, s")
plt.ylabel("Pulse length, us")
plt.title("Zero crossing pulse length, R input = 16.4k R pullup = 10k")
plt.figure()

xvals = np.arange(0.0, N/2+1, 1)
xvals = xvals/np.max(xvals)*50
plt.plot(xvals, signal.medfilt(
	20*scipy.log10(
		abs(
			ff.rfft(
				(x) / np.max(x)
			)
		)
	),15) )
plt.xlabel("Hz")
plt.ylabel("Magnitude, db")
plt.title("Zero crossing pulse length FFT, R input = 16.4k R pullup = 10k")
plt.show()

