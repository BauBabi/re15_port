import sys, collections
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/tools')
sys.path.insert(0, r'C:/Users/MJOEDI~1/AppData/Local/Temp/claude/1ad3836299811990/c41eae99-e724-4cb3-afb9-119709f20a9d/scratchpad/karte/s3r4')
import kartlib as K, geh
from PIL import Image, ImageDraw, ImageFont
def flut(rid, starts, step=200):
    s,d,_=K.read_rdt(rid); cells=list(dict.fromkeys(s))
    x0=min(c[0] for c in cells); x1=max(c[0]+c[2] for c in cells); z0=min(c[1] for c in cells); z1=max(c[1]+c[3] for c in cells)
    frei=lambda x,z: not any(geh.fest(c,x,z) for c in cells)
    seen=set(); q=collections.deque()
    for sx,sz in starts:
        p=(sx-sx%step, sz-sz%step)
        for a in range(-2,3):
            for b in range(-2,3):
                pp=(p[0]+a*step,p[1]+b*step)
                if frei(*pp) and pp not in seen: seen.add(pp); q.append(pp)
    while q:
        x,z=q.popleft()
        for a,b in ((step,0),(-step,0),(0,step),(0,-step)):
            p=(x+a,z+b)
            if p in seen or not (x0<=p[0]<=x1 and z0<=p[1]<=z1): continue
            if frei(*p): seen.add(p); q.append(p)
    return seen,(x0,z0,x1,z1),d
if __name__=='__main__':
    fb=ImageFont.truetype('C:/Windows/Fonts/consolab.ttf',18)
    jobs=[(0x1260,[(-21159,3289),(-8750,-6900),(19700,-5000)]),(0x10B0,[(-20250,-6700),(4014,-7436)]),(0x20A0,[(6300,-16250)])]
    out=Image.new('RGB',(3*600,30+600),(15,15,25)); dr=ImageDraw.Draw(out)
    dr.rectangle([0,0,out.size[0],28],fill=(160,0,0)); dr.text((6,4),'Messbild - kein Ergebnis   Flut ueber ALLE Baender, 1 px = 100 Einheiten',font=fb,fill=(255,255,255))
    for k,(rid,st) in enumerate(jobs):
        pts,bb,d=flut(rid,st)
        xs=[p[0] for p in pts]; zs=[p[1] for p in pts]
        print('%04X'%rid,len(pts),'x',min(xs),max(xs),'z',min(zs),max(zs))
        for x,z in pts:
            px=k*600+5+(x-bb[0])//100; py=32+(bb[3]-z)//100
            dr.rectangle([px,py,px+1,py+1],fill=(120,200,120))
        for t in d:
            px=k*600+5+(t['lx']-bb[0])//100; py=32+(bb[3]-t['lz'])//100
            dr.rectangle([px-3,py-3,px+3,py+3],fill=(255,220,0))
    out.save('mb_flut3.png')
