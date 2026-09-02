import sys; sys.path.insert(0,'.')
from lib import *
from PIL import Image, ImageDraw
OUT=r'C:\workspace\git\reAi_v2\analysis\karte_audit_0901'
PAL={0:None,1:(90,90,90),2:(120,120,120),3:(150,150,150),4:(176,176,176),
     5:(200,200,200),6:(220,220,220),7:(240,240,240)}
def render(pg,S=3):
    pix=page_pix(pg)
    im=Image.new('RGB',(320*S,240*S),(10,10,20))
    d=ImageDraw.Draw(im)
    for i,(RX,RY,RW,RH) in enumerate(rects(pg)):
        U,V=rect_uv(pg,i)
        for j in range(RH):
            for k in range(RW):
                if V+j>=256 or U+k>=256: continue
                c=PAL.get(pix[V+j][U+k])
                if c is None: continue
                d.rectangle([(RX+k)*S,(RY+j)*S,(RX+k)*S+S-1,(RY+j)*S+S-1],fill=c)
    for i,(RX,RY,RW,RH) in enumerate(rects(pg)):
        d.rectangle([RX*S,RY*S,(RX+RW)*S-1,(RY+RH)*S-1],outline=(0,90,160))
        d.text((RX*S+2,RY*S+2),'r%d'%i,fill=(0,160,255))
    for gx,gy,gw,gh,wd in GLYPHEN.get(pg,()):
        d.ellipse([gx*S-4,gy*S-4,gx*S+4,gy*S+4],outline=(255,220,0),width=2)
    for k,z in sorted(ZONES.items()):
        if z['page']!=pg: continue
        if z['sx']:
            p0=proj_cal(z,z['x0'],z['z1']); p1=proj_cal(z,z['x1'],z['z0']); col=(255,60,60)
        else:
            p0=proj_bbox(z,z['x0'],z['z1']); p1=proj_bbox(z,z['x1'],z['z0']); col=(60,220,120)
        x0,x1=sorted((p0[0],p1[0])); y0,y1=sorted((p0[1],p1[1]))
        d.rectangle([x0*S,y0*S,x1*S,y1*S],outline=col,width=2)
        d.text((x0*S+2,y0*S+2),'%04X.%d'%(k[0],k[1]),fill=col)
    im.save('%s\seite%d_ist.png'%(OUT,pg))
    print('geschrieben:',OUT+'\seite%d_ist.png'%pg)
for pg in (2,3,4):
    render(pg)
