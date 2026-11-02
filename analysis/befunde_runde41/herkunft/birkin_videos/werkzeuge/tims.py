import struct, glob, os, numpy as np
from PIL import Image, ImageDraw
def c15(v):
    r=(v&31)<<3; g=((v>>5)&31)<<3; b=((v>>10)&31)<<3
    return np.stack([r,g,b],-1).astype(np.uint8)
def tim(d):
    flag=struct.unpack_from('<I',d,4)[0]; bpp=flag&7; o=8; pal=None
    if flag&8:
        ln,x,y,w,h=struct.unpack_from('<IHHHH',d,o); pal=np.frombuffer(d,'<u2',w*h,o+12).reshape(h,w); o+=ln
    ln,x,y,w,h=struct.unpack_from('<IHHHH',d,o); px=np.frombuffer(d,'<u2',w*h,o+12).reshape(h,w)
    if bpp==2: return c15(px)
    if bpp==1:
        idx=px.view(np.uint8).reshape(h,w*2); return c15(pal[0][idx])
    if bpp==0:
        b=px.view(np.uint8).reshape(h,w*2); idx=np.stack([b&15,b>>4],-1).reshape(h,w*4); return c15(pal[0][idx])
    return None
out=[]
for p in sorted(glob.glob(r'C:/workspace/git/reAi_v2/Version_40%/PSX/DATA/*.TIM')):
    try:
        im=tim(open(p,'rb').read())
        out.append((os.path.basename(p), Image.fromarray(im)))
    except Exception as e: print(p,e)
W=1600; x=y=0; rowh=0
S=Image.new('RGB',(W,2400))
for n,im in out:
    im.thumbnail((400,300))
    if x+im.width>W: x=0; y+=rowh+14; rowh=0
    S.paste(im,(x,y+14)); ImageDraw.Draw(S).text((x+2,y),n,fill=(255,255,0)); x+=im.width+6; rowh=max(rowh,im.height)
S.crop((0,0,W,y+rowh+16)).save('bv/d/disc_tims.jpg',quality=88); print(len(out))
