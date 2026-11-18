import sys
from PIL import Image
im=Image.open(sys.argv[1]).convert('RGB'); p=im.load()
cx,cy,r=int(sys.argv[2]),int(sys.argv[3]),int(sys.argv[4])
def k(c):
    if 40<=c[0]<=56 and c[1]<=12 and 40<=c[2]<=72: return 'R'
    if c==(176,176,176): return '#'
    if c[0]>=120 and c[2]<=60 and c[1]>=100: return 'o'
    if c[2]>c[0]+20: return '.'
    return '?'
print('    '+''.join(str((cx-r+i)%10) for i in range(2*r+1)))
for y in range(cy-r,cy+r+1):
    print('%3d '%y+''.join(k(p[x,y]) for x in range(cx-r,cx+r+1)))
