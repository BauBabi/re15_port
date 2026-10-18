# ORIGINAL-Masken enden INNERHALB des gemalten Objekts? ROOM1030 Cut 3, Maske 2 (Pfeiler), Kante aus dem BG.
import struct
from PIL import Image
S=r"C:\Users\mjoedicke\AppData\Local\Temp\claude\c--workspace-git-reAi-v2\c41eae99-e724-4cb3-afb9-119709f20a9d\scratchpad\L39"
R=r"C:\workspace\git\reAi_v2\.claude\worktrees\r39_optik\re15_port\shared_assets\PSX\STAGE1\ROOM1030.RDT"
b=open(R,"rb").read()
cut=3
fl,fov,fx,fy,fz,tx,ty,tz,pri=struct.unpack_from("<HHiiiiiiI",b,0x80+32*(cut-1)) if False else struct.unpack_from("<HHiiiiiiI",b,0x24+32*cut+ (0x80-0x24-32*1))
# Kamera-Saetze beginnen @0x60 (ROOM1100: Cut1 @0x80 -> Cut0 @0x60), 32 B je Satz
fl,fov,fx,fy,fz,tx,ty,tz,pri=struct.unpack_from("<HHiiiiiiI",b,0x60+32*cut)
print("ROOM1030 Cut %d: fov %d from (%d,%d,%d) to (%d,%d,%d) pri @0x%X"%(cut,fov,fx,fy,fz,tx,ty,tz,pri))
ng,nm=struct.unpack_from("<HH",b,pri); print("Gruppen %d Masken %d"%(ng,nm))
o=pri+4; groups=[]
for g in range(ng):
    cnt,base,dx,dy=struct.unpack_from("<HHhh",b,o); groups.append((cnt,base,dx,dy)); o+=8
masks=[]
for g,(cnt,base,dx,dy) in enumerate(groups):
    for i in range(cnt):
        sx,sy,mx,my,depth,size=struct.unpack_from("<BBBBHH",b,o); o+=8
        if size&0xf000==0:
            w,h=struct.unpack_from("<HH",b,o); o+=4
        else:
            w=h=(size>>12)<<3
        masks.append((len(masks),g,sx,sy,dx+mx,dy+my,w,h,depth))
for m in masks: print("  Maske %d (Gruppe %d): dst (%d,%d) %dx%d Tiefe %d src (%d,%d)"%(m[0],m[1],m[4],m[5],m[6],m[7],m[8],m[2],m[3]))
im=Image.open(S+r"\bg\ROOM10303.ppm").convert("RGB"); px=im.load()
def L(x,y): r,g,bb=px[x,y]; return 0.299*r+0.587*g+0.114*bb
# Maske 2: rechte Kante x_end; die gemalte Pfeilerkante (Runde 38 K: Cut 3 Spalte 228,6) je Zeile als Halbwert-Kante
m=masks[2]; x1=m[4]+m[6]-1
print("Maske 2 letzte Spalte %d, Zeilen %d..%d"%(x1,m[5],m[5]+m[7]-1))
rows=range(m[5],m[5]+m[7])
import statistics
ks=[]
for y in rows:
    l=sum(L(x,y) for x in range(x1-4,x1+1))/5; r=sum(L(x,y) for x in range(x1+5,x1+10))/5; mid=(l+r)/2
    for x in range(x1-2,x1+8):
        a,c=L(x,y),L(x+1,y)
        if (a-mid)*(c-mid)<=0 and a!=c:
            ks.append(x+0.5+(mid-a)/(c-a)); break
print("Halbwert-Kante Pfeiler (Zeilen der Maske): n=%d Mittel %.2f min %.2f max %.2f"%(len(ks),statistics.mean(ks),min(ks),max(ks)))
# Zum Ansehen
c=im.crop((x1-20,m[5]-5,x1+20,m[5]+m[7]+5)).resize((40*6,(m[7]+10)*6),Image.NEAREST); c.save(S+r"\img\bg1030_c3_pfeiler_x6.png")
