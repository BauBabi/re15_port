import struct, sys
def u16(b,o): return struct.unpack_from("<H",b,o)[0]
def u32(b,o): return struct.unpack_from("<I",b,o)[0]
def rec(r, i):
    tot = u32(r,0); pre, edd = u32(r, tot+8*i), u32(r, tot+8*i+4)
    o_arm,o_frm,cnt,ksz = struct.unpack_from("<HHHH", r, pre+4)
    kf = pre+4+o_frm
    c0, off0 = u16(r,edd), u16(r,edd+2); n = off0//4
    lens = [c0] + [u16(r, edd+4*k) for k in range(1, n)]
    clips = []; first = 0
    for cnt_k in lens:
        clips.append([u32(r, edd+off0+4*(first+j)) for j in range(cnt_k)]); first += cnt_k
    return dict(kf=kf, ksz=ksz, clips=clips, marker=u32(r,pre))
def inhalt(r, R, c):
    return [(e >> 12, r[R["kf"]+(e&0xFFF)*R["ksz"]:R["kf"]+((e&0xFFF)+1)*R["ksz"]]) for e in R["clips"][c]]
a = open(sys.argv[1],"rb").read(); b = open(sys.argv[2],"rb").read()
ra = rec(a, int(sys.argv[3])); 
d2 = b
o = struct.unpack_from("<I", b, 0x5C)[0]
tot = u32(b, o); n = b[o+4]
rbj = b[o:o+tot+8*n]
for ri in range(n):
    rb = rec(rbj, ri)
    for ca in range(len(ra["clips"])):
        ia = inhalt(a, ra, ca)
        if len(ia) <= 1: continue
        for cb in range(len(rb["clips"])):
            ib = inhalt(rbj, rb, cb)
            if ia == ib: print("RE2 clip %d == 11B0 rec%d clip %d (%d Bilder, Bildwort-Flags+Keyframe-Bytes gleich)" % (ca, ri, cb, len(ia)))
