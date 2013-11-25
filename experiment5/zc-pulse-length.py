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

x = np.loadtxt("zc-pulse-length-filtered", delimiter=",")
N = x.shape[0]
scatter = np.max(x) - np.min(x)
diff = differential(x[:,0])
print "min ", np.min(x[:,0]), " ", np.min(x[:,0])/16e6, "s"
print "max ", np.max(x[:,0]), " ", np.max(x[:,0])/16e6, "s"
print "scater ", scatter, " ", scatter/16e6, "s"
print "max differential ", np.max(diff), " ", np.max(diff)/16e6, "s"
print "min differential ", np.min(diff), " ", np.min(diff)/16e6, "s"
print "min differential abs ", np.min(np.abs(diff)), " ", np.min(np.abs(diff))/16e6, "s"
print "differential abs median ", np.median(np.abs(diff)), " ", np.median(np.abs(diff))/16e6, "s"
print "differential abs mean ", np.mean(np.abs(diff)), " ", np.mean(np.abs(diff))/16e6, "s"
print "mean ", np.mean(x[:,0]), " ", np.mean(x[:,0])/16e6, "s"
print "median ", np.median(x[:,0]), " ", np.median(x[:,0])/16e6, "s"

#plt.plot(range(0, N-1), diff+np.mean(x[:,0]), color="grey")
xvals = np.arange(0, x[:,0].shape[0], 1)/100
plt.plot(xvals, x[:,0]/16, color="grey")
plt.plot(xvals, x[:,1]/16, color="green")
plt.plot(xvals, x[:,2]/16, color="red")
plt.xlabel("Time, s")
plt.ylabel("Pulse length, us")
plt.title("Zero crossing pulse length, R input = 16.4k R pullup = 1k")
plt.savefig("Zero crossing pulse length, R input = 16k4 R pullup = 1k")
plt.figure()

xvals = np.arange(0.0, N/2+1, 1)
xvals = xvals/np.max(xvals)*50
plt.plot(xvals, signal.medfilt(
	20*scipy.log10(
		abs(
			ff.rfft(
				(x[:,0]) / np.max(x[:,0])
			)
		)
	),355) )
plt.xlabel("Hz")
plt.ylabel("Magnitude, db")
plt.title("Zero crossing pulse length FFT, R input = 16.4k R pullup = 1k")
plt.savefig("Zero crossing pulse length FFT, R input = 16k4 R pullup = 1k")
plt.figure()

std = np.std(diff)

sigma = std

plt.hist(diff, bins=(
	np.min(diff)*-1 + 
	np.max(diff)
	), color="black")

sigma2 = 2*std
plt.axvline(np.mean(diff) - sigma2)
plt.axvline(np.mean(diff) + sigma2)

plt.axvline(np.mean(diff) - sigma, color="yellow")
plt.axvline(np.mean(diff) + sigma, color="yellow")
plt.title("Pulse length differeces for adjacent pulses, R input = 16.4k R pullup = 1k")
plt.savefig("Pulse length differeces for adjacent pulses, R input = 16k4 R pullup = 1k")
plt.show()

