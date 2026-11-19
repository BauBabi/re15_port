import sys, collections
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/tools')
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r4')
import kartlib as K, geh, anker
from PIL import Image, ImageDraw, ImageFont
STEP=200
def begehbar(rid):
    s,d,_=K.read_rdt(rid); cells=list(dict.fromkeys(s)); qs=anker.rvd_quads(rid)
    pts=set()
    for cf,ct,xs,zs in qs:
        for x in range(min(xs)-min(xs)%STEP, max(xs)+1, STEP):
            for z in range(min(zs)-min(zs)%STEP, max(zs)+1, STEP):
                if (x,z) in pts: continue
                if anker.in_quad(x,z,xs,zs) and not any(geh.fest(c,x,z) for c in cells): pts.add((x,z))
    return pts,d,qs
if __name__=='__main__':
    fb=ImageFont.truetype('C:/Windows/Fonts/consolab.ttf',18); f=ImageFont.truetype('C:/Windows/Fonts/consola.ttf',12)
    rooms=[int(a,16) for a in sys.argv[2:]]
    out=Image.new('RGB',(len(rooms)*600,30+600),(15,15,25)); dr=ImageDraw.Draw(out)
    dr.rectangle([0,0,out.size[0],28],fill=(160,0,0)); dr.text((6,4),'Messbild - kein Ergebnis   begehbar = in einer Kamera-Zone (RVD) und nicht in einer festen Zelle, 1 px = 100',font=fb,fill=(255,255,255))
    for k,rid in enumerate(rooms):
        pts,d,qs=begehbar(rid)
        xs=[p[0] for p in pts]; zs=[p[1] for p in pts]
        X0=min(xs); Z1=max(zs)
        print('%04X'%rid,len(pts),'x',min(xs),max(xs),'z',min(zs),max(zs),'rvd',len(qs))
        for x,z in pts:
            px=k*600+5+(x-X0)//100; py=40+(Z1-z)//100
            dr.rectangle([px,py,px+1,py+1],fill=(120,200,120))
        for t in d:
            px=k*600+5+(t['lx']-X0)//100; py=40+(Z1-t['lz'])//100
            dr.rectangle([px-3,py-3,px+3,py+3],fill=(255,220,0)); dr.text((px+4,py-6),'%X'%t['dest'],font=f,fill=(255,220,0))
        dr.text((k*600+5,30),'%04X x %d..%d z %d..%d'%(rid,min(xs),max(xs),min(zs),max(zs)),font=f,fill=(255,255,255))
    out.save(sys.argv[1])
