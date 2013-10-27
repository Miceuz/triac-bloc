#solving x - sin(x) = C, f(x) = C - 2x + sin(2x) 
#derivative f'(x) = -4sin^2(x)
#x[n+1] = x[n] - f(x[n])/f'(x[n])

import math
import sys
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator
import numpy as np

def guess(y):
	return y/2;


def fderivative(x):
	return -4 * math.sin(x) * math.sin(x)	

#x + sin(x) = y
def f(x, y):
	return 4 * y - 2 * x + math.sin(2 * x)

def solveIteration(xPrevious, y):
	if 0 == fderivative(xPrevious):
		return xPrevious
	return xPrevious - f(xPrevious, y)/fderivative(xPrevious)

def solve(y):
	x=math.pi/2;#guess(y)
	for i in range(0,100): # 100 iterations proves to be enough
		x = solveIteration(x, y)
	
	return x


maxConductionAngle = 10000
conductionAngles=range(maxConductionAngle)

for i in range(0,maxConductionAngle):
	#print i, math.pi / 2 / maxConductionAngle * i, solve(math.pi / 2 / maxConductionAngle * i)
	conductionAngles[i]=solve(math.pi / 2 / (maxConductionAngle) * i);

xvalues = np.arange(0.0, maxConductionAngle * 1.0, 1)
xvalues = (xvalues/np.max(xvalues))*100
#print xvalues
#print xvalues.shape
#print len(conductionAngles)

table = np.arange(0, 256, 1)
for i in range(0, 256):
	j = int(i * maxConductionAngle / 255.0)
	print i, j
	if j < 10000:
		table[i] = conductionAngles[j] / math.pi * 10000;
	else:
		table[i] = 10000;

for i in range(0, 256):
	print table[i], ","
#	print "conductionAngles[" + str(i) + "] = " + str(table[i]) + ";"
	
#print table;

conductionAngles = np.array(conductionAngles)

fig=plt.figure()
ax = fig.add_subplot(111)
ax.xaxis.set_minor_locator(MultipleLocator(10))
ax.grid(True, which='both')
plt.plot(xvalues, conductionAngles/math.pi*10000)
plt.ylabel("conduction angle, degrees")
plt.xlabel("power, %")

plt.figure()

plt.plot(range(0, table.shape[0]), table)
plt.show()

	
