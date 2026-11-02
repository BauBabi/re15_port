import struct, hashlib, os, glob
R='C:/workspace/git/reAi_v2/'
def a2o(a): return a-0x80010000+0x800
for tag,base in (('DISC',R+'Version_40%/'),('MOD',R+'info/Re1.5/')):
    exe=open(base+'PSX.EXE','rb').read()
    for toc,arc in ((0x80072f38,'CDEMD0.EMS'),(0x80073178,'CDEMD1.EMS')):
        ems=open(base+'PSX/EMD/'+arc,'rb').read()
        ent=[]
        for i in range(0x40):  # Typen 0x10..0x4F
            s,l=struct.unpack_from('<II',exe,a2o(toc+i*8))
            if l: ent.append((s,l,0x10+i))
        ent.sort()
        nsec=len(ems)//2048
        used=set()
        for s,l,t in ent:
            for k in range(s,s+(l+2047)//2048): used.add(k)
        free=[k for k in range(nsec) if k not in used]
        nz=[k for k in free if any(ems[k*2048:(k+1)*2048])]
        print(tag,arc,'TOC-Eintraege',len(ent),'versch. Sektoren',len(set(e[0] for e in ent)),'Sektoren',nsec,'ohne TOC',len(free),'davon nicht-null',len(nz), (nz[:3],nz[-3:]) if nz else '')
        runs=[];
        for k in free:
            if runs and runs[-1][1]==k-1: runs[-1][1]=k
            else: runs.append([k,k])
        print('   freie Laeufe:',[(a,b) for a,b in runs if any(ems[a*2048:(b+1)*2048])][:12])
