import math
import numpy as np
import matplotlib.pyplot as plt
import numpy.fft as ff
from pylab import *
import scipy.signal as signal

def magnitude(x):
	ret = []
	for sample in x:
		ret.append(math.sqrt(sample.real**2 + sample.imag**2))
	return np.array(ret)

def differential(x):
	ret = []
	for i in range(0, x.shape[0]-1):
		if i < x.shape[0]-1:
			ret.append(x[i+1] - x[i])
		else:
			ret.append(x[i])
	return np.array(ret)
def mfreqz(b,a=1):
    w,h = signal.freqz(b,a)
    h_dB = 20 * log10 (abs(h))
    subplot(211)
    plot(w/max(w),h_dB)
    ylim(-150, 5)
    ylabel('Magnitude (db)')
    xlabel(r'Normalized Frequency (x$\pi$rad/sample)')
    title(r'Frequency response')
    subplot(212)
    h_Phase = unwrap(arctan2(imag(h),real(h)))
    plot(w/max(w),h_Phase)
    ylabel('Phase (radians)')
    xlabel(r'Normalized Frequency (x$\pi$rad/sample)')
    title(r'Phase response')
    subplots_adjust(hspace=0.5)

#Plot step and impulse response
def impz(b,a=1):
    l = len(b)
    impulse = repeat(0.,l); impulse[0] =1.
    x = arange(0,l)
    response = signal.lfilter(b,a,impulse)
    subplot(211)
    stem(x, response)
    ylabel('Amplitude')
    xlabel(r'n (samples)')
    title(r'Impulse response')
    subplot(212)
    step = cumsum(response)
    stem(x, step)
    ylabel('Amplitude')
    xlabel(r'n (samples)')
    title(r'Step response')
    subplots_adjust(hspace=0.5)


y = np.loadtxt("step-response-linear2.csv")
y = hstack((np.ones(100)*23.50, y))
y = y - min(y)
n = 13
a = signal.firwin(n, cutoff = 0.037, window = "hamming")

#plt.plot(range(0, (y.shape[0])), y)

yfiltered = signal.lfilter(a, 1, y)[n:]
irfiltered = differential(yfiltered)
ir = differential(y)[n:]

plt.plot(range(0, (ir.shape[0])), ir, c="grey")
plt.plot(range(0, (irfiltered.shape[0])), irfiltered, c="red", linewidth=2)

irfiltered = np.hstack((np.zeros(1000), irfiltered, np.zeros(1000)))
ir = np.hstack((np.zeros(10000), ir, np.zeros(10000)))
#plt.plot(range(0, (ir.shape[0])/2+1), magnitude(ff.rfft(ir)))

H = abs(ff.rfft(irfiltered))[0:]
H = H/H.shape[0]
#plt.plot(range(0, H.shape[0]), H)


#plt.plot(np.array(range(0, irfiltered.shape[0] / 2 + 1), float) / (irfiltered.shape[0] / 2 + 1) * 0.5, magnitude(ff.rfft(irfiltered)))

#plt.plot(range(0, (y.shape[0])), y)
#plt.plot(range(0, (y.shape[0]-10)), signal.lfilter(a, 1, y)[10:])
#plt.plot(range(0, (a.shape[0])/2+1), magnitude(ff.rfft(a)))
#mfreqz(irfiltered)

plt.show()
