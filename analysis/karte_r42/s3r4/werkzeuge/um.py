import sys
from PIL import Image
im=Image.open(sys.argv[1]).convert('RGB'); p=im.load()
mx,my=int(sys.argv[2]),int(sys.argv[3])   # marker wie auswerten2 (Mitte+... ) 
def rot(c): return (40<=c[0]<=56 and c[1]<=12 and 40<=c[2]<=72) or (c[0]>=120 and c[2]<=60)
def um(m):
    pts=[(m[0]-3+dx,m[1]-3+dy) for dx in range(-2,7) for dy in range(-2,7) if not (0<=dx<=4 and 0<=dy<=4)]
    return sum(1 for x,y in pts if rot(p[x,y]))/len(pts)
print('jetzt',(mx,my),round(um((mx,my)),2))
for dy in range(-3,4):
    print(' '.join('%4.2f'%um((mx+dx,my+dy)) for dx in range(-3,4)))
