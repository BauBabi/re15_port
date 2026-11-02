import sys
from ssram import R
from PIL import Image
r=R(sys.argv[1]); vb=r.vram_base; b=r.blob
im=Image.new("RGB",(1024,512)); px=im.load()
for y in range(512):
    for x in range(1024):
        o=vb+(y*1024+x)*2; v=b[o]|(b[o+1]<<8)
        px[x,y]=((v&31)<<3,((v>>5)&31)<<3,((v>>10)&31)<<3)
im.save(sys.argv[2])
