# raster4.py <laufordner> <aus.jpg> <titel> F1 F2 ...: Raster aus RE15_FRAMEDUMP-Serie (fd_NNNNNN.ppm, Rueckleser vor dem Present), 4 Spalten,
# je Bild 320x240 (Fenster 640x480 halbiert), Beschriftung: F, Figur (x,y,z) und Spieler-Clip aus state.log/climb.log desselben Laufs. Optional Aufhellung env HELL.
import sys,os,re
from PIL import Image,ImageDraw,ImageEnhance
run,aus,titel=sys.argv[1],sys.argv[2],sys.argv[3]; Fs=[int(f) for f in sys.argv[4:]]
hell=float(os.environ.get('HELL','1'))
st={}
for l in open(os.path.join(run,'state.log'),errors='replace'):
    m=re.match(r'F(\d+) pad=\w+ PL\((-?\d+),(-?\d+),rot=(-?\d+),hp=(-?\d+)\).*? mo=(\d+) ',l)
    if m: st[int(m.group(1))]=m.groups()
yv={}
for l in open(os.path.join(run,'climb.log'),errors='replace'):
    m=re.match(r'T(\d+) .*?pos=\((-?\d+),(-?\d+),(-?\d+)\)',l)
    if m: yv[int(m.group(1))]=int(m.group(3))
W,H,C=320,240,4
R=(len(Fs)+C-1)//C
img=Image.new('RGB',(C*(W+4)+4,22+R*(H+18)),(20,20,20)); d=ImageDraw.Draw(img); d.text((4,4),titel,fill=(230,230,230))
for i,F in enumerate(Fs):
    p=os.path.join(run,'fd_%06d.ppm'%F)
    x=4+(i%C)*(W+4); y=22+(i//C)*(H+18)
    if os.path.exists(p):
        im=Image.open(p).convert('RGB').resize((W,H))
        if hell!=1: im=ImageEnhance.Brightness(im).enhance(hell)
        img.paste(im,(x,y))
    s=st.get(F)
    lab='F%d'%F+(' Figur (%s,%s) mo=%s hp=%s'%(s[1],s[2],s[5],s[4]) if s else '')
    d.text((x,y+H+2),lab,fill=(255,255,0))
img.save(aus,quality=88); print(aus,img.size)
