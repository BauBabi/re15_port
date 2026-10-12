import sys, struct, subprocess, re
from PIL import Image, ImageDraw
T="C:/workspace/git/reAi_v2/.claude/worktrees/r36_integration"
def s16(b,o): v=b[o]|(b[o+1]<<8); return v-65536 if v>=32768 else v
def load(r):
    d=open(T+"/re15_port/shared_assets/PSX/STAGE1/ROOM%s.RDT"%r,"rb").read()
    so=struct.unpack_from("<I",d,0x20)[0]
    cnt=struct.unpack_from("<5I",d,so+4)
    cells=[];o=so+24;seen=set()
    for g in range(5):
        for i in range(cnt[g]):
            w,dn,x,z,t,u0,u1,fl=struct.unpack_from("<HHhhBBBB",d,o); o+=12
            k=(w,dn,x,z,t,u0,u1,fl)
            if k in seen: continue
            seen.add(k); cells.append((g,i,x,z,w,dn,t,u0,u1,fl))
    out=subprocess.run([sys.executable,"-I",T+"/re15_port/tools/scd_dump_room.py",T+"/re15_port/shared_assets/PSX/STAGE1/ROOM%s.RDT"%r],capture_output=True,text=True).stdout
    aots=[]
    for line in out.splitlines():
        m=re.match(r"\s+0x([0-9A-F]+) \(\+\w+\) (Door_aot_set|Aot_set|Item_aot_set)\s+((?:[0-9a-f]{2} )+)",line)
        if not m: continue
        b=bytes(int(x,16) for x in m.group(3).split())
        aots.append((m.group(2),b[1],b[2],s16(b,6),s16(b,8),s16(b,10),s16(b,12),m.group(1), b[23] if m.group(2)=="Door_aot_set" else None, b[4]))
    return cells,aots
def plot(r,x0,x1,z0,z1,S,extra=[],name=None,bands=None):
    cells,aots=load(r)
    W=int((x1-x0)/S)+1; H=int((z1-z0)/S)+1
    img=Image.new("RGB",(W,H),(255,255,255)); dr=ImageDraw.Draw(img)
    def P(x,z): return ((x-x0)/S, (z1-z)/S)
    for gx in range((x0//1000)*1000, x1, 1000):
        p=P(gx,z0); dr.line([p[0],0,p[0],H],fill=(235,235,235) if gx%5000 else (200,200,200)); 
        if gx%5000==0: dr.text((p[0]+1,H-12),str(gx),fill=(120,120,120))
    for gz in range((z0//1000)*1000, z1, 1000):
        p=P(x0,gz); dr.line([0,p[1],W,p[1]],fill=(235,235,235) if gz%5000 else (200,200,200));
        if gz%5000==0: dr.text((1,p[1]),str(gz),fill=(120,120,120))
    cols=[(200,0,0),(0,140,0),(0,0,200),(180,120,0),(150,0,150),(0,150,150),(90,90,90)]
    for (g,i,x,z,w,dn,t,u0,u1,fl) in cells:
        if bands is not None and (fl>>4) not in bands: continue
        a=P(x,z+dn); b=P(x+w,z)
        c=cols[(fl>>4)%len(cols)]
        dr.rectangle([a,b],outline=c,width=2)
        dr.text((a[0]+2,a[1]+2),"%d.%d t%d f%02x u%02x"%(g,i,t,fl,u0),fill=c)
    for (k,sl,sce,x,z,w,dn,off,dest,fl) in aots:
        if w==0: continue
        a=P(x,z+dn); b=P(x+w,z)
        c=(255,0,255) if k=="Door_aot_set" else ((0,180,255) if k=="Item_aot_set" else (255,150,0))
        dr.rectangle([a,b],outline=c,width=3)
        lab=("D%d->%02X"%(sl,dest)) if dest is not None else ("%s%d s%d"%("I" if k[0]=="I" else "A",sl,sce))
        dr.text((a[0]+2,b[1]-12),lab,fill=c)
    for (x,z,lab) in extra:
        p=P(x,z); dr.ellipse([p[0]-4,p[1]-4,p[0]+4,p[1]+4],fill=(0,0,0)); dr.text((p[0]+6,p[1]-5),lab,fill=(0,0,0))
    img.save(name or ("crop_%s.png"%r))
if __name__=="__main__":
    r=sys.argv[1]; x0,x1,z0,z1,S=map(int,sys.argv[2:7])
    ex=[]
    for a in sys.argv[7:]:
        x,z,l=a.split(","); ex.append((int(x),int(z),l))
    plot(r,x0,x1,z0,z1,S,ex)
