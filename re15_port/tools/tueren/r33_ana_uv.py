import sys, os, struct
sys.path.insert(0,'re15_port/tools/tor')
import do2_format as fmt
R='C:/workspace/git/reAi_v2/info/re2leon/COMMON/DOOR/'
def lade(n):
    d=open(R+'DOOR%s.DO2'%n,'rb').read()
    o=fmt.Do2.lesen(d); md=fmt.Md1.lesen(o.md1); tim,_=fmt.Tim.lesen(o.tim)
    return o,md,tim
ref=None
for n in sys.argv[1:]:
    o,md,tim=lade(n)
    info=[]
    for i,m in enumerate(md.meshes):
        us=[t[0] for t in m.tri_tex]+[t[3] for t in m.tri_tex]+[t[6] for t in m.tri_tex]
        vs=[t[1] for t in m.tri_tex]+[t[4] for t in m.tri_tex]+[t[7] for t in m.tri_tex]
        pages=sorted(set(t[5] for t in m.tri_tex)); cl=sorted(set(t[2] for t in m.tri_tex))
        info.append('m%d v%d t%d q%d u%d..%d v%d..%d pg%s cl%s'%(i,len(m.vertices),len(m.tris),len(m.quads),min(us) if us else -1,max(us) if us else -1,min(vs) if vs else -1,max(vs) if vs else -1,[hex(p) for p in pages],[hex(c) for c in cl]))
    m0=md.meshes[0]
    key=(tuple(m0.vertices),tuple(m0.tris),tuple(tuple(t[:2])+tuple(t[3:5])+tuple(t[6:8]) for t in m0.tri_tex))
    if ref is None: ref=key
    print('DOOR%s tim flags %d clut %s pix %s len %d | m0 geo=%s uv=%s | skripte %d'%(n,tim.flags,tim.clut_rect,tim.pix_rect,len(o.tim),key[0]==ref[0] and key[1]==ref[1],key[2]==ref[2],len(o.skripte)))
    for s in info: print('   ',s)
