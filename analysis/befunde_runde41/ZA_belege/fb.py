import sys
sys.path.insert(0, r"C:\workspace\git\reAi_v2\.claude\skills\re15-savestate-ghidra\scripts")
from re15_ss import Ram
from PIL import Image
r=Ram(sys.argv[1]); x0=int(sys.argv[3]) if len(sys.argv)>3 else 448; y0=int(sys.argv[4]) if len(sys.argv)>4 else 0
w,h=320,240
im=Image.new("RGB",(w,h))
px=im.load()
for y in range(h):
    for x in range(w):
        v=r.vpix(x0+x,y0+y)
        px[x,y]=((v&31)<<3,((v>>5)&31)<<3,((v>>10)&31)<<3)
im=im.resize((w*3,h*3),Image.NEAREST)
im.save(sys.argv[2])
