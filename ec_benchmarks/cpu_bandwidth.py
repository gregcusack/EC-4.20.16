import numpy as np
import matplotlib
matplotlib.use('GTK')
import matplotlib.pyplot as plt
import sys
import os
import glob


def average(lst): 
    return sum(lst) / len(lst) 

cpu_usage = [200000]
N = len(cpu_usage)
ind = np.arange(N)  # the x locations for the groups
width = 0.27       # the width of the bars

fig = plt.figure()
ax = fig.add_subplot(111)

# get the non_ec_vals..
non_ec_vals = []
ec_vals = []
ec_subcontainers = []
multiple_ec = []
path = ""

for i in cpu_usage:
	path = 'non_ec_baseline/' + str(i) + '*.txt'
	for filename in glob.glob(path):
		with open(filename, 'r') as f:
			for line in f:
				non_ec_vals.append(line[:-2])
	path = '192.168.6.10/' + str(i) + '*.txt'
        for filename in glob.glob(path):
                with open(filename, 'r') as f:
                        for line in f:
                                ec_vals.append(line[:-2])

	path = 'ec_subcontainers/' + str(i) + '*.txt'
        for filename in glob.glob(path):
                with open(filename, 'r') as f:
                        for line in f:
                                ec_subcontainers.append(line[:-2])

	path = 'ec_multiple/' + str(i) + '*.txt'
        for filename in glob.glob(path):
                with open(filename, 'r') as f:
                        for line in f:
                                multiple_ec.append(line[:-2])

#print(non_ec_vals)
#print(ec_vals)
#print(ec_subcontainers)
#print(multiple_ec)

non_ec_vals = [float(i) for i in non_ec_vals]
ec_vals = [float(i) for i in ec_vals]
ec_subcontainers = [float(i) for i in ec_subcontainers]
multiple_ec = [float(i) for i in multiple_ec]

non_ec = ax.bar(ind, average(non_ec_vals), width, color='r')
ec = ax.bar(ind+width, average(ec_vals), width, color='g')
ec_sub = ax.bar(ind+width*2, average(ec_subcontainers), width, color='b')
ec_multiple = ax.bar(ind+width*3, average(multiple_ec), width, color='c')

ax.set_ylabel('Elasped time(s)')
ax.set_xticks(ind+width)
ax.set_xticklabels( ('2') )
ax.legend( (non_ec[0], ec[0], ec_sub[0], ec_multiple[0]), ('Non-DC Containers', 'DC Container', 'DC Multiple Subcontainers, Multiple DC Containers') )

plt.grid(True)
plt.show()

