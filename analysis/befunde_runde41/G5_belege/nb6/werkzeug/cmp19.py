# Bank 0x19 von ROOM7040 effect.esp gegen alle Banken der shared_assets/RE2/*.ESP und aller RE2-Leon-RDTs vergleichen
import struct, glob, os, hashlib
def u32(b,o): return struct.unpack_from('<I',b,o)[0]
def banks(esp):
    al=(len(esp)+3)&~3; ende=al-4; ids=[]; offs=[]
    for n in range(8):
        if esp[n]==0xff: break
        ids.append(esp[n]); offs.append(u32(esp,ende-4*n))
    res={}
    # Bank-Ende = naechster groesserer Bank-Offset bzw. Anfang der Offset-Tafel
    tafel=ende-4*(len(ids)-1)
    so=sorted(offs)+[tafel]
    for i,o in zip(ids,offs):
        e=so[so.index(o)+1]
        res[i]=(o,esp[o:e])
    return res
r7=open('ROOM7040_effect.esp','rb').read()
b7=banks(r7)[0x19]
print('ROOM7040 Bank19 off 0x%X len %d md5 %s'%(b7[0],len(b7[1]),hashlib.md5(b7[1]).hexdigest()))
SA=r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_g5auftritt\re15_port\shared_assets\RE2'
for f in sorted(glob.glob(SA+r'\*.ESP')):
    e=open(f,'rb').read()
    try: bk=banks(e)
    except Exception as ex: print(f,ex); continue
    print(os.path.basename(f), len(e), 'Ids', ' '.join('%02X'%i for i in bk), 'ganz==7040:', e==r7)
    if 0x19 in bk:
        o,d=bk[0x19]
        print('  Bank19 off 0x%X len %d md5 %s gleich=%s'%(o,len(d),hashlib.md5(d).hexdigest(), d==b7[1]))
        if d!=b7[1]:
            n=min(len(d),len(b7[1])); diff=[k for k in range(n) if d[k]!=b7[1][k]]
            print('  erste Abweichungen', diff[:20])
# alle RE2-Leon-RDTs: welche Raeume haben eine Bank 19, gleich?
RD=r'C:\workspace\git\reAi_v2\info\re2leon\PL0\RDT'
hits=[]
for f in sorted(glob.glob(RD+r'\ROOM*.RDT')):
    b=open(f,'rb').read()
    try:
        offs=struct.unpack_from('<23I',b,8)
        if not offs[18] or not offs[19]: continue
        esp=b[offs[18]:offs[19]+4]
        bk=banks(esp)
    except Exception: continue
    if 0x19 in bk:
        hits.append((os.path.basename(f), bk[0x19][1]==b7[1], len(bk[0x19][1]), ' '.join('%02X'%i for i in bk), esp==r7))
for h in hits: print('RDT', *h)
