import glob, os, re, struct, sys
R='C:/workspace/git/reAi_v2/'
T=set(range(0x30,0x38))|{0x22,0x23}
def regions(d):
    offs=list(struct.unpack_from('<21I',d,8))
    nz=sorted(set(o for o in offs if 0<o<len(d)))
    out=[]
    for idx in (14,15,16):
        o=offs[idx]
        if 0<o<len(d):
            nxt=[x for x in nz if x>o]; e=nxt[0] if nxt else len(d)
            out.append((idx,o,e))
    return out
alle={}
for tag,base in (('DISC','Version_40%/PSX'),('MOD','info/Re1.5/PSX'),('PORT','re15_port/shared_assets/PSX')):
    fl=sorted(glob.glob(R+base+'/STAGE*/ROOM*.RDT'))
    hits=[]; n44=0
    for p in fl:
        d=open(p,"rb").read()
        if len(d)<92: print("   kurz:",os.path.basename(p),len(d)); continue
        for idx,a,e in regions(d):
            for m in re.finditer(b'\x44',d[a:e]):
                o=a+m.start(); 
                if o+20>len(d): continue
                ty=d[o+2]
                if ty in T and (d[o+1]&0x7f)<16:
                    x,y,z=struct.unpack_from('<hhh',d,o+8)
                    hits.append((os.path.basename(p),idx,o,d[o:o+20].hex(' '),x,y,z))
    alle[tag]=hits
    print(tag,'RDT',len(fl),'Treffer in SCD',len(hits))
    for h in hits: print('  %s scd%d @%#x  %s  xyz %d %d %d'%h)
