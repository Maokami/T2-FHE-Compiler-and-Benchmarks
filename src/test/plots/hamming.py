#!/usr/bin/python

import matplotlib.pyplot as plt
import numpy as np

mydpi = 300
pltsize = (6, 2)

# Milliseconds
data = {
'Integer |v| = 4': {
  'helib': 27702,
  'lattigo': 13782,
  'palisade': 4811,
  'seal': 25464,
  'tfhe': 1390 },
'Binary |v| = 4': {
  'helib': 7388,
  'lattigo': 96341.97564100001,
  'palisade': 12219,
  'seal': 49446,
  'tfhe': 2018 },
'Integer |v| = 8': {
  'helib': 54002,
  'lattigo': 27395,
  'palisade': 9639,
  'seal': 50236,
  'tfhe': 1390 },
'Binary |v| = 8': {
  'helib': 15052,
  'lattigo': 246567.374508,
  'palisade': 28175,
  'seal': 1400,
  'tfhe': 3940 }
}

helib = []
lattigo = []
palisade = []
seal = []
tfhe = []

x_axis_label = []
for k,val in data.items():
  x_axis_label.append(k)
  helib.append(val['helib'] / 1000)
  lattigo.append(val['lattigo'] / 1000)
  palisade.append(val['palisade'] / 1000)
  seal.append(val['seal'] / 1000)
  tfhe.append(val['tfhe'] / 1000)

N = len(palisade)
index = np.arange(N) # the x locations for the groups
width = 0.16 # the width of the bars

fig, ax = plt.subplots(figsize=pltsize)
ax.margins(0.02, 0.02)

rects1 = ax.bar(index - 3*width/2, helib, width,
                color='xkcd:light salmon', hatch='//', edgecolor='black', linewidth=1)
rects2 = ax.bar(index - width/2, lattigo, width,
                color='#ffddbf', hatch='xx', edgecolor='black', linewidth=1)
rects3 = ax.bar(index + width/2, palisade, width,
                color='xkcd:ecru', hatch='--', edgecolor='black', linewidth=1)
rects4 = ax.bar(index + 3*width/2, seal, width,
                color='xkcd:very light green', hatch='..', edgecolor='black', linewidth=1)
rects5 = ax.bar(index + 5*width/2, tfhe, width,
                color='xkcd:very light blue', hatch='\\\\', edgecolor='black', linewidth=1)

ax.set_yscale('log')
ax.set_ylim([1, 1000])
ax.set_ylabel("Time (sec.)")
ax.set_xlabel("Encrypted Domain")
ax.set_xticks(index + width / 2)
ax.set_xticklabels(x_axis_label)
# ax.legend((rects1[0], rects2[0], rects3[0], rects4[0], rects5[0]),
#           ("HElib", "Lattigo", "PALISADE", "SEAL", "TFHE"),
#           fontsize=9, ncol=5, loc='upper center')

def autolabel_above(rects):
  for rect in rects:
    height = rect.get_height()
    if height <= 1.39:
      ax.text(rect.get_x() + rect.get_width()/2., 1.7, 'N/A', ha='center', va='bottom', fontsize=7)
    elif height <= 1.4:
      ax.text(rect.get_x() + rect.get_width()/2., 1.7, 'Noisy', ha='center', va='bottom', fontsize=7, rotation=90)
    elif height > 10:
      ax.text(rect.get_x() + rect.get_width()/2., 1.1*height, '%2.0f' % (height), ha='center', va='bottom', fontsize=7)
    else:
      ax.text(rect.get_x() + rect.get_width()/2., 1.1*height, '%2.1f' % (height), ha='center', va='bottom', fontsize=7)

autolabel_above(rects1)
autolabel_above(rects2)
autolabel_above(rects3)
autolabel_above(rects4)
autolabel_above(rects5)

# plt.show()

plt.tight_layout()
plt.savefig("./hamming.png", dpi=mydpi, bbox_inches="tight", pad_inches=0.03)
