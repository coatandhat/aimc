#!/usr/bin/python
"""
plot_munged_results.py

"""

import numpy as np
import pylab as p
import matplotlib.pyplot as plt

f=open("results_test_all.csv","r")
results = dict()
for line in f:
  if line[0] != "#":
    values = line.strip().split(",")
    results.setdefault(values[3],dict())
    results[values[3]].setdefault(values[0], dict())
    results[values[3]][values[0]].setdefault(values[1], dict())
    results[values[3]][values[0]][values[1]].setdefault(int(values[4]), dict())
    results[values[3]][values[0]][values[1]][int(values[4])].setdefault(int(values[5]), dict())
    results[values[3]][values[0]][values[1]][int(values[4])][int(values[5])].setdefault(int(values[6]), dict())
    if values[2] == 'clean':
       snr = 50
    else:
       snr = int(values[2])
       results[values[3]][values[0]][values[1]][int(values[4])][int(values[5])][int(vlues[6])][snr] = float(values[7])
#    results[values[3]].append((values[1],values[2],values[2],values[4]))

ax = plt.subplot(111)

train_set = 'inner'
lines = []
labels = []
hmm_iterations = 2
hmm_states = 4
hmm_components = 4
for feature_type in ('mfcc', 'mfcc_vtln', 'aim'):
  for feature_subtype in results[train_set][feature_type].keys():
    this_line = results[train_set][feature_type][feature_subtype][hmm_states][hmm_components][hmm_iterations].items()
    this_line.sort(cmp=lambda x,y: x[0] - y[0])
    xs, ys = zip(*this_line)
    xs = list(xs)
    ys = list(ys)
    line, = ax.plot(xs,ys,'-o',linewidth=2)
    lines.append(line)
    labels.append(feature_type + "_" + feature_subtype)
p.legend(lines, labels, 'upper left', shadow=True)
p.xlabel('SNR/dB')
p.ylabel('Recognition performance %')
plt.show()
