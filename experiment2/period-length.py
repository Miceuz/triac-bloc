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

x = np.loadtxt("period-length-filtered", delimiter=",")
N = x.shape[0]
scatter = np.max(x) - np.min(x)
diff = differential(x[:,0])
print "min ", np.min(x[:,0]), " ", np.min(x[:,0])*8/16e6, "s"
print "max ", np.max(x[:,0]), " ", np.max(x[:,0])*8/16e6, "s"
print "scater ", scatter, " ", scatter*8/16e6, "s"
print "max differential ", np.max(diff), " ", np.max(diff)*8/16e6, "s"
print "min differential ", np.min(diff), " ", np.min(diff)*8/16e6, "s"
print "min differential abs ", np.min(np.abs(diff)), " ", np.min(np.abs(diff))*8/16e6, "s"
print "differential abs median ", np.median(np.abs(diff)), " ", np.median(np.abs(diff))*8/16e6, "s"
print "mean ", np.mean(x[:,0]), " ", np.mean(x[:,0])*8/16e6, "s"
print "median ", np.median(x[:,0]), " ", np.median(x[:,0])*8/16e6, "s"

#plt.plot(range(0, N-1), diff+np.mean(x[:,0]), color="grey")
plt.plot(range(0, N), x[:,0], color="red")
plt.plot(range(0, N), x[:,1], color="green")
plt.plot(range(0, N), x[:,2], color="red")
#plt.plot(range(0, N/2+1), signal.medfilt(20*scipy.log10(abs(ff.rfft(x))),355) )

plt.show()

