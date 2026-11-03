import struct, sys, numpy as np
sys.path.insert(0, r'C:\workspace\git\reAi_v2\.claude\worktrees\r41_annette\re15_port\tools')
import emd_ansichtsblatt as EA
import re2emd, rbj2
ROOM = r'C:/workspace/git/reAi_v2/info/re2clair/PL1/RDT/ROOM6141.RDT'
u16=lambda b,o: struct.unpack_from('<H',b,o)[0]
def rbj_skel_clip(rec=0, clip=24):
    d, s, recs = rbj2.re2_rbj(ROOM)
    r = d[s:]
    R = recs[rec]
    total = struct.unpack_from('<I', r, 0)[0]
    pre, edd = struct.unpack_from('<II', r, total + rec*8)
    o_arm, o_frm, count, ksize = struct.unpack_from('<4H', r, pre+4)
    kf_off = pre + 4 + o_frm
    kfdata = r[kf_off:edd]
    count0, off0 = u16(r, edd), u16(r, edd+2)
    ncl = off0//4
    counts = [count0] + [u16(r, edd+4*c) for c in range(1, ncl)]
    first = sum(counts[:clip])
    ents = [struct.unpack_from('<I', r, edd+off0+4*(first+f))[0] for f in range(counts[clip])]
    return kfdata, ksize, ents, (o_arm, o_frm, count)
class M(EA.Modell):
    def __init__(self, kind=0x49, kf=None, ksize=80):
        e = re2emd.emd(kind); d = re2emd.pairs(e)
        self.meshes = EA.md1_parse(e[d[7]:d[8]])
        self.skel = EA.emr_parse(e[d[2]:d[3]])
        if kf is not None:
            self.skel.kf_data = kf; self.skel.kf_size = ksize; self.skel.kf_count = len(kf)//ksize
if __name__ == '__main__':
    kf, ks, ents, hdr = rbj_skel_clip()
    print('hdr', hdr, 'frames', len(ents), [e & 0xfff for e in ents][:40])
    m = M(kf=kf, ksize=ks)
    for f in (0, 10, 20, 30, 39):
        k = ents[f] & 0xfff
        R, T = m.pose(k)
        ys = [int(t[1]) for t in T]; xs=[int(t[0]) for t in T]; zs=[int(t[2]) for t in T]
        print('f%02d kf%3d root %s  bone-y %d..%d  x %d..%d z %d..%d' % (f, k, EA.kf_pos(m.skel,k), min(ys), max(ys), min(xs), max(xs), min(zs), max(zs)))
        V,N,F = m.geometrie(k)
        print('     mesh y %d..%d x %d..%d z %d..%d' % (V[:,1].min(), V[:,1].max(), V[:,0].min(), V[:,0].max(), V[:,2].min(), V[:,2].max()))
