import sys,struct,os
R=r'C:/workspace/git/reAi_v2'
sys.path.insert(0,R+'/re15_port/tools')
import rdt_objekt_ansicht as A
u32=A.u32
for nm,p in (('DISC',R+'/Version_40%/PSX/STAGE2/ROOM2090.RDT'),('MOD',R+'/info/Re1.5/PSX/STAGE2/ROOM2090.RDT')):
    d=open(p,'rb').read()
    print('==',nm,len(d),'hdr',list(d[:8]),'offs',[hex(u32(d,o)) for o in range(8,0x60,4)])
    pr=A.props_lesen(d)
    for i,(t,m) in enumerate(pr):
        try:
            ms=A.md1_parse(m)
            vs=[v[:3] for x in ms for v in x.tv+x.qv]
            xs,ys,zs=zip(*vs) if vs else ((0,),(0,),(0,))
            print(i,'tim',len(t),'md1',len(m),'meshes',len(ms),'tri',sum(len(x.tf) for x in ms),'quad',sum(len(x.qf) for x in ms),'v',len(vs),'bb',(min(xs),max(xs)),(min(ys),max(ys)),(min(zs),max(zs)), 'timhdr',t[:4].hex() if t else '')
        except Exception as ex:
            print(i,'tim',len(t),'md1',len(m),'FEHLER',ex)
    tb=u32(d,0x30)
