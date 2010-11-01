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
    if values[2] == 'clean':
       snr = 40
    else:
       snr = int(values[2])
       results[values[3]][values[0]][values[1]][snr] = float(values[4])
#    results[values[3]].append((values[1],values[2],values[2],values[4]))

ax = plt.subplot(111)

train_set = 'inner'
lines = []
labels = []
for feature_type in ('mfcc', 'mfcc_vtln', 'aim'):
  for feature_subtype in results[train_set][feature_type].keys():
    this_line = results[train_set][feature_type][feature_subtype].items()
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
