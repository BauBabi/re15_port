import sys
from offen import klassen
p = sys.argv[1]; x0, y0, x1, y1 = map(int, sys.argv[2:6])
K = klassen(p)
print('     ' + ''.join(str(x % 10) for x in range(x0, x1)))
for y in range(y0, y1):
    print(f'{y:4d} ' + ''.join(K[y, x] for x in range(x0, x1)))
