# Kennzahlen aller EMD-Bloecke Disc/Mod + RE2 EM23. Parser aus re15_port/tools/emd_ansichtsblatt.py.
import sys,struct,hashlib
R=r'C:/workspace/git/reAi_v2'
sys.path.insert(0,R+'/re15_port/tools')
import emd_ansichtsblatt as E
u32=E.u32
def blocks(path):
    d=open(path,'rb').read(); cur=0; out=[]
    while cur+4<=len(d):
        dir_off=u32(d,cur)
        if dir_off<36 or dir_off&3 or cur+dir_off+36>len(d): break
        ln=dir_off+36; out.append((cur,d[cur:cur+ln]))
        cur=(cur+ln+2047)&~2047
        sk=0
        while cur+4<=len(d) and u32(d,cur)==0: cur+=4; sk+=4
        if sk: cur=(cur+2047)&~2047
    return out
def info(b):
    D=E.emd_dir(b)
    n=len(D)
    edd=b[D[1]:D[2]]; emr=b[D[2]:D[3]]
    s=E.emr_parse(emr)
    clips,fr=E.edd_parse(edd)
    md1end=D[8] if n>8 else u32(b,0)
    m=E.md1_parse(b[D[7]:md1end])
    tv=sum(len(x.tv) for x in m); qv=sum(len(x.qv) for x in m); tf=sum(len(x.tf) for x in m); qf=sum(len(x.qf) for x in m)
    xs=[v[0] for x in m for v in x.tv+x.qv]; ys=[v[1] for x in m for v in x.tv+x.qv]; zs=[v[2] for x in m for v in x.tv+x.qv]
    t=b[D[8]:] if n>8 else b''
    tw=th=0
    if len(t)>40 and u32(t,0)==0x10:
        o=8+u32(t,8)
        tw,th=E.u16(t,o+8)*2,E.u16(t,o+10)
    return dict(n=n,bones=s.bone_count,meshes=len(m),clips=len(clips),frames=len(fr),kf=s.kf_count,tri=tf,quad=qf,verts=tv+qv,bx=(min(xs),max(xs)),by=(min(ys),max(ys)),bz=(min(zs),max(zs)),tim=(tw,th),parent=s.parent,dirs=D)
if __name__=='__main__':
    ORD=[0x10,0x11,0x12,0x13,0x16,0x18,0x1A,0x20,0x21,0x24,0x25,0x26,0x27,0x29,0x2B,0x2D,0x30,0x36,0x40,0x42,0x45,0x47,0x49,0x4B,0x4D]
    for nm,p in (('DISC CDEMD0',R+'/Version_40%/PSX/EMD/CDEMD0.EMS'),('MOD CDEMD0',R+'/info/Re1.5/PSX/EMD/CDEMD0.EMS')):
        print('==',nm)
        for i,(c,b) in enumerate(blocks(p)):
            try:
                I=info(b)
                print(f"{i:2d} typ {ORD[i] if i<25 else 0:02X} sek {c//2048:5d} len {len(b):7d} kno {I['bones']:2d} mesh {I['meshes']:2d} clips {I['clips']:3d} frames {I['frames']:5d} kf {I['kf']:5d} tri {I['tri']:4d} quad {I['quad']:4d} v {I['verts']:5d} bx {I['bx']} by {I['by']} bz {I['bz']} tim {I['tim']}")
            except Exception as ex:
                print(i,'FEHLER',ex)
    I=info(open(R+'/re15_port/shared_assets/RE2/EM23.EMD','rb').read())
    print('RE2 EM23',{k:v for k,v in I.items() if k!='parent'})
