# Zensus: Wurzelhoehe (Keyframe-Position y, kf_pos) je Bild fuer alle Leon-Clips (PL00.EDD + Raum-RBJ-Records
# mit PL00-Knochenzahl). Ziel: Clips, die zwischen KNIEN und STEHEN wechseln, und ihre Laenge.
import glob, os, sys, struct
T = "C:/workspace/git/reAi_v2/.claude/worktrees/r35_cut1150/re15_port/tools"
sys.path.insert(0, T); sys.path.insert(0, T + "/r34n_d")
import emd_ansichtsblatt as E
import rbj_zensus as R
A = "C:/workspace/git/reAi_v2/.claude/worktrees/r35_cut1150/re15_port/shared_assets/PSX"
emr = E.emr_parse(open(A + "/PLD/PL00.EMR", "rb").read())
clips, frames = E.edd_parse(open(A + "/PLD/PL00.EDD", "rb").read())
def ys(sk, clips, frames, c):
    first, n = clips[c]
    out = []
    for f in range(n):
        k = frames[first + f] & 0xFFF
        if k >= sk.kf_count: out.append(None); continue
        out.append(E.kf_pos(sk, k)[1])
    return out
print("PL00.EDD bones", emr.bone_count, "ksize", emr.kf_size, "clips", len(clips))
for c in range(len(clips)):
    y = ys(emr, clips, frames, c)
    yy = [v for v in y if v is not None]
    if not yy: continue
    print("PL00 clip %2d n=%3d y0=%5d y_end=%5d min=%5d max=%5d" % (c, len(y), yy[0], yy[-1], min(yy), max(yy)))
mode = sys.argv[1] if len(sys.argv) > 1 else "rbj"
if mode == "rbj":
    for rdt in sorted(glob.glob(A + "/STAGE*/ROOM*.RDT")):
        d = open(rdt, "rb").read()
        blk = R.rbj_block(d)
        if not blk: continue
        s, size = blk; r = d[s:s+size]
        total, nrec = E.u32(r, 0), E.u32(r, 4)
        for i in range(nrec):
            pre, edd = E.u32(r, total+i*8), E.u32(r, total+i*8+4)
            if pre == 0 or edd == 0 or edd >= len(r): continue
            o_arm, o_frm, count, ksize = struct.unpack_from("<4H", r, pre+4)
            if count != emr.bone_count: continue
            sk = E.Skel(); sk.bone_count, sk.kf_size = count, ksize
            sk.kf_data = r[pre+4+o_frm:edd]; sk.kf_count = len(sk.kf_data)//ksize if ksize else 0
            try:
                cl, fr = E.edd_parse(r[edd:])
            except Exception:
                continue
            for c in range(len(cl)):
                try: y = ys(sk, cl, fr, c)
                except Exception: continue
                yy = [v for v in y if v is not None]
                if len(yy) < 2: continue
                if max(yy) - min(yy) >= 250:
                    print("%s rec%d marker=%08x clip %2d n=%3d y0=%5d y_end=%5d min=%5d max=%5d" % (os.path.basename(rdt), i, E.u32(r, pre), c, len(y), yy[0], yy[-1], min(yy), max(yy)))
