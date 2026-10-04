# Wie kniezensus.py, aber RE2-Retail-RDTs: RBJ-Zeiger @RDT+0x60 (Offset-Index 22; RE1.5: +0x5C Index 21).
import glob, os, sys, struct
T = "C:/workspace/git/reAi_v2/.claude/worktrees/r35_cut1150/re15_port/tools"
sys.path.insert(0, T)
import emd_ansichtsblatt as E
def u32(b,o): return struct.unpack_from("<I",b,o)[0]
for rdt in sorted(glob.glob(sys.argv[1])):
    d = open(rdt, "rb").read()
    s = u32(d, 0x60)
    if s == 0 or s + 8 > len(d): continue
    total, nrec = u32(d, s), u32(d, s+4)
    if total == 0 or nrec == 0 or nrec > 8 or s+total+nrec*8 > len(d): continue
    r = d[s:s+total+nrec*8]
    for i in range(nrec):
        pre, edd = u32(r, total+i*8), u32(r, total+i*8+4)
        if pre == 0 or edd == 0 or edd >= len(r): continue
        marker = u32(r, pre)
        o_arm, o_frm, count, ksize = struct.unpack_from("<4H", r, pre+4)
        if count != 15 or not (marker & 1): continue
        sk = E.Skel(); sk.bone_count, sk.kf_size = count, ksize
        sk.kf_data = r[pre+4+o_frm:edd]; sk.kf_count = len(sk.kf_data)//ksize if ksize else 0
        try: cl, fr = E.edd_parse(r[edd:])
        except Exception: continue
        for c in range(len(cl)):
            first, n = cl[c]; y = []
            for f in range(n):
                if first+f >= len(fr): break
                k = fr[first+f] & 0xFFF
                if k < sk.kf_count: y.append(E.kf_pos(sk, k)[1])
            if len(y) < 2: continue
            if max(y) - min(y) >= 250:
                print("%s rec%d marker=%08x clip %2d n=%3d y0=%5d y_end=%5d min=%5d max=%5d" % (os.path.basename(rdt), i, marker, c, n, y[0], y[-1], min(y), max(y)))
