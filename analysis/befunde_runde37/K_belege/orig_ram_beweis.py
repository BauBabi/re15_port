import sys, struct
sys.path.insert(0, r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/.claude/skills/re15-savestate-ghidra/scripts')
import re15_ss
W = r'C:/workspace/git/reAi_v2/.claude/worktrees/r37_roy/re15_port/shared_assets/PSX/'
def blob(ems, sec, sz): d = open(W+'EMD/'+ems,'rb').read(); return d[sec*2048:sec*2048+sz]
E = blob('CDEMD1.EMS', 1639, 142272); L = blob('CDEMD0.EMS', 1644, 127640)
def dirs(b):
    do = struct.unpack_from('<I', b, 0)[0]; return list(struct.unpack_from('<9I', b, do))
dE, dL = dirs(E), dirs(L)
def cmp(r, ent, name):
    e = 0x800acc2c + ent*0x1f4
    edd = r.u32(e+0x16c); base = edd - dE[1]
    out = []
    for k, nm in ((1,'dir1 EDD'),(2,'dir2 EMR'),(3,'dir3 EDD'),(4,'dir4 EMR')):
        ln = dE[k+1]-dE[k]
        ram = r.bytes(base+dE[k], ln)
        out.append('%s %s(%dB)=CDEMD1:%s CDEMD0:%s' % (nm, '', ln, ram == E[dE[k]:dE[k+1]],
                   r.bytes(edd - dL[1] + dL[k], dL[k+1]-dL[k]) == L[dL[k]:dL[k+1]]))
    md1 = r.u32(e+0x10) - 0xc
    out.append('mesh +0x10-0xc = blob+%#x (CDEMD1 dir7 %#x)' % (md1-base, dE[7]))
    print(name, 'blob@%08x' % base, '; '.join(out))
def rbj(r, ent, room):
    e = 0x800acc2c + ent*0x1f4
    rdt = r.u32(0x800ac778); f = open(W+'STAGE1/ROOM%s.RDT' % room,'rb').read()
    ao = struct.unpack_from('<I', f, 0x5c)[0]; tot = struct.unpack_from('<I', f, ao)[0]
    nrec = struct.unpack_from('<I', f, ao+4)[0]
    for k in range(nrec):
        pre, edd = struct.unpack_from('<II', f, ao+tot+k*8)
        print('   %s rec%d: EMR %08x EDD %08x' % (room, k, rdt+ao+pre+4, rdt+ao+edd))
    print('   Entity +0x180=%08x +0x184=%08x' % (r.u32(e+0x180), r.u32(e+0x184)))
    pre, edd = struct.unpack_from('<II', f, ao+tot+8)
    n = struct.unpack_from('<H', f, ao+edd+2)[0]
    print('   rec1 EDD-Kopf im RAM == Datei:', r.bytes(r.u32(e+0x184), n) == f[ao+edd:ao+edd+n])
for sav, ent, room in (('user_slot1.sav',0,'1011'),('user_slot2.sav',0,'1011'),('user_slot5.sav',1,'1021')):
    r = re15_ss.Ram(sav); cmp(r, ent, sav); rbj(r, ent, room)
