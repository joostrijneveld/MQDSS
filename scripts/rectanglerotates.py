""" Rectangle rotation arrangement script.

This script arranges multiplications in such a way that a minimal amount of
rotates is needed, and, more importantly, that these rotates do not cross
register boundaries. Note that this requires manual tweaking of the script,
which can then be ran to verify the arrangement is correct by seeing
that there are no duplicates and no missing binomials.
"""

from copy import copy
from collections import Counter
from itertools import combinations

N = 16  # number of elements in a register
R = 4  # number of registers


def ROL(l):
    return l[1:N] + [l[0]]

regs = [range(i * N, (i+1) * N) for i in range(R)]
regs = list(map(list, regs))

pairs = []

for i in range(R):
    y = copy(regs[i])
    for j in range(N // 2):
        for k in range(0 if j > 0 else i, R):
            pairs.extend(tuple(sorted(x)) for x in zip(y, regs[k]))
        y = ROL(y)

# This is a generalized version of the half-row that needs to be added below
# the rectangle in the 1-register case.
for h in [x[N // 2:] for x in regs]:
    for l in [x[:N // 2] for x in regs]:
        pairs.extend(tuple(sorted(x)) for x in zip(l, h))

print(len(pairs))
print(pairs)

# We should not have any duplicates.
print(Counter(pairs))
dups = [k for k, v in Counter(pairs).items() if v > 1]
print('dups:', dups)
print(len(dups))

# All pairs should be generated.
allpairs = set([tuple(sorted((x, y))) for x in range(N*R) for y in range(N*R)])
pairs = set(pairs)
print(len(allpairs - pairs))
print('diff:', sorted(list(allpairs - pairs)))
